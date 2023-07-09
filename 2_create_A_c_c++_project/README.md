# 万事开头难
    分享适用于有一定 c/c++ 语言基础，但是项目经验不足的同学。
    很多同学在系统的学习了 c/c++ 语法之后，仍然无法搭建一个 c/c++ 语言的工程。纵使自己对语法非常了解但是因为没有项目经验，
    无法系统的搭建一个 c/c++ 项目，进行迭代开发。导致自己始终只会做一些语法级别的编程开发。无法独立开启一个复杂的项目。
    我这里就以最简单的 hello world 开始一步步带领大家，搭建一个典型的 c/c++ 工程。
    搭建过程中将会用到 gcc/g++ 编译器，make，cmake，git 等一些最基本的 shell 操作。
# CREATE A c/c++ PROJTCT
把简单的事情做复杂
## [hello world 第一个c/c++工程](https://github.com/jinfeihan57/start_A_c_cpp_project/tree/hello_world)
各个功能块耦合严重，后期维护困难，随着业务复杂度，函数体积迅速膨胀，导致逻辑不清晰，功能责任田混乱，团队合作困难。不要说大项目，连小项目也难以开发。
## [解耦 hello world](https://github.com/jinfeihan57/start_A_c_cpp_project/tree/separate_stage2)
各个功能块按照责任田划分不同的目录和文件以及函数，逻辑清晰易于管理，团队合作效率高。但是编译操作复杂，低效。
## [使用 makefile](https://github.com/jinfeihan57/start_A_c_cpp_project/tree/use_makefile)
Makefile 只能在类 unix 系统上使用，无法在 Windows 系统。c/c++语言是跨平台的，只因为构建方式导致项目跨平台失败，这显然是不可接受的。
## [使用 cmake](https://github.com/jinfeihan57/start_A_c_cpp_project/tree/cmake_stage2)
cmake 是跨平台的 c/c++ 编译脚本。完美解决项目构建跨平台问题。最大化利用 c/c++ 的跨平台特性。一个典型的 c/c++ 工程搭建完成。
## [gtest测试框架](https://github.com/jinfeihan57/start_A_c_cpp_project/tree/add_gtest)
项目在开发过程中，如何保证开发的代码是正确的？答案显而易见 **"测试"**。任何一个完整的项目都必须引入测试框架，以保证各个模块提供的能力是正确可靠的。googletest 是当下最流行的 c/c++ 测试框架。
## [使用gtest](https://github.com/jinfeihan57/start_A_c_cpp_project/tree/use_gtest_stage1)
熟练掌握 gtest ，并且真实的去开发测试用例。能为我们的项目提前屏蔽很多不必要的问题（尤其项目有一定规模时）。例如：项目迭代过程中，常常会出现修复一个 bug，又引入了多个 bug 的情况，如果此时对应的 api 有完整的测试用例，则可以提前暴露解决上述 bug。一个优秀的开发，必定也是一个好的测试。
<br>
