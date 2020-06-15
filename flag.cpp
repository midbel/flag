#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <string>
#include <exception>

using namespace std;
namespace fs = filesystem;

namespace flag {

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

  struct flag {
    string help;
    char narg;
    bool required;
  };

  private:
    string name;
    string desc;
    map<string, flag> flags;

    map<string, string> options;
    vector<string> args;

    bool is_registered(string name)
    {
      return flags.find(name) != flags.end();
    }

    bool is_defined(string name)
    {
      return options.find(name) != options.end();
    }

    pair<string, bool> option(string name)
    {
      bool ok = false;
      string value = "";
      if (options.count(name) > 0)
      {
        ok = true;
        value = options[name];
      }
      return make_pair(value, ok);
    }

    void add_option(string name, char narg, string help)
    {
      if (name.size() == 0)
      {
        return;
      }
      if (is_registered(name))
      {
        throw already_defined(name);
      }
      flags[name] = {
        .help = help,
        .narg = narg,
      };
    }

    pair<string, int> parse_option(string str)
    {
      if (str[0] != '-')
      {
        return make_pair("", 0);
      }
      int minus = 1;
      if (str[minus] == '-')
      {
        minus++;
        if (str.size() == minus)
        {
          return make_pair("", 0);
        }
      }
      str = str.substr(minus);
      if (str.size() == 0 || str[0] == '-' || str[0] == '=')
      {
        throw bad_syntax(str);
      }
      return make_pair(str, minus);
    }

    void parse_arguments(int i, int argc, char** argv)
    {
      if (i < argc && string(argv[i]) == "--")
      {
        i++;
      }
      while (i < argc)
      {
        args.push_back(argv[i]);
        i++;
      }
    }

  public:
    Set(string prog = "", string help = ""): name(prog), desc(help) {}

    int narg()
    {
      return args.size();
    }

    string arg(int i)
    {
      if (i >= 0 || i < args.size())
      {
        return args.at(i);
      }
      return "";
    }

    string help();
    string usage();
    string program()
    {
      return name;
    }

    void add_option(string sh, string lg = "", char narg = 0, string help = "")
    {
      add_option(sh, narg, help);
      add_option(lg, narg, help);
    }

    void parse(int argc, char** argv);

    void option(string name, string& value);
    void option(string name, int& value);
    void option(string name, unsigned int& value);
    void option(string name, double& value);
    void option(string name, bool& value);
  };

  string Set::usage()
  {
    return name;
  }

  string Set::help()
  {
    if (desc.size() == 0) {
      return usage();
    }
    return desc;
  }

  void Set::parse(int argc, char** argv)
  {
    if (name != "") {
      name = string(argv[0]);
    }
    int i = 1;
    while (i < argc)
    {
      auto [opt, minus] = parse_option(string(argv[i]));
      if (minus == 0)
      {
        break;
      }
      string val = "";
      bool done = false;
      int eq = opt.find("=");
      if (eq == string::npos && minus == 1 && opt.size() > 1)
      {
        throw bad_syntax(opt);
      }
      if (eq == string::npos)
      {
        val = string(argv[++i]);
        if (val[0] == '-')
        {
          done = true;
          val = "";
          i--;
        }
      }
      else
      {
        opt = opt.substr(0, eq);
        val = opt.substr(eq+1);
        done = true;
      }
      if (!is_registered(opt))
      {
        throw not_defined(opt);
      }
      if (is_defined(opt))
      {
        throw already_defined(opt);
      }
      options[opt] = val;
      i++;
    }
    parse_arguments(i, argc, argv);
  }

  void Set::option(string name, string& value)
  {
    auto [val, ok] = option(name);
    if (ok)
    {
      value = val;
    }
  }

  void Set::option(string name, int& value)
  {
    auto [val, ok] = option(name);
    if (ok)
    {
      value = stoi(val);
    }
  }

  void Set::option(string name, unsigned int& value)
  {
    auto [val, ok] = option(name);
    if (ok)
    {
      value = stoul(val);
    }
  }

  void Set::option(string name, double& value)
  {
    auto [val, ok] = option(name);
    if (ok)
    {
      value = stod(val);
    }
  }

  void Set::option(string name, bool& value)
  {
    auto [val, ok] = option(name);
    if (ok)
    {
      value = val == "" || val == "1";
    }
  }
}

void print_args(flag::Set &set)
{
  for (int i = 0; i < set.narg(); i++)
  {
    cout << "arg: " << set.arg(i) << endl;
  }
}

int main(int argc, char** argv)
{
  cout << "Hello world!" << endl;
  cout << fs::current_path() << endl;
  cout << "------" << endl;
  flag::Set set;
  try
  {
    set.add_option("d");
    set.add_option("e");
    set.add_option("w");
    set.add_option("j");
    set.add_option("", "long");

    set.parse(argc, argv);
    print_args(set);
  }
  catch (flag::flag_error& e)
  {
    cerr << e.what() << ": " << e.invalid_option() << endl;
    cerr << set.usage() << endl;
  }
  catch(...)
  {
    cerr << "unexpected error" << endl;
  }

}
