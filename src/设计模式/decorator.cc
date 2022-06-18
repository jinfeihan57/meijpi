/*
 * 关键代码：1、Component 类充当抽象角色，不应该具体实现。 2、修饰类引用和继承
 * Component 类，具体扩展类重写父类方法。
 */
#include <iostream>

using namespace std;

//抽象构件（Component）角色：给出一个抽象接口，以规范准备接收附加责任的对象。
class Component {
 public:
  virtual ~Component() {}

  virtual void configuration() = 0;
};

//具体构件（Concrete Component）角色：定义一个将要接收附加责任的类。
class Car : public Component {
 public:
  void configuration() override { cout << "A Car" << endl; }
};

//装饰（Decorator）角色：持有一个构件（Component）对象的实例，并实现一个与抽象构件接口一致的接口。
class DecorateCar : public Component {
 public:
  DecorateCar(Component* car) : m_pCar(car) {}

  void configuration() override { m_pCar->configuration(); }

 private:
  Component* m_pCar;
};

//具体装饰（Concrete Decorator）角色：负责给构件对象添加上附加的责任。
class DecorateLED : public DecorateCar {
 public:
  DecorateLED(Component* car) : DecorateCar(car) {}

  void configuration() override {
    DecorateCar::configuration();
    addLED();
  }

 private:
  void addLED() { cout << "Install LED" << endl; }
};

//具体装饰（Concrete Decorator）角色：负责给构件对象添加上附加的责任。
class DecoratePC : public DecorateCar {
 public:
  DecoratePC(Component* car) : DecorateCar(car) {}

  void configuration() override {
    DecorateCar::configuration();
    addPC();
  }

 private:
  void addPC() { cout << "Install PC" << endl; }
};

//具体装饰（Concrete Decorator）角色：负责给构件对象添加上附加的责任。
class DecorateEPB : public DecorateCar {
 public:
  DecorateEPB(Component* car) : DecorateCar(car) {}

  void configuration() override {
    DecorateCar::configuration();
    addEPB();
  }

 private:
  void addEPB() { cout << "Install Electrical Park Brake" << endl; }
};

int main() {
  Car* car = new Car;
  DecorateLED* ledCar = new DecorateLED(car);
  DecoratePC* pcCar = new DecoratePC(ledCar);
  DecorateEPB* epbCar = new DecorateEPB(pcCar);

  epbCar->configuration();

  Car* car1 = new Car;
  DecorateEPB* epbCar1 = new DecorateEPB(car1);
  DecoratePC* pcCar1 = new DecoratePC(epbCar1);
  DecorateLED* ledCar1 = new DecorateLED(pcCar1);

  epbCar1->configuration();
  ledCar1->configuration();

  delete epbCar;
  epbCar = nullptr;

  delete pcCar;
  pcCar = nullptr;

  delete ledCar;
  ledCar = nullptr;

  delete car;
  car = nullptr;

  return 0;
}
