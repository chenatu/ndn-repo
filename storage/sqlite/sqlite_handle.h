#ifndef SQLITE_HANDLE_H
#define SQLITE_HANDLE_H

#include "../storage_handle.h"

#include <string>
#include <iostream>

class sqlite_handle:public storage_handle{
public:
	sqlite_handle();
};

#endif