#ifndef _TableIndexHelperInterface_h_
#define _TableIndexHelperInterface_h_

#include "Type.h"

class TableIndexHelperInterface
{
public:
	TableIndexHelperInterface() {}
	virtual ~TableIndexHelperInterface() {}

	virtual TableIndex GetUseableTableIndex() = 0;
};

#endif