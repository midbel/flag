#include <iostream>
#include "flag.hpp"

using namespace std;

void print_args(flag::Set &set);

int main(int argc, char** argv)
{
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

void print_args(flag::Set &set)
{
  for (int i = 0; i < set.narg(); i++)
  {
    cout << "arg: " << set.arg(i) << endl;
  }
}
