#include "sqlite_handle.h"

sqlite_handle::sqlite_handle(){
	set_storage_method(STORAGE_METHOD_SQLITE);
	//Open the database in default place
	string home(getenv("HOME"));
	string rpath = home + string("/.ndn/ndn_repo.db");
	int rc = sqlite3_open(rpath.c_str(), &db);
	if(rc == SQLITE_OK){
		char *zErrMsg = 0;
		//The name is without component version and segment, The data is the whole Class Data containg name, digest and other component
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name BLOB PRIMARY KEY, data BLOB, pname BLOB);"
			, NULL, 0, &zErrMsg);


		if(rc != SQLITE_OK){
			cout<<zErrMsg<<endl;
			exit(EXIT_FAILURE);
		}
	}else{
		cout<<"Database file open failure rc:"<<rc<<endl;
		exit(EXIT_FAILURE);
	}

	Name rootname;
	string sql = string("select * from NDN_REPO where name = ?;");

	sqlite3_stmt* pStmt = NULL;

	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK){
	        rc = sqlite3_step(pStmt);
	        if (rc == SQLITE_ROW) {
	            cout<<"root name has been created"<<endl;
	        }
	        else if (rc == SQLITE_DONE) {
	        	sqlite3_stmt* p2Stmt = NULL;
	        	sql = string("INSERT INTO NDN_REPO (name, data, pname) VALUES (?, ?, ?);");
	        	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &p2Stmt, NULL);
	        	if(rc == SQLITE_OK){
		        	if (sqlite3_bind_blob(p2Stmt, 1, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK && 
		        		sqlite3_bind_blob(p2Stmt, 2, NULL, 0, NULL) == SQLITE_OK &&
		        		sqlite3_bind_blob(p2Stmt, 3, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK){
						rc = sqlite3_step(p2Stmt);
						if(rc != SQLITE_ROW && rc != SQLITE_DONE){
							cout<<"Root name insert failure rc:"<<rc<<endl;
							sqlite3_finalize(p2Stmt);
							exit(EXIT_FAILURE);
						}
		        	} else {
		        		cout<<"bind blob failure rc:"<<rc<<endl;
		        		sqlite3_finalize(p2Stmt);
		        		exit(EXIT_FAILURE);
		        	}
	    		} else {
	    			cout<<"p2Stmt prepared rc:"<<rc<<endl;
	    			sqlite3_finalize(p2Stmt);
	    			exit(EXIT_FAILURE);
	    		}
	    		sqlite3_finalize(p2Stmt);
	        }
		    else {
				cout<<"Database query failure rc:"<<rc<<endl;
				sqlite3_finalize(pStmt);
				exit(EXIT_FAILURE);
		    }
	    }  
	    sqlite3_finalize(pStmt);
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
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name BLOB PRIMARY KEY, data BLOB, pname BLOB);"
			, NULL, 0, &zErrMsg);
		if(rc != SQLITE_OK){
			cout<<zErrMsg<<" rc:"<<rc<<endl;
			exit(EXIT_FAILURE);
		}
	}else{
		cout<<"Database file open failure rc:"<<rc<<endl;
		exit(EXIT_FAILURE);
	}

	Name rootname;
	string sql = string("select * from NDN_REPO where name = ?;");

	sqlite3_stmt* pStmt = NULL;

	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK){
	        rc = sqlite3_step(pStmt);
	        if (rc == SQLITE_ROW) {
	            cout<<"root name has been created"<<endl;
	        }
	        else if (rc == SQLITE_DONE) {
	        	sqlite3_stmt* p2Stmt = NULL;
	        	sql = string("INSERT INTO NDN_REPO (name, data, pname) VALUES (?, ?, ?);");
	        	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &p2Stmt, NULL);
	        	if(rc == SQLITE_OK){
		        	if (sqlite3_bind_blob(p2Stmt, 1, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK && 
		        		sqlite3_bind_blob(p2Stmt, 2, NULL, 0, NULL) == SQLITE_OK &&
		        		sqlite3_bind_blob(p2Stmt, 3, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK){
						rc = sqlite3_step(p2Stmt);;
						if(rc != SQLITE_ROW && rc != SQLITE_DONE){
							cout<<"Root name insert failure rc:"<<rc<<endl;
							sqlite3_finalize(p2Stmt);
							exit(EXIT_FAILURE);
						}
		        	} else {
		        		cout<<"bind blob failure rc:"<<rc<<endl;
		        		sqlite3_finalize(p2Stmt);
		        		exit(EXIT_FAILURE);
		        	}
	    		} else {
	    			cout<<"p2Stmt prepared rc:"<<rc<<endl;
	    			sqlite3_finalize(p2Stmt);
	    			exit(EXIT_FAILURE);
	    		}
	    		sqlite3_finalize(p2Stmt);
	        }
		    else {
				cout<<"Database query failure rc:"<<rc<<endl;
				sqlite3_finalize(pStmt);
				exit(EXIT_FAILURE);
		    }
	    }  
	    sqlite3_finalize(pStmt);
	}
}

