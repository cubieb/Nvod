#ifndef _TableIndexHelper_h_
#define _TableIndexHelper_h_

/* Foundation */
#include "SystemInclude.h"

/* Functions */
#include "TableIndexHelperInterface.h"

/**********************class OrderEntityIndexHelper**********************/
class TableIndexHelper : public TableIndexHelperInterface
{
public:
    TableIndexHelper();
    ~TableIndexHelper();

    TableId GetUseableTableIndex(const char *tableName);

private:
    std::map<std::string, TableId> tableIds;
    std::mutex mtx;
};

#endif