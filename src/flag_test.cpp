#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "flag.h"

TEST_CASE("parse args", "basic") {
  std::vector<std::string> args = {"flagtest", "-s", "string", "--uint", "1234", "--int", "5432", "-b", "--", "golang", "c++"};
  int argc = args.size();
  char* argv[argc];
  for (int i = 0; i < argc; i++) {
    argv[i] = const_cast<char*>(args[i].c_str());
  }

  std::string str = "";
  unsigned pos = 0;
  int neg = 0;
  bool boolean = false;

  flag::Set set{"flag_test", "run tests"};

  set.string_var(&str, "s", "string");
  set.uint_var(&pos, "u", "uint");
  set.int_var(&neg, "n", "int");
  set.bool_var(&boolean, "b", "boolean");

  REQUIRE_NOTHROW(set.parse(argc, argv));

  CHECK(str == "string");
  CHECK(pos == 1234);
  CHECK(neg == 5432);
  CHECK(boolean == true);

  CHECK(set.narg() == 2);
  CHECK(set.arg(0) == "golang");
  CHECK(set.arg(1) == "c++");
}
