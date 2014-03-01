#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <time.h>

#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>
#include <ndn-cpp-dev/helpers/command-interest-generator.hpp>
//#include <ndn-cpp-dev/helper/command-interest-validator.hpp>

#include "../storage/storage_handle.h"
#include "../storage/sqlite/sqlite_handle.h"
#include "../ndn_handle/write_echo.h"
#include "../ndn_handle/read_echo.h"
#include "../ndn_handle/delete_echo.h"

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
        read_echo recho(&face, p_handle);
        //read prefix set
        Name rprefix("/a/b/c/d");
        cout << "Register prefix  " << rprefix.toUri() << endl;
        face.setInterestFilter(rprefix, func_lib::ref(recho), func_lib::ref(recho));
        //validation set up
        KeyChain keyChain;
        repovalidator validator;
        validator.addInterestRule("^<>", *keyChain.getCertificate(keyChain.getDefaultCertificateName()));
        cout<<"default cert"<<keyChain.getDefaultCertificateName()<<endl;
        //write prefix set up
        /*Name wprefix("/a/b/c/e");
        write_echo wecho(&face, p_handle, validator);
        cout << "Register prefix  " << wprefix.toUri() << endl;
        face.setInterestFilter(wprefix, func_lib::ref(wecho), func_lib::ref(wecho));*/

        Name wprefix("/a/b/c/e");
        write_echo wecho(&face, p_handle, validator);
        wecho.writeListen(wprefix);

        //delete prefix set up
        Name dprefix("/a/b/c/f");
        delete_echo decho(&face, p_handle, validator);

        face.processEvents();
    } catch (std::exception& e) {
        cout << "exception: " << e.what() << endl;
    }


    return 0;
}

