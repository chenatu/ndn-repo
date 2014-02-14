#ifndef STORAGE_HANDLE_H
#define STORAGE_HANDLE_H

#include <string>
#include <stdint.h>
#include "storage_define.h"
#include "ndn-cpp-dev/interest.hpp"
#include "ndn-cpp-dev/name.hpp"
#include "ndn-cpp-dev/data.hpp"

using namespace std;
using namespace ndn;

class storage_handle{
public:
	storage_handle();
	int get_storage_method();
	void set_storage_method(int storage_method);
	virtual int insert_data(const Interest& name, Data &data) = 0;
	virtual int delete_data(const Interest& interest, const Name& name) = 0;
	//The simpliest check that to check the specific version and segment
	virtual int check_data(const Interest &interest, Data& data) = 0;

	virtual int check_name(const Name& name) = 0;

private:
	int __storage_method;
};

#endif