
几年前看《Effective C++》知道我们一般写下：
Type type = new Type();
这样的句子，实际执行的两步：
申请内存空间
在内存空间上构造对象
      看到placement new操作符之后，我错误地以为其中的两步，都被placement new代替了。甚至错误地认为后面的一步是直接用新的对象，使用了类似拷贝构造函数那样的二进制拷贝做的（这么想都是因为读了《深度探索C++对象模型》才这么想的，看来不仔细读书，以至于误读了书，比不读书的害处少不了多少啊！），所以才省却了内存分配。这几天捡起《Exceptional C++》来看的时候，读到异常安全这块儿，突然想起一件事来，我们使用
new Type（）；
这样的句子实际上是向系统申请了sizeof(Type)大小的内存。但是我们知道，如果这个类的内部又有一个指针，这样就有需要一个新的堆数据，那么上面的句子申请到的一般只是sizeof(int)大小的空间，而我们需要的可一般不是这么大小的。这样的话，只能这样来解释了：
在我们的程序中使用上面的句子申请内存的时候，得到的只是sizeof(Type)的大小，而对象内部还需要什么其他（需要深拷贝）的内存，就需要类的构造函数来申请空间了。所以不管是new，还是placement new，都是需要调用构造函数的。也就是说placement new其实只省却了前述两步中的第一步。所以即使使用placement new操作符，有时候（具体看类的实现）还是需要申请新的空间的。
      下面看一个我学习这部分内容的时候写的代码，看到打印的结果就可以知道怎么回事了：

```C++
#include <iostream>
using namespace std;
class TNew
{
public:
    TNew():m_pRes(0){
        cout << "TNew()" << endl;
    }
    ~TNew(){
        cout << "~TNew()" << endl;
        if (m_pRes)
            delete m_pRes;
        m_pRes = NULL;
    }
    TNew(TNew const& rhs){
        cout << "TNew(TNew const& rhs)" << endl;
        init(rhs);
    }
    TNew& operator = (TNew const& rhs){
        cout << "operator = (TNew const& rhs)" << endl;
        if (this != &rhs)
            init(rhs);
        return *this;
    }
    int* get()const{
        cout << "get() const" << endl;
        return m_pRes;
    }
    int* get(){
        cout << "get()" << endl;
        return m_pRes;
    }
    static void* operator new(std::size_t size)throw(){
        cout << "normal operator new" << endl;
        return ::operator new(size);
    }
    static void* operator new(std::size_t size, void* ptr)throw(){
        cout << "placement new" << endl;
        return ::operator new(size, ptr);
    }
    static void* operator new(std::size_t size, const std::nothrow_t& nt)throw(){
        cout << "nothrow placement new" << endl;
        return ::operator new(size, nt);
    }
    static void operator delete(void* pMem)throw(){
        cout << "normal operator delete" << endl;
        ::operator delete(pMem);
    }
    static void operator delete(void* pMem, void* ptr)throw(){
        cout << "placement delete" << endl;
        ::operator delete(pMem, ptr);
    }
    static void operator delete(void* pMem, const std::nothrow_t&)throw(){
        cout << "nothrow placement new" << endl;
        ::operator delete(pMem);
    }
private:
    void init(TNew const& rhs){
        if (rhs.m_pRes){
            if (m_pRes) delete m_pRes;
            m_pRes = new int;
        }
    }
private:
    int* m_pRes;
};
int main()
{
    cout << "TNew * p = new TNew(); called:" << endl;
    TNew * p = new TNew();
    void* pBuf = malloc(sizeof(TNew));
    cout << "TNew * p2 = new (pBuf)TNew; called:" << endl;
    TNew * p2 = new (pBuf)TNew;
    cout << "TNew * p3 = new(std::nothrow)TNew; called:" << endl;
    TNew * p3 = new(std::nothrow)TNew;//只能保证申请内存不抛出异常，不能保证TNew()中不抛出异常。详见《Effective C++》第49条。因而这个版本的new一般不用。
    cout << "delete p3; called:" << endl;
    delete p3;
    cout << "delete p; called:" << endl;
    delete p;//called normal delete operator
    cout << "p2->~TNew(); called:" << endl;
    p2->~TNew();
    free(pBuf);
    
    cout << "TNew tn1, tn2; called:" << endl;
    TNew tn1, tn2;
    cout << "const TNew ctn; called:" << endl;
    const TNew ctn;
    cout << "tn1 = tn2; called:" << endl;
    tn1 = tn2;
    cout << "tn1 = ctn; called:" << endl;
    tn1 = ctn;
    
    cout << "tn1.get(); called:" << endl;
    tn1.get(); //if non-const version get() exist,call it; else callconst version get()
    cout << "ctn.get(); called:" << endl;
    ctn.get();
    class TCC{
    public:
        TCC():m_i(10), m_d(1.0){}
        int geti(){return m_i;}
        double getd(){return m_d;}
        //private:
        int m_i;
        double m_d;
    };
    void *pbuf = malloc(sizeof(TCC));
    TCC tcc;tcc.m_d = 3.14; tcc.m_i = 30;
    memcpy(pbuf, &tcc, sizeof(TCC));
    TCC* ptcc = reinterpret_cast<TCC*>(pbuf);
    cout << "ptcc->getd() : " << ptcc->getd() << endl;
    cout << "ptcc->geti() : " << ptcc->geti() << endl;
    
    cout << "before exit:" << endl;
    return 0;
}
```

