/******************************* -*- C -*- ****************************
 *
 *      Expat bindings
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 2009 Free Software Foundation, Inc.
 * Written by Paolo Bonzini.
 *
 * This file is part of GNU Smalltalk.
 *
 * GNU Smalltalk is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * Linking GNU Smalltalk statically or dynamically with other modules is
 * making a combined work based on GNU Smalltalk.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the Free Software Foundation
 * give you permission to combine GNU Smalltalk with free software
 * programs or libraries that are released under the GNU LGPL and with
 * independent programs running under the GNU Smalltalk virtual machine.
 *
 * You may copy and distribute such a system following the terms of the
 * GNU GPL for GNU Smalltalk and the licenses of the other code
 * concerned, provided that you include the source code of that other
 * code when and as the GNU GPL requires distribution of source code.
 *
 * Note that people who make modified versions of GNU Smalltalk are not
 * obligated to grant this special exception for their modified
 * versions; it is their choice whether to do so.  The GNU General
 * Public License gives permission to release a modified version without
 * this exception; this exception also makes it possible to release a
 * modified version which carries forward this exception.
 *
 * GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "gstpub.h"
#include <expat.h>

typedef struct st_XMLExpatPullParser
{
  OBJ_HEADER;
  OOP needFlags[4];
  OOP xpOOP;
  OOP nextParserOOP;
  OOP currentEventOOP;
  OOP pendingEventOOP;
  OOP sourceOOP;
  OOP sourceStackOOP;
} *XMLExpatPullParser;

typedef struct st_SAXEventSequence
{
  OBJ_HEADER;
  OOP eventOOP;
  OOP nextOOP;
} *SAXEventSequence;

static OOP attributeClass;
static OOP nodeTagClass;
static OOP saxEndCdataSectionClass;
static OOP saxEndDoctypeDeclClass;
static OOP saxEndDocumentFragmentClass;
static OOP saxEndDocumentClass;
static OOP saxStartCdataSectionClass;
static OOP saxStartDocumentFragmentClass;
static OOP saxStartDocumentClass;
static OOP saxTagEventClass;
static OOP saxEndTagClass;
static OOP saxStartTagClass;
static OOP saxProcessingInstructionClass;
static OOP saxTextClass;
static OOP saxCommentClass;
static OOP saxStartPrefixMappingClass;
static OOP saxEndPrefixMappingClass;
static OOP saxStartDoctypeDeclClass;
static OOP saxNotationDeclClass;
static OOP saxUnparsedEntityDeclClass;
static OOP saxEventSequenceClass;
static VMProxy *vmProxy;

static OOP
make_node_tag (const char *n)
{
  return vmProxy->stringToOOP (n);
}

static void
make_event (OOP parserOOP, OOP classOOP, ...)
{
  va_list va;
  OOP eventOOP, ptr;
  OOP sentinelOOP, headOOP;
  XMLExpatPullParser parserObj;
  SAXEventSequence pendingObj, sentinelObj;
  mst_Object obj;
  int i;

  eventOOP = vmProxy->objectAlloc (classOOP, 0);
  obj = OOP_TO_OBJ (eventOOP);

  va_start (va, classOOP);
  for (i = 0; (ptr = va_arg (va, OOP)); i++)
    obj->data[i] = ptr;
  va_end (va);

  parserObj = (XMLExpatPullParser) OOP_TO_OBJ (parserOOP);
  if (parserObj->currentEventOOP == vmProxy->nilOOP)
    {
      parserObj->currentEventOOP = eventOOP;
      return;
    }

  /* Put the event in the current sentinel node.  */
  pendingObj = (SAXEventSequence) OOP_TO_OBJ (parserObj->pendingEventOOP);
  pendingObj->eventOOP = eventOOP;
  headOOP = pendingObj->nextOOP;

  /* Allocate a new sentinel node and make it the tail.  */
  sentinelOOP = vmProxy->objectAlloc (saxEventSequenceClass, 0);

  parserObj = (XMLExpatPullParser) OOP_TO_OBJ (parserOOP);
  pendingObj = (SAXEventSequence) OOP_TO_OBJ (parserObj->pendingEventOOP);
  sentinelObj = (SAXEventSequence) OOP_TO_OBJ (sentinelOOP);

  sentinelObj->nextOOP = pendingObj->nextOOP;
  pendingObj->nextOOP = sentinelOOP;
  parserObj->pendingEventOOP = sentinelOOP;
}

