MethodWrapper subclass: CodeCoverageWrapper [
    | called |

    <category: 'SUnit-CodeCoverage'>
    <comment: nil>

    beforeMethod [
	<category: 'wrapping'>
	called := true
    ]

    isCalled [
	<called: 'testing'>
	^called ifNil: [false]
    ]
]

Object subclass: CodeCoverageTester [
    | namespace |

    <category: 'SUnit-CodeCoverage'>
    <comment: nil>

    CodeCoverageTester class [

	on: aNamespaceString [
	    <category: 'instance creation'>
	    ^self new
		namespace: (self lookupNamespace: aNamespaceString);
		yourself
	]
    
	lookupNamespace: aString [
	    <category: 'lookup'>
	    | ns |
	    ns := Smalltalk.
	    aString isNil ifTrue: [^ ns].
	    (aString subStrings: $.) do: [ :each |
                | key |
                key := each asSymbol.
                (ns includesKey: key) ifFalse: [ 
		    self error: 'namespace is not correct'].
                ns := ns at: key].
	    ^ ns
	]
    ]

    namespace [
	<category: 'accessing'>
	^namespace
    ]

    namespace: aNamespace [
	<category: 'accessing'>
	namespace := aNamespace
    ]

    classesDo: aBlock [
	<category: 'enumerating'>
	self namespace withAllSubspacesDo: [:each |
	    each allClassesDo: aBlock]
    ]

    methods [
	<category: 'accessing'>
	^Array streamContents: [:str |
	    self classesDo: [:each |
		each methodDictionary ifNotNil: [:dict |
		    str nextPutAll: dict values]]]
    ]

    result [
	<category: 'accessing'>
	^CodeCoverageResult on: self methods
    ]

    withCoverageDo: aBlock [
	<category: 'running'>
	^[
	    self install.
	    aBlock value.
	    self result
	] ensure: [
	    self uninstall]
    ]

    install [
	<category: 'installing'>
	self methods do: [:each | self installWrapperOn: each]
    ]

    uninstall [
	<category: 'installing'>
	self methods do: [:each | self uninstallWrapper: each]
    ]

    installWrapperOn: aMethod [
	<category: 'private'>
	(CodeCoverageWrapper on: aMethod selector in: aMethod methodClass) install
    ]

    uninstallWrapper: aWrapper [
	<category: 'private'>
	aWrapper uninstall
    ]
]

Object subclass: CodeCoverageResult [
    | wrappers |

    <category: 'SUnit-CodeCoverage'>
    <comment: nil>

    CodeCoverageResult class [
	
	on: aCollection [
	    <category: 'instance creation'>
	    ^self new
		wrappers: aCollection;
		yourself
	]
    ]

    wrappers [
	<category: 'accessing'>
	^wrappers
    ]

    wrappers: aCollection [
	<category: 'accessing'>
	wrappers := aCollection
    ]
    
    allMethods [
	<category: 'accessing'>
	^self wrappers
    ]

    calledMethods [
	<category: 'accessing'>
	^self wrappers select: [:each | each isCalled]
    ]

    printResult [
	<category: 'printing'>
	self resultString printNl
    ]

    resultString [
	<category: 'printing'>
	^String streamContents: [:str |
	    str
		nextPutAll: 'Code coverage:';
		nextPut: Character cr;
		nextPutAll: 'Called methods: ';
		nextPutAll: ((self calledMethods size / self allMethods size) * 100 asScaledDecimal: 2) displayString;
		nextPutAll: '%']
    ]
] 
