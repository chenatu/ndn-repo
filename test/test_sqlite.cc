#include <unistd.h>
#include <iostream>


#include "../storage/sqlite/sqlite_handle.h"

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
    string name = string("/a/b/c");
    string &rname = name;
    string data = string("abc");
    string &rdata = data;
    handle.insert_encrypted_data(rname, rdata);
    handle.check_data(rname, rdata);
    cout<<rdata<<endl;
    handle.delete_data(rname);
    return 0;
}

