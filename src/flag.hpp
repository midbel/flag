#include <string>
#include <vector>
#include <map>
#include <exception>

using namespace std;

namespace flag
{
  class flag_error: public exception
  {
  private:
    string opt;
    string msg;

  public:
    flag_error(string o, string m): opt(o), msg(m) {}
    virtual ~flag_error() {}

    virtual const char* what() const throw()
    {
      if (msg.size() == 0)
      {
        return "unexpected error";
      }
      return msg.c_str();
    }

    string invalid_option()
    {
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

  class Set
  {

  struct setting {
    string help;
    char narg;
    bool required;
  };

  private:
    string name;
    string desc;
    map<string, setting> flags;

    map<string, string> options;
    vector<string> args;

    bool is_registered(string name);
    bool is_defined(string name);

    pair<string, bool> option(string name);
    void add_option(string name, char narg, string help);

    pair<string, int> parse_option(string str);
    void parse_arguments(int i, int argc, char** argv);

  public:
    Set(string prog = "", string help = "");

    int narg();
    string arg(int i);

    string help();
    string usage();
    string program();

    void add_option(string sh, string lg = "", char narg = 0, string help = "");

    void parse(int argc, char** argv);

    void option(string name, string& value);
    void option(string name, int& value);
    void option(string name, unsigned int& value);
    void option(string name, double& value);
    void option(string name, bool& value);
  };
}
