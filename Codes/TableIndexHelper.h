#ifndef _TableIndexHelper_h_
#define _TableIndexHelper_h_

#include "Type.h"
#include "TableIndexHelperInterface.h"

/**********************class RefsEventTableIndexHelper**********************/
class RefsEventTableIndexHelper: public TableIndexHelperInterface
{
public:
    friend class ACE_Singleton<RefsEventTableIndexHelper, ACE_Recursive_Thread_Mutex>;

    ~RefsEventTableIndexHelper();
    TableIndex GetUseableTableIndex();

    static TableIndexHelperInterface& GetInstance()
    {
        typedef ACE_Singleton<RefsEventTableIndexHelper, ACE_Recursive_Thread_Mutex> TheHelper;
        return *TheHelper::instance();
    }

private:
    RefsEventTableIndexHelper();

private:
    TableIndex index;
};

/**********************class TmssEventTableIndexHelper**********************/
class TmssEventTableIndexHelper: public TableIndexHelperInterface
{
public:
    TmssEventTableIndexHelper();
    ~TmssEventTableIndexHelper();
    TableIndex GetUseableTableIndex();

    static TableIndexHelperInterface& GetInstance()
    {
        typedef ACE_Singleton<TmssEventTableIndexHelper, ACE_Recursive_Thread_Mutex> TheHelper;
        return *TheHelper::instance();
    }

private:
    TableIndex index;
};

#endif