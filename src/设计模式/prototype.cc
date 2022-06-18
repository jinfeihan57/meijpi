/*
 * 关键代码：拷贝，return new className(*this);
 */
#include <iostream>

using namespace std;

//提供一个抽象克隆基类。
class Clone {
 public:
  virtual Clone* clone() = 0;
  virtual void show() = 0;
};

//具体的实现类
class Sheep : public Clone {
 public:
  Sheep(int id, string name) : Clone(), m_id(id), m_name(name) {
    cout << "Sheep() id address:" << &m_id << endl;
    cout << "Sheep() name address:" << &m_name << endl;
  }
  ~Sheep() {}
  //关键代码拷贝构造函数
  Sheep(const Sheep& obj) {
    this->m_id = obj.m_id;
    this->m_name = obj.m_name;
    cout << "Sheep(const Sheep& obj) id address:" << &m_id << endl;
    cout << "Sheep(const Sheep& obj) name address:" << &m_name << endl;
  }
  //关键代码克隆函数，返回return new Sheep(*this)
  Clone* clone() { return new Sheep(*this); }
  void show() {
    cout << "id  :" << m_id << endl;
    cout << "name:" << m_name.data() << endl;
  }

 private:
  int m_id;
  string m_name;
};

int main() {
  Clone* s1 = new Sheep(1, "abs");
  s1->show();
  Clone* s2 = s1->clone();
  s2->show();

  delete s1;
  s1 = nullptr;
  delete s2;
  s2 = nullptr;
  return 0;
}
