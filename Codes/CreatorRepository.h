#ifndef _CreatorRepository_h_
#define _CreatorRepository_h_
#include <map>
#include <functional>

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/Singleton.h"

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
有时间的时候, 将CreatorRepository 用C++11 的variadic template来实现一个通用的Repository方案.  
参考StudyPlusPlus 的ClassCreater。 但是要在2个方面比ClassCreator做得更好：
1 支持VC， GCC。
2 被创建（/存取）的类为一个虚基类， 让本方案通用于各种类似的场合。
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

Template Para: 
    I:   Index type, used to find constructor.
    F:   Functor type.
Example: class PlayerInterfaceFactory
*/
template<typename I, typename F>
class CreatorRepository
{
public:
    typedef CreatorRepository<I, F> MyType;
    typedef std::map<I, F> MapType;
    typedef ACE_Singleton<MyType, ACE_Recursive_Thread_Mutex> SingletonType;
    friend class SingletonType;

    virtual ~CreatorRepository() {}

    static MyType& GetInstance()
    {
        return *SingletonType::instance();
    }

    F GetCreator(I index)
    {
        MapType::iterator iter;
        iter = functors.find(index);
        assert(iter != functors.end());
        return iter->second;
    }

    void Register(I index, F functor)
    {
        functors.insert(make_pair(index, functor));
    }
    
private:
    CreatorRepository() {}

private:
    MapType functors;
};

template<typename I, typename F>
class CreatorRepositoryRegistor
{
public:
    CreatorRepositoryRegistor(I index, F functor)
    {
        CreatorRepository<I, F>& instance = CreatorRepository<I, F>::GetInstance();
        instance.Register(index, functor);
    }
};

#pragma warning(pop)
#endif