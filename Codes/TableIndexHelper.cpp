#include "SystemInclude.h"

#pragma warning(push)
#pragma warning(disable:702)   //disable warning caused by ACE library.
#pragma warning(disable:4251)  //disable warning caused by ACE library.
#pragma warning(disable:4996)  //disable warning caused by ACE library.
#include "ace/OS.h"
#include "ace/Singleton.h"

/* Controller */
#include "TableIndexHelper.h"
using namespace std;

typedef ClassFactoriesRegistor<TableIndexHelperInterface, std::string> RefsEventRegistor;
static RefsEventRegistor refse01("ReferenceServiceEvent", RefsEventTableIndexHelper::CreateSingletonInstance);

typedef ClassFactoriesRegistor<TableIndexHelperInterface, std::string> TmssEventRegistor;
static TmssEventRegistor tmsse01("TimeShiftedServiceEvent", TmssEventTableIndexHelper::CreateSingletonInstance);

/**********************class RefsEventTableIndexHelper**********************/
RefsEventTableIndexHelper::RefsEventTableIndexHelper()
{
	index = 1;
}

RefsEventTableIndexHelper::~RefsEventTableIndexHelper()
{
}

TableIndex RefsEventTableIndexHelper::GetUseableTableIndex()
{
	return index++;
}

TableIndexHelperInterface* RefsEventTableIndexHelper::CreateSingletonInstance()
{
    return TheSingleton::instance();
}

/**********************class TmssEventTableIndexHelper**********************/
TmssEventTableIndexHelper::TmssEventTableIndexHelper()
{
	index = 1;
}

TmssEventTableIndexHelper::~TmssEventTableIndexHelper()
{
}

TableIndex TmssEventTableIndexHelper::GetUseableTableIndex()
{
	return index++;
}

TableIndexHelperInterface* TmssEventTableIndexHelper::CreateSingletonInstance()
{
    return TheSingleton::instance();
}

#pragma warning(pop)