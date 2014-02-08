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
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name BLOB PRIMARY KEY, data BLOB, pname BLOB, childnum INTEGER);"
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
	        	sql = string("INSERT INTO NDN_REPO (name, data, pname, childnum) VALUES (?, ?, ?, ?);");
	        	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &p2Stmt, NULL);
	        	if(rc == SQLITE_OK){
		        	if (sqlite3_bind_blob(p2Stmt, 1, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK && 
		        		sqlite3_bind_blob(p2Stmt, 2, NULL, 0, NULL) == SQLITE_OK &&
		        		sqlite3_bind_blob(p2Stmt, 3, NULL, 0, NULL) == SQLITE_OK &&
		        		sqlite3_bind_int(p2Stmt, 4, 0) == SQLITE_OK){
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
		rc = sqlite3_exec(db, "create table if not exists NDN_REPO (name BLOB PRIMARY KEY, data BLOB, pname BLOB, childnum INTEGER);"
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
	        	sql = string("INSERT INTO NDN_REPO (name, data, pname, childnum) VALUES (?, ?, ?, ?);");
	        	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &p2Stmt, NULL);
	        	if(rc == SQLITE_OK){
		        	if (sqlite3_bind_blob(p2Stmt, 1, rootname.wireEncode().wire(), rootname.wireEncode().size(), NULL) == SQLITE_OK && 
		        		sqlite3_bind_blob(p2Stmt, 2, NULL, 0, NULL) == SQLITE_OK &&
		        		sqlite3_bind_blob(p2Stmt, 3, NULL, 0, NULL) == SQLITE_OK &&
		        		sqlite3_bind_int(p2Stmt, 4, 0) == SQLITE_OK){
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
	if(check_name(name)){
		cout<<"The name has existed"<<endl;
		return 0;
	}
	if(name.empty()){
		cout<<"name is empty"<<endl;
		return 0;
	}

	sqlite3_stmt* piStmt = NULL;
	sqlite3_stmt* puStmt = NULL;
	string insert_sql = string("INSERT INTO NDN_REPO (name, data, pname, childnum) VALUES (?, ?, ?, ?);");
	string update_sql = string("UPDATE NDN_REPO SET childnum = childnum + 1 WHERE name = ?;");
	Name rootname;

	int rc = 0;
	if(sqlite3_prepare_v2(db, insert_sql.c_str(), -1, &piStmt, NULL) != SQLITE_OK){
		sqlite3_finalize(piStmt);
		cout<<"insert sql not prepared"<<endl;
	}
	if(sqlite3_prepare_v2(db, update_sql.c_str(), -1, &puStmt, NULL) != SQLITE_OK){
		sqlite3_finalize(puStmt);
		cout<<"update sql not prepared"<<endl;
		exit(EXIT_FAILURE);
	}
	//Insert and check the prefix
	Name parentname = name;
	Name grandname;
	do{
		parentname = parentname.getPrefix(-1);
		if(!check_name(parentname)){
			grandname = parentname.getPrefix(-1);			
			if (sqlite3_bind_blob(piStmt, 1, parentname.wireEncode().wire(), parentname.wireEncode().size(), NULL) == SQLITE_OK && 
			    sqlite3_bind_blob(piStmt, 2, NULL, 0, NULL) == SQLITE_OK &&
			    sqlite3_bind_blob(piStmt, 3, grandname.wireEncode().wire(), grandname.wireEncode().size(), NULL) == SQLITE_OK &&
			    sqlite3_bind_int(piStmt, 4, 1) == SQLITE_OK){	
				rc = sqlite3_step(piStmt);
				if(rc == SQLITE_CONSTRAINT){
					cout<<"Insert parent prefix failed"<<endl;
					sqlite3_finalize(piStmt);
					exit(EXIT_FAILURE);
				}
				sqlite3_reset(piStmt);
			}
		}else{
			break;
		}
	}while(!parentname.empty());

	//The existed parent childnum + 1

	if(sqlite3_bind_blob(puStmt, 1, parentname.wireEncode().wire(), parentname.wireEncode().size(), NULL) == SQLITE_OK){
		rc = sqlite3_step(puStmt);
		if(rc != SQLITE_ROW && rc != SQLITE_DONE){
			cout<<"update error rc:"<<rc<<endl;
			sqlite3_finalize(puStmt);
			sqlite3_finalize(piStmt);
			exit(EXIT_FAILURE);
		}
		sqlite3_reset(puStmt);
	}

	//Insert the name and the data
	parentname = name.getPrefix(-1);
	sqlite3_reset(piStmt);
	if (sqlite3_bind_blob(piStmt, 1, name.wireEncode().wire(), name.wireEncode().size(), NULL) == SQLITE_OK && 
		sqlite3_bind_blob(piStmt, 2, data.wireEncode().wire(), data.wireEncode().size(), NULL) == SQLITE_OK &&
		sqlite3_bind_blob(piStmt, 3, parentname.wireEncode().wire(), parentname.wireEncode().size(), NULL) == SQLITE_OK &&
		sqlite3_bind_int(piStmt, 4, 0) == SQLITE_OK){ 
		rc = sqlite3_step(piStmt);
		//cout<<"insert the data: "<<data.wireEncode().wire()<<endl;
		if(rc == SQLITE_CONSTRAINT){
        	cout<<"The name of the data has existed!"<<endl;
        	sqlite3_finalize(piStmt);
        	return 0;
		}
	}
	sqlite3_finalize(puStmt);
	sqlite3_finalize(piStmt);
	return 1;
}



int sqlite_handle::delete_data(Name& name){
	sqlite3_stmt* pqStmt = NULL;
	sqlite3_stmt* pdStmt = NULL;
	sqlite3_stmt* puStmt = NULL;
	string sql_query = string("SELECT * from NDN_REPO where name = ?;");
	string sql_delete = string("DELETE from NDN_REPO where name = ?;");
	string sql_update = string("UPDATE NDN_REPO SET childnum = childnum - 1 WHERE name = ?;");
	int rc = sqlite3_prepare_v2(db, sql_query.c_str(), -1, &pqStmt, NULL);
	Name tmpname = name;
	int childnum;
	if(sqlite3_prepare_v2(db, sql_delete.c_str(), -1, &pdStmt, NULL) != SQLITE_OK){
	    sqlite3_finalize(pdStmt);
	    cout<<"delete statement prepared failed"<<endl;
	    exit(EXIT_FAILURE);
	}
	if(sqlite3_prepare_v2(db, sql_update.c_str(), -1, &puStmt, NULL) != SQLITE_OK){
	    sqlite3_finalize(puStmt);
	    cout<<"delete update prepared failed"<<endl;
	    exit(EXIT_FAILURE);
	}
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pqStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
	        rc = sqlite3_step(pqStmt);
	        if (rc == SQLITE_ROW) {
	        	childnum = sqlite3_column_int(pqStmt, 3);
	        }
		    else {
				cout<<"Database query no such name or failure rc:"<<rc<<endl;
				sqlite3_finalize(pqStmt);
				return 0;
		    }
	    }
	    if(childnum > 0){
	    	cout<<"This is internal node which cannot be deleted"<<endl;
	    	sqlite3_finalize(pqStmt);
			return 0;
	    }else{
	    	//Delete the leaf node
	    	if(sqlite3_bind_blob(pdStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
			    rc = sqlite3_step(pdStmt);
			  	if(rc != SQLITE_DONE && rc !=SQLITE_ROW){
			   		cout<<"leaf node delete error rc:"<<rc<<endl;
			   		sqlite3_finalize(pdStmt);
		    		exit(EXIT_FAILURE);
		   		}
		   	}else{
		  		cout<<"delete bind error"<<endl;
			   	sqlite3_finalize(pdStmt);
		    	exit(EXIT_FAILURE);
		    }
		    sqlite3_reset(pdStmt);
	    }

	    sqlite3_reset(pqStmt);

	    do{
	    	tmpname = tmpname.getPrefix(-1);
	    	if (sqlite3_bind_blob(pqStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
	        	rc = sqlite3_step(pqStmt);
		        if (rc == SQLITE_ROW) {
		        	childnum = sqlite3_column_int(pqStmt, 3);
		        }else {
					cout<<"Database query no such name or failure rc:"<<rc<<endl;
					sqlite3_finalize(pqStmt);
					return 0;
			    }
			    if(childnum == 1 && !tmpname.empty()){
			    	//Delete this internal node
			    	if(sqlite3_bind_blob(pdStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
			    		rc = sqlite3_step(pdStmt);
			    		if(rc != SQLITE_DONE && rc !=SQLITE_ROW){
			    			cout<<"internal node delete error rc:"<<rc<<endl;
			    			sqlite3_finalize(pdStmt);
			    			exit(EXIT_FAILURE);
			    		}
			    	}else{
			    		cout<<"delete bind error"<<endl;
			    		sqlite3_finalize(pdStmt);
			    		exit(EXIT_FAILURE);
			    	}
			    	sqlite3_reset(pdStmt);
			    }else{
			    	//childnum - 1
			    	if(sqlite3_bind_blob(puStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
			    		rc = sqlite3_step(puStmt);
			    		if(rc != SQLITE_DONE && rc !=SQLITE_ROW){
			    			cout<<"internal node childnum update error rc:"<<rc<<endl;
			    			sqlite3_finalize(puStmt);
			    			exit(EXIT_FAILURE);
			    		}
			    	}else{
			    		cout<<"update bind error"<<endl;
			    		sqlite3_finalize(puStmt);
			    		exit(EXIT_FAILURE);
			    	}
			    	sqlite3_reset(puStmt);
			    	break;
			    }
			}else{
				cout<<"query bind error"<<endl;
				sqlite3_finalize(pqStmt);
				exit(EXIT_FAILURE);
			}
			sqlite3_reset(pqStmt);
	    }while(!tmpname.empty());
   
	}else{
		cout<<"query prepared failure rc:"<<rc<<endl;
		sqlite3_finalize(pqStmt);
		exit(EXIT_FAILURE);
	}
	return 1;
}

int sqlite_handle::check_data(const ptr_lib::shared_ptr<const Interest> &interest, Data& data){
	Name name = interest->getName();
	return check_data_plain(name, data);
}

//This function is the first version of data check following longest prefix match. It will return the leftmost data
int sqlite_handle::check_data_plain(Name& name, Data& data){
	sqlite3_stmt* pStmt = NULL;
	sqlite3_stmt* ppStmt = NULL;
	//string sql = string("select * from NDN_REPO where name = ?;");
	string sql = string("select * from NDN_REPO where name = ?;");
	string sql_parent = string("select * from NDN_REPO where pname = ?;");
	int rc = sqlite3_prepare_v2(db, sql_parent.c_str(), -1, &ppStmt, NULL);
	vector<Data> vdata;
	Name tmpname = name;
	if(rc == SQLITE_OK){
		while(check_pname(tmpname)){
			if (sqlite3_bind_blob(ppStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
		        while (1) {
		            rc = sqlite3_step(ppStmt);
		            if (rc == SQLITE_ROW) {
		            	Data edata;
		            	//data.wireDecode(Block(sqlite3_column_blob(pStmt, 1),sqlite3_column_bytes(pStmt, 1)));
		            	edata.wireDecode(Block(sqlite3_column_blob(pStmt, 1),sqlite3_column_bytes(pStmt, 1)));
		            	vdata.push_back(edata);
		            }
		            else if (rc == SQLITE_DONE) {
		               	break;
		            }
			        else {
						cout<<"Database parent query failure rc:"<<rc<<endl;
						sqlite3_finalize(ppStmt);
						exit(EXIT_FAILURE);
			        }
		        }
		    } else{
		    	cout<<"Database parent bind failure rc:"<<rc<<endl;
				sqlite3_finalize(ppStmt);
				exit(EXIT_FAILURE);
		    }
		    sqlite3_reset(ppStmt);
		    //here use sort data, but sort name may be faster. It can be updatated later
		    sort_data_small(vdata);
		    tmpname = vdata.begin()->getName();
		    vdata.clear();
		}

		sqlite3_finalize(ppStmt);


		rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
		if(rc == SQLITE_OK){
			if (sqlite3_bind_blob(pStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
				while (1) {
		            rc = sqlite3_step(pStmt);
		            if (rc == SQLITE_ROW) {
		            	Data edata;
		            	//data.wireDecode(Block(sqlite3_column_blob(pStmt, 1),sqlite3_column_bytes(pStmt, 1)));
		            	edata.wireDecode(Block(sqlite3_column_blob(pStmt, 1),sqlite3_column_bytes(pStmt, 1)));
		            	vdata.push_back(edata);
		            }
		            else if (rc == SQLITE_DONE) {
		               	break;
		            }
			        else {
						cout<<"Database query failure rc:"<<rc<<endl;
						sqlite3_finalize(pStmt);
						exit(EXIT_FAILURE);
			        }
			    }
			}
			sort_data_small(vdata);
			if(vdata.begin()->getContent().empty()){
				cout<<"no such data"<<tmpname<<endl;
			}
		    data.wireDecode(vdata.begin()->wireEncode());
		    /*cout<<"check_data size: "<<data.wireEncode().size()<<endl;
		    cout<<"name: "<<data.getName()<<endl;*/
	    	cout<<data.wireEncode().wire()<<endl;
		    sqlite3_finalize(pStmt);
		} else{
			cout<<"pStmt prepared failed rc:"<<rc<<endl;
			sqlite3_finalize(pStmt);
			exit(EXIT_FAILURE);
		}	    
	} else{
		cout<<"ppStmt prepared failed rc:"<<rc<<endl;
		sqlite3_finalize(ppStmt);
		exit(EXIT_FAILURE);
	}  
	return 0;
}

//retrieve all the leaf nodes of a subtree
int sqlite_handle::check_data_name(Name& name, vector<Name>& vname){
	if(name.empty()){
		cout<<"The name is empty"<<endl;
		return 0;
	}
	sqlite3_stmt* pStmt = NULL;
	sqlite3_stmt* ppStmt = NULL;
	string sql = string("select * from NDN_REPO where name = ?;");
	string psql = string("select * from NDN_REPO where pname = ?;");
	Name tmpname = name;
	//This queue is for internal node;
	queue<Name> qname;
	int childnum;
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if(sqlite3_bind_blob(pStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
			rc = sqlite3_step(pStmt);
			if(rc == SQLITE_ROW){
				childnum = sqlite3_column_int(pStmt, 3);
				cout<<"tmpname:"<<tmpname<<endl;
				cout<<"check_data_name childnum:"<<childnum<<endl;
				if(childnum == 0){
					Name ename;
					ename.wireDecode(Block(sqlite3_column_blob(ppStmt, 0),sqlite3_column_bytes(ppStmt, 0)));
					vname.push_back(ename);
					sqlite3_finalize(pStmt);
					return 1;
				}
			}else if(rc == SQLITE_DONE){
				sqlite3_finalize(pStmt);
				return 0;
			}else{
				cout<<"check error rc:"<<rc<<endl;
				sqlite3_finalize(pStmt);
				exit(EXIT_FAILURE);
			}
		}
	}

	sqlite3_finalize(pStmt);

	rc = sqlite3_prepare_v2(db, psql.c_str(), -1, &ppStmt, NULL);
	qname.push(tmpname);
	if(rc == SQLITE_OK){
		do{
			tmpname = qname.front();
			qname.pop();
			if(sqlite3_bind_blob(ppStmt, 1, tmpname.wireEncode().wire(), tmpname.wireEncode().size(), NULL) == SQLITE_OK){
				while(1){
					rc = sqlite3_step(ppStmt);
					if(rc == SQLITE_ROW){
						Name ename;
						ename.wireDecode(Block(sqlite3_column_blob(ppStmt, 0),sqlite3_column_bytes(ppStmt, 0)));
						childnum = sqlite3_column_int(ppStmt, 3);
						if(childnum > 0){
							qname.push(ename);
						}else{
							vname.push_back(ename);
						}
					}else if(rc == SQLITE_DONE){
						break;
					}else{
						cout<<"check error rc:"<<rc<<endl;
						sqlite3_finalize(ppStmt);
						exit(EXIT_FAILURE);
					}
				}
				sqlite3_reset(ppStmt);
			}else{
				cout<<"bind error"<<endl;
				sqlite3_finalize(ppStmt);
				exit(EXIT_FAILURE);
			}
		}while(!qname.empty());
		sqlite3_finalize(ppStmt);
		return 1;
	}else{
		cout<<"pStmt prepared failed rc:"<<rc<<endl;
		sqlite3_finalize(ppStmt);
		exit(EXIT_FAILURE);
	}
}

//This is the exact name qeury in database.
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

//This is the exact name qeury in database.
int sqlite_handle::check_pname(Name& pname){
	sqlite3_stmt* pStmt = NULL;
	string sql = string("select * from NDN_REPO where pname = ?;");
	int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &pStmt, NULL);
	if(rc == SQLITE_OK){
		if (sqlite3_bind_blob(pStmt, 1, pname.wireEncode().wire(), pname.wireEncode().size(), NULL) == SQLITE_OK){
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

void sqlite_handle::sort_data_small(vector<Data>& datas){
	sort(datas.begin(), datas.end(), sqlite_handle::compare_data_small);
}

bool sqlite_handle::compare_data_small(Data data1, Data data2){
	return (data1.getName()<data2.getName());
}

void sqlite_handle::sort_data_big(vector<Data>& datas){
	sort(datas.begin(), datas.end(), sqlite_handle::compare_data_big);
}

bool sqlite_handle::compare_data_big(Data data1, Data data2){
	return (data1.getName()<data2.getName());
}