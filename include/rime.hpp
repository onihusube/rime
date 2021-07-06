#pragma once

#include <regex>
#include <ranges>

#define fn static constexpr auto
//#define Error(msg) throw msg;

namespace rime {

  void REGEX_PATERN_ERROR(const char* str) {
    throw str;
  }

  void regex_error_unimplemented() {
    throw "Unimplemented...";
  }

  inline namespace concepts {
    template <typename T>
    concept regex_usable_character = std::same_as<T, char> or std::same_as<T, wchar_t>;
  }

  template<typename CharT>
  struct character_constant;

  template<>
  struct character_constant<char> {
    using T = char;

    static constexpr T backslash = '\\';
    static constexpr T lparen = '(';
    static constexpr T rparen = ')';
    static constexpr T lbrace = '{';
    static constexpr T rbrace = '}';
    static constexpr T lbracket = '[';
    static constexpr T rbracket = ']';
    static constexpr T bitwise_or = '|';
    static constexpr T question = '?';
    static constexpr T comma = ',';
    static constexpr T space = ' ';
    static constexpr T quantifier_prefix_symbols[] = R"_(*+?{)_";
    static constexpr T decimal_digits[] = "0123456789";
    static constexpr T not_pattern_characters[] = R"++(^$\.()[]}*+?{|)++";
  };

  template<>
  struct character_constant<wchar_t> {
    using T = wchar_t;

    static constexpr T backslash = L'\\';
    static constexpr T lparen = L'(';
    static constexpr T rparen = L')';
    static constexpr T lbrace = L'{';
    static constexpr T rbrace = L'}';
    static constexpr T lbracket = L'[';
    static constexpr T rbracket = L']';
    static constexpr T bitwise_or = L'|';
    static constexpr T question = L'?';
    static constexpr T comma = L',';
    static constexpr T space = L' ';
    static constexpr T quantifier_prefix_symbols[] = LR"_(*+?{)_";
    static constexpr T decimal_digits[] = L"0123456789";
    static constexpr T not_pattern_characters[] = LR"++(^$\.()[]}*+?{|)++";
  };

  template<regex_usable_character CharT>
  struct patern_check {

    using chars = character_constant<CharT>;

    using I = std::ranges::iterator_t<std::basic_string_view<CharT>>;
    using S = std::ranges::sentinel_t<std::basic_string_view<CharT>>;

    fn start(std::basic_string_view<CharT> patern) {
      auto it = patern.begin();
      const auto fin = patern.end();

      disjunction(it, fin);
      // ここにきたらOK
    }

    fn disjunction(I& it, const S fin) {
      alternative(it, fin);

      if (it == fin) return;

      if (const auto c = *it; c == chars::bitwise_or) {
        ++it;
        return disjunction(it, fin);
      } else {
        // 構文エラー？
        REGEX_PATERN_ERROR("The expected '|' did not appear.");
      }
    }

    fn alternative(I& it, const S fin) {
      while (it != fin) {
        if (const auto c = *it; c == chars::bitwise_or) return;
        term(it, fin);
      }
    }

    fn term(I& it, const S fin) {
      if (assertion(it, fin) == true) {
        regex_error_unimplemented();
      } else {
        atom(it, fin);

        if (it != fin) {
          quantifier(it, fin);
        }
      }
    }

    fn assertion(I&, const S) -> bool {
      return false;
    }

    fn quantifier(I& it, const S fin) {
      quantifier_prefix(it, fin);

      if (it != fin and *it == chars::question) {
        ++it;
      }

      return;
    }

    fn quantifier_prefix(I& it, const S fin) {
      // termの呼び出しで終端チェック済

      const std::input_or_output_iterator auto p = std::ranges::find(chars::quantifier_prefix_symbols, *it);
      const auto e = std::ranges::end(chars::quantifier_prefix_symbols);

      if (p != e) {
        // * + ? { だった時
        if (auto t = e - 1; p == t) {
          // { だった時

          bool before_digits = true;
          ++it;
          while(true) {
            if (it == end) {
              // \d{0,10}のようなかっこが閉じていない
              REGEX_PATERN_ERROR(R"_(Quantifiers braces are not closed. (Example: \d{0, 10 ) )_");
            }

            const auto c = *it;
            if (before_digits and c == chars::comma) {
              // 数字が出る前にカンマが出現
              REGEX_PATERN_ERROR(R"_(You need a number before the ',' in the Quantifiers.)_");
            }
            if (c == chars::comma or c == chars::space) {
              ++it;
              continue;
            }
            if (c == chars::rbrace) {
              ++it;
              return;
            }

            const bool is_digits = std::ranges::any_of(chars::decimal_digits, [c](auto ch) { return ch == c; });
            if (is_digits) {
              before_digits = false;
              ++it;
              continue;
            }

            // 数字でもカンマでもスペースでも閉じかっこでもない何か、エラー
            REGEX_PATERN_ERROR(R"_(Quantifiers braces are not closed. (Example: \d{0, 10 ) )_");
          }
        }
        // 消費する
        ++it;
      }
      // * + ? { 以外は消費しないで戻る
      return;
    }

    fn atom(I& it, const S) {
      return pattern_character(it);
    }

    fn pattern_character(I& it) {
      // ^ $ \ . * + ? ( ) [ ] { } | を除いた1文字
      // * + ? { | => 消費せずに戻る
      // ^ $ \ . ( ) [ ] } => エラー
      // othewise => 消費して戻る

      const std::input_or_output_iterator auto p = std::ranges::find(chars::not_pattern_characters, *it);
      const auto e = std::ranges::end(chars::not_pattern_characters);

      if (p == e) {
        // 通常のパターン文字
        ++it;
        return;
      }

      // エラーにしない文字は後ろの方で見つかるようにしてある
      if (auto t = e - 5; t <= p) {
        // * + ? { |
        return;
      } else {
        // ^ $ \ . ( ) [ ] }
        REGEX_PATERN_ERROR(R"_(These symbols need escaping (one of ^ $ \ . ( ) [ ] } ).)_");
      }
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
//#undef Error