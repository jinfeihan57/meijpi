[TOC]



# 改善程序与设计的55个具体做法

## 第一章 让自己习惯 C++



### 条款01：视C++为一个语言的联邦

### 条款02：尽量以const，enum，inline，替换 #define

### 条款03：尽可能使用const

### 条款04：确定对象被使用前已先被初始化

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

​		auto_ptr和shared_ptr都是在其析构函数内做delete而不是delete[ ] 动作，因此这两个智能指针都不能在动态分配的array身上使用。

```c++
shared_ptr<int> spi(new int[10])   // 虽然编译器不会报错，但是这样做显然不对
```

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

## 第四章

## 第五章 实现

### 条款26：

### 条款27：

### 条款28：

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

### 条款30：

### 条款31：

## 第六章

## 第七章 

## 第八章 

## 第九章 