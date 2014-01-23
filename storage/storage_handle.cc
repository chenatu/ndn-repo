#include "storage_handle.h"

storage_handle::storage_handle(){
	set_storage_method(STORAGE_METHOD_BASE);
}

int storage_handle::get_storage_method(){
	return __storage_method;
}

void storage_handle::set_storage_method(int storage_method){
	__storage_method = storage_method;
}
