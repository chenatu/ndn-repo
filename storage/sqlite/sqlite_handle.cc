#include "sqlite_handle.h"

sqlite_handle::sqlite_handle(){
	set_storage_method(STORAGE_METHOD_SQLITE);
}