/*
 * 关键代码：一个是真正的你要访问的对象(目标类)，一个是代理对象,真正对象与代理对象实现同一个接口,先访问代理*
 * 类再访问真正要访问的对象。
 */
#include <iostream>

using namespace std;

class Gril {
 public:
  Gril(const string& name = "gril") : m_string(name) {}
  ~Gril() {}
  string getName() const { return m_string; }

 private:
  string m_string;
};

class Profession {
 public:
  virtual ~Profession() {}
  virtual void profess() = 0;
};

class YoungMan : public Profession {
 public:
  YoungMan(const Gril& gril) : m_gril(gril) {}
  ~YoungMan() {}
  void profess() {
    cout << "Young man love " << m_gril.getName().data() << endl;
  }

 private:
  Gril m_gril;
};

class ManProxy : public Profession {
 public:
  ManProxy(const Gril& gril) : m_pMan(new YoungMan(gril)) {}
  ~ManProxy() {
    delete m_pMan;
    m_pMan = nullptr;
  }
  void profess() { m_pMan->profess(); }

 private:
  YoungMan* m_pMan;
};

int main(int argc, char* argv[]) {
  Gril gril("heihei");
  ManProxy* proxy = new ManProxy(gril);
  proxy->profess();

  delete proxy;
  proxy = nullptr;
  return 0;
}