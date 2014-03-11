/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <time.h>
//compile json
//#include <jsoncpp/json/json.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp> 
#include <fstream>

#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>
#include <ndn-cpp-dev/helpers/command-interest-generator.hpp>
#include <assert.h>
//#include <ndn-cpp-dev/helper/command-interest-validator.hpp>

#include "../storage/storage-handle.hpp"
#include "../storage/sqlite/sqlite-handle.hpp"
#include "../ndn-handle/write-echo.hpp"
#include "../ndn-handle/read-echo.hpp"
#include "../ndn-handle/delete-echo.hpp"

using namespace std;
using namespace ndn;

static const string 
ndn_repo_usage_message(
"ndn-repo - NDNx Repository Daemon\n"
"Welcome!\n"
);

int 
confInit(string confpath, ReadEcho& recho, WriteEcho& wecho, DeleteEcho& decho);

int 
main(int argc, char **argv) {
    int opt;
    string dbpath;
    string confpath;
    while ((opt = getopt(argc, argv, "d:hc:")) != -1) {
        switch (opt) {
        case 'd':
            dbpath = string(optarg);
        case 'h':
            cout<<ndn_repo_usage_message<<endl;
        case 'c':
            confpath = string(optarg);
        default:
            break;
        }
    }

    SqliteHandle s_handle = SqliteHandle(dbpath);
    StorageHandle *p_handle = &s_handle;

//copy from ndn-ccp testfile
    try {
        Face face;

        //validation set up
        KeyChain keyChain;
        RepoCommandValidator validator;
        validator.addInterestRule("^<>", *keyChain.getCertificate(keyChain.getDefaultCertificateName()));
        cout<<"default cert"<<keyChain.getDefaultCertificateName()<<endl;

        ReadEcho recho(&face, p_handle);
        WriteEcho wecho(&face, p_handle, validator);
        DeleteEcho decho(&face, p_handle, validator);
        if(confpath.empty()){
            confpath = "./repo.conf";
        }
        confInit(confpath, recho, wecho, decho);
        face.processEvents();
    } catch (std::exception& e) {
        cout << "exception: " << e.what() << endl;
    }


    return 0;
}

int 
confInit(string confpath, ReadEcho& recho, WriteEcho& wecho, DeleteEcho& decho){
    ifstream fin;
    cout<<confpath.c_str()<<endl;
    fin.open(confpath.c_str());
    assert(fin.is_open());

    boost::property_tree::ptree pt;  
    try{      
        boost::property_tree::read_json(fin, pt);  
    }  
    catch(boost::property_tree::ptree_error & e) {
        cout<<"read_json error"<<endl;
        return 0;   
    }
    boost::property_tree::ptree dataConf = pt.get_child("data-conf");
    BOOST_FOREACH(const boost::property_tree::ptree::value_type& prefix, dataConf) {
        cout<<"readListen: "<<prefix.second.get<std::string>("prefix")<<endl;
        recho.readListen(Name(prefix.second.get<std::string>("prefix")));
    }
    boost::property_tree::ptree commandConf = pt.get_child("command-conf");
    BOOST_FOREACH(const boost::property_tree::ptree::value_type& command, commandConf){
        string commandVerb = command.second.get<std::string>("verb");
        if(commandVerb == "insert"){
            wecho.writeListen(Name(command.second.get<std::string>("repo-prefix")).append("insert"));
            cout<<"writeListen: "<<Name(command.second.get<std::string>("repo-prefix")).append("insert")<<endl;
        }else if(commandVerb == "insert-check"){
            wecho.writeCheckListen(Name(command.second.get<std::string>("repo-prefix")).append("insert-check"));
            cout<<"writeCheckListen: "<<Name(command.second.get<std::string>("repo-prefix")).append("insert-check")<<endl;
        }else if(commandVerb == "delete"){
            decho.deleteListen(Name(command.second.get<std::string>("repo-prefix")).append("delete"));
            cout<<"deleteListen: "<<Name(command.second.get<std::string>("repo-prefix")).append("delete")<<endl;
        }else if(commandVerb == "delete-check"){

        }else{
            cout<<"command verb not supported"<<endl;
        }
    }
    return 1;
}