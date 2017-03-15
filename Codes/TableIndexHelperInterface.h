#ifndef _TableIndexHelperInterface_h_
#define _TableIndexHelperInterface_h_

/* Entity */
#include "BaseType.h"

/**********************class TableIndexHelperInterface**********************/
class TableIndexHelperInterface
{
public:
	TableIndexHelperInterface() {}
	virtual ~TableIndexHelperInterface() {}
    static TableIndexHelperInterface& GetInstance();

    virtual TableId GetUseableTableIndex(const char *tableName) = 0;
};

#endif