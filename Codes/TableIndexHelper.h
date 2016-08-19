#ifndef _TableIndexHelper_h_
#define _TableIndexHelper_h_

#include "Type.h"
#include "TableIndexHelperInterface.h"

class RefsEventTableIndexHelper: public TableIndexHelperInterface
{
public:
    RefsEventTableIndexHelper();
    ~RefsEventTableIndexHelper();
    TableIndex GetUseableTableIndex();

    static TableIndexHelperInterface& GetInstance()
    {
        typedef ACE_Singleton<RefsEventTableIndexHelper, ACE_Recursive_Thread_Mutex> TheHelper;
        return *TheHelper::instance();
    }

private:
    TableIndex index;
};

#endif