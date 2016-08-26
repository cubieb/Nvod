#ifndef _ClassFactories_h_
#define _ClassFactories_h_
#include <map>
#include <functional>

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/Singleton.h"

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
        typedef ACE_Singleton<MyType, ACE_Recursive_Thread_Mutex> TheSingleton;
        return *TheSingleton::instance();
    }

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

#pragma warning(pop)
#endif