# 改善程序与设计的55个具体做法

## 第一章 让自己习惯 C++



### 条款01：视C++为一个语言的联邦

### 条款02：尽量以const，enum，inline，替换 #define

### 条款03：尽可能使用const

​		如果你的变量不可以改变其值，请使用const修饰明确的指示给编译器，让编译器帮你检查。

​		STL迭代器iterator同样有const版本：const_iterator。

​		const最具权威的用法是面对函数声明时的应用。在一个函数声明式内，const可以和函数返回值、各参数、函数自身(如果是成员函数)产生关联。

​		自定义类型的操作符重写后，操作符的表现应当和内置类型的相同操作符一致。例如乘法操作符返回值不可以再次赋值因此用const修饰operator*()返回值。

​		const成员函数承诺不对对象的属性进行修改，使对const修饰的变量的操作成为可能。并且对条款20 （pass-by-reference-to-const方式传递对象）的技术前提就是我们有const成员函数处理const修饰的变量。依据bitwise const，const成员函数不可以更改对象内任何non-static成员变量。

​		如果某个成员变量即便在const成员函数内也要改变，可以用mutable关键字修饰; mutable int a;

​		项目中可以为类单独封装一个数组类重写operator[ ] 方法。operator[ ]方法重写时，建议参考本条款示例。

```c++
class TextBlock{
public:
	const char& operator[](std::size_t position) const {	// const 成员函数不可以调用non-const成员函数，因为对象有被修改的风险，
		...													// 除非使用const_cast将对象的const属性释放掉
		...
		return text[position];
	}
	char& operatoe[](std::size_t position){			// 为了避免重复代码和方便后期维护，non-const版本借助const版本实现。
		return const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
	}
}
```

- [ ] 将某些东西声明为const可以帮助编译器侦测出错误的用法。const可被施加于任何作用域内的对象、函数参数、函数返回值、成员函数本体。
- [ ] 编译器强制实施（bitwise constness），但你编写程序时应该使用“概念上的常量”（conceptual contness）。
- [ ] 当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const版本可避免代码重复。（重复代码多时建议使用本规则，为了方便后续的代码维护更应该遵循本规则）

### 条款04：确定对象被使用前已先被初始化

​		至于内置类型，读取前必须对其进行初始化。而非内置类型（自定义或者STL 类），初始化的责任落在了构造函数身上。所以必须确保构造函数将对象内的每一个成员进行初始化。

​		注意对象的成员初始化和成员赋值不同，C++规定对象的成员变量的初始化动作发生在进入构造函数本体之前。使用初值列（member initialization list）将对象的成员进行初始化。

```c++
ABEntry::ABEntry(const string& name, const list<PhoneNumber> phone, const int age)
				: thename(name), thephone(phone), theage(age)   // 使用初值列 string list<> 通常效率更高，而内置类型int的初值列和采用赋值时效率相同
{ }																// 但为了保证代码一致性，均采用初值列方式初始化对象
```

​		总是在初值列中列出所有成员变量（无参的类成员可调用其无参构造函数），以免还要记住那些成员变量可以无需初值。

​		内置类型如果是，const或者references类型则必须使用初值列进行初始化。因此养成无论什么类型成员，均在初值列进行初始化。

​		C++成员初始化次序：base class总是早于derived class。class 的成员变量总是以其声明的次序被初始化。而与初值列的顺序无关，因此初值列最好也与声明次序一致。 

​		场景：如果某编译单元内的某个 non-local static 对象的初始化动作使用了另一个编译单元内的某个 non-local static对象，它所用到的这个对象可能尚未初始化，因为c++对定义于不同编译单元内的 non-local static 对象的初始化次序并无明确定义。对于此中情况，使用的对象将不能保证以正确的顺序初始化。

​		解决方法：**将每一个 non-local static 对象搬到自己专属的函数内（在函数内将对象声明为static），函数返回一个reference指向它所包含的对象。然后用户调用这些函数间接引用对象，而不是直接使用对象。c++保证函数内的local static对象会在该函数被调用期间（首次调用时被初始化（调用构造函数））。方法类似设计模式单例模式，但是不要求构造函数私有化。**

​		任何一种non-const static对象，不论它是 local 还是non-local，在多线程环境下“等待某事发生都会有麻烦”。处理这个麻烦的一种做法是：在程序的单线程启动阶段手工调用所有的 reference-returning函数，这可消除与初始化有关的竞速现象。当然reference-returning函数的初始化顺序需要注意。如果A依赖B，就先初始化B在初始化A。不允许出现互相依赖的情况。

- [ ] 为对象的c++内置类型（int，float ...）进行手动初始化而非内置类型将会有构造（默认构造）函数初始化，因为C++不保证初始化它们。
- [ ] 构造函数最好使用成员初始化列表（），而不要在构造函数本体内使用赋值操作。**初值列列出的成员变量，其排列次序应该和他们在class中声明次序相同**。
- [ ] 为免除“跨编译单元之初始化次序”问题，请以 local static 对象替换 non-local static对象。

## 第二章 构造/析构/赋值运算

​		几乎你写的每一个类都会有一个或多个构造函数，一个析构函数，一个copy assignment操作符。这些基础的类操作，控制着对象的初始化和确保对象被适当的清理掉。因此这些函数的正确行为至关重要。

### 条款05：了解C++默默编写并调用那些函数

​		经过C++编译器处理，如果你自己没有声明一个copy构造函数，一个copy assignment操作符，一个析构函数，一个构造函数。则编译器会为你自动生成对应的函数，注意编译器生成的析构函数为non-virtual，编译器生成的构造为default（无参）构造。并且由编译器生成的这些函数都是public且inline的。

​		default 构造函数和析构函数主要是为了给编译器一个地方来放置藏身幕后的代码，像是调用base classes 和 non-static成员变量的构造函数和析构函数。

​		至于copy构造和copy assignment操作符，编译器创建的版本，只是单纯的将来源对象的每一个非静态成员变量拷贝到目标对象。如果成员为内置类型（int、double...）在采用真是copy每一个bit的方法copy，如果成员为对象则会调用成员的copy构造和copy assignment。因此要保证成员有copy构造和copy assignment默认copy构造和copy assignment才可以成功，否则报错。此外成员还要满足可以copy assignment （引用和const类型的成员变量则不可以copy assignment）： string &name; const int value;包含这两种类型成员的情况不允许做copy assignment动作。

​		**const 和引用类型的初始化只能在构造函数的初始化列表做，包含这两种类型的类必须遵守条款06，明确的拒绝copy assignment。**

- [ ] 编译器可以暗自为class创建default构造函数，copy构造函数，copy assignment操作符，以及析构函数。

### 条款06：若不想使用编译器自动生成的函数，就该明确拒绝

​		在一些场景下，不允许出现对象的copy构造和copy赋值。就像不能出现一个人的clone题一样，你不能拿一个人去初始化另一个人。这种类就要明确的拒绝copy构造和copy赋值函数。

