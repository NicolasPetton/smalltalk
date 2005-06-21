"======================================================================
|
|   Smalltalk-in-Smalltalk library loading script.
|
|
 ======================================================================"


"======================================================================
|
| Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
| Written by Paolo Bonzini.
|
| This file is part of GNU Smalltalk.
|
| GNU Smalltalk is free software; you can redistribute it and/or modify it
| under the terms of the GNU General Public License as published by the Free
| Software Foundation; either version 2, or (at your option) any later version.
| 
| GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
| ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
| FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
| details.
| 
| You should have received a copy of the GNU General Public License along with
| GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
| Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
|
 ======================================================================"

Smalltalk addSubspace: #STInST!
Namespace current: STInST!

FileStream fileIn: 'RBToken.st'.
FileStream fileIn: 'RBParseNodes.st'.
FileStream fileIn: 'RBParser.st'.
FileStream fileIn: 'ParseTreeSearcher.st'.
FileStream fileIn: 'RBFormatter.st'.
FileStream fileIn: 'STCompLit.st'.
FileStream fileIn: 'STSymTable.st'.
FileStream fileIn: 'STCompiler.st'.
FileStream fileIn: 'STDecompiler.st'.
FileStream fileIn: 'STLoaderObjs.st'.
FileStream fileIn: 'STLoader.st'!

Namespace current: Smalltalk!

