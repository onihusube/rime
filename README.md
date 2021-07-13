# rime

rime is an extension for `<regex>`.


## ECMAScript RegExp Patterns

- [15.10 RegExp (Regular Expression) Objects - ECMA-262 (ES 3)](https://www.ecma-international.org/wp-content/uploads/ECMA-262_3rd_edition_december_1999.pdf)

```
Pattern ::
    Disjunction

Disjunction ::
    Alternative
    Alternative | Disjunction

Alternative ::
    [empty]
    Alternative Term

Term ::
    Assertion
    Atom
    Atom Quantifier

Assertion :: 
    ^
    $
    \b
    \B

Quantifier ::
    QuantifierPrefix
    QuantifierPrefix ?

QuantifierPrefix ::
    *
    +
    ?
    { DecimalDigits }
    { DecimalDigits , }
    { DecimalDigits , DecimalDigits }

Atom ::
    PatternCharacter
    .
    \ AtomEscape
    CharacterClass
    ( Disjunction )
    ( ? : Disjunction )
    ( ? = Disjunction )
    ( ? ! Disjunction )

PatternCharacter :: SourceCharacter but not any of: 
    ^ $ \ . * + ? ( ) [ ] { } |

AtomEscape ::
    DecimalEscape
    CharacterEscape
    CharacterClassEscape

CharacterEscape ::
    ControlEscape
    c ControlLetter
    HexEscapeSequence
    UnicodeEscapeSequence
    IdentityEscape

ControlEscape :: one of
    f n r t v

ControlLetter :: one of
    a b c d e f g h i j k l m n o p q r s t u v w x y z
    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z

IdentityEscape ::
    SourceCharacter but not IdentifierPart

DecimalEscape ::
    DecimalIntegerLiteral [lookahead ∉ DecimalDigit]

CharacterClassEscape :: one of
    d D s S w W

CharacterClass ::
    [ [lookahead ∉ {^}] ClassRanges ]
    [ ^ ClassRanges ]

ClassRanges ::
    [empty]
    NonemptyClassRanges

NonemptyClassRanges ::
    ClassAtom
    ClassAtom NonemptyClassRangesNoDash
    ClassAtom - ClassAtom ClassRanges

NonemptyClassRangesNoDash ::
    ClassAtom
    ClassAtomNoDash NonemptyClassRangesNoDash
    ClassAtomNoDash - ClassAtom ClassRanges

ClassAtom ::
    -
    ClassAtomNoDash

ClassAtomNoDash ::
    SourceCharacter but not one of \ ] -
    \ ClassEscape

ClassEscape ::
    DecimalEscape
    b
    CharacterEscape
    CharacterClassEscape

HexEscapeSequence ::
    x HexDigit HexDigit

UnicodeEscapeSequence ::
    u HexDigit HexDigit HexDigit HexDigit

HexDigit :: one of
    0 1 2 3 4 5 6 7 8 9 a b c d e f A B C D E F
```
