#include "sqlite_handle.h"

sqlite_handle::sqlite_handle(){
	set_storage_method(STORAGE_METHOD_SQLITE);
	//Open the database in default place
	string home(getenv("HOME"));
	string path = home + string("/.ndn/ndn_repo.db");
	int rc = sqlite3_open(path.c_str(), &db);
	if(rc == SQLITE_OK){
		char *zErrMsg = 0;
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name TEXT PRIMARY KEY, content BLOB, encrypted INTEGER);"
			, NULL, 0, &zErrMsg);
		if(rc != SQLITE_OK){
			cout<<zErrMsg<<endl;
			exit(EXIT_FAILURE);
		}
	}else{
		cout<<"Database file open failure rc:"<<rc<<endl;
		exit(EXIT_FAILURE);
	}

}

sqlite_handle::sqlite_handle(char* dbpath){
	set_storage_method(STORAGE_METHOD_SQLITE);
	int rc;
	if(dbpath == 0){
		string home(getenv("HOME"));
		string path = home + string("/.ndn/ndn_repo.db");
		rc = sqlite3_open(path.c_str(), &db);
		cout<<"default"<<endl;
	}else{
		rc = sqlite3_open(dbpath, &db);
		cout<<"custom"<<endl;
	}
	if(rc == SQLITE_OK){
		char *zErrMsg = 0;
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name TEXT PRIMARY KEY, content BLOB, encrypted INTEGER);"
			, NULL, 0, &zErrMsg);
		if(rc != SQLITE_OK){
			cout<<zErrMsg<<endl;
			exit(EXIT_FAILURE);
		}
	}else{
		cout<<"Database file open failure rc:"<<rc<<endl;
		exit(EXIT_FAILURE);
	}
}

int sqlite_handle::insert_encrypted_data(char* name, char* data){
	return 0;
}

int sqlite_handle::delete_data(char* name){
	return 0;
}