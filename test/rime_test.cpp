#include <concepts>

#define RIME_TEST 1
#include "rime.hpp"

#define BOOST_UT_DISABLE_MODULE
#include "boost/ut.hpp"

namespace ut = boost::ut;

int main() {
  using namespace boost::ut::literals;
  using namespace boost::ut::operators::terse;
  using namespace rime::literals;
  using namespace std::string_view_literals;

  "select literal"_test = [] {
    ut::expect(LITERAL(char, 'a') == 'a');
    ut::expect(LITERAL(char, '0') == '0');
    ut::expect(LITERAL(char, ' ') == ' ');
    ut::expect(LITERAL(char, '\\') == '\\');

    ut::expect(LITERAL(wchar_t, 'a') == L'a');
    ut::expect(LITERAL(wchar_t, '0') == L'0');
    ut::expect(LITERAL(wchar_t, ' ') == L' ');
    ut::expect(LITERAL(wchar_t, '\\') == L'\\');

    ut::expect(LITERAL(char, "abcdefghijklmnopqrstuvwxyz") == "abcdefghijklmnopqrstuvwxyz"sv);
    ut::expect(LITERAL(char, "0123456789") == "0123456789"sv);
    ut::expect(LITERAL(char, R"_(^$\.()[]}*+?{|)_") == R"_(^$\.()[]}*+?{|)_"sv);

    ut::expect(LITERAL(wchar_t, "abcdefghijklmnopqrstuvwxyz") == L"abcdefghijklmnopqrstuvwxyz"sv);
    ut::expect(LITERAL(wchar_t, "0123456789") == L"0123456789"sv);
    ut::expect(LITERAL(wchar_t, R"_(^$\.()[]}*+?{|)_") == LR"_(^$\.()[]}*+?{|)_"sv);
  };

  "pattern character"_test = [] {
    // コンパイルが通っていればOK
    [[maybe_unused]]
    auto rstr1 = R"(abcdefghijklmnopqrstuvwxyz0123456789)"_re;
    [[maybe_unused]]
    auto rstr2 = R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ)"_re;
    [[maybe_unused]]
    auto rstr3 = R"_(<>,_/`@~=-!"#%&')_"_re;

    // パースエラーで例外を投げる
    ut::expect(ut::throws([]{ 
      auto pat = R"_(()_"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"_())_"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([)"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"(])"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"(})"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "pattern character wide"_test = [] {
    // コンパイルが通っていればOK
    [[maybe_unused]]
    auto rstr1 = LR"(abcdefghijklmnopqrstuvwxyz0123456789)"_re;
    [[maybe_unused]]
    auto rstr2 = LR"(ABCDEFGHIJKLMNOPQRSTUVWXYZ)"_re;
    [[maybe_unused]]
    auto rstr3 = LR"_(<>,_/`@~=-!"#%&')_"_re;

    // パースエラーで例外を投げる
    ut::expect(ut::throws([]{ 
      auto pat = LR"_(()_"sv;
      rime::pattern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"_())_"sv;
      rime::pattern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"([)"sv;
      rime::pattern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(])"sv;
      rime::pattern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(})"sv;
      rime::pattern_check<wchar_t>::start(pat);
    }));
  };

  "disjunction"_test = []{
    [[maybe_unused]]
    auto rstr1 = R"(a|b)"_re;
    [[maybe_unused]]
    auto rstr2 = LR"(a|b)"_re;
    [[maybe_unused]]
    auto rstr3 = R"(a|b|c)"_re;
    [[maybe_unused]]
    auto rstr4 = LR"(a|b|c)"_re;
  };

  "alternative"_test = []{
    [[maybe_unused]]
    auto rstr1 = R"()"_re;
    [[maybe_unused]]
    auto rstr2 = LR"()"_re;
  };

  "assertion"_test = []{
    [[maybe_unused]]
    auto rstr1 = R"(^)"_re;
    [[maybe_unused]]
    auto rstr2 = R"($)"_re;
    [[maybe_unused]]
    auto rstr3 = R"(\b)"_re;
    [[maybe_unused]]
    auto rstr4 = R"(\B)"_re;
    [[maybe_unused]]
    auto rstr5 = R"(^abc213$)"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"(\)"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(\)"sv;
      rime::pattern_check<wchar_t>::start(pat);
    }));
  };

  "quantifier(prefix)"_test = [] {
    {
      [[maybe_unused]]
      auto rstr1 = R"(a+)"_re;
      [[maybe_unused]]
      auto rstr2 = R"(a*)"_re;
      [[maybe_unused]]
      auto rstr3 = R"(a?)"_re;
      [[maybe_unused]]
      auto rstr4 = R"(a+?)"_re;
      [[maybe_unused]]
      auto rstr5 = R"(a*?)"_re;
    }
    {
      [[maybe_unused]]
      auto rstr1 = LR"(a+)"_re;
      [[maybe_unused]]
      auto rstr2 = LR"(a*)"_re;
      [[maybe_unused]]
      auto rstr3 = LR"(a?)"_re;
      [[maybe_unused]]
      auto rstr4 = LR"(a+?)"_re;
      [[maybe_unused]]
      auto rstr5 = LR"(a*?)"_re;
    }
    {
      [[maybe_unused]]
      auto rstr1 = R"(a{2})"_re;
      [[maybe_unused]]
      auto rstr2 = R"(a{2, 3})"_re;
      [[maybe_unused]]
      auto rstr3 = R"(a{     2     })"_re;
      [[maybe_unused]]
      auto rstr4 = R"(a{2, })"_re;
      [[maybe_unused]]
      auto rstr5 = R"(a{ 2 }?)"_re;
      [[maybe_unused]]
      auto rstr6 = R"(a{1234567890})"_re;
      [[maybe_unused]]
      auto rstr7 = R"(a{0})"_re;
      [[maybe_unused]]
      auto rstr8 = R"(a{1234567890, 1234567890})"_re;

      ut::expect(ut::throws([]{ 
        auto pat = R"(a{})"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{   })"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{,})"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{ ,  })"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{)"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{  )"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{a})"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{12n})"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, n})"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, 2, 3})"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, 2)"sv;
        rime::pattern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, 2\d+?)"sv;
        rime::pattern_check<char>::start(pat);
      }));
    }
  };

  "decimal escape"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_(\0)_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_(\1)_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_(\9)_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_(\10)_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_(\99)_"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"((\00))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\123))"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "character class escape"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_(\d)_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_(\D)_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_(\s)_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_(\S)_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_(\w)_"_re;
    [[maybe_unused]]
    auto rstr6 = R"_(\W)_"_re;
    [[maybe_unused]]
    auto rstr7 = R"_(\f)_"_re;
    [[maybe_unused]]
    auto rstr8 = R"_(\n)_"_re;
    [[maybe_unused]]
    auto rstr9 = R"_(\r)_"_re;
    [[maybe_unused]]
    auto rstr10 = R"_(\t)_"_re;
    [[maybe_unused]]
    auto rstr11 = R"_(\v)_"_re;
  };
  "HexEscapeSequence"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_(\x00)_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_(\xff)_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_(\xFF)_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_(\xf9)_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_(\xfff)_"_re;
    [[maybe_unused]]
    auto rstr6 = R"_(\x000)_"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"((\x0))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\x9))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\xa))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\xf))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\xF))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\xA))"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "unicode escape sequence"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_(\u0000)_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_(\uffff)_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_(\uFFFF)_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_(\uabCd)_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_(\uf382)_"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"((\u))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\u0))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\u0f))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\u0f2))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\uFED))"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "character escape"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_(\f\n\r\t\v)_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_(\cA\cZ\ca\cA\z)_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_(\!\"\#\^\$\%\&\'\-\^\=\~\|\@\;\:\*\+\,\.\/\?\_\<\>)_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_(\\abcdef\\\\\\)_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_(\{\(\[)_"_re;
    [[maybe_unused]]
    auto rstr6 = R"_(\}\)\])_"_re;

    ut::expect(ut::throws([] {
      auto pat = R"((\c))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([] {
      auto pat = R"(([\c]))"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "lookahead assertion or group"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_(())_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_((?:))_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_((?=))_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_((?!))_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_((a|b))_"_re;
    [[maybe_unused]]
    auto rstr6 = R"_((|))_"_re;
    [[maybe_unused]]
    auto rstr7 = R"_((a|(c|d)))_"_re;
    [[maybe_unused]]
    auto rstr8 = R"_((?:abcd))_"_re;
    [[maybe_unused]]
    auto rstr9 = R"_((?=0))_"_re;
    [[maybe_unused]]
    auto rstr10 = R"_((?!a1gf3))_"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"((?a))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((?1))"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((?*))"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "character class"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_([])_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_([a])_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_([a-z])_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_([lmn])_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_([^a])_"_re;
    [[maybe_unused]]
    auto rstr6 = R"_([^a-z])_"_re;
    [[maybe_unused]]
    auto rstr7 = R"_([^lmn])_"_re;
    [[maybe_unused]]
    auto rstr8 = R"_([a-zA-H1-9])_"_re;
    [[maybe_unused]]
    auto rstr9 = R"_([\w-])_"_re;
    [[maybe_unused]]
    auto rstr10 = R"_([^?!])_"_re;
    [[maybe_unused]]
    auto rstr11 = R"_([^])_"_re;
    [[maybe_unused]]
    auto rstr12 = R"_([--])_"_re;
    [[maybe_unused]]
    auto rstr13 = R"_([-------])_"_re;
    [[maybe_unused]]
    auto rstr14 = R"_([-abc])_"_re;
    [[maybe_unused]]
    auto rstr15 = R"_([-\w+])_"_re;
    [[maybe_unused]]
    auto rstr16 = R"_([\w\b\0\12\f\n\r\t\v\d\D\s\S\w\W])_"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"([)"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([a-b)"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([a-b0-2A-G)"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

  "POSIX class"_test = [] {
    [[maybe_unused]]
    auto rstr1 = R"_([[:digit:]])_"_re;
    [[maybe_unused]]
    auto rstr2 = R"_([^[:digit:]])_"_re;
    [[maybe_unused]]
    auto rstr3 = R"_([[:space:]])_"_re;
    [[maybe_unused]]
    auto rstr4 = R"_([^[:space:]])_"_re;
    [[maybe_unused]]
    auto rstr5 = R"_([_[:alnum:]])_"_re;
    [[maybe_unused]]
    auto rstr6 = R"_([^_[:alnum:]])_"_re;
    [[maybe_unused]]
    auto rstr7 = R"_([abcdde[:digit:]])_"_re;
    [[maybe_unused]]
    auto rstr8 = R"_([[:digit:]abdsafjk213])_"_re;
    //[[maybe_unused]]
    //auto rstr7 = R"_([abc[def]])_"_re;

    ut::expect(ut::throws([]{ 
      auto pat = R"([a[:digit:]a])"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([[:])"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([[:digit)"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([[:digit:)"sv;
      rime::pattern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([[:digit:])"sv;
      rime::pattern_check<char>::start(pat);
    }));
  };

#ifndef _MSC_VER
  "regex_searches"_test = [] {
    const auto regex = rime::regex(R"(\d+)");
    std::string_view expects[] = {"1421", "34353", "7685", "12765", "976754"};

    int i = 0;
    for (const auto &m : rime::regex_searches("1421, 34353, 7685, 12765, 976754", regex)) {
      ut::expect(m.str() == expects[i]) << i;
      ++i;
    }

    {
      [[maybe_unused]]
      std::ranges::forward_range auto r1 = rime::regex_searches("1421, 34353, 7685, 12765, 976754", regex);
      [[maybe_unused]]
      std::ranges::viewable_range auto r2 = rime::regex_searches("1421, 34353, 7685, 12765, 976754", regex);
    }
  };
#endif // !_MSC_VER
}