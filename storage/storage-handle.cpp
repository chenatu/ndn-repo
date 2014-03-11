/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#include "storage-handle.hpp"

StorageHandle::StorageHandle(){
	set_storage_method(STORAGE_METHOD_BASE);
}

int 
StorageHandle::get_storage_method(){
	return __storage_method;
}

void 
StorageHandle::set_storage_method(int storage_method){
	__storage_method = storage_method;
}
