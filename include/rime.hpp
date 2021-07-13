#pragma once

#include <regex>
#include <ranges>

#define fn static constexpr auto
//#define Error(msg) throw msg;

namespace rime::detail {

  template<typename CharT>
  consteval auto select_literal(char cstr, wchar_t wstr, char8_t u8str, char16_t u16str, char32_t u32str) -> CharT {
    if constexpr (std::same_as<CharT, char>) {
      return cstr;
    }
    if constexpr (std::same_as<CharT, wchar_t>) {
      return wstr;
    }
    if constexpr (std::same_as<CharT, char8_t>) {
      return u8str;
    }
    if constexpr (std::same_as<CharT, char16_t>) {
      return u16str;
    }
    if constexpr (std::same_as<CharT, char32_t>) {
      return u32str;
    }
  }

  template<typename CharT, std::size_t N>
  consteval auto select_literal(const char (&cstr)[N], const wchar_t (&wstr)[N], const char8_t (&u8str)[N], const char16_t (&u16str)[N], const char32_t (&u32str)[N]) -> const CharT(&)[N] {
    if constexpr (std::same_as<CharT, char>) {
      return cstr;
    }
    if constexpr (std::same_as<CharT, wchar_t>) {
      return wstr;
    }
    if constexpr (std::same_as<CharT, char8_t>) {
      return u8str;
    }
    if constexpr (std::same_as<CharT, char16_t>) {
      return u16str;
    }
    if constexpr (std::same_as<CharT, char32_t>) {
      return u32str;
    }
  }
}

#define LITERAL(CharT, ltrl) rime::detail::select_literal<CharT>(ltrl, L ## ltrl, u8 ## ltrl, u ## ltrl, U ## ltrl)

namespace rime {

  using std::ranges::begin;
  using std::ranges::end;

  [[noreturn]]
  void REGEX_PATERN_ERROR(const char* str) {
    throw str;
  }

  [[noreturn]]
  void regex_error_unimplemented() {
    throw "Unimplemented...";
  }

  inline namespace concepts {
    template <typename T>
    concept regex_usable_character = std::same_as<T, char> or std::same_as<T, wchar_t>;
  }

  template<regex_usable_character CharT>
  struct character_constant {
    using CArray = std::basic_string_view<CharT>;

    static constexpr CharT backslash = LITERAL(CharT, '\\');
    static constexpr CharT lparen = LITERAL(CharT, '(');
    static constexpr CharT rparen = LITERAL(CharT, ')');
    static constexpr CharT lbrace = LITERAL(CharT, '{');
    static constexpr CharT rbrace = LITERAL(CharT, '}');
    static constexpr CharT lbracket = LITERAL(CharT, '[');
    static constexpr CharT rbracket = LITERAL(CharT, ']');
    static constexpr CharT bitwise_or = LITERAL(CharT, '|');
    static constexpr CharT question = LITERAL(CharT, '?');
    static constexpr CharT comma = LITERAL(CharT, ',');
    static constexpr CharT space = LITERAL(CharT, ' ');
    static constexpr CharT caret = LITERAL(CharT, '^');
    static constexpr CharT dollar = LITERAL(CharT, '$');
    static constexpr CharT b = LITERAL(CharT, 'b');
    static constexpr CharT B = LITERAL(CharT, 'B');
    static constexpr CharT c = LITERAL(CharT, 'c');
    static constexpr CharT x = LITERAL(CharT, 'x');
    static constexpr CharT u = LITERAL(CharT, 'u');
    static constexpr CharT dot = LITERAL(CharT, '.');
    static constexpr CharT equal = LITERAL(CharT, '=');
    static constexpr CharT colon = LITERAL(CharT, ':');
    static constexpr CharT exclamation = LITERAL(CharT, '!');
    static constexpr CArray quantifier_prefix_symbols = LITERAL(CharT, R"_(*+?{)_");
    static constexpr CArray decimal_digits = LITERAL(CharT, "0123456789");
    static constexpr CArray hex_digits = LITERAL(CharT, "0123456789abcdefABCDEF");
    static constexpr CArray not_pattern_characters = LITERAL(CharT, R"_(^$\.([]}*+?{|))_");
    static constexpr CArray character_class_escapes = LITERAL(CharT, "dDsSwW");
    static constexpr CArray control_escapes = LITERAL(CharT, "fnrtv");
    static constexpr CArray control_letters = LITERAL(CharT, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  };