​		根据条款05,如果用户未提供，编译器会默认生成copy构造和copy赋值函数。因此拒绝copy构造和copy赋值需要特殊方法。

​		~~1.将copy构造和copy赋值声明为private并且不实现它们。这样在有友元函数时，可能会调用他。这种情况编译不会报错但在连接时报错（因为函数未实现）。~~

​		~~2.实现Uncopyable类作为基类子类使用编译器默认的copy构造和copy赋值函数，所有拒绝copy构造和copy赋值的类都继承Uncopyable。编译时即可发现错误，因为子类的默认copy构造和copy赋值函数会去调用父类的copy构造和copy赋值函数，但是父类Uncopyable的copy构造和copy赋值函数为private。无法调用。并且有可能存在多重继承的问题。~~

​		3.采用 =delete; 关键字。主动拒绝编译器提供的函数。

- [ ] 为驳回编译器自动提供的机能，~~可将相应的成员函数声明为private并且不予实现。使用像Uncopyable这样的基类也是一种做法。~~或者声明为=delete;

### 条款07：为多态基类声明virtual析构函数

​		C++明确的支出，当derived class 对象经由一个base class 指针被删除，而该base class 带着一个non-virtual析构函数，其结果未有定义（实际执行时通常只销毁了base成分，对象的derived成分没有被销毁）。消除这个问题的做法很简单即将积累的析构函数声明为virtual。任何class只要带有virtual函数就可以确定也有一个virtual析构函数。

​		virtual析构是将析构函数也放入子类的虚函数表中，因此在析构时会去虚函数表中查找析构函数。如果析构函数不是virtual则无法构成多态，则基类指针或引用在调用析构函数时，调用的就是基类的析构函数。此外不含虚函数的类将其析构函数声明为virtual则会为类无端增加虚函数指针，增大内存消耗。

​		没有virtual函数（虚析构也是虚函数）的类最好不要继承，因为当derived类指针强转为base类后，delete base 指针时derived中的子类成分无法销毁。标准库中的所有容器，string都是没有虚函数的。**C++11新标准中对于不想被继承的类提供final关键字。**

​		在实现多态时我们通常需要一个抽象类（无法被实例化），这个抽象类提供一系列的函数接口。等待子类继承后在具体实现。这个抽象类的析构函数必须是virtual函数，并且这个virtual析构必须实现为虚函数。因为子类在析构时会首先调用父类的析构函数，因此必须实现（空函数即可）。

- [ ] polymorphic (多态性质) base class 应该声明一个virtual析构函数。如果class带有任何virtual函数，他就应该拥有一个virtual析构函数。
- [ ] classes的设计目的如果不是为了作为base class使用，或不是为了具备多态性质，就不该声明virtual虚构函数。

### 条款08：别让异常逃离析构函数

​		析构函数中的异常会使程序朝着未知的方向运行（容器对象在析构时如果中途抛异常后续的成员将无法正常析构），如果是RAII类则还可能造成资源未被释放（归还系统）。因此C++不建议在析构函数中调用有异常抛出的函数。但是如果在析构函数中调用了可能抛出异常的函数，请一定在析构函数内把异常处理掉。

​		1.如果析构函数抛出异常，通常调用abort结束程序。

```c++
DBConn::~DBConn(){
	try{
		db.close();
	}catch(...){
		std::abort();
	}
}
```

​		2.如果析构函数抛出异常，吞下析构时发生的异常，进行记录或者日志。方便后续处理和问题追踪。一般而言直接吞掉异常是一个坏主意，因为毕竟某些动作失败了。会造成未知的后果。程序如不能正确处理会造成未知错误。

```c++
DBConn::~DBConn(){
	try{
		db.close();
	}catch(...){
		// 制作运转记录，记下调用失败。方便后续程序处理或者回溯问题日志
	}
}
```

​		3.在无可避免需要在析构函数中调用有可能抛异常的函数时，一种较佳的策略。把调用可能抛出异常的函数的责任从析构函数移交到用户手上，并且在析构函数中在加一个双保险。这样用户在使用中就多了一个选择。建议用户主动调用可能抛出异常的函数，进行异常的捕获。在用户忘记主动调用时，又可以通过析构函数调用。

```c++
class DBConn{
public:
    ...
    void close(){
        db.close();
        closed = true;
    }
    ~DBConn(){
        if(!closed){
            try{
                db.close();
            }catch(...){
                // 方法1，2
            }
        }
    }
private:
    DBConnection db;
    bool closed;
}
```

- [ ] 析构函数绝对不要抛出异常，如果一个被析构函数调用的函数可能抛出异常，析构函数应该捕捉任何异常，并处理他（不传播）或者结束程序。
- [ ] 如果客户需要对某个操作函数运行期间抛出的异常做出反应，那么class应该提供一个普通函数执行该操作。（而非在析构函数中）

### 条款09：绝不再构造和析构过程调用virtual函数

​		子类构造函数若调用了虚函数，则子类构造函数调用的是父类的虚函数而非子类的，因此在构造函数中不要调用虚函数。

​		由于子类在构造时会先构造父类，因此当父类的构造函数执行时，子类的成员还未初始化，如果此时调用了子类的virtual函数，要知道子类的virtual函数几乎必然调用local成员变量，而这些成员变量未被初始化，因此调用子类的virtual不安全。所以编译器在构造函数中调用virtual函数时会调用父类的。

​		当构造子类中的父类时，对象的类型是父类而不是子类，不只virtual函数会被编译器解析为父类的。构造期间virtual函数不是virtual函数。

​		相同的道理析构函数也一样，只是和构造函数顺序相反。

​		在多个构造函数时难免有很多重复代码，此时在父类可以采用非虚函数的方式，在子类构造时，指定父类的构造函数。

```c++
class Transaction{
public:
	explicit Transaction(const std::stding & logInfo);
    void logTranscation(const std::stding & logInfo)) const;
    ...
};
Transaction::Transaction(const std::stding & logInfo){
    ...
    logTranscation(logInfo);
    ...
}
class BuyTransaction:public Transaction{
public:
    BuyTransaction(parameters):Transaction(createLogString(parameters)){	// 指定父类构造函数 初始化列表除了初始化类成员还可以初始化 基类
        ...																	// 使用静态函数对父类的参数进行辅助
    }
    ...
private:
    static std::string createLogString(parameters);		// 类内静态函数可以理解为，全局函数，因此不受“初期未成熟之对象内未初始化的影响”
}
```

- [ ] 在构造和析构期间不要调用virtual函数，应为这类调用从不下降至derived class（比起当前执行构造函数和析构函数的那层）。

### 条款10：令operator=返回一个reference to * this

​		为了实现连锁赋值，赋值操作符必须返回一个reference指向操作符的左侧实参。

```c++
Widget& operator=(){
	...;
	return *this;
}
```

```
Widget& operator+=(){
	...;
	return *this;
}
```

- [ ] 令 =，+=，-= 等赋值操作符，返回一个reference to *this。

