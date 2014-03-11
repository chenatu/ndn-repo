/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef StorageHandle_H
#define StorageHandle_H

#include <string>
#include <stdint.h>
#include "storage-define.hpp"
#include <ndn-cpp-dev/interest.hpp>
#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/data.hpp>
#include <ndn-cpp-dev/selectors.hpp>

using namespace std;
using namespace ndn;

class StorageHandle{
public:
	StorageHandle();
	int
	get_storage_method();
	void
	set_storage_method(int storage_method);
	virtual int 
	insert_data(const Interest& interest, Data &data) = 0;
	virtual int 
	delete_data(const Name& name) = 0;
	//The simpliest check that to check the specific version and segment
	virtual int 
	check_data(const Interest& interest, Data& data) = 0;

	virtual int 
	check_name(const Name& name) = 0;
	//select any data conforms the selector
	virtual int 
	check_name_any(const Name& name, const Selectors& selectors, vector<Name>& vname) = 0;
private:
	int __storage_method;
};

#endif