  template<std::weakly_incrementable I>
  constexpr void consume(I& i) {
    ++i;
  }

  template<std::random_access_iterator I>
  constexpr void consume_n(I& i, std::iter_difference_t<I> n) {
    i += n;
  }

  template<std::ranges::input_range R, std::equality_comparable_with<std::ranges::range_reference_t<R>> T>
  constexpr auto contains(R&& r, const T& value) -> bool {
    return std::ranges::any_of(std::forward<R>(r), [&value](const auto& v) { return v == value; });
  }

  template<regex_usable_character CharT>
  struct patern_check {

    using chars = character_constant<CharT>;

    using I = std::ranges::iterator_t<std::basic_string_view<CharT>>;
    using S = std::ranges::sentinel_t<std::basic_string_view<CharT>>;

    fn start(std::basic_string_view<CharT> patern) {
      auto it = patern.begin();
      const auto fin = patern.end();

      disjunction(it, fin);

      if (it != fin) {
        REGEX_PATERN_ERROR("Parse error.");
      }
      // ここにきたらOK
    }

    static constexpr void disjunction(I& it, const S fin) {
      alternative(it, fin);

      if (it == fin) return;

      if (const auto c = *it; c == chars::bitwise_or) {
        consume(it);
        return disjunction(it, fin);
      } else if (c == chars::rparen) {
        // 先読みアサーションとグループ内のパース時にdisjunctionを終了する
        return;
      } else {
        // 構文エラー？
        REGEX_PATERN_ERROR("The expected '|' did not appear.");
      }
    }

    fn alternative(I& it, const S fin) {
      while (it != fin) {
        const auto c = *it;
        if (c == chars::bitwise_or or c == chars::rparen) return;
        term(it, fin);
      }
    }

    fn term(I& it, const S fin) {
      if (assertion(it, fin) == true) {
        return;
      } else {
        atom(it, fin);

        if (it != fin) {
          quantifier(it, fin);
        }
      }
    }

    fn assertion(I& it , const S fin) -> bool {
      const auto c = *it;

      switch (c) {
      case chars::caret: [[fallthrough]];
      case chars::dollar:
        consume(it);
        return true;
      case chars::backslash:
        {
          auto next = it + 1;
          if (next == fin) {
            // エスケープシーケンスの不正な終了
            REGEX_PATERN_ERROR("The escape sequence is empty.");
          }

          const auto c2 = *next;
          if (c2 == chars::b or c2 == chars::B) {
            // \bの次まで消費
            consume_n(it, 2);
            return true;
          }
        }
        [[fallthrough]];
      default:
        return false;
      }
    }

    fn quantifier(I& it, const S fin) {
      quantifier_prefix(it, fin);

      if (it != fin and *it == chars::question) {
        consume(it);
      }

      return;
    }

