#ifndef STORAGE_HANDLE_H
#define STORAGE_HANDLE_H

#include "storage_define.h"

class storage_handle{
public:
	storage_handle();
	int get_storage_method();
	void set_storage_method(int storage_method);
private:
	int __storage_method;
};

#endif