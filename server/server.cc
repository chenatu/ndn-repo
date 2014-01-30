#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <time.h>

#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>

#include "../storage/storage_handle.h"
#include "../storage/sqlite/sqlite_handle.h"
#include "../ndn_handle/write_echo.h"
#include "../ndn_handle/read_echo.h"

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

    sqlite_handle s_handle = sqlite_handle(dbpath);
    storage_handle *p_handle = &s_handle;

//copy from ndn-ccp testfile
    try {
        Face face;

        read_echo echo(face, p_handle);

        Name prefix("/local/test");
        cout << "Register prefix  " << prefix.toUri() << endl;
        echo.id_ = face.setInterestFilter(prefix, func_lib::ref(echo), func_lib::ref(echo));

        face.processEvents();
    } catch (std::exception& e) {
        cout << "exception: " << e.what() << endl;
    }


    return 0;
}

