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
			cout<<zErrMsg<<" rc:"<<rc<<endl;
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

int sqlite_handle::insert_encrypted_data(Name& name, Data &data){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("INSERT INTO NDN_REPO (name, data) VALUES (?, ?);");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, name.wireEncode().wire(), name.wireEncode().size(), NULL) == SQLITE_OK && 
			sqlite3_bind_blob(pStmt, 2, data.wireEncode().wire(), data.wireEncode().size(), NULL) == SQLITE_OK)  
        {  
            rc = sqlite3_step(pStmt); 
        }
        if(rc == SQLITE_CONSTRAINT)
        	cout<<"The name of the data has existed!"<<endl;
        sqlite3_finalize(pStmt);   
	}
	return 0;
}



int sqlite_handle::delete_data(Name& name){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("DELETE from NDN_REPO where name = ?;");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, name.wireEncode().wire(), name.wireEncode().size(), NULL) == SQLITE_OK) {
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

int sqlite_handle::check_data(Name& name, Data& data){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("select * from NDN_REPO where name = ?;");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, name.wireEncode().wire(), name.wireEncode().size(), NULL) == SQLITE_OK){
	        while (1) {
	            rc = sqlite3_step(pStmt);
	            if (rc == SQLITE_ROW) {
	                data.wireDecode(Block(sqlite3_column_blob(pStmt, 1),sqlite3_column_bytes(pStmt, 1)));
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