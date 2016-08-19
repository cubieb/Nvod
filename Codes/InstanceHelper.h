#ifndef _InstanceHelper_h_
#define _InstanceHelper_h_
#include <map>
#include <functional>

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/Singleton.h"

/*
Example:
class Xxx
{
public:
    virtual int GetValue() = 0;
};
class Xxx01: public Xxx
{
public:
	static Xxx& GetInstance() 
	{
		static Xxx01 xxx;
		return xxx;
	}
};

class Xxx02: public Xxx
{
public:
	static Xxx& GetInstance() 
	{
		static Xxx02 xxx;
		return xxx;
	}
};

typedef InstanceHelper<Xxx, std::string> XxxInstHelper;
typedef InstanceHelperRegisterSuite<XxxInstHelper> XxxInstHelperRegisterSuite;
static XxxInstHelperRegisterSuite refsEventIndex("Xxx01", Xxx01::GetInstance);
static XxxInstHelperRegisterSuite refsEventIndex("Xxx02", Xxx02::GetInstance);

XxxInstHelper& helper = XxxInstHelper::GetInstance();
int value = helper.GetInstance("Xxx01").GetValue();
*/
template<typename Instance, typename Index>
class InstanceHelper
{
public:
    typedef Index     IndexType;
    typedef Instance  InstanceType;
    typedef std::function<InstanceType&()> FunctorType;
    typedef InstanceHelper<Instance, IndexType> MyType;

    typedef std::map<Index, FunctorType> ContainerType;
    
    InstanceHelper() {}
    virtual ~InstanceHelper() {}

    static InstanceHelper& GetInstance()
    {
        typedef ACE_Singleton<MyType, ACE_Recursive_Thread_Mutex> TheHelper;
        return *TheHelper::instance();
    }

    InstanceType& GetInstance(IndexType index)
    {
        ContainerType::iterator iter;
        iter = functors.find(index);
        assert(iter != functors.end());
        return iter->second();
    }

    void Register(IndexType index, FunctorType functor)
    {
        functors.insert(make_pair(index, functor));
    }
    
private:
    ContainerType functors;
};

template<typename Helper>
class InstanceHelperRegisterSuite
{
public:
    InstanceHelperRegisterSuite(typename Helper::IndexType index, 
		                        typename Helper::FunctorType functor)
    {
        Helper& helper = Helper::GetInstance();
        helper.Register(index, functor);
    }
};

#pragma warning(pop)
#endif