# rime

rime is an extension for `<regex>`.

## Overview

- Compile time regex pattern check
- Some utilities for regex
- Header Only
- Requires C++20 or later
    - GCC 11.1 or later
    - MSVC 2019 Preview latest or later

## Facility

### Compile time regex patern check

Checks the validity of a regular expression string and raises an Compile Error if it is wrong.

If there is no problem, the compiler will not say anything.

※ Currently, it only supports the ECMAScript format.

#### UDL

Pass a string to `std::regex` using User Defined Literal (`""_re`).

```cpp
#include <iostream>
#include "rime.hpp"

using namespace rime::literals;

int main() {
  std::regex re{R"_(\d{1,)_"_re};
}
```
```
In file included from prog.cc:6:
rime.hpp: In function 'int main()':
prog.cc:11:17:   in 'constexpr' expansion of 'rime::literals::operator""_re(((const char*)"\\d{1,"), 5)'
rime.hpp:724:32:   in 'constexpr' expansion of 'rime::patern_check<char>::start(std::basic_string_view<char>(str, len))'
rime.hpp:135:18:   in 'constexpr' expansion of 'rime::patern_check<char>::disjunction(it, ((const char*)fin))'
rime.hpp:144:18:   in 'constexpr' expansion of 'rime::patern_check<char>::alternative((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:164:13:   in 'constexpr' expansion of 'rime::patern_check<char>::term((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:175:21:   in 'constexpr' expansion of 'rime::patern_check<char>::quantifier((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:210:24:   in 'constexpr' expansion of 'rime::patern_check<char>::quantifier_prefix((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:280:33: error: call to non-'constexpr' function 'void rime::REGEX_PATERN_ERROR(const char*)'
  280 |               REGEX_PATERN_ERROR(R"_(Quantifiers braces are not closed. [Example: `\d{0, 10` ] )_");
      |               ~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```
- [[Wandbox]三へ( へ՞ਊ ՞)へ ﾊｯﾊｯ](https://wandbox.org/permlink/we21OQLZGbcMQLJi)

This can be used for `wchar_t` as well.

#### `std::regex` factory function

Use factory function `rime::regex()` to create a `std::regex` from a string.

`rime::regex(regex_str)` returns `std::regex`.

```cpp
#include <iostream>
#include "rime.hpp"

using namespace rime::literals;

int main() {
  auto re = rime::regex(R"_(\d{1,)_");
}
```
```
In file included from prog.cc:3:
rime.hpp: In function 'int main()':
prog.cc:8:24:   in 'constexpr' expansion of 'rime::detail::regex_patern_str<char>("\\d{1,")'
rime.hpp:745:35:   in 'constexpr' expansion of 'rime::patern_check<char>::start(((rime::detail::regex_patern_str<char>*)this)->rime::detail::regex_patern_str<char>::str)'
rime.hpp:135:18:   in 'constexpr' expansion of 'rime::patern_check<char>::disjunction(it, ((const char*)fin))'
rime.hpp:144:18:   in 'constexpr' expansion of 'rime::patern_check<char>::alternative((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:164:13:   in 'constexpr' expansion of 'rime::patern_check<char>::term((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:175:21:   in 'constexpr' expansion of 'rime::patern_check<char>::quantifier((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:210:24:   in 'constexpr' expansion of 'rime::patern_check<char>::quantifier_prefix((* & it), ((rime::patern_check<char>::S)fin))'
rime.hpp:280:33: error: call to non-'constexpr' function 'void rime::REGEX_PATERN_ERROR(const char*)'
  280 |               REGEX_PATERN_ERROR(R"_(Quantifiers braces are not closed. [Example: `\d{0, 10` ] )_");
      |               ~~~~~~~~~~~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
```
- [[Wandbox]三へ( へ՞ਊ ՞)へ ﾊｯﾊｯ](https://wandbox.org/permlink/Mz78iG2rUAmEAGfK)

This can be used for `wchar_t` as well.

### `rime::regex_searches()`

`rime::regex_searches()` will search the input string for all substrings that match the regular expression pattern.

The return value of `rime::regex_searches()` is a `range` object that represents the entire search result.

If the return type of `rime::regex_searches(str, regex)` is `R`, then `R` is `forward_range` and `viewable_range`.

```cpp
#include <iostream>
#include "rime.hpp"

using namespace rime::literals;

int main() {
  const auto regex = rime::regex(R"(\d+)");

  for (const auto &m : rime::regex_searches("1421, 34353, 7685, 12765, 976754", regex)) {
    std::cout << m.str() << ' ';
  }
}

// Output
// 1421 34353 7685 12765 976754
```
- [[Wandbox]三へ( へ՞ਊ ՞)へ ﾊｯﾊｯ](https://wandbox.org/permlink/R9NkDCIxSg1wmcVA)

This is a wrapper for `std::regex_iterator`, which does `std::regex_search` in succession.

# Appendix : ECMAScript RegExp Patterns

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

IdentifierStart ::
    UnicodeLetter
    $
    _
    \ UnicodeEscapeSequence

IdentifierPart ::
    IdentifierStart
    UnicodeCombiningMark
    UnicodeDigit
    UnicodeConnectorPunctuation
    \ UnicodeEscapeSequence

UnicodeLetter
    any character in the Unicode categories “Uppercase letter (Lu)”, “Lowercase letter (Ll)”, “Titlecase letter (Lt)”, “Modifier letter (Lm)”, “Other letter (Lo)”, or “Letter number (Nl)”.

UnicodeCombiningMark
    any character in the Unicode categories “Non-spacing mark (Mn)” or “Combining spacing mark (Mc)”

UnicodeDigit
    any character in the Unicode category “Decimal number (Nd)”

UnicodeConnectorPunctuation
    any character in the Unicode category “Connector punctuation (Pc)”
```
