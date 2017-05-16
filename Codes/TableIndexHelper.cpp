#include "TableIndexHelper.h"

/* Entity */
#include "Entities.h"

using namespace std;

/**********************class TableIndexHelperInterface**********************/
TableIndexHelperInterface& TableIndexHelperInterface::GetInstance()
{
    /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
    static TableIndexHelper instance;
    return instance;
}

/**********************class TableIndexHelper**********************/
TableIndexHelper::TableIndexHelper()
{
    tableIds = std::initializer_list<map<std::string, TableId>::value_type>
    {
        { "Refs", 1 }, { "RefsEvent", 1 }, 
        { "Tmss", 1 }, { "TmssEvent", 1 },
		{ "MovieOrPoster", 1 }, 
        { "GlobalCfg", 1 }, {"Ts", 1}
    };
}

TableIndexHelper::~TableIndexHelper()
{}

TableId TableIndexHelper::GetUseableTableIndex(const char *tableName)
{
    TableId tableId;
    lock_guard<std::mutex> lock(mtx);

    map<string, TableId>::iterator iter = tableIds.find(tableName);
    if (iter == tableIds.end())
    {
        assert(false);
        tableId = InvalidTableId;
    }
    else
    {
        tableId = iter->second;
        ++iter->second;
    }

    return tableId;
}