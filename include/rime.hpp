#pragma once

#include <regex>
#include <ranges>
#include <algorithm>
#include <cassert>

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning(disable : 702)
#endif // _MSC_VER

#define fn static constexpr auto

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
  void REGEX_PATTERN_ERROR(const char* str) {
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
    static constexpr CharT hyphen = LITERAL(CharT, '-');
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
    static constexpr CArray control_escapes = LITERAL(CharT, "tnvfr");
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

  template<std::ranges::input_range R, std::equality_comparable_with<std::ranges::range_reference_t<R>> T>
  constexpr auto char_to_num(R&& r, const T& value) -> std::size_t {
    return std::ranges::distance(begin(r), std::ranges::find(r, value));
  }

  template<regex_usable_character CharT>
  struct pattern_check {

    using chars = character_constant<CharT>;

    using I = std::ranges::iterator_t<std::basic_string_view<CharT>>;
    using S = std::ranges::sentinel_t<std::basic_string_view<CharT>>;

    fn start(std::basic_string_view<CharT> pattern) {
      auto it = pattern.begin();
      const auto fin = pattern.end();

      disjunction(it, fin);

      if (it != fin) {
        REGEX_PATTERN_ERROR("Parse error.");
      }
      // ここにきたらOK
    }

    static constexpr void disjunction(I& it, const S fin) {
      // Alternative
      while (it != fin) {
        const auto c = *it;
        if (c == chars::bitwise_or or c == chars::rparen) break;

        // Term
        if (assertion(it, fin) == false) {
          atom(it, fin);

          if (it != fin) {
            quantifier(it, fin);
          }
        }
      }

      if (it == fin) return;

      if (const auto c = *it; c == chars::bitwise_or) {
        consume(it);
        return disjunction(it, fin);
      } else if (c == chars::rparen) {
        // 先読みアサーションとグループ内のパース時にdisjunctionを終了する
        return;
      } else {
        // ここくる？
        REGEX_PATTERN_ERROR("The expected '|' did not appear.");
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
            REGEX_PATTERN_ERROR("The escape sequence is empty.");
          }

          const auto c2 = *next;
          if (c2 == chars::b or c2 == chars::B) {
            // \bの次まで消費
            consume_n(it, 2);
            return true;
          }
          /*if (c2 == chars::backslash) {
            // `\\`を消費
            consume_n(it, 2);
            return true;
          }*/
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

          auto copy_it = it;
          // 前半DecimalDigits
          const auto num_of_digits_l = decimal_digits(it, fin);

          if (it == fin) {
            // \d{0,10}のようなかっこが閉じていない
            REGEX_PATTERN_ERROR(R"_(Quantifiers braces are not closed. [Example: `\d{0` ] )_");
          }
          if (*it == chars::rbrace) {
            if (num_of_digits_l == 0) {
              // 数字が現れる前に閉じている
              REGEX_PATTERN_ERROR(R"_(Quantifiers must have at least one number. [Example: `\d{}` ] )_");
            }
            // 数量詞終端
            consume(it);
            return;
          }
          if (*it != chars::comma) {
            // 数字でもカンマでも閉じかっこでもないものが現れている
            REGEX_PATTERN_ERROR(R"_(You can't use anything but numbers within Quantifiers. [Example: `\d{@}`, `a{a}`, `\d{0 }` ] )_");
          }
          if (num_of_digits_l == 0) {
            // 数字が現れる前にカンマが現れている
            REGEX_PATTERN_ERROR(R"_(Within Quantifiers, you need a digits before the ','. [Example: `\d{,2}` ] )_");
          }
          
          // 左側の数字を取得
          std::size_t l = decode_decimal_digits(copy_it, it, num_of_digits_l);
          
          // カンマを消費
          consume(it);

          copy_it = it;
          // 後半DecimalDigits
          const auto num_of_digits_r = decimal_digits(it, fin);

          if (it == fin) {
            // \d{0,10}のようなかっこが閉じていない
            REGEX_PATTERN_ERROR(R"_(Quantifiers braces are not closed. [Example: `\d{0,10` ] )_");
          }
          if (*it == chars::comma) {
            // 後半にカンマはない
            REGEX_PATTERN_ERROR(R"(A ',' can appear only once in Quantifiers. [Example: `\d{0,10,2}` ] )");
          }
          if (*it == chars::rbrace) {
            // 後半に数字がある場合、範囲チェックを行う（`a{0,}`のような場合はスキップする）
            if (it != copy_it) {
              // 右側の数字を取得
              std::size_t r = decode_decimal_digits(copy_it, it, num_of_digits_r);

              if (not (l <= r)) {
                // 数値範囲が逆転している
                REGEX_PATTERN_ERROR(R"_(Invalid range in Quantifiers. [Example: `\d{5,2}`, `a{1,0}` ])_");
              }
            }

            // 数量詞終端
            consume(it);
            return;
          }
          // それ以外の出現はエラー
          REGEX_PATTERN_ERROR(R"_(You can't use anything but numbers within Quantifiers. [Example: `\d{0, 5}`, `\d{0,@}`, `a{1,a}` ] )_");
          return;
        }
        consume(it);
      }
      // * + ? { 以外は消費しないで戻る
      return;
    }

    fn decimal_digits(I& it, const S fin) -> std::size_t {
      std::size_t count = 0;

      while (it != fin and contains(chars::decimal_digits, *it)) {
        consume(it);
        ++count;
      }

      return count;
    }

    fn decode_decimal_digits(I it, const S fin, int num_of_digits) -> std::size_t {
      std::size_t coeff = 1;
      for (int i = num_of_digits; i --> 0; ) {
        coeff *= 10;
      }

      std::size_t val = 0;

      do {
        std::size_t n = char_to_num(chars::decimal_digits, *it);
        val += n * coeff;
        coeff /= 10;
        ++it;
      } while (it != fin);

      return val;
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

    fn lookahead_assertion_or_group(I &it, const S fin) {
      consume(it);
      if (it == fin) {
        // グループが閉じていない
        REGEX_PATTERN_ERROR("The group is not closed.");
      }

      // 先読みアサーションのチェック
      if (const auto c = *it; c == chars::question) {
        consume(it);
        if (it == fin) {
          // グループが閉じていない
          REGEX_PATTERN_ERROR("The group is not closed.");
        }
        const auto c2 = *it;
        if (c2 == chars::colon or c2 == chars::equal or c2 == chars::exclamation) {
          consume(it);
        } else {
          // 有効な先読みアサーションではない
          REGEX_PATTERN_ERROR("You got the wrong Lookahead Assertion.");
        }
      }

      // グループとして一括処理
      disjunction(it, fin);
      if (const auto c = *it; c != chars::rparen) {
        // グループが閉じていない
        REGEX_PATTERN_ERROR("The group is not closed.");
      }
      consume(it);
      return;
    }

    fn pattern_character(I& it) {
      // ^ $ \ . * + ? ( ) [ ] { } | を除いた1文字
      // | ) => 消費せずに戻る
      // * + ? { => エラー（Quantifierの開始文字）
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
      if (auto t = e - 2; t <= p) {
        // | )
        return;
      }
      if (auto t = e - 6; t <= p) {
        // * + ? {
        // 単独の（Atomに続かない）Quantifier
        REGEX_PATTERN_ERROR(R"_(The Quantifier must come after some character (or character class, etc.). [Example: `*`, `+`, `?`, `{2}` ])_");
      } else {
        // ^ $ \ . ( [ ] }
        REGEX_PATTERN_ERROR(R"_(These(any of ^ $ \ . ( [ ] } ) symbols need escaping.)_");
      }
    }

    // DecimalEscapeの種類を伝える
    enum class decimal_escape_result : unsigned char {
      null_char,
      back_ref,
      reject
    };

    enum class character_escape_result : unsigned char {
      reject,
      control,
      control_letter,
      hex,
      unicode,
      identity
    };
  
    fn atom_escape(I& it, const S fin) {
      consume(it);
      if (it == fin) {
        // 孤立したバックスラッシュ（assertionでチェックしてるのでここには来ないのでは・・・？）
        REGEX_PATTERN_ERROR("The last backslash is isolated.");
      }

      if (decimal_escape(it, fin) != decimal_escape_result::reject) {
        return;
      }
      if (character_class_escape(it) == true) {
        return;
      }
      if (character_escape(it, fin) != character_escape_result::reject) {
        return;
      }

      // ここにきたらエラー？
      REGEX_PATTERN_ERROR("There's an unknown escape sequence.");
    }

    fn decimal_escape(I& it, const S fin) -> decimal_escape_result {
      if (const auto d1 = *it; d1 == chars::decimal_digits[0]) {
        // \0 の時は1桁オンリー
        consume(it);
        return decimal_escape_result::null_char;
      }
      if (0 < decimal_digits(it, fin)) {
        return decimal_escape_result::back_ref;
      }

      // 数値が現れない場合は別のエスケープ文字
      return decimal_escape_result::reject;
    }

    fn character_escape(I& it, const S fin) -> character_escape_result {
      const auto c = *it;
      // ControlEscape
      if (contains(chars::control_escapes, *it)) {
        consume(it);
        return character_escape_result::control;
      }
      // c ControlLetter
      if (c == chars::c) {
        consume(it);
        if (it == fin) {
          REGEX_PATTERN_ERROR(R"_(`\c` is not a valid escape sequence.)_");
        }
        if (contains(chars::control_letters, *it)) {
          consume(it);
          return character_escape_result::control_letter;
        } else {
          REGEX_PATTERN_ERROR(R"_(`\cn`(n is not character) is not a valid escape sequence.)_");
        }
      }
      // HexEscapeSequence
      if (c == chars::x) {
        consume(it);
        if (it == fin) {
          REGEX_PATTERN_ERROR(R"_(`\x` is not a valid escape sequence.)_");
        }

        // 16進エスケープシーケンス（\xhh）は2桁必要
        if (not hex_digit(it)) {
          REGEX_PATTERN_ERROR(R"_(`\xn`(n is not hexadecimal number) is a hexadecimal escape sequence that is not valid.)_");
        }
        if (it == fin) {
          REGEX_PATTERN_ERROR("Hexadecimal escape sequence must be two digits.");
        }

        if (not hex_digit(it)) {
          REGEX_PATTERN_ERROR("Hexadecimal escape sequence must be two digits.");
        }
        return character_escape_result::hex;
      }
      // UnicodeEscapeSequence
      if (unicode_escape_seqence(it, fin) == true) {
        return character_escape_result::unicode;
      }
      // IdentityEscape
      if (*it != chars::c) {
        consume(it);
        return character_escape_result::identity;
      }
      // ここに来ることってある？
      REGEX_PATTERN_ERROR("Unreachable");

      return character_escape_result::reject;
    }
    
    fn unicode_escape_seqence(I& it, const S fin, bool should_return = false) -> bool {
      if (*it == chars::u) {
        consume(it);
        if (it == fin) {
          if (should_return) return false;
          REGEX_PATTERN_ERROR(R"_(`\u` is not a valid escape sequence.)_");
        }

        // ユニコードエスケープシーケンスは4桁
        for (int i = 4; i --> 0;) {
          if (not hex_digit(it)) {
            if (should_return) return false;
            REGEX_PATTERN_ERROR(R"_(Unicode Escape Sequence(`\uhhhh `) requires 4 hexadecimal digits.)_");
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

    fn character_class(I& it, const S fin) {
      consume(it);
      if (it != fin and *it == chars::caret) {
        consume(it);
      }

      class_ranges(it, fin);
      consume(it);
    }

    // class_atomがどの構文をパースして帰っているのかを伝える
    enum class class_atom_result : unsigned char {
      rbracket,
      one_char,
      char_set
    };


    fn class_ranges(I& it, const S fin) {
      for (;;) {
        // 文字範囲の最初と最後の文字を数字にして保持する
        class_atom_result first_kind{}, last_kind{};
        std::size_t first = 0, last = 0;

        // NonemptyClassRanges
        if (std::tie(first_kind, first) = class_atom(it, fin); first_kind == class_atom_result::rbracket) {
          // 空の場合
          return;
        }

        if (it != fin and *it == chars::hyphen) {
          consume(it);
          if (std::tie(last_kind, last) = class_atom(it, fin); last_kind == class_atom_result::rbracket) {
            // 続くClassRangesは空
            return;
          }

          // 文字範囲の妥当性チェック
          if (first_kind == class_atom_result::one_char and last_kind == class_atom_result::one_char) {
            if (not (first <= last)) {
              REGEX_PATTERN_ERROR(R"_(Invalid range in character class. [Example: `[z-a]`, `[5-2]` ])_");
            }
            continue;
          } else {
            // 文字範囲の開始と終端はそれぞれ1文字を示すものでなければならない
            REGEX_PATTERN_ERROR(R"_(The start and end of range of character(character class) must be specified to indicate a single character. [Example: `[\w-a]`, `[\s-\d]` ])_");
          }
        }
      }
    }

    fn class_atom(I& it, const S fin) -> std::pair<class_atom_result, std::size_t> {
      if (it == fin) {
        // []が閉じていない
        REGEX_PATTERN_ERROR("The range of character(character class) is not closed.");
      }

      const auto c = *it;

      switch (c) {
      case chars::backslash:
        return class_escape(it, fin);
      case chars::rbracket:
        return {class_atom_result::rbracket, 0};
      case chars::lbracket:
        if (posix_class(it, fin) == true) {
          return {class_atom_result::char_set, 0};
        }
        [[fallthrough]];
      default:
        consume(it);
        return {class_atom_result::one_char, std::size_t(c)};
      }
    }

    fn posix_class(I& it, const S fin) -> bool {
      // バックトラックの可能性があるためコピーする
      auto it2 = it;

      consume(it2);
      if (it2 == fin) {
        // []が閉じていない
        REGEX_PATTERN_ERROR(R"_(The POSIX class is not closed. [Example: `[[` ] )_");
      }

      switch (*it2) {
      case chars::colon: [[fallthrough]];
      case chars::equal: [[fallthrough]];
      case chars::dot:
      {
        // POSIXクラス
        consume_n(it, 2);
        if (it == fin) {
          // []が閉じていない
          REGEX_PATTERN_ERROR(R"_(The POSIX class is not closed. [Example: `[[:` ] )_");
        }

        // 導入子（: = .）を保存
        const auto introducer = *it2;

        class_name(it, fin);
        if (it == fin) {
          // []が閉じていない
          REGEX_PATTERN_ERROR(R"_(The POSIX class is not closed. [Example: `[[:digit` ] )_");
        }

        // POSIXクラス導入と終了が一貫していることをチェック
        if (*it == introducer) {
          // : = .のいずれかを消費
          consume(it);
        } else {
          REGEX_PATTERN_ERROR(R"_(POSIX class introduction characters are inconsistent. [Example: `[:digit=]`, `[=upper.]`, `[.space:]` ] )_");
        }

        // ]で閉じているはず
        if (it == fin or *it != chars::rbracket) {
          REGEX_PATTERN_ERROR(R"_(The POSIX class is not closed. [Example: `[[:digit:` `[[:digit:}]` ] )_");
        }

        // 1つのPOSIXクラスを終了（]を消費）して戻る
        consume(it);
        return true;
      }
      default:
        //`[abc[def]`みたいのは有効
        return false;
      }
    }

    fn class_name(I& it, const S fin) {
      do {
       const auto c = *it;
       if (c == chars::colon or c == chars::dot or c == chars::equal) {
         return;
       }
       consume(it);
      } while (it != fin);
    }
  
    fn class_escape(I& it, const S fin) -> std::pair<class_atom_result, std::size_t>  {
      // バックスラッシュを消費
      consume(it);
      if (it == fin) {
        // []が閉じていない
        REGEX_PATTERN_ERROR("The range of character(character class) is not closed.");
      }

      if (*it == chars::b) {
        consume(it);
        return {class_atom_result::char_set, 0};
      }
      if (const auto dec_escape_kind = decimal_escape(it, fin); dec_escape_kind != decimal_escape_result::reject) {
        if (dec_escape_kind == decimal_escape_result::null_char) {
          // ClassRangeの中のDecimalEscapeは\0のみ
          return {class_atom_result::one_char, 0};
        }
        // DecimalEscapeの後方参照は禁止
        REGEX_PATTERN_ERROR("You cannot refer to a capture group in [].");
      }
      if (character_class_escape(it) == true) {
        return {class_atom_result::char_set, 0};
      }

      // デコードに使用するためにコピーしておく
      // エラーになっていなければ、バックスラッシュの次の文字を指している
      auto it2 = it;
      if (const auto esc_kind = character_escape(it, fin); esc_kind != character_escape_result::reject) {
        switch (esc_kind) {
        case character_escape_result::control :
          // \f \n \r \t \v
          return {class_atom_result::one_char, decode_control(*it2)};
        case character_escape_result::control_letter :
          // \c の後にアルファベット1文字
          // キャレット記法によってASCII制御文字にマッチする
          consume(it2);
          return {class_atom_result::one_char, decode_caret_notation(*it2)};
        case character_escape_result::hex :
          // 16進エスケープ、\xhh
          consume(it2);
          return {class_atom_result::one_char, decode_hex_digits(it2, it, false)};
        case character_escape_result::unicode :
          // ユニコードエスケープ、\uhhhh
          consume(it2);
          return {class_atom_result::one_char, decode_hex_digits(it2, it, true)};
        case character_escape_result::identity :
          // バックスラッシュの後に任意の1文字
          return {class_atom_result::one_char, std::size_t(*it2)};
        default:
          return {class_atom_result::char_set, 0};
        }
      }

      // ここにきたらエラー？
      REGEX_PATTERN_ERROR("There's an unknown escape sequence.");
    }

    fn decode_control(CharT c) -> std::size_t {
      // control_escapesでの位置を求める
      // tnvfrの順で並んでいる
      // 先頭からの距離を求める
      std::size_t n = char_to_num(chars::control_escapes, c);

      // tnvfrの順でAsciiコードとしても連続して並んでいて
      // \t = 9なので、先頭からの相対位置に9を足す
      return std::size_t(n) + 9u;
    }

    fn decode_caret_notation(CharT c) -> std::size_t {
      // chars::control_lettersでの位置を求め先頭からの距離を求める
      std::size_t n = char_to_num(chars::control_letters, c);

      // 大文字なら25を引く事で小文字と一貫させる
      if (std::cmp_less(25, n)) n -= 25;

      // 先頭からの相対位置（0始まり）が制御文字のコードに対応する
      // 0始まりなので+1する
      return std::size_t(n) + 1u;
    }

    fn decode_hex_digits(I it, const S fin, bool is_unicode_escs) -> std::size_t {
      std::size_t coeff = 16 * 16;
      if (is_unicode_escs) {
        // ユニコードエスケープシーケンスは4桁
        coeff *= 16 * 16;
      }

      std::size_t val = 0;

      do {
        // chars::hex_digitsでの位置を求める
        std::size_t n = char_to_num(chars::hex_digits, *it);

        // 大文字なら6を引く事で小文字と一貫させる
        if (16 <= n) n -= 6;

        val += n * coeff;
        coeff /= 16;
        ++it;
      } while (it != fin);

      return val;
    }
  };

  inline namespace literals {

    [[nodiscard]]
    consteval auto operator""_re(const char* str, std::size_t len) -> const char* {
      pattern_check<char>::start({str, len});
      return str;
    }

    [[nodiscard]]
    consteval auto operator""_re(const wchar_t* str, std::size_t len) -> const wchar_t* {
      pattern_check<wchar_t>::start({str, len});
      return str;
    }
  }

  namespace detail {
    template<typename CharT>
    struct regex_pattern_str {
      std::basic_string_view<CharT> str;

      template<typename T>
        requires std::convertible_to<const T&, std::basic_string_view<CharT>>
      consteval regex_pattern_str(const T& s)
        : str(s)
      {
        pattern_check<CharT>::start(this->str);
      }
    };
  }

  [[nodiscard]]
  inline auto regex(detail::regex_pattern_str<char> pattern) -> std::regex {
    return std::basic_regex<char>(pattern.str.data());
  }

  [[nodiscard]]
  inline auto regex(detail::regex_pattern_str<wchar_t> pattern) -> std::wregex {
    return std::basic_regex<wchar_t>(pattern.str.data());
  }

  template<regex_usable_character CharT>
  [[nodiscard]]
  std::ranges::forward_range auto regex_searches(std::basic_string_view<std::type_identity_t<CharT>> input_str, const std::basic_regex<CharT>& re){
    using namespace std::ranges;
    using reiter_t = std::regex_iterator<iterator_t<decltype(input_str)>>;

    return subrange{reiter_t{begin(input_str), end(input_str), re}, reiter_t{}};
  }
}

namespace rime::ranges {

  template<std::ranges::view V, typename CharT = char>
  class regex_filter_view : public std::ranges::view_interface<regex_filter_view<V, CharT>> {
    V m_view;
    std::basic_regex<CharT> m_regex;

    regex_filter_view(V view, std::basic_regex<CharT> regex)
      : m_view(std::move(view))
      , m_regex(std::move(regex))
    {}


  };

  template<typename R, typename CharT>
  regex_filter_view(R&&, std::basic_regex<CharT>&&) -> regex_filter_view<std::views::all_t<R>, CharT>;



  namespace views {

  }
}

#undef fn
#ifndef RIME_TEST
#undef LITERAL
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif // _MSC_VER