sqlite_handle::~sqlite_handle(){
	sqlite3_close(db);
}


//Temporarily assigned the datatype of every component. needs to be further discussed

int sqlite_handle::insert_data(Name& name, Data &data){
	sqlite3_stmt* piStmt = NULL;
	string insert_sql = string("INSERT INTO NDN_REPO (name, data, pname) VALUES (?, ?, ?);");
	Name rootname;

	int rc = 0;
	if(sqlite3_prepare_v2(db, insert_sql.c_str(), -1, &piStmt, NULL) != SQLITE_OK){
		sqlite3_finalize(piStmt);
		cout<<"insert sql not prepared"<<endl;
	}
	//Insert and check the prefix
	Name childname = Name(name);
	Name parentname;
	Name grandname;
	if(!name.empty()){
		parentname = childname.getPrefix(-1);
	}else{
		cout<<"name is empty"<<endl;
		return 0;
	}
	while(!check_name(parentname)){
		if(parentname.empty()){
			cout<<"loop break"<<endl;
			break;
		}else{
			grandname = parentname.getPrefix(-1);			
			if (sqlite3_bind_blob(piStmt, 1, parentname.wireEncode().wire(), parentname.wireEncode().size(), NULL) == SQLITE_OK && 
			    sqlite3_bind_blob(piStmt, 2, NULL, 0, NULL) == SQLITE_OK &&
			    sqlite3_bind_blob(piStmt, 3, grandname.wireEncode().wire(), grandname.wireEncode().size(), NULL) == SQLITE_OK){	
				rc = sqlite3_step(piStmt);
				if(rc == SQLITE_CONSTRAINT){
					cout<<"Insert parent prefix failed"<<endl;
					sqlite3_finalize(piStmt);
					exit(EXIT_FAILURE);
				}
				sqlite3_reset(piStmt);
			}
			parentname = grandname;
		}
	}

	//Insert the name and the data
	parentname = childname.getPrefix(-1);
	if (sqlite3_bind_blob(piStmt, 1, name.wireEncode().wire(), name.wireEncode().size(), NULL) == SQLITE_OK && 
		sqlite3_bind_blob(piStmt, 2, data.wireEncode().wire(), data.wireEncode().size(), NULL) == SQLITE_OK &&
		sqlite3_bind_blob(piStmt, 3, parentname.wireEncode().wire(), parentname.wireEncode().size(), NULL) == SQLITE_OK){ 
		rc = sqlite3_step(piStmt);
		if(rc == SQLITE_CONSTRAINT){
        	cout<<"The name of the data has existed!"<<endl;
        	sqlite3_finalize(piStmt);
        	return 0;
		}
	}
	sqlite3_finalize(piStmt);
	return 1;
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

int sqlite_handle::check_name(Name& name){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("select * from NDN_REPO where name = ?;");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, name.wireEncode().wire(), name.wireEncode().size(), NULL) == SQLITE_OK){
	        rc = sqlite3_step(pStmt);
	        if (rc == SQLITE_ROW) {
	        	sqlite3_finalize(pStmt);
	            return 1;
	        }
	        else if (rc == SQLITE_DONE) {
	            return 0;
	        }
		    else {
				cout<<"Database query failure rc:"<<rc<<endl;
				sqlite3_finalize(pStmt);
				return 0;
		    }
	    }  
	    sqlite3_finalize(pStmt);
	}  
	return 1;
}