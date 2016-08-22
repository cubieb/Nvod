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
��ʱ���ʱ��, ��CreatorRepository ��C++11 ��variadic template��ʵ��һ��ͨ�õ�Repository����.  
�ο�StudyPlusPlus ��ClassCreater�� ����Ҫ��2�������ClassCreator���ø��ã�
1 ֧��VC�� GCC��
2 ��������/��ȡ������Ϊһ������࣬ �ñ�����ͨ���ڸ������Ƶĳ��ϡ�
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