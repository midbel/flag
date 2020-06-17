#include <iostream>
#include "flag.hpp"

using namespace std;

void print_args(flag::Set &set);

int main(int argc, char** argv)
{
  flag::Set set("flagtest", "flagtest shows how to use the flag library in command line tool");
  string dir = "/var";
  bool verbose = false;
  bool help = false;
  bool dryrun = false;
  int number = 0;
  try
  {
    set.string_var(&dir, "d", "dir", "set the working directory");
    set.bool_var(&verbose, "v", "verbose", "increase verbosity");
    set.bool_var(&help, "h", "help", "show help and exit");
    set.bool_var(&dryrun, "n", "dry-run", "run command but do nothing");
    set.int_var(&number, "c", "limit", "limit recursion level");
    set.parse(argc, argv);
    print_args(set);
    cout << "working directory: " << dir << endl;
    cout << "verbose: " << boolalpha << verbose << endl;
    cout << "help: " << boolalpha << help << endl;
    cout << "dryrun: " << boolalpha << dryrun << endl;
    cout << "number: " << number << endl;
    cout << set.usage() << endl;

    if (help) {
    }
    cout << set.help() << endl;
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

void print_args(flag::Set &set)
{
  for (int i = 0; i < set.narg(); i++)
  {
    cout <<  i <<" arg: " << set.arg(i) << endl;
  }
}
