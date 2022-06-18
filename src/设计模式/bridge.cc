/*
 * 关键代码：将现实独立出来，抽象类依赖现实类。
 * 以下示例中，将各类App、各类手机独立开来，实现各种App和各种手机的自由桥接。
 */
#include <iostream>

using namespace std;

//抽象App类，提供接口
class App {
 public:
  virtual ~App() { cout << "~App()" << endl; }
  virtual void run() = 0;
};

//具体的App实现类
class GameApp : public App {
 public:
  void run() { cout << "GameApp Running" << endl; }
};

//具体的App实现类
class TranslateApp : public App {
 public:
  void run() { cout << "TranslateApp Running" << endl; }
};

//抽象手机类，提供接口
class MobilePhone {
 public:
  virtual ~MobilePhone() { cout << "~MobilePhone()" << endl; }
  virtual void appRun(App* app) = 0;  //实现App与手机的桥接
};

//具体的手机实现类
class XiaoMi : public MobilePhone {
 public:
  XiaoMi() {}
  ~XiaoMi() { cout << "~XiaoMi()" << endl; }
  void appRun(App* app) {
    cout << "XiaoMi: ";
    app->run();
  }
};

//具体的手机实现类
class HuaWei : public MobilePhone {
 public:
  HuaWei() {}
  ~HuaWei() { cout << "~HuaWei" << endl; }
  void appRun(App* app) {
    cout << "HuaWei: ";
    app->run();
  }
};

int main() {
  App* gameApp = new GameApp;
  App* translateApp = new TranslateApp;
  MobilePhone* mi = new XiaoMi;
  MobilePhone* hua = new HuaWei;
  mi->appRun(gameApp);
  mi->appRun(translateApp);
  hua->appRun(gameApp);
  hua->appRun(translateApp);

  delete hua;
  hua = nullptr;
  delete mi;
  mi = nullptr;
  delete gameApp;
  gameApp = nullptr;
  delete translateApp;
  translateApp = nullptr;

  return 0;
}