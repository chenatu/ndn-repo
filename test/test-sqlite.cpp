/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#include <unistd.h>
#include <iostream>


#include "../storage/sqlite/sqlite-handle.hpp"
#include <ndn-cpp-dev/security/key-chain.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


using namespace std;
using namespace ndn;

static const string ccnr_usage_message(
"ndn_repo - NDNx Repository Daemon\n"
"Welcome!\n"
);

int main(int argc, char **argv) {
    int opt;
    string dbpath;
    while ((opt = getopt(argc, argv, "d:h")) != -1) {
        switch (opt) {
        case 'd':
            dbpath = string(optarg);
        case 'h':
            cout<<ccnr_usage_message<<endl;
        default:
            break;
        }
    }
   SqliteHandle handle = SqliteHandle(dbpath);

    KeyChain keyChain;

    Name name("/a/8");
    uint8_t content[10000];
    memset(content, '-', 10000);
    /*cout<<"data size: "<<data.wireEncode().size()<<endl;
    cout<<data.wireEncode().wire()<<endl;

    cout<<"data: "<<data.getName().toUri()<<endl;

    Name name2("/a/b/c/d/1/2");
    //Name name3("/a/b/c/d/3");

    Data data2(name2);
    data2.setFreshnessPeriod(2000); // 10 sec
    data2.setContent((const uint8_t*)"HELLO DOGGY", sizeof("HELLO DOGGY"));
    keyChain.sign(data2);
    cout<<"data2 size: "<<data2.wireEncode().size()<<endl;
    cout<<data2.wireEncode().wire()<<endl;
    Name name3("/a/b");
    Interest interest;
    interest.setName(name);
    Exclude exclude;
    Name::Component comp = name.get(3);
    exclude.appendExclude(comp, 0);
    interest.setExclude(exclude);
    interest.setChildSelector(1);

    Interest interest2;
    interest2.setName(name2);
    Interest interest3;*/
    Data newdata;
    Interest i;
    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();
    Name tmpname;   
    cout<<"start"<<endl;
    int j = 0;
    for(j = 1; j <= 1000; j++){
        tmpname.wireDecode(name.wireEncode());
        tmpname.appendSegment(j);
        i.setName(tmpname);
        Data data(tmpname);
        data.setContent((const uint8_t*)content, sizeof(content));
        keyChain.sign(data);
        //cout<<"tmpname: "<<tmpname<<endl;
        //cout<<"seg:"<<j<<endl;
        handle.insert_data(tmpname, data); 
       //handle.delete_data(tmpname);
        //cout<<j<<endl;
    }
    boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = t2 - t1;

    cout<<diff<<endl;
    cout<<"end"<<endl;
    //Data newdata;
    //handle.insert_data(name2, data2);
    //handle.insert_data(name, data);
    //handle.check_data(interest, newdata);
    //cout<<"newdata size: "<<newdata.wireEncode().size()<<endl;
    //cout<<newdata.wireEncode().wire()<<endl;
    //handle.insert_data(name, data);
    //handle.check_data(name, newdata);
    //cout<<newdata.wireEncode().wire()<<endl;
    //handle.insert_data(prefix, data);
    //handle.check_data(prefix, newdata);
    //cout<<newdata.wireEncode().wire()<<endl;
    //handle.delete_data(interest, name);
    //handle.delete_data(interest2, name2);
    //handle.delete_data(name3);

    /*vector<Name> vname;
    Name name4("/a/b");
    handle.check_data_name(name4, vname);

    vector<Name>::iterator iter;
    for(iter = vname.begin(); iter != vname.end(); iter++){
        cout<<"vname:"<<iter->wireEncode().wire()<<endl;
    }*/

    return 0;
}

