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

#pragma warning(pop)