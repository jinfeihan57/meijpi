// https://docs.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-160
/* 
lamda 表达式:	[ capture ] ( params ) opt -> ret { body; };
	 capture 是捕获列表，params 是参数表，opt 是函数选项(mutable)，ret 是返回值类型，body是函数体。
[] 			不捕获任何变量。
[&] 		捕获外部作用域中所有变量，并作为引用在函数体中使用（按引用捕获）。
[=] 		捕获外部作用域中所有变量，并作为副本在函数体中使用（按值捕获）。
[=，&foo] 	按值捕获外部作用域中所有变量，并按引用捕获 foo 变量。
[bar] 		按值捕获 bar 变量，同时不捕获其他变量。
[this] 		捕获当前类中的 this 指针，让 lambda 表达式拥有和当前类成员函数同样的访问权限。
			如果已经使用了 & 或者 =，就默认添加此选项。捕获 this 的目的是可以在 lamda 中使用当前类的成员函数和成员变量。
lamda 表达式像普通函数一样抛异常
*/ 
//

#include <iostream>
#include <functional>

auto addint = [](int a, int b) -> int  // lamda 表达式作为全局变量可以
{
	return a + b;
};

auto t = [] // lamda 匿名无参表达式，定义后直接调用等同于 auto t = 1; 无参时 "()" 省略
{
	return 1;
}();

using addfunc = int (*)(int, int);

class A {
public:
	A(int f, int s):a{f}, b{s}
	{}
	// 有capture clause的lamda表达式不可以声明为函数指针类型
/* 	auto b = [this]() -> int  // lamda 表达式作为类内变量不可以
	{
		return a + b;
	}; */
	// 没有capture clause的lamda表达式可以声明为函数指针类型
	addfunc af = [](int a, int b) -> int  // lamda 表达式作为类内变量需要提前声明表达式类型可以
		{
			throw 0;
			return a + b;
		};
		
	int add(){
		auto y = [this]() -> int  // lamda 表达式作为函数内变量可以
		{
			a = 3;
			return a + b;
		};
		std::function<int(void)>  f1 = [this]{ return a; }; // 无参时 "()" 省略 通过capture clause传参
		return y();
	}
private:
	int a;
	int b;

};

/*
	我们可以认为它是一个带有 operator() 的类，即仿函数。
	因此，我们可以使用 std::function 和 std::bind 来存储和操作 lambda 表达式
*/

std::function<int(int)>  f1 = [](int a){ return a; };
std::function<int(void)> f2 = std::bind([](int a) -> int { return a;}, 123);

int main(){
	A aaa(1, 2);
	std::cout << aaa.add() << std::endl;
	
	std::cout << aaa.af(3, 4) << std::endl;
	
	std::cout << f1(345) << std::endl;
	std::cout << f2() << std::endl;
	
	int c = 4;
	auto f3 = [=](int a) mutable { c = 5; return a + c; }; // 以值传递的变量是const(只读型)的不可改变其值，加mutable参数即可。
	std::cout << f3(1) << std::endl;
	auto f4 = [&](int a) mutable { c = 5; return a + c; }; // 以引用传递的变量可改变其值，并同时改变外部的值（引用特性）。
	std::cout << f4(2) << std::endl;
	std::cout << c << std::endl;
	
	[=](int a) {  	// 声明后马上调用
        std::cout << a << std::endl;
    }(100);
}