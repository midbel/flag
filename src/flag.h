#ifndef __FLAG_H__
#define __FLAG_H__

#include <string>
#include <vector>
#include <map>
#include <exception>

using namespace std;

namespace flag {

  class flag_error: public exception
  {
  private:
    string opt;
    string msg;

  public:
    flag_error(string o, string m): opt(o), msg(m) {}
    virtual ~flag_error() {}

    virtual const char* what() const throw() {
      if (msg.size() == 0) {
        return "unexpected error";
      }
      return msg.c_str();
    }

    string invalid_option() {
      return opt;
    }
  };

  class bad_syntax: public flag_error
  {
  public:
    bad_syntax(string o): flag_error(o, "flag: bad syntax") {}
  };

  class not_defined: public flag_error
  {
  public:
    not_defined(string o): flag_error(o, "flag: not defined") {}
  };

  class already_defined: public flag_error
  {
  public:
    already_defined(string o): flag_error(o, "flag: already defined") {}
  };

  class missing_value: public flag_error
  {
  public:
    missing_value(string o): flag_error(o, "flag: value is missing") {}
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
    string help;
    string optshort;
    string optlong;
    flagtype type;
    bool required;
    void* ptr;

    friend ostream& operator<<(ostream& os, const option& opt) {
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

  private:
    string name;
    string desc;
    vector<option> list;
    map<string, option> flags;

    map<string, string> options;
    vector<string> args;

    void var(void* val, flagtype type, string sh, string lg, string help, bool required);
    void register_option(option opt);
    void update_option(string name, string value);
    void validate_option(string name, bool shortopt);
    bool is_registered(string name);
    bool is_defined(string name);
    bool is_flag(string name);

    pair<string, int> parse_option(string str);
    int parse_options(int argc, char** argv);
    void parse_arguments(int i, int argc, char** argv);

  public:
    Set(string prog = "", string help = "");
    ~Set();

    int narg();
    string arg(int i);

    void string_var(string* val, string sh, string lg = "", string help = "", bool required = false);
    void int_var(int *val, string sh, string lg = "", string help = "", bool required = false);
    void uint_var(unsigned int *val, string sh, string lg = "", string help = "", bool required = false);
    void double_var(double *val, string sh, string lg = "", string help = "", bool required = false);
    void bool_var(bool *val, string sh, string lg = "", string help = "", bool required = false);

    string help();
    string usage();
    string program();

    void parse(int argc, char** argv);
  };
}

#endif
