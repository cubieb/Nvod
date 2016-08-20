#ifndef _PrototypeCache_h_
#define _PrototypeCache_h_
#include <map>
#include <functional>

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/Singleton.h"

/*
Template Para: 
    I:   Id used to find constructor.
    P:   Prototype, created by constructor.
Example: class PlayerInterfaceFactory
*/
template<typename I, typename P>
class RefPrototypeCache
{
public:
    typedef RefPrototypeCache<I, P> MyType;
    typedef I IndexType;
    typedef P PrototypeType;
    typedef std::function<P&()> FunctorType;
    typedef std::map<I, FunctorType> MapType;
    
    RefPrototypeCache() {}
    virtual ~RefPrototypeCache() {}

    static MyType& GetInstance()
    {
        typedef ACE_Singleton<MyType, ACE_Recursive_Thread_Mutex> TheSingleton;
        return *TheSingleton::instance();
    }

    P& GetInstance(I index)
    {
        MapType::iterator iter;
        iter = functors.find(index);
        assert(iter != functors.end());
        return iter->second();
    }

    void Register(I index, FunctorType functor)
    {
        functors.insert(make_pair(index, functor));
    }
    
private:
    MapType functors;
};

template<typename I, typename P>
class PtrPrototypeCache
{
public:
    typedef PtrPrototypeCache<I, P> MyType;
    typedef I IndexType;
    typedef P PrototypeType;
    typedef std::function<P*()> FunctorType;
    typedef std::map<I, FunctorType> MapType;
    
    PtrPrototypeCache() {}
    virtual ~PtrPrototypeCache() {}

    static MyType& GetInstance()
    {
        typedef ACE_Singleton<MyType, ACE_Recursive_Thread_Mutex> TheSingleton;
        return *TheSingleton::instance();
    }

    P* CreateInstance(I index)
    {
        MapType::iterator iter;
        iter = functors.find(index);
        assert(iter != functors.end());
        return iter->second();
    }

    void Register(I index, FunctorType functor)
    {
        functors.insert(make_pair(index, functor));
    }
    
private:
    MapType functors;
};

template<typename P>
class PrototypeCacheRegisterSuite
{
public:
    PrototypeCacheRegisterSuite(typename P::IndexType index, 
                                typename P::FunctorType functor)
    {
        P& instance = P::GetInstance();
        instance.Register(index, functor);
    }
};

#pragma warning(pop)
#endif