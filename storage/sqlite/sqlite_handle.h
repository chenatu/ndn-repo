#ifndef SQLITE_HANDLE_H
#define SQLITE_HANDLE_H

#include "../storage_handle.h"

#include <string>
#include <iostream>
#include <sqlite3.h> 
#include <stdlib.h>

using namespace std;

class sqlite_handle:public storage_handle{
public:
	sqlite_handle();
	sqlite_handle(string dbpath);
	~sqlite_handle();
	virtual int insert_data(Name& name, Data &data);
	virtual int delete_data(Name& name);
	virtual int check_data(Name& name, Data& data);
	//If has the name in database, return 1, else return 0
	int check_name(Name& name);
private:
	//This is for name and data database
	sqlite3 *db;
	//db and index will be in the same directory
	char* dbpath;
};

#endif