// placement new && shared new
#include <iostream>
#include <memory>
// #include <stdio.h>
// #include <stdlib.h>
class A {
public:
  A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
  ~A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }

private:
 int a{10};
};

int main() {
  char *ptr = new char[20];
  A *a = new (ptr) A();

  {
    std::shared_ptr<A> spa(a, [](void *ptr) {
        A *a = reinterpret_cast<A *>(ptr);
         a->~A();
        delete[] reinterpret_cast<char *>(ptr);
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    });
  }
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return 0;
}
