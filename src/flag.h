#ifndef __FLAG_H__
#define __FLAG_H__

#include <string>
#include <vector>
#include <map>
#include <exception>

namespace flag {

  class flag_error: public std::exception
  {
  private:
    std::string opt;
    std::string msg;

  public:
    flag_error(std::string o, std::string m): opt(o), msg(m) {}
    virtual ~flag_error() {}

    virtual const char* what() const throw() {
      if (msg.size() == 0) {
        return "unexpected error";
      }
      return msg.c_str();
    }

    std::string invalid_option() {
      return opt;
    }
  };

  class bad_syntax: public flag_error
  {
  public:
    bad_syntax(std::string o): flag_error(o, "flag: bad syntax") {}
  };

  class not_defined: public flag_error
  {
  public:
    not_defined(std::string o): flag_error(o, "flag: not defined") {}
  };

  class already_defined: public flag_error
  {
  public:
    already_defined(std::string o): flag_error(o, "flag: already defined") {}
  };

  class missing_value: public flag_error
  {
  public:
    missing_value(std::string o): flag_error(o, "flag: value is missing") {}
  };

  class Set
  {
  enum flagtype {
    Int,
    Uint,
    Double,
    String,
    Bool,
  };

  struct option {
    std::string help;
    std::string optshort;
    std::string optlong;
    flagtype type;
    bool required;
    void* ptr;

    friend std::ostream& operator<<(std::ostream& os, const option& opt) {
      os << "[";
      if (opt.optshort.size()) {
        os << "-" << opt.optshort;
        if (opt.optlong.size()) {
          os << ",";
        }
      }
      if (opt.optlong.size()) {
        os << "--" << opt.optlong;
      }
      os << "] ";
      return os;
    }
  };

  public:
    Set(std::string prog = "", std::string help = "");
    ~Set();

    int narg();
    std::string arg(int i);
    std::vector<std::string> arguments();

    void string_var(std::string* val, std::string sh, std::string lg = "", std::string help = "", bool required = false);
    void int_var(int *val, std::string sh, std::string lg = "", std::string help = "", bool required = false);
    void uint_var(unsigned *val, std::string sh, std::string lg = "", std::string help = "", bool required = false);
    void double_var(double *val, std::string sh, std::string lg = "", std::string help = "", bool required = false);
    void bool_var(bool *val, std::string sh, std::string lg = "", std::string help = "", bool required = false);

    std::string help();
    std::string usage();
    std::string program();

    void parse(int argc, char** argv);

  private:
    std::string name;
    std::string desc;
    std::vector<option> list;
    std::map<std::string, option> flags;

    std::map<std::string, std::string> options;
    std::vector<std::string> args;

    void var(void* val, flagtype type, std::string sh, std::string lg, std::string help, bool required);
    void register_option(option opt);
    void update_option(std::string name, std::string value);
    void validate_option(std::string name, bool shortopt);
    bool is_registered(std::string name);
    bool is_defined(std::string name);
    bool is_flag(std::string name);

    std::pair<std::string, int> parse_option(std::string str);
    int parse_options(int argc, char** argv);
    void parse_arguments(int i, int argc, char** argv);
  };
}

#endif
