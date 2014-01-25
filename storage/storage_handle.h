#ifndef STORAGE_HANDLE_H
#define STORAGE_HANDLE_H

#include "storage_define.h"

class storage_handle{
public:
	storage_handle();
	int get_storage_method();
	void set_storage_method(int storage_method);
	virtual int insert_encrypted_data(char* name, char* data) = 0;
	virtual int delete_data(char* name) = 0;
private:
	int __storage_method;
};

#endif