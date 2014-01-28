#include <unistd.h>
#include <iostream>

#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/common.hpp>

#include "storage/sqlite/sqlite_handle.h"

using namespace std;
using namespace ndn;

static const string ccnr_usage_message(
"ndn_repo - NDNx Repository Daemon\n"
"Welcome!\n"
);


int main(int argc, char **argv) {
    int opt;
    char* dbpath = 0;
    while ((opt = getopt(argc, argv, "d:h")) != -1) {
        switch (opt) {
        case 'd':
            dbpath = optarg;
        case 'h':
            cout<<ccnr_usage_message<<endl;
        default:
            break;
        }
    }
    sqlite_handle handle(string(dbpath));
    return 0;
}

