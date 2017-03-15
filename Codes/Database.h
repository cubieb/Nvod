#ifndef _Database_h_
#define _Database_h_

#include <memory>

/* ODB */
#include <odb/database.hxx>
#include <odb/mysql/database.hxx>

/**********************class Database**********************/
/* with multi-thread context, you must set database isolate level, for example (mysql):
use BtcTradeRobot;
SET GLOBAL TRANSACTION ISOLATION LEVEL Serializable;
//SET session TRANSACTION ISOLATION LEVEL Serializable;
SELECT @@GLOBAL.tx_isolation, @@tx_isolation;

with MySql, the isolate levle could be: Read uncommitted,Read committed,Repeatable,Serializable
*/
template<typename T>
class DatabaseTemplate
{
public:
    static DatabaseTemplate<T>& GetInstance()
	{
        /* In C++11, the following is guaranteed to perform thread-safe initialisation: */
        satic DatabaseTemplate<T> instance;
        return instance;
	}

	std::shared_ptr<odb::database> GetDatabase() const
	{
		return db;
	}

private:
	DatabaseTemplate()
	{
#ifdef UnitTestMode
		db = make_shared<T>("root", "000000", "BtcTradeRobotUnitTest");
#else
		db = make_shared<T>("root", "000000", "BtcTradeRobot");
#endif
	}

	~DatabaseTemplate()
	{}

private:
	std::shared_ptr<odb::database> db;
};

typedef DatabaseTemplate<odb::mysql::database> Database;

#define StartTransaction(db, retryNumber) for (uint_t iii = 0; iii < retryNumber; ++iii) \
{ \
	try \
{ \
	odb::transaction trans(db->begin());

#define EndTransaction() \
	trans.commit(); \
	break; \
} \
	catch (const odb::recoverable& e) \
{ \
	dbgstrm << "failed, " << e.what() << endl; \
	continue; \
} \
}

#endif