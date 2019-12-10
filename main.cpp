#include <bits/stdc++.h>
#include <iostream>
using namespace std;

// struct Foo
//{
//  void bar() const &  { std::cout << "const lvalue Foo\n"; }
//  void bar()       &  { std::cout << "lvalue Foo\n"; }
//  void bar() const && { std::cout << "const rvalue Foo\n"; }
//  void bar()       && { std::cout << "rvalue Foo\n"; }
//};
//
// const Foo&& getFoo() { return std::move(Foo()); }
//

int main() {
  //  assert(f() && "wtf??");
  srand(time(0));
  switch (rand() % 3) {
  default:
    cout << "default\n";
    break;

  case 2:
    cout << "2\n";
    //    break;
  case 1:
    cout << "1\n";
    break;
  }
}