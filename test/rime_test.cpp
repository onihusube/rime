#include "rime.hpp"
#include "boost/ut.hpp"

namespace ut = boost::ut;

int main() {
  using namespace boost::ut::literals;
  using namespace boost::ut::operators::terse;
  using namespace rime::literals;
  using namespace std::string_view_literals;

  "parnt correspondence"_test = []{
    std::regex re{R"(\d{2})"_re};

    ut::expect(std::regex_match("33", re));
    ut::expect(ut::throws([]{ 
      auto pat = R"(\d{2)"sv;
      rime::patern_check<char>::start(pat); // 例外を投げる
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"(\d{2)"sv;
      rime::patern_check<wchar_t>::start(pat); // 例外を投げる
    }));
  };
  
  "parnt correspondence"_test = []{
    std::regex re{R"((\d{2}))"_re};

    ut::expect(std::regex_match("33", re));
    ut::expect(ut::throws([]{ 
      auto pat = R"((\d{2})"sv;
      rime::patern_check<char>::start(pat); // 例外を投げる
    }));
    ut::expect(ut::throws([]{ 
      auto pat = LR"((\d{2})"sv;
      rime::patern_check<wchar_t>::start(pat); // 例外を投げる
    }));
  };
}