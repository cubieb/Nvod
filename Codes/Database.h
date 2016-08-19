// file      : hello/database.hxx
// copyright : not copyrighted - public domain

//
// Create concrete database instance based on the DATABASE_* macros.
//

#ifndef _Database_h_
#define _Database_h_

#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
odb::database *CreateDatabase(const char *user, const char *password, const char *schema);

#endif // DATABASE_HXX
