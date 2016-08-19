#include "Database.h"

odb::database *CreateDatabase(const char *user, const char *password, const char *schema)
{
    odb::mysql::database *database = new odb::mysql::database(user, password, schema);
    return database;
}
