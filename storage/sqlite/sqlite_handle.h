#ifndef SQLITE_HANDLE_H
#define SQLITE_HANDLE_H

#include "../storage_handle.h"

#include <string>
#include <iostream>
#include <sqlite3.h> 
#include <stdlib.h>
#include <string>

using namespace std;

class sqlite_handle:public storage_handle{
public:
	sqlite_handle();
	sqlite_handle(char* dbpath);
	virtual int insert_encrypted_data(char* name, char* data);
	virtual int delete_data(char* name);
private:
	sqlite3 *db;
	char* dbpath;
};

#endif