    fn quantifier_prefix(I& it, const S fin) {
      // termの呼び出しで終端チェック済

      const std::input_or_output_iterator auto p = std::ranges::find(chars::quantifier_prefix_symbols, *it);
      const auto e = end(chars::quantifier_prefix_symbols);

      if (p != e) {
        // * + ? { だった時
        if (auto t = e - 1; p == t) {
          // { だった時
          consume(it);

          bool follow_digits = false;

          // 前半DecimalDigits
          while(true) {
            if (it == fin) {
              // \d{0,10}のようなかっこが閉じていない
              REGEX_PATERN_ERROR(R"_(Quantifiers braces are not closed. [Example: `\d{0, 10` ] )_");
            }

            const auto c = *it;
            if (c == chars::comma) {
              if (not follow_digits) {
                // 数字が現れる前にカンマが現れている
                REGEX_PATERN_ERROR(R"_(Within Quantifiers, you need a digits before the ','. [Example: `\d{, 2}` ] )_");
              }
              // 後半読み取りへ
              consume(it);
              break;
            }
            if (c == chars::rbrace) {
              if (not follow_digits) {
                // 数字が現れる前に閉じている
                REGEX_PATERN_ERROR(R"_(Quantifiers must have at least one number. [Example: `\d{}` ] )_");
              }
              // 数量詞終端
              consume(it);
              return;
            }
            if (c == chars::space) {
              // スペースは読み飛ばす
              consume(it);
              continue;
            }
            // 数字のチェック
            const bool is_digits = contains(chars::decimal_digits, c);
            if (is_digits) {
              follow_digits = true;
              consume(it);
              continue;
            }

            // それ以外の出現はエラー
            REGEX_PATERN_ERROR(R"_(You can't use anything but numbers within Quantifiers. [Example: `\d{@}`, `a{a}` ] )_");
          }

          // 後半DecimalDigits
          while(true) {
            if (it == fin) {
              // \d{0,10}のようなかっこが閉じていない
              REGEX_PATERN_ERROR(R"_(Quantifiers braces are not closed. [Example: `\d{0, 10` ] )_");
            }

            const auto c = *it;
            if (c == chars::comma) {
              // 後半にカンマはない
              REGEX_PATERN_ERROR(R"(A ',' can appear only once in Quantifiers. [Example: `\d{0, 10, 2}` ] )");
            }
            if (c == chars::rbrace) {
              // 数量詞終端
              consume(it);
              return;
            }
            if (c == chars::space) {
              // スペースは読み飛ばす
              consume(it);
              continue;
            }
            // 数字のチェック
            const bool is_digits = contains(chars::decimal_digits, c);
            if (is_digits) {
              consume(it);
              continue;
            }

            // それ以外の出現はエラー
            REGEX_PATERN_ERROR(R"_(You can't use anything but numbers within Quantifiers. [Example: `\d{@}`, `a{a}` ] )_");
          }
        }
        consume(it);
      }
      // * + ? { 以外は消費しないで戻る
      return;
    }

    fn atom(I& it, const S fin) {
      const auto c = *it;

      switch (c) {
      case chars::dot:
        consume(it);
        return;
      case chars::backslash:
        atom_escape(it, fin);
        return;
      case chars::lbracket:
        consume(it);
        character_class(it, fin);
        return;
      case chars::lparen:
        lookahead_assertion_or_group(it, fin);
        return;
      default:
        pattern_character(it);
        return;
      }
    }

    fn pattern_character(I& it) {
      // ^ $ \ . * + ? ( ) [ ] { } | を除いた1文字
      // * + ? { | ) => 消費せずに戻る
      // ^ $ \ . ( [ ] } => エラー
      // othewise => 消費して戻る

      const std::input_or_output_iterator auto p = std::ranges::find(chars::not_pattern_characters, *it);
      const auto e = std::ranges::end(chars::not_pattern_characters);

      if (p == e) {
        // 通常のパターン文字
        consume(it);
        return;
      }

      // エラーにしない文字は後ろの方で見つかるようにしてある
      // -1しているのは、文字配列の終端が文字終端'\0'の次の位置であるため
      if (auto t = e - 6; t <= p) {
        // * + ? { | )
        return;
      } else {
        // ^ $ \ . ( [ ] }
        REGEX_PATERN_ERROR(R"_(These symbols need escaping (one of ^ $ \ . ( [ ] } ).)_");
      }
    }
  
    fn atom_escape(I& it, const S fin) {
      consume(it);
      if (it == fin) {
        // 孤立したバックスラッシュ
        REGEX_PATERN_ERROR("The last backslash is isolated.");
      }

      if (decimal_escape(it, fin) == true) {
        return;
      }
      if (character_escape(it, fin) == true) {
        return;
      }
      if (character_class_escape(it) == true) {
        return;
      }

      // ここにきたらエラー？
      regex_error_unimplemented();
    }

    fn decimal_escape(I& it, const S fin) -> bool {
      if (const auto d1 = *it; d1 == chars::decimal_digits[0]) {
        // \0 の時は1桁オンリー
        consume(it);
      } else if (contains(chars::decimal_digits, d1)) {
        // \n (n != 0)の時は2桁ok
        consume(it);
        if (it == fin) return true;
        if (contains(chars::decimal_digits, *it)) {
          consume(it);
        } else {
          // 1桁数値エスケープ
          return true;
        }
      } else {
        // 数値が現れない場合は別のエスケープ文字
        return false;
      }

      if (it == fin) return true;

      // その次の文字としてDecimalDigitsが現れてはならない
      if (contains(chars::decimal_digits, *it)) {
        REGEX_PATERN_ERROR("Numeric escape is up to two digits. But, when it starts with 0, it is one digit.");
      }

      return true;
    }