### 条款11：在operator=中处理“自我赋值”

​		在使用对象管理资源（动态内存申请，文件描述符，互斥锁，数据库连接，网络socket连接，图形界面的字形和笔刷）时，要保证在资源copy时也能有正确的操作，需要对资源进行特殊的关注。而当这种管理资源的对象，进行operator=操作时。目标对象需要先释放资源，在使用来源对象的资源进行相关属性的初始化。如果目标对象和来源对象是同一个对象时则会出现，使用已经释放的来源对象属性来初始化目标对象属性，导致不可预知错误。

​		在指针和引用大量使用时，容易造成两个对象实际指向同一块内存。或者在多态中，即父类指针或者引用指向子类对象时，也是两个对象指向同一内存区域。极易造成，使用中无法准确判断是否自我赋值。

解决方案：

```c++
Widget& Widget::operator=(const Wiget &rhs){
	if(this == &rhs) return *this;  // 如果是自我赋值则直接返回
	// 否则正常赋值
	delete pb;
	pb = new Bitmap(*(rhs.pb));
	return *this;
}
```

​		上述方法中如果在new Bitmap中产生了异常则导致 pb指向的内存无法安全访问。

解决方案：

```c++
Widget& Widget::operator=(const Wiget &rhs){
	Bitmap *pOrig = pb;
	pb = new Bitmap(*(rhs.pb));
	delete pOrig;
	return *this;
}
```

​		这样在 new Bitmap异常时，pb指向的内存仍然为原来的指向。当然这样不管三七二十一先new一个对象会导致在自我赋值的情况下效率较低，因此可以和自我赋值测试语句联合使用。同样的增加的自我赋值检测只是为了在自我赋值时的效率提高，但是非自我赋值时的效率则会有所下降。因此同样要考虑自我赋值和非自我赋值发生的概率。整体来看，不加更好。

​		在operator=函数中使用 copy and swap 技术，可以更好的解决上述问题。详细用法可跳转条款29中查看 copy and swap 的使用。

以下代码展示：

```c++
class Widget{
	...
	void swap(Widget& rhs);  // 内部可调用 std::swap();
	...
}

Widget& Widget::operator=(const Wiget &rhs){
	Widget tmp（rhs);        // copy构造函数异常，则不会调用后续代码，不会改变 *this 对象。
	swap(tmp);
	return *this;
}

```

```c++
Widget& Widget::operator=(Wiget rhs){   // 采用值传递的方式可以避免 tmp 的创建，避免copy构造异常
	swap(rhs);
	return *this;
}
```

- [ ] 确保当对象自我赋值时 operator= 有良好的行为，其中技术包括比较“来源对象”和“目的对象”的地址，精心周到的语句顺序以及copy-and-swap.
- [ ] 确定任何函数如果操作一个以上的对象，而其中多个对象是同一个对象时，其行为仍然正确。

### 条款12：赋值对象是勿忘其中每一个成分

​		在对象赋值或copy时，确保对象中的每一个元素都被赋值。

​		1.当对象增加属性时，必须对其所有构造函数和operator=进行必要更新。静态属性则需要进行初始化。

​		2.当有继承发生时，子类的copy动作，必须包括父类属性的copy（显式的调用父类的copy构造或者operator=函数）。

```c++
class PriortyCustomer：public Customer{
public:
    PriortyCustomer(const PriortyCustomer & rhs);
    PriortyCustomer& operator=(const PriortyCustomer & rhs)
private:
    int priority;
}
PriortyCustomer::PriortyCustomer(const PriortyCustomer & rhs)	// 子类copy构造
    							:Customer(rhs),		// 基类的copy构造调用  如果不指定基类的copy构造则基类只进行无参构造过程 同样在普通构造函数中也要显式的指定基类的构造 否则父类都是采用默认构造
								priority(rhs.priority){		//	子类属性copy构造
    ...
}
PriortyCustomer& PriortyCustomer::operator=(const PriortyCustomer & rhs){
    Customer::operator=(rhs);	// 基类operator=调用，赋值基类的属性
    priority = rhs.priority;
    return *this;
}
```

- [ ] copying函数应该确保赋值“对象内的所有变量”及“所有base class成分”。
- [ ] 不要尝试以某个copying函数实现另一个copying函数。应该将共同机能放进第三个函数中，并有两个copying函数共同调用。

## 第三章 资源管理

​		所谓资源就是，一旦用了它，将来一定要还给系统。如果不这样糟糕的事情就会发生。C++程序中最常用的资源就是动态内存分配，但是内存分配只是你必须管理的众多资源之一。其他常见的资源还包括问文件描述符，互斥锁，数据库连接，网络连接socket以及图形界面中的字形和笔刷。

### 条款13：以对象管理资源

​		当程序非正常返回或退出时（提前return，goto语句，continue，异常发生等），会造成资源泄露。我们将资源放进对象（auto_ptr 和 shared_ptr），当控制流离开函数时，该对象的析构函数会自动释放那些资源。

​		**获得资源后立即方剂对象内**：（RAII）

​		**管理对象运用析构函数确保资源被释放**：如果资源释放动作可能导致异常抛出，事情将棘手，参考条款8解决异常问题。

​		**使用智能指针对heap资源进行管理：**

​		1：auto_ptr ：在赋值后即会失去自己对对象的拥有权，拥有权转移到了新的指针对象上，自己变为了空指针。

​		2：shared_ptr：采用RCSP（Reference counting smart pointer）技术，shared_ptr允许指针对象赋值并且在无人指向时自动delete。不同的时RCSP无法打破环状引用，例如两个对象彼此互指。（需要使用weak_ptr来解决环状引用的问题）

​		**auto_ptr和shared_ptr都是在其析构函数内做delete而不是delete[ ] 动作，因此这两个智能指针都不能在动态分配的array身上使用。**

```c++
shared_ptr<int> spi(new int[10])   // 虽然编译器不会报错，但是这样做显然不对
```

​		可以将数组包装在一个对象内，对象的构造函数调用new[ ]，对象的析构函数调用delete[ ]，并重载数组对象类的operator[ ]接口，方便访问数组元素。可以通过智能指针的operator->操作等方法对，数组进行访问。

针对数组型智能指针参见条款55中 boost::scoped_array 和boost::shared_array 类。

- [ ] 为防止资源泄露，请使用RAII（Resource Acquisition Is Initialization）对象，他们在构造函数中获得资源并在析构函数中释放资源。
- [ ] 两个常被使用的RAII class分别是 shared_ptr 和 auto_ptr，通常使用前者。

### 条款14：在资源管理类中小心copying行为

​		如果资源不是heap-based资源，auto_ptr和shared_ptr将不适合对这类资源进行管理。例如：

```c++
class Lock{
public:
    explicit Lock(Mutex *pm):mutexPtr(pm){
        lock(mutexPtr);
    }
    ~Lock(){ unlock(mutexPtr);}
private:
    Mutex *mutexPtr;
}

int fun(){
    ...
    Mutex m;		// 定义互斥器
    {
        Lock m1(&m);// 锁定互斥器 在大括号内有效
        ...
    }				
    ...				// 互斥器，自动解锁，m1 出生命区间自动析构解锁
}
```

