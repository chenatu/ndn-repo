#include <unistd.h>
#include <iostream>


#include "../storage/sqlite/sqlite_handle.h"
#include <ndn-cpp-dev/security/key-chain.hpp>

using namespace std;

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
    sqlite_handle handle = sqlite_handle(dbpath);

    KeyChain keyChain;

    Name name("/a/b/c/d/1");

    Data data;
    data.setFreshnessPeriod(1000); // 10 sec
    data.setContent((const uint8_t*)"HELLO KITTY", sizeof("HELLO KITTY"));
    keyChain.sign(data);

    Data newdata;

    Name prefix("/local/test");

    handle.insert_data(name, data);
    //handle.check_data(name, newdata);
    //cout<<newdata.wireEncode().wire()<<endl;
    //handle.insert_data(name, data);
    //handle.check_data(name, newdata);
    //cout<<newdata.wireEncode().wire()<<endl;
    //handle.insert_data(prefix, data);
    //handle.check_data(prefix, newdata);
    //cout<<newdata.wireEncode().wire()<<endl;
    //handle.delete_data(name);
    return 0;
}

