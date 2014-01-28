#ifndef STORAGE_HANDLE_H
#define STORAGE_HANDLE_H

#include <string>
#include <stdint.h>
#include "storage_define.h"

using namespace std;

class storage_handle{
public:
	storage_handle();
	int get_storage_method();
	void set_storage_method(int storage_method);
	virtual int insert_encrypted_data(string& name, string &data) = 0;
	virtual int delete_data(string& name) = 0;
	//The simpliest check that to check the specific version and segment
	virtual int check_data(string& name, string& blobdata) = 0;
private:
	int __storage_method;
};

#endif