​		禁止复制：许多时候RAII类资源被复制时不合理的，例如上面 Mutex 的例子。如果复制动作对RAII不合理，应该禁止它。参考条款06，将其声明到private域或者声明为delete。

​		对底层资源祭出“引用计数法”：有时候我们希望保有资源，直到最后一个使用者被销毁。这种情况下复制RAII对象，只是将资源的引用数递增。shared_ptr就是reference count实现的。通常只要内含一个shared_ptr即可，但是shared_ptr在引用为0时会删除所指物。例如在Mutex上我们只是希望它unlock而非delete。 幸运的是shared_ptr允许我们指定删除器，是一个函数或者仿函数。

```c++
class Lock{
public:
    explicit Lock(Mutex * const pm):mutexPtr(pm, unlock){  // 第二参数为智能指针引用为0时调用的delete函数。
        lock(mutexPtr.get());    
    }
private:
    shared_ptr<Mutex> mutexPtr;  // 采用智能指针而非普通指针
};
```

​		本例中 Lock class不在声明析构函数（编译器自动生成），因为没有必要。条款05:析构函数（编译器生成的）会自动调用其non-static成员变量的析构函数，这里mutexPtr的析构函数在引用计数到达0时被调用，而mutexPtr的析构函数被重定向为unlock。注解:上面代码中涉及两个对象，一个是Lock，另一个shared_ptr<Mutex>，Lock采用编译器默认的析构函数，而mutexPtr使用的析构函数为unlock。具体参见shared_ptr的构造函数第二参数。

​		复制底层资源：在复制资源管理对象时，进行深拷贝。例如 heap-based 资源。

​		转移底部资源的拥有权：即copy构造和copy复制函数会剥夺被复制物的资源控制权给目标。动作如auto_ptr。

- [ ] 复制RAII对象必须一起复制他所管理的资源，所以资源的copying行为决定RAII对象的copying行为。
- [ ] 普遍而常见的RAII class copying行为是：抑制copying、施行引用计数法（reference counting）。不过其他行为也都是可以被实现的。

### 条款15：在资源管理类中提供对原始资源的访问

​		在API需要接受资源作为参数时，若采用了对象管理资源，则需要先从对象内获取资源在作为参数传给API。

​		例如使用智能指针管理资源，入参需要资源指针。此时就需要调用智能指针的get()方法获取资源，作为参数传入API。智能指针所指向的资源的获取可以依赖智能指针重载的指针取值操作符（operator->, operator*）。

​		同理在不使用智能指针作为资源管理类时，也需要提供资源访问接口。分为显示转换函数和隐式转换函数。

​		显示转换函数：

```c++
class Font{
public:
    explicit Font(FontHandle fh):f(fh){ }
    ~Font(){ releaseFont(f); }
    FontHandle get() const { return f; }   // 显示转换函数，通过调用get函数来获取类管理的资源
private:
    FontHandle f;
};
```

​		隐式转换函数：

```c++
class Font{
public:
	...;
    operator FontHandle () const { return f; }   // 使用隐式函数（C++类型转换）实现类中资源的获取 Font f1; FontHandle f2 = f1;隐式类型强转。
	...;
};
```

​		显示转换的代码较为冗长但是可读性维护性很高，出错率也低。隐式转换函数虽然代码的书写很简单，但是隐式类型转换容易造成使用者的困扰。RAII class 并不是为了封装某物或代码简洁而存在的，他是为了确保一个特殊行为：资源的释放会发生。所以使用显示的转换更优。无论采用什么转换，都要清楚的知道，转换后，会产生同一份资源的副本。例如智能指针，如果显示转换的返回值被释放了，智能指针则会出错。反之亦然。

- [ ] APIs往往要求访问原始资源，所以每一个RAII class 应该提供一个“取得其所管理的资源”的方法。
- [ ] 对原始资源的访问可能经由显示的转换和隐式的转换，一般而言显示的转换比较安全，可读性高。但是隐式的转换更加方便。

### 条款16：成对使用new和delete时要采取相同的形式

​		没啥好说的，数组指针和对象指针是两种内存形式，析构时必须区别对待。

​		特殊说明：最好不要对数组形式做typedef动作。

```c++
typedef std::string AddressLines[4];    // std::string[4] 被重定向为 AddressLines。

std::string* pal = new AddressLines;   // 等价 new std::string[4];
```

- [ ] 如果你在new表达式中是用了[ ]，必须在相应的delete表达式中是用[ ].如果new没有使用[ ],相应的delete也一定不要使用[ ].

### 条款17：以独立语句将newed对象放入智能指针

​		指针是指针类型，智能指针是对象类型。只是行为表现上像一个指针。

​		假设如下代码：

```c++
processWidget(shared_ptr<Widget>(new Widget), priority());
```

​		processWidget函数第一参数为智能指针对象，第二参数为priority( )函数返回值。这个语句的参数初始化，必须完成以下三件事：

​		1.调用priority( );

​		2.new Widget;

​		3.shared_ptr 构造智能指针

​		但是编译器以什么样的顺序完成以上工作是很灵活的。因此，如果以2，1，3的顺序初始化入参。如果priority( )抛出了异常，则会造成new Widget的资源泄露。因为资源还没有放入智能指针中。

​		为确保指针与智能指针结合之间没有其他（可能抛出异常）语句的穿插。采用以下语句顺序：

```c++
shared_ptr<Widget> pw(new Widget);   // 保证资源与智能指针的结合没有被中断

processWidget(pw, priority());
```

- [ ] 以独立的语句将newed对象存储于智能指针内，如果不这样做，一但异常被抛出，有可能导致难以察觉的资源泄露。

## 第四章 设计与声明

​		让接口容易被正确使用，不容易被误使用。包括正确性、高效性、封装性、维护性、延展性、以及协议的一致性。

### 条款18 ：让接口容易被正确使用，不易被误用

​		假如你要开发一个日期class：

```c++
class Date{
public:
	Date(int month, int day, int year);
	...
}
```

​		Date构造函数的参数类型相同，使用时如果填错顺序不易发现。产生BUG后不易追查。此时采用新增类型即可解决：

```c++
struct day{
    explicit day(int d) : val(d) {}
    int val;
}
```

​		**明智审慎的导入新类型对预防“接口被误用”有神器疗效。**

​		一旦正确的类型可以保证之后，限制其值有时候就显得十分重要。例如month，只能取1-12之间的数。比较安全的做法是预先定义所有的有效的month。

```c++
class month{
public:
    static month Jan( ){ return month(1);}
    static month Feb( ){ return month(2);}
    ...
    static month Dec( ){ return month(12);}
    ...
private:
    explicit month(int m) : val(m) {}
    int val;
}
```

