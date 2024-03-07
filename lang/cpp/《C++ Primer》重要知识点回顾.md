```
1. 引用：
const int ival = 1024;                    
const int &refval = ival;      //可以
int &ref2 = ival;         //错误，
不可以将const对象引用给非const类型变量
int  i = 42;
const int &r = 42;
const int &r2 = r + i;//注意：r和i是不同类型的量，在这里是可以的。
const int &r3 = r + i;//错误
const 引用可以初始化为不同类型的对象或者初始化为右值。
另， const变量默认为文件域变量，那么即使是在一个文件的全局作用域中const量，在其他文件中使用的时候也需要使用extern的方式引用：
就是说非const量默认为extern。
//file1.c
extern const int bufSize = 64;//必须使用extern来声明
//file2.c
extern const int bufSize;//引用过来
但是，事实上我在VC2010和g++下测试了一下，没有这回事，不需要extern就可以直接使用。看来标准是标准，实现是实现，不一码事儿啊！

2. 变量、数组的初始化：
变量和数组的类型若是内置数据类型，那么定义在函数体内没有做初始化，若定义在函数体外，则全部初始化为0.
若是类类型，则调用默认构造函数
对于数组的初始化，若给出的元素个数小于指定的维数，那么只初始化前面几个元素，剩下的，若是内置类型则初始化为0，若是类类型，全部调用默认构造函数。
另，char ca[] = "C++";//null符号自动加在末尾，sizeof(ca)的值为4;

3. void*指针：
void指针可以保存所有类型的指针。
double obj = 3.14;
double * pd = &obj;
void *pv = &obj;//ok
pv = pd;//ok
但是，void*只支持几种有限的操作：a，与另一个指针比较；b，向函数传递void*指针或者从函数返回void*指针；c，给另一个void*指针赋值；d，不允许使用void*指针操纵他所指的对象。
显然，由于void*指针不知道自身的大小，void*指针不支持自增自减操作，不能解引用。
cout << pv++ << ++pv << --pv << pv-- //全部错误    error C2036: 'void *' : unknown size
        <<*pv << endl;//错误

4. typedef和const指针：
typedef string * pstring;
const pstring cstr;
那么请问，cstr是什么类型？
答，非const string*，而是string * const类型。
首先要区分指向const类型的指针和const指针：
const string * pstr; 等价于string const *pstr;都是指向const类型的指针，不可以通过这样的指针修改所指对象的值，原因很简单，const对象不可以被修改，但是指针本身可以被修改，并且，所指对象也可以是非const类型。
而，string *const cstr;指的就是const指针，他是本身不能修改的指针。
    int i = 42;
    const int * p1 = &i;
    int const * p2 = &i;
    int * const p3 = &i;
    p1++;//ok
    p2++;//ok
    p3++;//error

5. 动态数组：
int *p = new int[10];//定义了10个int元素，但是没有做初始化
int *p1 = new int[10]();//定义了10个int元素，并且做了初始化
string *p2 = new string[10];//定义了10个string元素，调用默认构造函数做了初始化
const 对象动态数组：
const int *p3 = new const int[10];//错误
const int *p4 = new const int[10]();//正确

int *p5 = new int[0];//动态分配0个空间是可以的，但是不能对p5做解引用
int a[0];//这是不对的，维数应该不小于1

6. 多维数组：
int ia[3][4];
int (*ip)[4] = ia;//指向数组的指针
ip = &ia[2];//指向ia的第3个元素，一个包含4个int元素的一维数组。

7. 隐式转换：
a，指针转换，多数情况下数组会转化成指针，有以下几种情况例外：
      ○ 数组用作取地址操作符或者sizeof的操作数
      ○ 用数组对数组的引用进行初始化
b，转换为bool型，算术值和指针都可以转化为bool型，若算术值或指针值为0，则bool值为false，其他值则为true。
c，算术类型与bool类型的转换，
      算术类型--------->bool类型  ：    0->false；其他->true
      bool类型--------->算术类型  ：    true->1  ；false->0
d，枚举类型，C++自动将枚举类型的对象或枚举成员转换为整形，其转换结果可以用于任何要求使用整型数值的地方。如：
  enum Points = {point2d = 2, point2w, point3d = 3, point3w};
  const size_t array_size = 1024;
  int chunk_size = array_size * pt2w;
  int array_3d = array_size * point3d;
   将enum对象或枚举成员提升为什么类型有机器定义，并且依赖于枚举成员的最大值。无论其最大值是什么，至少提升为int型，若int型无法表示枚举成员的最大值，则提升到能表示所有枚举成员值的、大于int型的最小类型（unsigned int，long或者unsigned long）
e，转换为const对象，使用非const对象初始化const对象的引用时，系统将非const对象转换为const对象，这个对象指的是内置类型或者类类型，及其对应的指针。
  int i;
  const int ci = 0;
  const int &j = i;      //将非const转换为const int的引用（convert no-const to reference to const int）
  const int *p = &ci; //将非const的地址转化为const地址（convert address of non-const to address of a const）
f，类类型定义的自动执行的类型转换。

8. 数组形参，以下3种方式等价：
  a，void func(int *){}
  b，void func(int []){}
  c，void func(int [10]){}
这3种形式都相当于形参为int *。数组作为实参在传递的时候被转化为了相应的指针类型，这是C语言考虑到效率这样做的，传递实参期间会产生一个临时指针作为数组第一个元素的指针的副本，故而在函数体内改变该指针本身是不会改变数组本身的。
在这里要注意的是，当编译器检查数组形参时，它只会检查实参是不是指针，指针的类型和数组元素的类型是否匹配，而不检查数组的长度。
所以即使在定义数组形参的时候，函数写作：
void func(int a[10]){}
但是在调用的时候，使用：
int i = 0, j[2] = {0};
int k[10] = {0, 1,2,3,4,5,6,7,8,9};
func(&i);               //正确
func(j);                  //正确
func(k);                 //当然正确了！
都是可以的。

9，通过引用传递数组，和其他类型一样，数组也可以声明为引用，如果形参是数组的引用，编译器不会讲数组实参转化为指针，而是传递数组本身。这时，编译器将会检查数组实参的大小与形参的大小是否匹配。
void printValue(int (&arr)[10]){}
int main()
{
   int i = 0, j[2] = {0};
   int k[10] = {0, 1,2,3,4,5,6,7,8,9};
   printValue(&i);               //错误，形参与实参不匹配
   printValue(j);                 //错误，形参与实参不匹配
   printValue(k);                //正确
   return 0;
}

10，内联函数的定义，必须放在头文件中，这一点不同于其他函数。

11， 重载和const形参，当形参是指针或引用时，形参是否为const也可以进行函数重载。当形参为指针时，要注意这里const要修饰不能使指针本身，必须是指针所指向的类型。
Record lookup(Account &);
Record lookup(const Account&);//重载函数
const Account a(0);
Account b;
lookup(a);//const 版本
lookup(b);//非const版本

void f(int *){}
void f(int *const){}//错误，重定义

12，指向函数的指针，函数指针像其他指针一样，函数指针也指向某个特定的类型。函数类型由其返回类型和形参表确定，与函数名无关，不同类型的函数指针之间不存在转换。一般使用typedef来简化函数指针的定义：
typedef bool (*cmpFun)(const string &, const string&);
该定义表示cmpFun是一种指向函数的指针类型的名字，该指针类型为“指向返回bool类型并带有两个const string引用形参的函数指针”。
函数指针的赋值和初始化：在引用函数名，但又没有调用该函数时，函数名将被自动解释为指向函数的指针。故而可使用函数名对函数指针做初始化或赋值：
bool lengthCompare(const string &, const string);
cmpFun pf1 = 0;
cmpFun pf2 = lengthCompare;
pf1 = lengthCompare;
pf2 = pf1;
此时，以下两种形式等价：
cmpFun pf1 = lengthCompare;
cmpFun pf2 = &lengthCompare;
函数指针只能通过同类型的函数或函数指针或0值常量表达式进行初始化或赋值。
通过指针调用函数，有两种方法，他们是等价的：
cmpFun pf = lengthCompare;
lengthCompare("hi", "bye");//直接调用
pf("hi", "bye");//等价调用
(*pf)("hi", "bye");//等价调用
函数指针形参，有两种形式：
a, void useBigger(const string&, const string&, bool(const string&, const string&) );//第三个参数是一个函数，但是将自动作为函数指针对待
b, void useBigger(const string&, const string&, bool (*) (const string&, const string&) );//第三个参数是一个函数指针
返回指向函数的指针，（返回类型可以是函数指针，但不可以是函数）例如：
int (*ff(int))(int *, int);//阅读方式是由里而外
解释：首先看ff(int)，这是一个函数，它有一个int型形参，返回类型是：int (*)(int *, int);它是一个指向函数的指针，所指向的函数返回int型并带有两个分别是int*型和int型形参。
注意区分：
void ff(vector<double>);
void ff(unsigned int);
作为重载函数，在为函数指针赋值、调用的时候要精确匹配，否则就是编译错误。

13，成员函数返回的this指针：对于非const成员函数，返回的this指针是一个指向类类型的const指针，可以改变this指针所指向的值，但是不可以改变this所保存的地址，而const成员函数返回的this指针时一个指向const类类型对象的const指针，既不能改变this所指向的对象，也不能改变this所保存的地址。那么：
Screen myScreen;
myScreen.display().set("*");//display函数为const函数，返回一个this的const引用
其中set函数明显是修改了对象，这是不允许的。

14，基于函数是否为const的重载：
class Screen{
public:
   Screen& display(std::ostream &os){
         do_display(os);  return *this;
   }
   const Screen& display(std::ostream& os) const {
         do_display(os);  return *this;
   }
private:
   void do_display(std::ostream &os)const{
        os << content;
   }
........
};
这时，
Screen myScreen(5, 3);
const Screen blank(5, 3);
myScreen.set("#").display(cout);//调用非const版本
blank.display(cout);//调用const版本
小结，基于成员函数是否为const可以重载一个成员函数（见11），同样，基于一个指针形参是否指向const成员可以重载一个函数。const对象调用const成员，非const对象可以使用任意成员，但非const成员将具有更好的函数匹配。

15，可变数据成员，有时候我们希望类的数据成员即使在const成员函数内也可以修改，这可以将其声明为mutable来实现。

16，隐式类类型转换，这个需要提供合适的单参数构造函数来实现。
同时C++提供了抑制这种隐式转换的方法，将构造函数声明为explicit。

17， static函数没有this指针，static成员函数不能声明为const，也不能声明为虚函数。这是由于static函数属于类而不属于对象，它独立于所有对象而存在， static数据成员必须在类定义体外部定义（正好一次），但是整型的const static数据成员就可以在类的定义体中进行初始化。
另外，static数据成员的类型可以是该成员所属的类类型，非static类型就必须声明为指针或者引用：
class Bar{
public:
...
private:
       static Bar mem1;//Bar类型
       Bar * mem2;//Bar的指针类型
       Bar * mem3;//Bar的引用类型
};
static数据成员可以作为成员函数的默认参数。

18，复制构造函数，在以下情况中会调用复制构造函数：
a，用另一个同类型的对象显式或隐式初始化一个对象
b，复制一个对象将其作为实参传给函数
c，从函数返回时复制一个对象
d，初始化顺序容器中元素
e，根据元素初始化式列表初始化数组元素

19， 如果一个类没有提供默认构造函数，那么：
a， 具有该类型成员的每个类的每个构造函数必须使用显式的参数初始化对象
b， 编译器将不会为具有该类型成员的类合成默认构造函数
c，该类型不可以用作动态分配数组的元素类型
d， 该类型的静态数组必须为每个元素提供一个显式的初始化式
//e， 不能为容纳该类型对象的容器调用给定大小而没有提供初始化式的构造函数。
使用默认构造函数，
newclass obj = newclass();//正确
newclass obj();//错误，这个将会被理解为返回newclass类型的函数obj。

20， 重载操作符。
首先，不能重载的操作符有：  ::、.* 、. 、?:四个；但这并不是说所有其他的操作符就都可以重载了，&&、||、和逗号操作符最好就不要重载。
其次，有四个操作符（+、-、*、&），既可作为一元操作符，也可以作为二元操作符。
最后，重载一元操作符，如果作为成员函数就没有（显式）形参，如果作为非成员函数就有一个形参。以此类推，重载二元操作符，如果作为成员函数就有一个形参，如果作为非成员函数就有两个形参。一般地，算术和关系操作符将定义为非成员函数，而将赋值操作符定义为成员。
    当操作符为成员函数的时候，this指向左操作数。
    操作符定义为非成员函数时，通常将其定义为操作数类的友元函数，这是因为难免要对类类型的成员变量做操作。
    在使用重载操作符的时候，可以使用以下两种方式，他们是等价的：
    cout << item1 + item2 << endl;
    cout << operator+(item1 , item2) << endl;
成员函数or非成员函数？
    有些操作符的重载必须定义为成员函数，有些则定义为非成员函数亦可，但是一些经验指导将指导我们做出好的决定：
    a、赋值（=）、下标（[]）、调用（()）和成员访问箭头（->）等操作符必须定义为成员，否则将会产生编译时错误。
    b、复合赋值操作符通常应定义为类的成员，但并非必须得这样做。
    c、改变对象状态或与给定类型紧密联系的其他一些操作符，如自增、自减和解引用，通常应定义为类成员。
    d、对称的操作符，如算术操作符、相等操作符、关系操作符和位操作符，最好定义为非成员函数。
具体地说：
     e、输出操作符<<的重载（必须作为非成员函数，否则使用起来就应该是obj.cout这样了）：
          一般形式：ostream operator<< (ostream& os, const classtype & object)
                           {
                                 //准备
                                 //实际的输出；
                                 return os;//返回output stream object
                           }
          说明：为了和IO标准库保持一致，操作符应该ostream&作为第一个形参，对类类型const对象的引用作为第二个形参，并返回ostream形参的引用。
        
     f、输入操作符>>的重载（必须作为非成员操作符，原因同上）：
         一般形式：istream& operator >> (istream& is, classtype& s)
                           {
                                //in的错误处理和文件结束处理
                                return in;
                           }
         说明：第一个形参是istream的引用，指向它要读取的流，第二个形参是要读入的对象的非const引用，并且必须是非const，因为要将istream的数据写入到对象中。同时要注意的是，有一点有别于输出操作符，就是输入操作符必须做流错误和文件结束的处理。

     g、算术操作符和关系操作符（一般定义为非成员函数）：
           加法操作符：
           一般形式：classType operator + (const classType& lhs, const classType& rhs)
                             {
                                  claddType ret(lhs);
                                  //do something
                                  return ret;
                             }
          说明：加法操作并不改变操作数的状态，故而形参用const，同时为了和内置操作符保持一致，返回的是一个新的对象，一个右值，而不是一个引用。然而一般的复合操作符返回的是类型是类类型的引用，效率较高，应优先使用。
          相等操作符：
          一般形式：inline bool operator == (const classType & lhs, const classType & rhs)
                           {
                                return （各个对象相等判定）；
                           }
                           inline bool operator != (const classType & lhs, const classType & rhs)
                           {
                                return !(lhs == rhs);
                           }
          说明：正如上面所做的一样，==和!=应该同时出现，且其中一个的实现应该调用另一个。
          关系操作符：
         
     h、赋值操作符（没有的话编译器会尝试合成。必须是类的成员，这样编译器可以知道是否应该自动合成）：
          一般形式：classType& operator = ([const] classType& obj)
                           {
                               .......
                               return *this
                           }
                           classType& operator += ([const] classType& obj)
                           {
                               ......
                               return *this;
                           }
          说明：形参可以是const也可以是非const，返回值最好是左值的引用，这样可以少创建（同时包括销毁）一个临时对象；另外有=的时候，最好也要有+=。其他的同理（-与-=，……）。
     i、下标操作符（必须是类成员）：
          一般形式：classType& operator[](const size_t){}
                            const classType& operator[](const size_t)const {}
          说明：返回为引用是为了使操作数可以是左值，也可以是右值。并且为了给const和非const对象提供合适的形式，应该同时定义const和非const两个版本的函数。
     j、 成员访问操作符（必须是类成员函数）：
          一般形式： classType &operator*() { ......  }
                             classType *operator->() {  ......  }
                             const classType &operator() const { ......    }
                             const classType *operator->()const { ......    }
          说明：如上，应该提供const和非const两个版本，并且const版本的返回值不可以修改，也应该为const类型。重载箭头的返回值必须是指向类类型的指针，或者是定义了自己箭头操作符的类类型对象。如果是类类型指针，则箭头操作可用于该指针，编译器对该指针解引用，并获得结果对象中指定的成员，如果被指向的类型没有定义那个成员，则编译器产生一个错误。如果返回的是类类型的其他对象（或者是这种对象的引用），就递归应用该操作。编译器检查返回对象所属类型是否具有箭头操作符成员，有的话就调用之，没有就产生一个错误。直至返回一个指向带有指定成员的对象的指针；或者返回某些值，但这时代码出错了。
          关于箭头操作符的重载要说的是：他的行为类似二元操作符，接受一个对象和一个成员名，但是，无论如何箭头操作符不接受显示形参。这里由编译器处理获取成员的工作。这样写的时候：point->action();按照优先级，它实际上等价于(point->action)();我们想要调用的是对point->action的求值结果，编译器这样工作：
          如果point是一个指针，指向具有名为action的成员的类对象，则编译器将代码便以为调用该对象的action成员。
          否则，如果action是定义了operator->操作符的类的一个对象，则point->action与point.opertaor->()->action相同。即，执行了point的operator->()，然后使用该结果重复这三步。
          否则，代码出错。
     k、自增、自减操作符（可以不作为类的成员，但是由于其改变类的状态，最好将其作为成员）：
          前自增/前自减操作符：
          一般形式：classType& operator ++ (){ ... }
                            classType& operator -- (){ ... }
          说明：自增、自减操作最好提供边界检查。并且返回操作数的引用，这样和内置类型一致。
          后自增/后自减操作符：
          一般形式：classType& operator ++ (int) {...}
                            classType& operator -- (int)  {... }
          说明：与前者的区别在于那个额外的int型形参（称为函数对象），使用后缀式操作符时，编译器提供0作为该形参的实参。
          最好两种操作符同时定义。
          两种操作符的显式调用：
          obj.operator++(0);//调用后置操作符
          obj.operator++();//调用前置操作符
     l、 调用操作符（必须是类成员）：
          一般形式：suitableType operator() (parameter list) {}
          说明：定义了这个操作符的类的对象，在使用时可以这样来调用：obj(实参)，看起来像是函数，但实际上却是对象对调用操作符的运行。
          函数对象常用于标准库算法，它使用起来，能提供比函数更灵活的操作（毕竟函数对象是可以设置，可以操作的对象，而不是写死了等着调用的代码块）。
     m、转换操作符（必须是类成员）：
           一般形式：operator type() {...}
           说明：type指的是内置类型名，类类型名或者由类型别名所定义的名字。对任何可作为函数返回类型（void除外）都可以定义转换函数。不允许转换为数组或函数类型，但是转换为指针类型（数据和函数的指针）以及引用类型都是可以的。另外，一般类型转换不改变对象的状态，最好将其定义为const成员函数：operator type()const {...}
           显式类型转换使用的既可以是转换操作符，也可以是构造函数。且这两者之间没有高下之分。
           二义性，通常给出一个类与两个内置类型之间的转换是不好的做法。同时要避免编写互相提供隐式转换的成对的类。显示提供构造函数的调用可以消除二义性。
```