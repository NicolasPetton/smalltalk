CompiledCode extend [

    basicMethod [
	<category: 'installing'>

	^ self
    ]
]

CompiledMethod subclass: MethodWrapper [

    <category: 'Examples-VM'>

    MethodWrapper class [ | WrapperList | ]

    MethodWrapper class >> on: aSymbol in: aClass [
	<category: 'instance creation'>

	^ (self numArgs: aSymbol numArgs)
	    selector: aSymbol;
	    methodClass: aClass;
	    yourself
    ]

    MethodWrapper class >> readdWrappersAfter: aWrapper [
        <category: 'testing'>

        | all index toAdd |
        all := self wrappersFor: aWrapper basicMethod.
        index := all identityIndexOf: aWrapper.
        toAdd := all copyFrom: index + 1.
        all empty.
        toAdd do: [ :each | each install ]
    ]

    MethodWrapper class >> recordWrapper: aWrapper [
        <category: 'testing'>

        (self wrappersFor: aWrapper basicMethod) addLast: aWrapper
    ]

    MethodWrapper class >> wrappersFor: aMethod [
        <category: 'testing'>

        WrapperList isNil ifTrue: [ WrapperList := IdentityDictionary new ].
        ^ WrapperList at: aMethod ifAbsentPut: [ OrderedCollection new ]
    ]

    | method methodClass selector |

    beforeMethod [
	<category: 'wrapping'>

    ]

    afterMethod [
	<category: 'wrapping'>

    ]

    valueWithReceiver: rec withArguments: args [
	<category: 'wrapping'>

	self beforeMethod.
	^ [ rec perform: method withArguments: args ]
	    ensure: [ self afterMethod ]
    ]

    basicMethod [
	<category: 'installing'>

	^ method basicMethod
    ]

    install [
	<category: 'installing'>

	method := self methodClass lookupSelector: selector.
	method isNil ifTrue: [ self error: 'cannot find method' ].
	self methodClass methodDictionary at: selector put: self.
	self class recordWrapper: self.
    ]

    uninstall [
	<category: 'installing'>

	method methodClass == self methodClass
	    ifTrue: [ self methodDictionary at: selector put: method ]
	    ifFalse: [ self methodDictionary removeKey: selector ifAbsent: [] ].
	self class readdWrappersAfter: self.
	method := nil.
    ]

    isInstalled [
	<category: 'installing'>

	^ method notNil
    ]

    methodClass [
	<category: 'installing'>

	^ methodClass
    ]

    methodDictionary [
	<category: 'installing'>

	^ methodClass methodDictionary
    ]

    methodClass: aClass [
	<category: 'installing'>

	methodClass := aClass
    ]

    selector: aSymbol [
	<category: 'installing'>

	selector := aSymbol
    ]
]

