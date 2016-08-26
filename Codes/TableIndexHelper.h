#ifndef _TableIndexHelper_h_
#define _TableIndexHelper_h_

#include "Type.h"
#include "TableIndexHelperInterface.h"

/**********************class RefsEventTableIndexHelper**********************/
class RefsEventTableIndexHelper: public TableIndexHelperInterface
{
public:
    typedef ACE_Singleton<RefsEventTableIndexHelper, ACE_Recursive_Thread_Mutex> TheSingleton;
    friend class TheSingleton;

    ~RefsEventTableIndexHelper();

    TableIndex GetUseableTableIndex();
    /* return the pointer of singleton instance */
    static TableIndexHelperInterface* CreateSingletonInstance();

private:
    RefsEventTableIndexHelper();

private:
    TableIndex index;
};

/**********************class TmssEventTableIndexHelper**********************/
class TmssEventTableIndexHelper: public TableIndexHelperInterface
{
public:
    typedef ACE_Singleton<TmssEventTableIndexHelper, ACE_Recursive_Thread_Mutex> TheSingleton;
    friend class TheSingleton;
        
    ~TmssEventTableIndexHelper();

    TableIndex GetUseableTableIndex();
    /* return the pointer of singleton instance */
    static TableIndexHelperInterface* CreateSingletonInstance();

private:
    TmssEventTableIndexHelper();
    TableIndex index;
};

#endif