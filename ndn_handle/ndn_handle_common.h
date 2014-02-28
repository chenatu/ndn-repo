#ifndef NDN_HANDLE_COMMON_H
#define NDN_HANDLE_COMMON_H


#include <cstdlib>
#include <sstream>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <ndn-cpp-dev/face.hpp>
//#include <ndn-cpp-dev/status-response.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>
#include "../helper/repovalidator.h"
//#include <ndn-cpp-dev/helper/command-interest-validator.hpp>
//#include <ndn-cpp-dev/management/nfd-control-response.hpp>
#include "../helper/repocommandresponse.h"
#include "../helper/repocommandparameter.h"
#include "../storage/storage_handle.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <map>

using namespace std;
using namespace ndn;
using namespace nfd;

#endif