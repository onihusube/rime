#pragma once

#include <regex>
#include <ranges>

#define fn static constexpr auto
//#define Error(msg) throw msg;

namespace rime {

  void REGEX_PATERN_ERROR(const char* str) {
    throw str;
  }

  inline namespace concepts {
    template <typename T>
    concept regex_usable_character = std::same_as<T, char> or std::same_as<T, wchar_t>;
  }

  template<typename CharT>
  struct character_constant;

  template<>
  struct character_constant<char> {
    static constexpr char backslash = '\\';
    static constexpr char lparen = '(';
    static constexpr char rparen = ')';
    static constexpr char lbrace = '{';
    static constexpr char rbrace = '}';
    static constexpr char lbracket = '[';
    static constexpr char rbracket = ']';
  };

  template<>
  struct character_constant<wchar_t> {
    static constexpr wchar_t backslash = L'\\';
    static constexpr wchar_t lparen = L'(';
    static constexpr wchar_t rparen = L')';
    static constexpr wchar_t lbrace = L'{';
    static constexpr wchar_t rbrace = L'}';
    static constexpr wchar_t lbracket = L'[';
    static constexpr wchar_t rbracket = L']';
  };

  template<regex_usable_character CharT>
  struct patern_check {

    using chars = character_constant<CharT>;

    fn start(std::basic_string_view<CharT> patern) {
      auto it = patern.begin();
      const auto fin = patern.end();

      while(it != fin) {
        const auto c = *it;

        switch (c) {
        case chars::lparen:
          // (
          inner_paren<chars::rparen>(it, fin);
          break;
        case chars::lbrace:
          // {
          inner_paren<chars::rbrace>(it, fin);
          break;
        case chars::lbracket:
          // [
          inner_paren<chars::rbracket>(it, fin);
          break;
        default:
          ++it;
          continue;
        }
      }

      // ここにきたらOK
    }

    template<CharT close_paren, std::contiguous_iterator I, std::sentinel_for<I> S>
    fn inner_paren(I& it, const S fin) {
      ++it;
      while(it != fin) {
        const auto c = *it;

        switch (c) {
        case close_paren:
          // 対応する閉じかっこを見つけた
          ++it;
          return;
        case chars::lparen:
          // (
          inner_paren<chars::rparen>(it, fin);
          break;
        case chars::lbrace:
          // {
          inner_paren<chars::rbrace>(it, fin);
          break;
        case chars::lbracket:
          // [
          inner_paren<chars::rbracket>(it, fin);
          break;
        default:
          ++it;
          continue;
        }
      }

      // ここにきてたら、かっこの対応が取れてない
      switch (close_paren) {
      case chars::rparen:
        REGEX_PATERN_ERROR("You're missing the closing parentheses.");
        break;
      case chars::rbrace:
        REGEX_PATERN_ERROR("You're missing the closing braces.");
        break;
      case chars::rbracket:
        REGEX_PATERN_ERROR("You're missing the closing brackets.");
        break;
      }

    }
  };

  inline namespace literals {

    consteval auto operator""_re(const char* str, std::size_t len) -> const char* {
      patern_check<char>::start({str, len});
      return str;
    }

    consteval auto operator""_re(const wchar_t* str, std::size_t len) -> const wchar_t* {
      patern_check<wchar_t>::start({str, len});
      return str;
    }
  }

}

#undef fn
//#undef Error