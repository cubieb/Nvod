#ifndef _TableIndexHelperInterface_h_
#define _TableIndexHelperInterface_h_

#include "Type.h"
#include "ClassFactories.h"

/**********************class TableIndexHelperInterface**********************/
class TableIndexHelperInterface
{
public:
	TableIndexHelperInterface() {}
	virtual ~TableIndexHelperInterface() {}

	virtual TableIndex GetUseableTableIndex() = 0;
};

/**********************class TableIndexHelperInterface Factory**********************/
template<typename ... Types>
TableIndexHelperInterface& GetTableIndexHelperInterface(const char *tableName, Types ... args)
{
    typedef ClassFactories<TableIndexHelperInterface, std::string, Types ...> ClassFactoriesType;
    ClassFactoriesType& instance = ClassFactoriesType::GetInstance();

    return *instance.CreateInstance(tableName, args ...);
}

#endif