/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef NDN_HANDLE_COMMON_H
#define NDN_HANDLE_COMMON_H


#include <cstdlib>
#include <sstream>
#include <iostream>
#include <time.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unistd.h>
#include <ndn-cpp-dev/face.hpp>
//#include <ndn-cpp-dev/status-response.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>
#include "../helper/repo-command-validator.hpp"
//#include <ndn-cpp-dev/helper/command-interest-validator.hpp>
//#include <ndn-cpp-dev/management/nfd-control-response.hpp>
#include "../helper/repo-command-response.hpp"
#include "../helper/repo-command-parameter.hpp"
#include "../storage/storage-handle.hpp"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <map>

#define TIMEOUTRETRY 3
#define DEFAULTCREDIT 12
#define NOENDTIMEOUT 100000

using namespace std;
using namespace ndn;
using namespace nfd;

#endif