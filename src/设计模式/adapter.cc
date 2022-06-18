/*
 * 关键代码：适配器继承或依赖已有的对象，实现想要的目标接口。
 * 以下示例中，假设我们之前有了一个双端队列，新的需求要求使用栈和队列来完成。
 * 双端队列可以在头尾删减或增加元素。而栈是一种先进后出的数据结构，添加数据时添加到栈的顶部，删除数据时先删
 * 除栈顶部的数据。因此我们完全可以将一个现有的双端队列适配成一个栈。
 */

#include <iostream>
using namespace std;

//双端队列， 被适配类

class Deque {
 public:
  void push_back(int x) { cout << "Deque push_back:" << x << endl; }
  void push_front(int x) { cout << "Deque push_front:" << x << endl; }
  int pop_back() {
    cout << "Deque pop_back" << endl;
    return 0;
  }
  int pop_front() {
    cout << "Deque pop_front" << endl;
    return 0;
  }
};

//顺序类，抽象目标类
class Sequence {
 public:
  virtual void push(int x) = 0;
  virtual int pop() = 0;
};

//栈,后进先出, 适配类  将 Deque 适配为一个 Sequence。
class Stack : public Sequence {
 public:
  //将元素添加到堆栈的顶部。
  void push(int x) override { m_deque.push_front(x); }
  //从堆栈中删除顶部元素
  int pop() override {
    m_deque.pop_front();
    return 0;
  }

 private:
  Deque m_deque;
};

//队列，先进先出，适配类 将 Deque 适配为一个 Sequence。
class Queue : public Sequence {
 public:
  //将元素添加到队列尾部
  void push(int x) override { m_deque.push_back(x); }
  //从队列中删除顶部元素
  int pop() override {
    m_deque.pop_front();
    return 0;
  }

 private:
  Deque m_deque;
};

// 使用继承的方式实现适配器模式，当父子类接口名称相同而功能有差别时。需要在子类的方法中调用父类的相同方法。

// //双端队列，被适配类
// class Deque {
//  public:
//   void push_back(int x) { cout << "Deque push_back:" << x << endl; }
//   void push_front(int x) { cout << "Deque push_front:" << x << endl; }
//   int pop_back() {
//     cout << "Deque pop_back" << endl;
//     return 0;
//   }
//   int pop_front() {
//     cout << "Deque pop_front" << endl;
//     return 0;
//   }
// };

// //顺序类，抽象目标类
// class Sequence {
//  public:
//   virtual void push(int x) = 0;
//   virtual int pop() = 0;
// };

// //栈,后进先出, 适配类
// class Stack : public Sequence, private Deque {
//  public:
//   void push(int x) { push_front(x); }
//   int pop() {
//     pop_front();
//     return 0;
//   }
// };

// //队列，先进先出，适配类
// class Queue : public Sequence, private Deque {
//  public:
//   void push(int x) { push_back(x); }
//   int pop() {
//     pop_front();
//     return 0;
//   }
// };

int main() {
  Stack s;
  s.push(1);
  s.pop();

  Queue q;
  q.push(2);
  q.pop();

  return 0;
}