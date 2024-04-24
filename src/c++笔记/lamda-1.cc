#include <iostream>
#include <memory>
#include <functional>

class A {
public:
    A (int a) : a_(a){
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    A (const A &a) : a_(a.a_){
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
    ~A () {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    int a_ {10};
};

int main ()
{
    std::cout << "hello world!!" << std::endl;
    auto a = std::make_shared<A>(3);
    auto xxx = [a] () mutable -> int { a->a_ = 7; return a->a_; };
    std::cout << xxx() << std::endl;
    auto bbb = std::make_unique<std::function<int()>>(xxx).release();  // 经此转换后 a 对象无法自动析构 必须主动 delete
    std::cout << (*bbb)() << std::endl;
    delete bbb;
    return 0;
}
