#include <iostream>
#include "flag.hpp"

using namespace std;

void print_args(flag::Set &set);

int main(int argc, char** argv)
{
  flag::Set set;
  string dir = "/var";
  bool verbose = false;
  int number = 0;
  try
  {
    set.string_var(&dir, "d", "dir", "working directory");
    set.bool_var(&verbose, "v", "verbose", "verbose");
    set.int_var(&number, "n", "number", "number");
    set.parse(argc, argv);
    print_args(set);
    cout << "working directory: " << dir << endl;
    cout << "verbose: " << verbose << endl;
    cout << "number: " << number << endl;
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
