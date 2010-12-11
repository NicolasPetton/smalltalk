MethodWrapper subclass: WrapperExample [

    <category: 'Examples-VM'>

    WrapperExample class [ | WrapperList | ]

    WrapperExample class >> test [
	<category: 'installing'>

	| a b |
	a := WrapperExample on: #asArray in: String.
	b := WrapperExample on: #asArray in: String.

	(a isInstalled->b isInstalled) printNl.
	(String includesSelector: #asArray) printNl.

	a install: 'wrapper1'.
	(a isInstalled->b isInstalled) printNl.
	(String includesSelector: #asArray) printNl.
	'abc' asArray.

	b install: 'wrapper2'.
	(a isInstalled->b isInstalled) printNl.
	(String includesSelector: #asArray) printNl.
	'abc' asArray.

	a uninstall.
	(a isInstalled->b isInstalled) printNl.
	(String includesSelector: #asArray) printNl.
	'abc' asArray.

	b uninstall.
	(a isInstalled->b isInstalled) printNl.
	(String includesSelector: #asArray) printNl.
	'abc' asArray
    ]

    | text |

    install: aString [
	<category: 'installing'>

	text := aString.
	self install
    ]

    beforeMethod [
	<category: 'installing'>

	('before ', text) printNl
    ]

    afterMethod [
	<category: 'installing'>

	('after ', text) printNl
    ]
]