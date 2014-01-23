
#include <iostream>

using namespace std;

#include <unistd.h>
#include <iostream>
#include "storage/sqlite/sqlite_handle.h"

static const string ccnr_usage_message(
"ndn_repo - NDNx Repository Daemon\n"
"Welcome!\n"
);


int main(int argc, char **argv) {
    int opt;
    sqlite_handle handle;
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
        case 'h':
            cout<<ccnr_usage_message<<endl;
        default:
            break;
        }
    }
    return 0;
}

