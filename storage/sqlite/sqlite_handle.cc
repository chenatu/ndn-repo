#include "sqlite_handle.h"

sqlite_handle::sqlite_handle(){
	set_storage_method(STORAGE_METHOD_SQLITE);
	//Open the database in default place
	string home(getenv("HOME"));
	string path = home + string("/.ndn/ndn_repo.db");
	int rc = sqlite3_open(path.c_str(), &db);
	if(rc == SQLITE_OK){
		char *zErrMsg = 0;
		//The name is without component version and segment, The data is the whole Class Data containg name, digest and other component
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name BLOB PRIMARY KEY, data BLOB, encrypted INTEGER);"
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

sqlite_handle::sqlite_handle(string dbpath){
	set_storage_method(STORAGE_METHOD_SQLITE);
	int rc;
	if(dbpath.empty()){
		string home(getenv("HOME"));
		string path = home + string("/.ndn/ndn_repo.db");
		rc = sqlite3_open(path.c_str(), &db);
	}else{
		rc = sqlite3_open(dbpath.c_str(), &db);
	}
	if(rc == SQLITE_OK){
		char *zErrMsg = 0;
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name BLOB PRIMARY KEY, data BLOB,encrypted INTEGER);"
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

sqlite_handle::~sqlite_handle(){
	sqlite3_close(db);
}


//Temporarily assigned the datatype of every component. needs to be further discussed

int sqlite_handle::insert_encrypted_data(string& name, string &data){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("INSERT INTO NDN_REPO (name, data) VALUES (")+ name +string(", '?);");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 2, data.c_str(), data.length(), NULL) == SQLITE_OK)  
        {  
            rc = sqlite3_step(pStmt); 
        }  
        sqlite3_finalize(pStmt);   
	}
	return 0;
}



int sqlite_handle::delete_data(string& name){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("DELETE from NDN_REPO where name = ?;");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_text(pStmt, 1, name.c_str(), name.length(), NULL) == SQLITE_OK) {
        	while(1){
            	rc = sqlite3_step(pStmt); 
            	if (rc == SQLITE_DONE) {
                	break;
            	}else {
					cout<<"Database delete failure rc:"<<rc<<endl;
					exit(EXIT_FAILURE);
				}
            }
        }  
        sqlite3_finalize(pStmt);   
	}else{
		cout<<"Database delete statement failure rc:"<<rc<<endl;
		exit(EXIT_FAILURE);
	}
	return 0;
}

int sqlite_handle::check_data(string& name, string& blobdata){
	sqlite3_stmt* pStmt = NULL;
	int rc = 0;
	string sql = string("select * from NDN_REPO where name = ?;");
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, name.c_str(), name.length(), NULL) == SQLITE_OK)  
        {
        	while (1) {
            	rc = sqlite3_step(pStmt);
            	if (rc == SQLITE_ROW) {
                	blobdata.assign((const char*)sqlite3_column_blob(pStmt, 0));
            	}
            	else if (rc == SQLITE_DONE) {
                	break;
            	}
	            else {
					cout<<"Database query failure rc:"<<rc<<endl;
					exit(EXIT_FAILURE);
	            }
            }
        }  
        sqlite3_finalize(pStmt);   
	}
	return 0;
}