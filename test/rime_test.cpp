#include "rime.hpp"
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
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"_())_"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"([)"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"(])"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"(})"sv;
      rime::patern_check<char>::start(pat);
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
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"_())_"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"([)"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(])"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(})"sv;
      rime::patern_check<wchar_t>::start(pat);
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
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(\)"sv;
      rime::patern_check<wchar_t>::start(pat);
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
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{   })"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{,})"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{ ,  })"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{)"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{  )"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{a})"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{12n})"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, n})"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, 2, 3})"sv;
        rime::patern_check<char>::start(pat);
      }));
      ut::expect(ut::throws([]{ 
        auto pat = R"(a{1, 2)"sv;
        rime::patern_check<char>::start(pat);
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
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\123))"sv;
      rime::patern_check<char>::start(pat);
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
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((?1))"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"((?*))"sv;
      rime::patern_check<char>::start(pat);
    }));
  };
}