输出结果：

```C++
New * p = new TNew(); called:
normal operator new
TNew()
TNew * p2 = new (pBuf)TNew; called:
placement new
TNew()
TNew * p3 = new(std::nothrow)TNew; called:
nothrow placement new
TNew()
delete p3; called:
~TNew()
normal operator delete
delete p; called:
~TNew()
normal operator delete
p2->~TNew(); called:
~TNew()
TNew tn1, tn2; called:
TNew()
TNew()
const TNew ctn; called:
TNew()
tn1 = tn2; called:
operator = (TNew const& rhs)
tn1 = ctn; called:
operator = (TNew const& rhs)
tn1.get(); called:
get()
ctn.get(); called:
get() const
ptcc->getd() : 3.14
ptcc->geti() : 30
before exit:
~TNew()
~TNew()
~TNew()
Program ended with exit code: 0
```

这个例子分别测试了四部分内容：

1. 各种new/delete操作
2. 拷贝/赋值构造函数（现在还没弄清楚r-value reference的概念，没添加这块C++11的新内容，以后加上）
3. const和非const对象对const和非const成员函数的调用
4. 成员变量中没有指针的类的拷贝

可以看出：
1. 首先是前面的猜测是对的，这里不再重复。
2. nothrow版本的new申请的空间，是被normal delete释放的。
3. 如果const对象调用函数，必须使用const类型的成员函数，否则编译报错，因为不可能让const类型对象调用非const类型的函数；但是非const对象，在没有const类型的成员函数的时候，可以调用const类型的函数（大不了不改变对象内部数据嘛）。这一点试着注释掉get()函数两个版本中的一个就可以知道。

读了《Effective C++》中的第52条（Write placement delete if you write placement new）可以知道，placement new和placement delete的调用时成对的，如果定义了placement new却让运行时找不到对应的placement delete的话，那么就不会释放内存了。

并且，系统内置的标准placement new操作，其实只是直接返回传给它的指针。从这一层意思上来看，使用placement完全是为了提高new的效率，如果没有placement这样一个新的new操作符的话，我们定义类对象就只能以一次申请内存得到一个对象的方式来进行，但是内存频繁申请一方面慢，再者会造成内存碎片（可以使用STL内存池的方式减少碎片）。所以placement new就可以用来在已经申请好大块内存上直接构造自己了。当然如果构造函数还需要去申请内存，那么这将是提高效率的一个考量点，可以在这上面再做做文章；否则构造对象的效率将大大提高，甚至是使用已经有的对象，直接复制到目标内存，就像是例子中的TCC类的例子。

当然，既然placement new使用的内存不是有placement new申请的，那么也不能由placement delete释放，直接delete对象也不能释放这块由对象占用的内存；同样，由构造函数申请的内存也不能由placement delete释放，还是需要由析构函数释放的。所以一旦使用了placement new，那么必然现有一块内存被申请出来，如果这块内存是动态申请的，那么需要在直接调用析构函数之后，再用与申请空间对应的方式释放内存。或者直接delete对象，这样的话，会先调用析构函数，之后再释放这块内存，所以就不能用与申请空间对应的方式再次释放内存了，同时，一旦这么做，那么释放内存的方式，就只能是挨个儿delete每个构造出来的对象了（因为可能将需要释放的内存空间截开），这样整块申请内存，分块释放内存的方式还是值得商榷的。当然如果不是动态申请的，那么就不需要手动释放了。