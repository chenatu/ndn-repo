#ifndef SQLITE_HANDLE_H
#define SQLITE_HANDLE_H

#include "../storage_handle.h"

#include <string>
#include <iostream>
#include <sqlite3.h> 
#include <stdlib.h>
#include <vector>
#include <queue>
//This is used for canonical order sorting
#include <algorithm>

using namespace std;

class sqlite_handle:public storage_handle{
public:
	sqlite_handle();
	sqlite_handle(string dbpath);
	~sqlite_handle();
	virtual int insert_data(Name& name, Data &data);
	virtual int delete_data(Name& name);
	//The simpliest check that to check the specific version and segment
	virtual int check_data(const Interest& interest, Data& data);
//All the fucntions and elements below should be private. Public this period just for testing
//private:
	//One name, one data response
	int check_data(Name &name, Data& data);
	//If has the name in database, return 1, else return 0
	int check_name(Name& name);
	//If has the pname in database, return 1, else return 0
	int check_pname(Name& pname);
	//This check_data is for no selector, it will reply the leftmost data
	int check_data_plain(Name& name, Data& data);
	//This filter ckeck will filter data by name
	int check_data_name(Name& name, vector<Name>& vname);
	//MinSuffixComponents filter
	int check_name_minsuffix(Name& name, int minSuffixComponents, vector<Name>& vname);
	//MaxSuffixComponents filter
	int check_name_maxsuffix(Name& name, int maxSuffixComponents, vector<Name>& vname);
	//Exclude filter
	int check_name_exclude(Name& name, const Exclude& exclude, vector<Name>& vname);
	//ChildSelector filter
	int check_name_child(Name& name, int childselector, vector<Name>& vname, Name& resname);
	//sort the names in canonical order
	void sort_data_small(vector<Data>& datas);
	//compare data with the name of the data in canonical order
	static bool compare_data_small(Data data1, Data data2);
	//sort the names in reverse canonical order
	void sort_data_big(vector<Data>& datas);
	//compare data with the name of the data in reverse canonical order
	static bool compare_data_big(Data data1, Data data2);
	
	void sort_name_small(vector<Name>& names);

	static bool compare_name_small(Name name1, Name name2);

	void sort_name_big(vector<Name>& names);

	static bool compare_name_big(Name name1, Name name2);

	//This is for name and data database
	sqlite3 *db;
	//db and index will be in the same directory
	char* dbpath;
};

#endif