static void
gst_StartElementHandler (void *userData,
			 const XML_Char * name,
			 const XML_Char ** atts)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxStartTagClass,
	      make_node_tag (name),
	      vmProxy->objectAlloc (vmProxy->arrayClass, 0),
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_EndElementHandler (void *userData,
		       const XML_Char * name)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxEndTagClass,
	      vmProxy->stringToOOP (name),
	      NULL);

  XML_StopParser (p, 1);
}


static void
gst_CharacterDataHandler (void *userData,
			  const XML_Char * s,
			  int len)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  OOP stringOOP;
  char *data = memcpy (alloca (len + 1), s, len);
  data[len] = 0;
  stringOOP = vmProxy->stringToOOP (data);
  make_event (parserOOP,
	      saxTextClass,
	      stringOOP,
	      vmProxy->intToOOP (1),
	      vmProxy->intToOOP (len),
	      stringOOP,
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_ProcessingInstructionHandler (void *userData,
				  const XML_Char * target,
       				  const XML_Char * data)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxProcessingInstructionClass,
	      vmProxy->stringToOOP (target),
	      vmProxy->stringToOOP (data),
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_CommentHandler (void *userData,
		    const XML_Char * data)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  OOP stringOOP = vmProxy->stringToOOP (data);
  make_event (parserOOP,
	      saxCommentClass,
	      stringOOP,
	      vmProxy->intToOOP (1),
	      vmProxy->intToOOP (strlen (data)),
	      stringOOP,
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_StartCdataSectionHandler (void *userData)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxStartCdataSectionClass,
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_EndCdataSectionHandler (void *userData)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxEndCdataSectionClass,
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_StartDoctypeDeclHandler (void *userData,
			     const XML_Char *doctypeName,
			     const XML_Char *systemId,
			     const XML_Char *publicId,
			     int has_internal_subset)

{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxStartDoctypeDeclClass,
	      vmProxy->stringToOOP (systemId),
	      vmProxy->stringToOOP (publicId),
	      vmProxy->stringToOOP (doctypeName),
	      vmProxy->boolToOOP (has_internal_subset),
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_EndDoctypeDeclHandler (void *userData)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxEndDoctypeDeclClass,
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_UnparsedEntityDeclHandler (void *userData,
			       const XML_Char * entityName,
			       const XML_Char * base,
			       const XML_Char * systemId,
			       const XML_Char * publicId,
			       const XML_Char * notationName)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxUnparsedEntityDeclClass,
	      vmProxy->stringToOOP (systemId),
	      vmProxy->stringToOOP (publicId),
	      vmProxy->stringToOOP (entityName),
	      vmProxy->stringToOOP (notationName),
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_NotationDeclHandler (void *userData,
			 const XML_Char * notationName,
			 const XML_Char * base,
			 const XML_Char * systemId,
			 const XML_Char * publicId)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxNotationDeclClass,
	      vmProxy->stringToOOP (systemId),
	      vmProxy->stringToOOP (publicId),
	      vmProxy->stringToOOP (notationName),
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_StartNamespaceDeclHandler (void *userData,
			       const XML_Char * prefix,
			       const XML_Char * uri)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxStartPrefixMappingClass,
	      vmProxy->stringToOOP (prefix),
	      vmProxy->stringToOOP (uri),
	      NULL);

  XML_StopParser (p, 1);
}

static void
gst_EndNamespaceDeclHandler (void *userData,
			     const XML_Char * prefix)
{
  XML_Parser p = userData;
  OOP parserOOP = XML_GetUserData (p);
  make_event (parserOOP,
	      saxEndPrefixMappingClass,
	      vmProxy->stringToOOP (prefix),
	      NULL);

  XML_StopParser (p, 1);
}


static void
initClasses (void)
{
  attributeClass = vmProxy->classNameToOOP ("XML.Attribute");
  nodeTagClass = vmProxy->classNameToOOP ("XML.NodeTag");
  saxCommentClass = vmProxy->classNameToOOP ("XML.SAXComment");
  saxEndCdataSectionClass = vmProxy->classNameToOOP ("XML.SAXEndCdataSection");
  saxEndDoctypeDeclClass = vmProxy->classNameToOOP ("XML.SAXEndDoctypeDecl");
  saxEndDocumentClass = vmProxy->classNameToOOP ("XML.SAXEndDocument");
  saxEndDocumentFragmentClass = vmProxy->classNameToOOP ("XML.SAXEndDocumentFragment");
  saxEndPrefixMappingClass = vmProxy->classNameToOOP ("XML.SAXEndPrefixMapping");
  saxEndTagClass = vmProxy->classNameToOOP ("XML.SAXEndTag");
  saxEventSequenceClass = vmProxy->classNameToOOP ("XML.SAXEventSequence");
  saxNotationDeclClass = vmProxy->classNameToOOP ("XML.SAXNotationDecl");
  saxProcessingInstructionClass = vmProxy->classNameToOOP ("XML.SAXProcessingInstruction");
  saxStartCdataSectionClass = vmProxy->classNameToOOP ("XML.SAXStartCdataSection");
  saxStartDoctypeDeclClass = vmProxy->classNameToOOP ("XML.SAXStartDoctypeDecl");
  saxStartDocumentClass = vmProxy->classNameToOOP ("XML.SAXStartDocument");
  saxStartDocumentFragmentClass = vmProxy->classNameToOOP ("XML.SAXStartDocumentFragment");
  saxStartPrefixMappingClass = vmProxy->classNameToOOP ("XML.SAXStartPrefixMapping");
  saxStartTagClass = vmProxy->classNameToOOP ("XML.SAXStartTag");
  saxTagEventClass = vmProxy->classNameToOOP ("XML.SAXTagEvent");
  saxTextClass = vmProxy->classNameToOOP ("XML.SAXText");
  saxUnparsedEntityDeclClass = vmProxy->classNameToOOP ("XML.SAXUnparsedEntityDecl");
}

static XML_Parser
gst_XML_ParserCreate (OOP parserOOP)
{
  XML_Parser p = XML_ParserCreateNS (NULL, ':');
  XML_UseParserAsHandlerArg (p);
  XML_SetUserData (p, parserOOP);
  
  /* Missing: XML_SetExternalEntityRefHandler */
  XML_SetStartElementHandler (p, gst_StartElementHandler);
  XML_SetEndElementHandler (p, gst_EndElementHandler);
  XML_SetCharacterDataHandler (p, gst_CharacterDataHandler);
  XML_SetProcessingInstructionHandler (p, gst_ProcessingInstructionHandler);
  XML_SetCommentHandler (p, gst_CommentHandler);
  XML_SetStartCdataSectionHandler (p, gst_StartCdataSectionHandler);
  XML_SetEndCdataSectionHandler (p, gst_EndCdataSectionHandler);
  XML_SetStartDoctypeDeclHandler (p, gst_StartDoctypeDeclHandler);
  XML_SetEndDoctypeDeclHandler (p, gst_EndDoctypeDeclHandler);
  XML_SetUnparsedEntityDeclHandler (p, gst_UnparsedEntityDeclHandler);
  XML_SetNotationDeclHandler (p, gst_NotationDeclHandler);
  XML_SetStartNamespaceDeclHandler (p, gst_StartNamespaceDeclHandler);
  XML_SetEndNamespaceDeclHandler (p, gst_EndNamespaceDeclHandler);
  XML_SetReturnNSTriplet (p, true);

  if (!saxEventSequenceClass)
    initClasses ();

  return p;
}

void
gst_initModule (VMProxy * proxy)
{
  vmProxy = proxy;
  vmProxy->defineCFunc ("gst_XML_ParserCreate", gst_XML_ParserCreate);
  vmProxy->defineCFunc ("XML_ParserFree", XML_ParserFree);
  vmProxy->defineCFunc ("XML_Parse", XML_Parse);
  vmProxy->defineCFunc ("XML_ResumeParser", XML_ResumeParser);
  vmProxy->defineCFunc ("XML_SetUserData", XML_SetUserData);
}