​		除非有好理由，否则应该尽量令你的types的行为与内置的types一致。即自定义的新类型的行为应该与内置类型 int，float等的相同行为如 +，* 等操作的行为表达一致。

​		一致性能使接口被更正确的被调用。STL容器的接口十分一致。例如每个容器都有size()接口。

​		任何接口如果要求用户必须记得做某事，就是有着“不正确使用”的倾向，因为用户可能忘记做。例如工厂模式中的返回值，返回一个抽象基类的指针，需要用户自己delete掉。用户可能忘记delete或者多次delete，这样都不安全。安全的做法是，返回抽象基类指针的智能指针对象。将资源交由智能指针管理。

​		tr1::shared_ptr 有一个特别好的性质：他会自动的使用它的“每个指针专属的删除器”，因而消除另一个潜在的客户错误，所谓的“cross-DLL problem”。这个问题发生于“对象在动态链接程序库DLL中被new构建，却要在另一个DLL中被delete”。在许多平台，这类跨DLL值new/delete成对使用，会导致运行期错误。shared_ptr没有这个问题，因为他的缺省值删除器就是来自诞生这个shared_ptr指针的DLL的delete。

​		shared_ptr固然对代码的正确使用提供了很多保证，但是也有其弊端。如智能指针是原始指针的两倍大，virtual形式调用删除器，在多线程中修改引用次数需要承受线程同步化的开销（锁）。在很多应用程序中这些额外的开销并不显著，但是降低客户的错误成效却很显著。

- [ ] 好的借口容易被正确使用，不容易被误用。你应该在你的所有接口中努力达成这些性质。
- [ ] “促进正确使用” 的办法包括接口的一致性，以及与内置类型的行为兼容。
- [ ] “阻止误用” 的办法包括建立新类型、限制类型上的操作，束缚对象的值，以及消除用户的资源管理责任。
- [ ] tr1::shared_ptr 支持定制型删除器。这可以防范DLL问题，可被用来自动解除互斥锁（条款 14）等系统资源。

### 条款19 : 设计class犹如设计type

​		C++程序员许多时间就是用来扩张你的类型系统，这意味着你并不只是class设计者，还是type设计者。重载函数和操作符、控制内存分配和归还、定义对象的初始化和终结...全部在你的手上。因此你应该带着和“语言设计者当初设计内置类型时”一样的谨慎来设计class。

​		**新的type对象应该如何被创建和销毁****。**即构造和析构以及内存的分配与释放，前提是你打算撰写他们。

​		**对象的初始化和对象的赋值该有什么样的差别。**这个答案取决于你的构造函数与赋值操作符的区别，他们是不同的函数。

​		**新type的对象如果被passed-by-value，意味着什么？**请记住，copy构造函数用来定义一个type的pass-by-value该如何实现。**swap 机制**

​		**什么是新type的合法值？**对class成员变量来说，通常只有某些数值是有效的。数值集决定了你的class的约束条件，也决定了你的构造函数，赋值操作符，和所有的set函数必须进行错误检查。他也影响函数抛出异常、以及函数异常明细列。

​		**你的新type需要配合某个继承图系吗？**如果你继承自某些既有的class，那你的class就受到那些已有的class的限制，特别是他们的 virtual和non-virtual函数的影响。如果你允许其他class继承你的class，那会影响你所声明的函数，尤其是析构函数是否为virtual。

​		**你的新type需要什么样的转换？**如果你希望允许类型T1转换为类型T2，就必须在class T1 内写一个类型转换函数，或在class T2 内写一个单一实参T1的构造函数，根据具体情况实现是否使用 explicit 关键字。

​		**什么样的操作符和函数对新type而言是合理的？**条款23、24、46.

​		**什么样的标准函数应该驳回？**那些正是你必须声明为private的。

​		**谁该取用新type的成员？**决定成员函数和属性的public，protected，private。决定哪个class，function时friend，以及将他们嵌套于另一个之内是否合理。

​		**什么是新type的未声明接口？**他对效率，异常安全性（条款29）以及资源运用（锁，内存...）提供何种保证。你在这些方面提供的保证将为你的class实现代码增加相应的约束条件。

​		**你的新type有多么的一般化？**新的type或许可以衍生出一个type家族，尝试使用 class template。

​		**你真的需要一个新的type吗？**

- [ ] class的设计就是type的设计，在定义新的class之前，请确定你已经考虑过上述主题。

### 条款20 : 宁以pass-by-reference-to-const替换pass-by-value

​		为避免构造函数和析构函数的成本，采用pass-by-reference-to-const。这种传递方式效率最高，没有任何构造函数或析构函数被调用。注意是const修饰的不会被函数修改。除非有修改需求。

​		以by-reference方式传递参数可以避免slicing（切割）问题。by-reference触发多态，切割问题及没有动态调用子类函数。

​		如果窥视c++编译器底层，可知reference往往以指针的形式实现，因此pass-by-reference意味着传递的是指针。指针大小在系统内是固定的32bit OR 64bit(根据系统而定),内置类型一般占用内存较小（小于64bit）。因此内置类型，以及STL的迭代器和函数对象采用pass-by-value方式比较合适。但是并不是对象比较小就采用pass-by-value，对象占内存小，不代表copy构造函数不昂贵。

​		即使小型对象拥有的copy构造函数并不昂贵，效率上仍然有争议。某些编译器对待内置类型，和用户自定义类型的态度是截然不同的。即便两者的底层描述一样，编译器拒绝把只有一个double组成的类放进缓存器内，确很乐意在一个光秃秃的double上那么做。

​		另一个自定义类型不必使用pass-by-value的原因是，自定义类型其大小是容易变化的，通常会变大。

- [ ] 尽量以pass-by-reference-to-const替换pass-by-value。前者通常比较高效，并且避免切割问题（多态被阉割问题）。
- [ ] 以上规则并不适用于内置类型，以及STL的迭代器和函数对象。对他们而言，pass-by-value比较适当。

### 条款21 : 必须返回对象时，别妄图返回其reference

​		local static上的对象在函数返回时会被销毁，如果返回其指针或者引用都是应该被禁止的。

​		妄图返回new出的对象的引用也是不安全的（如果返回值作为临时匿名对象将无法delete对应的内存）。

```c++
const Rational& operator* (const Rational &lhs, const Rational &rhs) {
    Rational* result = new Rational(lhs.nxxx*rhs.nxxx, lhs.dxxx*rhs.dxxx);
    return result;
}
Rational x, y, z;
Rational &w = x * y;		// delete &w可以释放内存
Rational &u = x * y * z;  	// 内存泄露 delete u 智能释放后一次的内存而前一次的匿名对象无法被释放
```

​		~~如果返回local static对象，则面临多线程的安全问题，以及无法保存临时匿名对象结果（如果需要比较两次operator* 的临时结果）。~~

​		我们做的一切都是希望避免对象构造和析构的成本，但是这是无法从变成语言角度避免的。别忘c++编译器拥有无上的权限，他可以做这个方面的编译优化事实上大多数编译器也是这样做的。

