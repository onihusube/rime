#include "rime.hpp"
#include "boost/ut.hpp"

namespace ut = boost::ut;

int main() {
  using namespace boost::ut::literals;
  using namespace boost::ut::operators::terse;
  using namespace rime::literals;
  using namespace std::string_view_literals;

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
      auto pat = R"(^)"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"($)"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"(\)"sv;
      rime::patern_check<char>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = R"(.)"sv;
      rime::patern_check<char>::start(pat);
    }));
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
      auto pat = LR"(^)"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"($)"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(\)"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(.)"sv;
      rime::patern_check<wchar_t>::start(pat);
    }));
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

  };
}