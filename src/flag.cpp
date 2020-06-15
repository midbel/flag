#include <iostream>
#include <string>

#include "flag.hpp"

using namespace std;

namespace flag {

  Set::Set(string prog, string help): name(prog), desc(help) {}

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

  string Set::program()
  {
    return name;
  }

  int Set::narg()
  {
    return args.size();
  }

  string Set::arg(int i)
  {
    if (i >= 0 || i < args.size())
    {
      return args.at(i);
    }
    return "";
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

  pair<string, int> Set::parse_option(string str)
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

  void Set::parse_arguments(int i, int argc, char** argv)
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

  bool Set::is_registered(string name)
  {
    return flags.find(name) != flags.end();
  }

  bool Set::is_defined(string name)
  {
    return options.find(name) != options.end();
  }

  void Set::add_option(string sh, string lg, char narg, string help)
  {
    add_option(sh, narg, help);
    add_option(lg, narg, help);
  }

  void Set::add_option(string name, char narg, string help)
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

  pair<string, bool> Set::option(string name)
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