- [ ] 绝对不要返回一个指向local stack对象的pointer或reference，或者返回一个reference指向heap-allocated对象（可能造成内存无法delete）。或者返回一个pointer和reference指向一个local static 对象而这个对象可能同时需要多个。local static对象在多线程中写时需要特殊考虑。

### 条款22 : 将成员变量声明为private

​		语法的一致性，类的所有访问都采用函数的方式，都采用小括号。使用函数可以让你对变量的处理更加精确：不准访问，只读访问，读写访问，甚至是只写访问。

​		封装性，如果你通过函数访问成员变量，日后可改为某个计算替换为这个成员变量，而class客户一点也不需要知道这个变化。例如相同功能因为追求效率或内存使用的不同而采取的两种实现方式。

​		将成员变量隐藏在借口背后，使得成员变量被读或被写时能轻松通知其他对象，可以验证class的约束条件以及函数的前提和事后状态，可以在多线程中执行同步控制等等。

​		如果一个public成员变量被删除多少代码会受到影响呢？答案是所有使用它的客户代码都会被破坏。public完全没有封装性，不封装意味着不可以改变。而一个protected成员变量被删除的影响呢？答案它是所有的子类都会被影响。因此public和protected都不能做到封装。

​		从封装的角度观之，其实只有两种访问权限：private（提供封装）和其他（不提供封装）。

- [ ] 切记将成员变量声明为private。这可赋予客户访问数据的一致性、可细微划分访问控制，允许约束条件获得保证，并提供class作者以允许的实现弹性。
- [ ] protected 并不比 public 更具有封装性。

### 条款23 : 宁以non-member、non-friend替换member函数

​		**慎重辨别此条款，不可一棒子打死。应根据类的属性和必要行为一起来决定！！！**

​		面向对象守则要求，数据 以及操作数据的那些函数应该被捆绑在一块，这意味着它建议member函数是较好的选择。不幸的是这个建议并不正确。这是基于面向对象真是意义的误解，面向对象守则要求数据应该尽可能被封装，然而与直观相反的，member函数带来的封装性比non-member，non-friend函数低。

​		封装：如果某些东西被封装，那他就不可见。愈多东西被封装，愈少人可以看到。而愈少人可以看到，我们就对他有愈大的弹性改变他。它使我们能够改变事物而影响有限用户。愈少的代码可以看到数据，愈多的数据被封装，而我们就越能自由的改变变量的数量和类型等。愈多函数可以访问它，数据的封装性愈低。

​		如果你要在一个member函数和一个non-member，non-friend函数之间做抉择，而这两个函数的功能相同。那么non-member，non-friend函数的封装性更好。friend和member函数都会破坏类的封装性。

​		我们暂且将以non-member，non-friend实现某功能的函数称为，便利函数。便利函数和其操作的类在同一个namespace中，但不在同一个文件内（便利函数根据类型也可以分多个文件）。实现编译的解耦。如同stl标准库，这样用户在使用中就可以只编译自己使用到的便利函数文件。如果相同功能的函数定义为member函数，则必须整体编译，应为类不能被分割为片片断断。

- [ ] 宁可拿non-member non-friend 函数替换member函数。这样做可以增加封装性，包裹弹性和机能扩充性。

### 条款24 : 若以有参数皆需类型转换，请为此采用non-member函数

​		令class支持隐式转换通常是个坏主意，当然这个也有例外。最常见的例外是建立数值类型时，假设设计有理数类，允许整数隐式转换为有理数是合理的要求。就像c++内置类中int向double的隐式转换。

```c++
class Rational{
public:
    Rational(int numerator = 0, int denominator = 1);  // 整数隐式转换为Rational类型
    int numerator() const;
    int denominator() const;
private:
    ...
}
```

​		现在使Rational类支持乘法运算，如果将 operator* 作为member函数。

```c++
class Rational{
public:
    ...
    const Rational operator* (const Rational &rhs) const;
private:
    ...
}
```

​		此时将两个Rational相乘是没问题的，但是我们也许需要他和其他可隐式转换的函数相乘。例如：

```c++
Rational oneHalf;
Rational result0 = oneHalf * 2;   // OK     reuslt0 = oneHalf.operator(Rational(2));  此处2倍隐式转换了
Rational result1 = 2 * Rational;  // ERROR	result1 = 2.operator(oneHalf);			  这里也有隐式转换只是转换失败
```

​		编译器在遇到第二种情况时，还会继续寻找 result = operator（2，oneHalf）函数。找不到即报错。

​		在此种需要类型转换的情况下，采用non-member函数，这样编译器就会匹配到能调用的函数。

```c++
const Rational operator* (const Rational &lhs，const Rational &rhs) {
    return Rational(lhs.numerator() * rhs.numerator(), lhs.denominator() * rhs.denominator());
}
```

​		operator* 函数是否应该成为Rational的friend函数？

​		就本例而言，Rational的public接口完全可以完成以上任务，因此不需要设计为friend函数。member函数的反义词时non-member函数不是friend函数。在有可能的情况下，应该尽可能的避免friend函数。

- [ ] 如果你要为某个函数的所有参数（包括this指针所指的那个隐喻参数）进行类型转换，那么这个函数必须是个non-member。

### 条款25 : 考虑写出一个不抛出异常的swap函数

​		**本节致力于实现一个高效且不抛出异常的swap函数，如果你的编译环境支持c++11，并且你为的所有自定义类（pimpl ）实现了一个不抛出异常的移动构造函数，则可跳过本节。因为在c++11中的swap采用了std::move函数，来高效的实现swap机制。**

​		本章所描述的都是在不支持c++11 的情况下如何实现一个高效且接口友好的swap函数。**function template不支持偏特化。**

​		如果swap缺省版的效率不足时通常你的class或者template使用了pimpl 机制：

1. 提供一个public swap成员函数，让他高效的swap你的类型，并且这个swap不可以抛出异常（不抛异常的swap是强烈异常安全性的保障：条款29）。
2. 在你的class或者template所在的名命空间内提供一个non-member swap，并使其调用member swap。
3. 如果你在编写一个class而非class template，为你的class特化std::swap.并使其调用你的member swap。
4. 最后，如果你调用swap，请确保包含一个using声明，以便让std::swap在你的函数内曝光可见，然后不加任何namespace修饰的，赤裸裸的调用swap。

- [ ] 当std::swap对你的类型（通常为pimpl “pointer to implementation”）效率不高时，提供一个swap成员函数，并确定这个函数不抛出异常。
- [ ] 如果你提供一个member swap，也该提供一个non-member swap 用来调用前者。对于classes（而非template），也请特化std::swap。
- [ ] 调用swap时应该针对std::swap使用using声明式，然后调用swap并且不带任何“命名空间资格修饰”。
- [ ] 为“用户定义类型”进行std template全特化是最好的，但千万不要尝试在std内加如某些对std而言全新的东西。

## 第五章 实现

