/*
 * 关键代码：在目标类中增加一个ArrayList来存放观察者们。
 */

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>

using namespace std;

//观察者抽象类   视图
class View {
 public:
  virtual ~View() { cout << "~View()" << endl; }
  virtual void update() = 0;
  virtual void setViewName(const string& name) = 0;
  virtual const string& name() = 0;
};

//被观察者抽象类   数据模型
class DataModel {
 public:
  virtual ~DataModel() {}
  virtual void addView(View* view) = 0;
  virtual void removeView(View* view) = 0;
  virtual void notify() = 0;  //通知函数
};

//具体的被观察类， 整数模型
class IntDataModel : public DataModel {
 public:
  ~IntDataModel() { m_pViewList.clear(); }

  virtual void addView(View* view) override {
    shared_ptr<View> temp(view);
    auto iter = find(m_pViewList.begin(), m_pViewList.end(), temp);
    if (iter == m_pViewList.end()) {
      m_pViewList.push_front(temp);
    } else {
      cout << "View already exists" << endl;
    }
  }

  void removeView(View* view) override {
    auto iter = m_pViewList.begin();
    for (; iter != m_pViewList.end(); iter++) {
      if ((*iter).get() == view) {
        m_pViewList.erase(iter);
        cout << "remove view" << endl;
        return;
      }
    }
  }

  virtual void notify() override {
    auto iter = m_pViewList.begin();
    for (; iter != m_pViewList.end(); iter++) {
      (*iter).get()->update();
    }
  }

 private:
  list<shared_ptr<View>> m_pViewList;
};

//具体的观察者类    表视图
class TableView : public View {
 public:
  TableView() : m_name("unknow") {}
  TableView(const string& name) : m_name(name) {}
  ~TableView() { cout << "~TableView(): " << m_name.data() << endl; }

  void setViewName(const string& name) { m_name = name; }

  const string& name() { return m_name; }

  void update() override { cout << m_name.data() << " update" << endl; }

 private:
  string m_name;
};

int main() {
  /*
   * 这里需要补充说明的是在此示例代码中，View一旦被注册到DataModel类之后，DataModel解析时会自动解析掉
   * * 内部容器中存储的View对象，因此注册后的View对象不需要在手动去delete，再去delete View对象会出错。
   */

  View* v1 = new TableView("TableView1");
  View* v2 = new TableView("TableView2");
  View* v3 = new TableView("TableView3");
  View* v4 = new TableView("TableView4");

  IntDataModel* model = new IntDataModel;
  model->addView(v1);
  model->addView(v2);
  model->addView(v3);
  model->addView(v4);

  model->notify();

  cout << "-------------\n" << endl;

  model->removeView(v1);

  model->notify();

  delete model;
  model = nullptr;

  return 0;
}
