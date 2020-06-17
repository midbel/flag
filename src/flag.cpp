#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "flag.hpp"

using namespace std;

namespace flag {

  string clean_flag(string str) {
    return str.erase(0, str.find_first_not_of("-"));
  }

  Set::Set(string prog, string help): name(prog), desc(help) {}

  Set::~Set() {}

  string Set::usage() {
    ostringstream os;
    os << "usage: ";
    os << name;
    os << " ";
    for (option opt: list) {
      os << opt;
    }
    os << "<arguments...>";
    return os.str();
  }

  string Set::help() {
    if (desc.size() == 0) {
      return usage();
    }
    ostringstream os;
    os << desc << "\n\n";
    os << "options:" << "\n\n";
    for (option opt: list) {
      if (opt.optshort.size()) {
        os << left << setw(16) << "  -" + opt.optshort;
        os << opt.help << "\n";
      }
    }
    for (option opt: list) {
      if (opt.optlong.size()) {
        os << left << setw(16) << "  --" + opt.optlong;
        os << opt.help << "\n";
      }
    }
    os << "\n" << usage();
    return os.str();
  }

  string Set::program() {
    return name;
  }

  int Set::narg() {
    return args.size();
  }

  string Set::arg(int i) {
    if (i >= 0 || i < args.size()) {
      return args.at(i);
    }
    return "";
  }

  void Set::parse(int argc, char** argv) {
    if (!name.size()) {
      name = string(argv[0]);
    }
    int i = parse_options(argc, argv);
    parse_arguments(i, argc, argv);
  }

  int Set::parse_options(int argc, char** argv) {
    int i = 1;
    while (i < argc) {
      auto [opt, minus] = parse_option(string(argv[i]));
      if (minus == 0) {
        break;
      }
      string val = "";
      bool done = false;
      int eq = opt.find("=");
      if (eq == string::npos) {
        val = string(argv[++i]);
        if (val[0] == '-') {
          done = true;
          val = "";
          i--;
        }
      } else {
        opt = opt.substr(0, eq);
        val = opt.substr(eq+1);
        done = true;
      }
      update_option(opt, val);
      i++;
    }
    return i;
  }

  void Set::parse_arguments(int i, int argc, char** argv) {
    if (i < argc && string(argv[i]) == "--") {
      i++;
    }
    while (i < argc) {
      args.push_back(argv[i]);
      i++;
    }
  }

  pair<string, int> Set::parse_option(string str) {
    if (str[0] != '-') {
      return make_pair("", 0);
    }
    int minus = 1;
    if (str[minus] == '-') {
      minus++;
      if (str.size() == minus) {
        return make_pair("", 0);
      }
    }
    str = str.substr(minus);
    if (str.size() == 0 || str[0] == '-' || str[0] == '=') {
      throw bad_syntax(str);
    }
    return make_pair(str, minus);
  }

  bool Set::is_registered(string name) {
    return flags.find(name) != flags.end();
  }

  bool Set::is_defined(string name) {
    return options.find(name) != options.end();
  }

  void Set::update_option(string name, string value) {
    if (!is_registered(name)) {
      throw not_defined(name);
    }
    if (is_defined(name)) {
      throw already_defined(name);
    }
    option opt = flags[name];
    switch (opt.type) {
      case String:
      *(string *)opt.ptr = value;
      break;
      case Uint:
      *(unsigned int *)opt.ptr = stoul(value);
      break;
      case Int:
      *(int *)opt.ptr = stoi(value);
      break;
      case Double:
      *(double *)opt.ptr = stod(value);
      break;
      case Bool:
      *(bool *)opt.ptr = value == "" || value == "1";
      break;
    }
    options[name] = value;
  }

  void Set::register_option(option opt) {
    if (opt.optshort == "" && opt.optlong == "") {
      throw bad_syntax("<empty>");
    }
    if (opt.optshort != "") {
      validate_option(opt.optshort, true);
      flags[opt.optshort] = opt;
    }
    if (opt.optlong != "") {
      validate_option(opt.optlong, false);
      flags[opt.optlong] = opt;
    }
    list.push_back(opt);
  }

  void Set::validate_option(string name, bool shortopt) {
    if ((shortopt && name.size() != 1) || (!shortopt && name.size() == 1)) {
      throw bad_syntax(name);
    }
    if (is_registered(name)) {
      throw already_defined(name);
    }
  }

  void Set::string_var(string *val, string sh, string lg, string help) {
    var(val, String, sh, lg, help);
  }

  void Set::int_var(int *val, string sh, string lg, string help) {
    var(val, Int, sh, lg, help);
  }

  void Set::uint_var(unsigned int *val, string sh, string lg, string help) {
    var(val, Uint, sh, lg, help);
  }

  void Set::double_var(double *val, string sh, string lg, string help) {
    var(val, Double, sh, lg, help);
  }

  void Set::bool_var(bool *val, string sh, string lg, string help) {
    var(val, Bool, sh, lg, help);
  }

  void Set::var(void* val, flagtype type, string sh, string lg, string help) {
    clean_flag(sh);
    clean_flag(lg);
    option opt{
      .help = help,
      .optshort = sh,
      .optlong = lg,
      .ptr = val,
    };
    opt.type = type;
    register_option(opt);
  }
}