### 条款26：尽可能延后变量定义式的出现时间

​		只要你定义一个变量而其类型带有一个构造函数或析构函数，那么当程序的控制流到达这个变量定义式时，你便要承受变量的构造成本，当这个变量离开作用域时，你便要承受它的析构成本。

​		应该在变量被使用时才被定义和者初始化。因为如果变量定义与2变量使用之间如果有异常产生或者因为某种条件提前返回。则创建出的变量是没有被使用，但是却承担了它的构造和析构成本。

```c++
std::string encryptPassword(const std::string& password){
    std::string encrypted;
    if(password.length() < MinPasswordLength){
        throw logic_error("password is too short");  // 抛出异常提前返回，亦然承担encrypted的构造和析构
    }
    ......
    return encrypted;
}
```

​		你不应该只是延后变量的定义，直到非要使用该变量为止。甚至应该尝试延后到这个变量能够给他初始实参为止。这样不仅可以避免构造和析构无关的对象，还可以避免无意义的无参构造函数行为。

​		关于循环：

```c++
// A 方案
Widget w;
for(int i = 0; i < N; i++){
    w = xxx;
    ...
}

// B 方案
for(int i = 0; i < N; i++){
    Widget w(xxx);
    ...
}
```

**A消耗 = 一次构造 + 一次析构 + n次赋值操作（或许还隐藏n次的临时对象构造析构（赋值操作））**

**B消耗 = n次构造 + n次析构**

​		从效率考虑，可以根据赋值操作和析构加构造的效率比较入手。在A方案中，w的作用域变大了。程序的可理解性和易维护性降低。除非确认A效率高，并且你在做效率高度敏感的代码，否则采用B方案。

- [ ] 尽可能延后变量定义式的出现。这样做可增加程序的清晰度并改善程序效率。

### 条款27：尽量少做类型转换

​		C++提供四种新式转型（相比于C语言的类型强转）。c++中旧式转换依然合法，但是推荐使用新式的类型转换。

const_cast<T>: 通常被用来将对象的常量属性移除。

dynamic_cast<T>: 安全的向下类型转换（基类转派生类）。唯一一个无法采用旧式转换的动作，通常耗费巨大的转换运行成本。

reinterpret_cast<T>: **慎用**意图执行低级转型，实际动作可能取决于编译器，因此不可移植。（不受限在各种类型间转换，但是必须清楚自己在做什么。否则出现位置错误。）(类型转换主要用在指针与整数之间的转换，reinterpret_cast在转换成其他类型的目的只是临时隐藏自己，正正想要使用这个值的时候，必须在转换为其原来类型。类似C语言线程传参，传入任何struct指针都必须以void*传入，接收到指针变量后再强转为真实的类型，完成API接口的调用)

static_cast<T>: 用来强迫隐式转换。（旧式转换大部分都可以采用static_cast替换（建议替换））。

​		优良的c++代码很少使用转型，尽量避免转型。尤其在效率优先的系统中避免dynamic_cast类型转换。（之所以使用dynamic_cast通常是因为你想在一个你认为derived class 身上执行derived class的操作函数，但你手上只有一个base class的pointer或者reference）采用父类虚函数指针的多态来避免类型的向下转型。

- [ ] 如果可以，尽量避免转型，特别是在注重效率的代码中避免dynamic_cast。更改设计方案。
- [ ] 如果转型是必要的，试着将它隐藏于某个函数背后。客户随后可以调用该函数，而不需要将转型放进他们自己的代码内。
- [ ] 宁可使用c++style新式转型，不要使用旧式转型。前者很容易辨识出来，而且也比较有着分门别类的职责。

### 条款28：避免返回handles指向对象的内部成分

​		reference、指针和迭代器统统都是handles，返回一个代表对象内部数据的handle，随之而来的便是降低封装性。你也绝不应该令成员函数返回一个指针指向“访问级别较低”的成员函数。如果你这么做了，后者的实际访问权限就被提高到前者的级别。因为客户可以通过这个指针调用低访问级别函数。

​		在一些场景需要返回handles时，可以将返回值加 const 修饰。让其只有访问权限而没有涂写权限。保护数据的封装性。

​		尤其需要注意，临时（匿名）对象在返回内部成分的引用时。临时对象随时可能析构，造成返回的handle无效（虚吊）。**为了避免这种情况，不要返回handles指向对象的内部成员。除非你仔细考虑过本条款。**

- [ ] 避免返回handles（包括reference、指针、迭代器）指向对象的内部。遵守这个条款可增加封装性，帮助const成员函数的行为像个const，并将发生“虚吊号码牌”（dangling handles（失效的内存访问指针或者变量引用））的可能性降至最低。

### 条款29：为“异常安全”而努力是值得的

异常被抛出时，带有异常安全性的函数会：

1，**不泄露任何资源**。尤其注意动态内存，互斥锁，文件描述符，socket链接，数据库链接等系统资源。

2，**不允许数据破坏**。在抛出异常后，函数不会破坏原有的数据，不会修改程序状态。

​		解决资源泄露的问题，可参考条款13如何以对象管理资源，条款14使用Lock class作为一种确保互斥器被及时释放的方法。构造函数上锁，离开作用域自动调用析构函数。

​		解决数据破坏的问题：

​		**基本保证**：如果异常被抛出，程序内的任何事物仍然保持在有效的状态下（不会出现无效属性）。没有任何对象或数据结构会因此而破坏，所以对象都处于一种内部前后一致的状态。例如，文件打开失败时，不会返回无效的文件描述符等。

​		**强烈保证**：如果异常被抛出，程序状态不改变。调用这样的函数，如果函数成功了，就是完全成功，如果失败了，就会回退到调用前状态。

​		基本承诺在异常时虽然，能保证数据合法，但是可能处于合法的任何一个状态，依然是不可控的。而强烈保证，在异常时则可以准确的知道程序数据的状态。是一种使用起来更容易的函数。

​		**不抛掷保证**：承诺绝不抛出异常，因为它们总是能够完成它们原先承诺的任务。

```c++
int doSomething() throw();				// 明确指明不会抛出异常
```

​		这并不是说doSomething绝不会抛出异常，而是说如果doSomething抛出异常，将会是严重错误，会有你意想不到的函数被调用。函数的声明式并不能够告诉你是否它是正确的、可以值的或者高效的，也不能告诉你它是否提供任何异常安全性保证。所有的那些性质都是由函数的实现决定，无关乎声明。



​		参考条款13可以采用智能指针shared_ptr来进行资源管理，调用shared_ptr的reset()函数，reset()函数只有在参数执行成功后才会被调用，在reset()函数内会对原来的指针进行delete。保证了 new class的成功。new 操作会调用内存申请，有机会产生 bad_alloc 异常。

​		copy and swap 的设计策略很典型地会导致强烈保证，其原则很简单：为你打算修改的对象作出副本，然后在那副本身上做一切必要修改。若有任何修改动作抛出异常，原对象扔保持未改变状态，待改变成功后再一个不抛出异常的操作值置换（swap）。

