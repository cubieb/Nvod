#ifndef _ClassFactories_h_
#define _ClassFactories_h_
#include <map>
#include <functional>

/**********************class ClassFactories**********************/
/*
Template Para: 
    C:   Class.
    I:   Index to find the factory.
    Types: Parameter sent to class factory.
Example: class PlayerInterfaceFactory
*/
template<typename C, typename I, typename ... Types>
class ClassFactories
{
public:
    typedef ClassFactories<C, I, Types ...> MyType;
    typedef std::function<C* (Types ...)> Factory;
    typedef std::map<I, Factory> FactoryMap;

    C* CreateInstance(I index, Types ... args)
    {
        FactoryMap::iterator iter = factories.find(index);
        return iter->second(args ...);
    }

    void Register(I index, Factory factory)
    {
        factories.insert(make_pair(index, factory));
    }

    static MyType& GetInstance()
    {
        /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
        static MyType instance;
        return instance;
    }

private:
    ClassFactories() {}

private:
    FactoryMap factories;
};

/**********************class ClassFactoriesRegistor**********************/
template<typename C, typename I, typename ... Types>
class ClassFactoriesRegistor
{
public:
    typedef ClassFactories<C, I, Types ...> ClassFactoriesType;

    ClassFactoriesRegistor(I index, typename ClassFactoriesType::Factory factory)
    {
        ClassFactoriesType& instance = ClassFactoriesType::GetInstance();
        instance.Register(index, factory);
    }
};

/*
* From boost documentation:
* The following piece of macro magic joins the two
* arguments together, even when one of the arguments is
* itself a macro (see 16.3.1 in C++ standard).  The key
* is that macro expansion of macro arguments does not
* occur in JoinName2 but does in JoinName.
*/
#define JoinName(symbol1, symbol2)  JoinName1(symbol1, symbol2)
#define JoinName1(symbol1, symbol2) JoinName2(symbol1, symbol2)
#define JoinName2(symbol1, symbol2) symbol1##symbol2

#define RegisterClassFactory(registor, var, index, factory)      \
	static registor JoinName(var, __LINE__)(index, factory)

#endif