    fn character_escape(I& it, const S fin) -> bool {
      const auto c = *it;
      // ControlEscape
      if (contains(chars::control_escapes, *it)) {
        consume(it);
        return true;
      }
      // c ControlLetter
      if (chars::c == c) {
        consume(it);
        if (it == fin) {
          REGEX_PATERN_ERROR(R"_(`\c` is not a valid escape sequence.)_");
        }
        if (contains(chars::control_letters, *it)) {
          consume(it);
          return true;
        } else {
          REGEX_PATERN_ERROR(R"_(`\cn`(n is not character) is not a valid escape sequence.)_");
        }
      }
      // HexEscapeSequence
      if (chars::x == c) {
        consume(it);
        if (it == fin) {
          REGEX_PATERN_ERROR(R"_(`\x` is not a valid escape sequence.)_");
        }

        // 16進エスケープシーケンス（\xhh）は2桁必要
        if (not hex_digit(it)) {
          REGEX_PATERN_ERROR(R"_(`\xn`(n is not hexadecimal number) is a hexadecimal escape sequence that is not valid.)_");
        }
        if (it == fin) {
          REGEX_PATERN_ERROR("Hexadecimal escape sequence must be two digits.");
        }

        if (not hex_digit(it)) {
          REGEX_PATERN_ERROR("Hexadecimal escape sequence must be two digits.");
        }
        return true;
      }
      // UnicodeEscapeSequence
      if (chars::u == c) {
        consume(it);
        if (it == fin) {
          REGEX_PATERN_ERROR(R"_(`\u` is not a valid escape sequence.)_");
        }

        // ユニコードエスケープシーケンスは4桁
        for (int i = 4; i --> 0;) {
          if (not hex_digit(it)) {
            REGEX_PATERN_ERROR(R"_(Unicode Escape Sequence(`\uhhhh `) requires 4 hexadecimal digits.)_");
          }
        }
        return true;
      }

      return false;
    }

    fn hex_digit(I &it) -> bool {
      if (not contains(chars::hex_digits, *it)) {
        return false;
      }

      consume(it);
      return true;
    }

    fn character_class_escape(I& it) -> bool {
      if (contains(chars::character_class_escapes, *it)) {
        consume(it);
        return true;
      }

      return false;
    }

    fn character_class(I &, const S) {
      regex_error_unimplemented();
    }

    fn lookahead_assertion_or_group(I &it, const S fin) {
      consume(it);
      if (it == fin) {
        // グループが閉じていない
        REGEX_PATERN_ERROR("The group is not closed.");
      }

      // 先読みアサーションのチェック
      if (const auto c = *it; c == chars::question) {
        consume(it);
        if (it == fin) {
          // グループが閉じていない
          REGEX_PATERN_ERROR("The group is not closed.");
        }
        const auto c2 = *it;
        if (c2 == chars::colon or c2 == chars::equal or c2 == chars::exclamation) {
          consume(it);
        } else {
          // 有効な先読みアサーションではない
          REGEX_PATERN_ERROR("You got the wrong Lookahead Assertion.");
        }
      }

      // グループとして一括処理
      disjunction(it, fin);
      if (const auto c = *it; c != chars::rparen) {
        // グループが閉じていない
        REGEX_PATERN_ERROR("The group is not closed.");
      }
      consume(it);
      return;
    }
  };

  inline namespace literals {

    [[nodiscard]]
    consteval auto operator""_re(const char* str, std::size_t len) -> const char* {
      patern_check<char>::start({str, len});
      return str;
    }

    [[nodiscard]]
    consteval auto operator""_re(const wchar_t* str, std::size_t len) -> const wchar_t* {
      patern_check<wchar_t>::start({str, len});
      return str;
    }
  }

}

#undef fn
//#undef LITERAL