```c++
struct PMImpl{
    shared_ptr<Image> bgImage;
    int imageChanges;
};
class PrettyMenu{
    ...
private:
    Mutex mutex;
    shared_ptr<PMImpl> pImpl;
};
void PrettyMenu::changeBackground(std::istream &imgSrc){
    using std::swap;
    //采用对象锁 
    Lock ml(&mutex);
    //创造副本并且做必要修改
    shared_ptr<PMImpl> pNew(new PMImpl(*pImpl));
    pNew->bgImage.reset(new Image(imgSrc));
    pNew->imageChanges++;
    // 采用指针交换，指针交换不会对对象本体进行再次的构造和析构，只需要交换地址指针即可
    swap(pNew, pImpl);     
}
```

​		如果异常安全函数fun()，中调用多个异常安全函数fun1(),fun2()。则fun1，fun2，和fun中异常安全等级（基本承诺< 强烈保证<不抛掷保证）最弱的即为fun的异常安全保证等级。并且当在fun1和fun2都为强烈保证时，如果fun1成功，fun2异常。程序状态会在任何方面被fun1改变而无法回到fun调用之前。例如fun1进行了数据库（非局部性数据）的修改。但如果fun1只操作局部性状态，例如只影响调用者的状态，则相对容易提供强烈保证。

​		另一个主题失效率，copy-and-swap的关键在于创建对象数据副本，然后在一个不抛异常的函数中完成置换，因此必须为每一个即将被改动的对象做出一个副本，这将耗用时间和空间，虽然大家都希望提供强烈保证，当它可被实现时你应该提供它，但是强烈保证并非在任何时刻都显得实际。当强烈保证不实际时，你必须提供基本保证。显示中你或许会发现，可以为某些函数提供强烈保证，但是效率和复杂度带来的成本会是他对许多人而言摇摇欲坠。                                                                                                                                                                                     

- [ ]  异常安全函数即使发生异常也不会泄露资源或允许任何数据结构的破坏。这样的函数区分为三种可能的保证：基本型，强烈性，不抛异常型。
- [ ] “强烈保证”往往能够以 copy and swap 实现出来，但是“强烈保证”并非对所有函数都可实现或者具备现实意义。
- [ ] 函数提供的“异常安全保证”通常最高只等于其所调用之各个函数的“异常安全保证”中的最弱者。——木桶原理。

### 条款30：透彻了解inline的里里外外

​		从可执行二进制程序的角度理解inline，inline函数和宏函数是一样的。根本不存在这个函数，因此取地址是错误的。

​		inline函数动作更像函数，比宏好很多条款2。免去函数的调用成本，又能像函数一样做参数类型的检测。

​		编译器最优化机制通常被设计来浓缩那些不含有函数调用的代码，所以当你inline某个函数时，编译器就有对这部分代码优化的可能。

​		当然inline函数造成的二进制膨胀。亦会导致额外的换页行为，降低指令告诉缓存装置的击中率（catch miss），以及伴随而来的效率问题。

​		记住，inline只是一个申请，不是强制命令。这项申请可以隐喻提出也可以明确指定。类内实现的函数为隐喻inline，显示inline为函数加inline修饰。

​		inline的实现方式，某些构建平台在连接时进行inline，少量构建环境在运行期间完成inline。大多数的c++构建在编译器完成inline行为。因此inline一般在头文件中实现。Templates通常被置于头文件内，因为他一旦被使用，编译器为了将他具现化，要知道它长什么样子。

​		大部分编译器会拒绝复杂（包含循环、递归）的inline函数。virtual函数一般不是inline函数，因为virtual是运行时动态绑定的。而inline函数要在编译时确定。编译器会根据具体情况来决定inline函数是否真的inline，大多数编译器提供诊断级别，如果不能inline指定的inline函数，将会报警告。（采用  -Winline 编译选项）

​		实际上inline函数没有地址，根本不存在。因此采用指针的形式调用inline函数时，对编译器会产生二异性，编译器不知道将其编译成inline还是non-inline。

​		当你创建一个对象时，构造、析构函数可能非常简单。但是这并不意味着将他们inline是一个好的选择。因为一个构造和析构的执行设计到，这个对象所有父类，所有内部成员的构造和析构，这些完全打开将可能产生巨量的代码。

​		对于程序库的设计，inline函数将无法随着库的升级而升级。如果程序库采用动态库连接，升级库函数可以主程序不知不觉中进行。

​		大部分调试器对inline函数束手无策，因为你无法对一个不存在的函数设置断点。因此许多构建环境在调试时，会采用non-inline模式。

​		虽然inline对执行效率有一定的提升，但是使用时也要谨慎添加。在一开始不加inline，后续优化中在根据分析慢慢添加。

- [ ] 将大多数inline限制在小型、被频繁调用的函数身上。这可使日后的调试过程和二进制升级（）更容易，也可使潜在的代码膨胀问题最小化，使程序的速度提升机会最大化。
- [ ] 不要只因为function template出现在头文件，就将他们声明为inline。

### 条款31：将文件间的编译依存关系降至最低  （首先服务于代码的实现逻辑，类的实现关系，文件的组织协调，最后在考虑编译效率）

​		提高项目的编译速度，将“声明的依存性”替换为“定义的依存性”。尽量将类成员属性定义为指针或者引用。

## 第六章 继承与面向对象设计

​		OOP不再是一项用来划分语言特性的仪典，而是可以让你通过它说出你对软件系统的想法。

### 条款32：确定你的public继承塑模出is-a关系

​		C++进行面向对象编程中，最重要的规则就是 public inheritance（公开继承）意味着“is-a”的关系。如果你令class D以 public inheritance 继承class B，那么每一个D对象同时也是一个B对象，反之则不成立。

​		**世界上并不存在一个适用于所有软件的完美设计。所谓最佳设计，取决于系统希望做什么事，包括现在和未来。**

​		在class进行 public 继承事，必须清楚认识基类和子类的关系。确保public继承继承是没有问题的，不要仅仅使用常识判断。要从基类子类的实现上判断。

​		is-a并非唯一存在于classes之间的关系，另两个常见的关系是 has-a 和 is-implemented-in-terms-of（根据某物实现出）关注条款38，39。

- [ ] “public继承”意味着 is-a。适用于base classes身上的每一件事情一定也适用于derived classes身上，因为每一个derived class对象也都是一个base class对象。

### 条款33：避免遮掩继承而来的名称



### 条款34：区分接口继承和实现继承

### 条款35：考虑virtual函数以外的其他选择

### 条款36：绝不重新定义继承而来的non-virtual函数

### 条款37：绝不重新定义继承而来的缺省参数值

### 条款38：通过复合塑造出has-a或者“根据某物实现出”

### 条款39：明智而审慎的使用private继承

### 条款40：明智而审慎的使用多重继承

## 第七章 模板与泛型编程

## 第八章 定制new和delete

## 第九章 杂项讨论