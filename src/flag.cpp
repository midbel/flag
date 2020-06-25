#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include "flag.h"

namespace flag {

  std::string clean_flag(std::string str) {
    return str.erase(0, str.find_first_not_of("-"));
  }

  Set::Set(std::string prog, std::string help): name(prog), desc(help) {}

  Set::~Set() {}

  std::string Set::usage() {
    std::ostringstream os;
    os << "usage: ";
    os << name;
    os << " ";
    for (option opt: list) {
      os << opt;
    }
    os << "<arguments...>";
    return os.str();
  }

  std::string Set::help() {
    if (desc.size() == 0) {
      return usage();
    }
    std::ostringstream os;
    os << desc << "\n\n";
    os << "options:" << "\n\n";
    for (option opt: list) {
      if (opt.optshort.size()) {
        os << std::left << std::setw(16) << "  -" + opt.optshort;
        os << opt.help << "\n";
      }
    }
    for (option opt: list) {
      if (opt.optlong.size()) {
        os << std::left << std::setw(16) << "  --" + opt.optlong;
        os << opt.help << "\n";
      }
    }
    os << "\n" << usage();
    return os.str();
  }

  std::string Set::program() {
    return name;
  }

  int Set::narg() {
    return args.size();
  }

  std::string Set::arg(int i) {
    if (i >= 0 || i < args.size()) {
      return args.at(i);
    }
    return "";
  }

  void Set::parse(int argc, char** argv) {
    if (!name.size()) {
      name = std::string(argv[0]);
    }
    int i = parse_options(argc, argv);
    parse_arguments(i, argc, argv);
  }

  int Set::parse_options(int argc, char** argv) {
    int i = 1;
    while (i < argc) {
      auto [opt, minus] = parse_option(std::string(argv[i]));
      if (minus == 0) {
        break;
      }
      std::string val = "";
      int eq = opt.find("=");
      if (eq == std::string::npos) {
        if (i+1 < argc && !is_flag(opt)) {
          val = std::string(argv[++i]);
        }
        if (val[0] == '-') {
          val = "";
          i--;
        }
      } else {
        opt = opt.substr(0, eq);
        val = opt.substr(eq+1);
      }
      update_option(opt, val);
      i++;
    }
    return i;
  }

  void Set::parse_arguments(int i, int argc, char** argv) {
    if (i < argc && std::string(argv[i]) == "--") {
      i++;
    }
    while (i < argc) {
      args.push_back(argv[i]);
      i++;
    }
  }

  std::pair<std::string, int> Set::parse_option(std::string str) {
    if (str[0] != '-') {
      return std::make_pair("", 0);
    }
    int minus = 1;
    if (str[minus] == '-') {
      minus++;
      if (str.size() == minus) {
        return std::make_pair("", 0);
      }
    }
    str = str.substr(minus);
    if (str.size() == 0 || str[0] == '-' || str[0] == '=') {
      throw bad_syntax(str);
    }
    return std::make_pair(str, minus);
  }

  bool Set::is_flag(std::string name) {
    if (is_registered(name)) {
      return flags[name].type == Bool;
    }
    return false;
  }

  bool Set::is_registered(std::string name) {
    return flags.find(name) != flags.end();
  }

  bool Set::is_defined(std::string name) {
    return options.find(name) != options.end();
  }

  void Set::update_option(std::string name, std::string value) {
    if (!is_registered(name)) {
      throw not_defined(name);
    }
    if (is_defined(name)) {
      throw already_defined(name);
    }
    option opt = flags[name];
    if (opt.type != Bool && opt.required && !value.size()) {
      throw missing_value(name);
    }
    switch (opt.type) {
      case String:
      *(std::string *)opt.ptr = value;
      break;
      case Uint:
      *(unsigned *)opt.ptr = std::stoul(value);
      break;
      case Int:
      *(int *)opt.ptr = std::stoi(value);
      break;
      case Double:
      *(double *)opt.ptr = std::stod(value);
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

  void Set::validate_option(std::string name, bool shortopt) {
    if ((shortopt && name.size() != 1) || (!shortopt && name.size() == 1)) {
      throw bad_syntax(name);
    }
    if (is_registered(name)) {
      throw already_defined(name);
    }
  }

  void Set::string_var(std::string *val, std::string sh, std::string lg, std::string help, bool required) {
    var(val, String, sh, lg, help, required);
  }

  void Set::int_var(int *val, std::string sh, std::string lg, std::string help, bool required) {
    var(val, Int, sh, lg, help, required);
  }

  void Set::uint_var(unsigned int *val, std::string sh, std::string lg, std::string help, bool required) {
    var(val, Uint, sh, lg, help, required);
  }

  void Set::double_var(double *val, std::string sh, std::string lg, std::string help, bool required) {
    var(val, Double, sh, lg, help, required);
  }

  void Set::bool_var(bool *val, std::string sh, std::string lg, std::string help, bool required) {
    var(val, Bool, sh, lg, help, required);
  }

  void Set::var(void* val, flagtype type, std::string sh, std::string lg, std::string help, bool required) {
    clean_flag(sh);
    clean_flag(lg);
    option opt{
      .help = help,
      .optshort = sh,
      .optlong = lg,
      .required = required,
      .ptr = val,
    };
    opt.type = type;
    register_option(opt);
  }
}
