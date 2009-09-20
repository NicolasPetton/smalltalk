"======================================================================
|
|   SAX event-based pull parser
|
|
 ======================================================================"

"======================================================================
|
 ======================================================================"

Object subclass: SAXEventSequence [
    | event next |

    SAXEventSequence class >> new [
	^super new initialize
    ]

    event [
	^event
    ]

    event: anObject [
	event := anObject
    ]

    initialize [
	next := self.
    ]

    isEmpty [
	^next == self
    ]

    next [
	^next
    ]

    next: aSAXList [
	next := aSAXList.
    ]
]



CObject subclass: XMLExpatParserObject [
    XMLExpatParserObject class >> createFor: anObject [
	<cCall: 'gst_XML_ParserCreate' returning: #{XMLExpatParserObject}
	    args: #(#smalltalk)>
    ]

    free [
	<cCall: 'XML_ParserFree' returning: #void args: #(#self)>
    ]

    parse: aString len: len isFinal: aBoolean [
	<cCall: 'XML_Parse' returning: #int args: #(#self #string #int #boolean)>
    ]

    resume [
	<cCall: 'XML_ResumeParser' returning: #int args: #(#self)>
    ]

    userData [
	^(self castTo: ##(CObject type ptrType)) value
    ]

    userData: aCObject [
	<cCall: 'XML_SetUserData' returning: #void args: #(#self #unknown)>
    ]
]



XMLPullParser subclass: XMLExpatPullParser [
    <category: 'Expat'>
    <comment: 'This is based on Antony Blakey''s XMLPullParser.'>

    | xp nextParser current pending source sourceStack buffer |

    XMLExpatPullParser class >> on: source [
	<category: 'instance creation'>
	^self new initialize: source
    ]

    atEnd [
	<category: 'core api'>
	^source isNil
    ]

    current [
	<category: 'core api'>
	^current
    ]

    initialize: aSource [
	<category: 'initialize-release'>
	source := aSource.
	sourceStack := OrderedCollection new.
	pending := SAXEventSequence new.
	buffer := String new: 1024.
	xp := XMLExpatParserObject createFor: self.
	current := SAXStartDocument new.
	self advance.
    ]

    push: anXPParser for: aStream [
	| userData |
	sourceStack add: source.
	source := aStream.
	userData := xp userData.
	xp setUserData: nextParser.
	nextParser := xp.
	xp := anXPParser.
	xp setUserData: userData.
    ]

    pop [
	xp free.
	xp := nextParser.
	xp isNil
	    ifTrue: [
		source := nil.
		current := SAXEndDocument new ]
	    ifFalse: [
	        source := sourceStack removeLast.
	        nextParser := xp userData castTo: XMLExpatParserObject type.
	        xp userData: self ].
    ]

    parseMore [
	| len |
	len := source nextAvailable: buffer size into: buffer startingAt: 1.
	xp
	    parse: buffer
	    len: len
	    isFinal: source atEnd
    ]

    advance [
	<category: 'private api'>
	| result |
	"First try removing the head of the pending event queue.
	 PENDING points to the tail of a circular list, and is
	 just a sentinel node."
	pending isEmpty ifFalse: [
	    current := pending next event.
	    pending next event: nil.
	    pending next: pending next next.
	    ^self ].

	"See if we're done."
	self atEnd ifTrue: [^self].

	"Start parsing.  On the first call we need to do #parseMore."
	result := current isStartDocument
	    ifTrue: [ self parseMore ]
	    ifFalse: [ current := nil. xp resume ].
	
	[ current isNil ] whileTrue: [
	    result = 0 ifTrue: [ ^self error: 'aaa' ].
	    result = 2 ifTrue: [ ^self halt ].
	    result := source atEnd
		ifTrue: [
		    self pop. 
		    current isNil ifFalse: [ 2 ] ifTrue: [ xp resume ] ]
		ifFalse: [ self parseMore ]
	].
	^current
    ]
]
