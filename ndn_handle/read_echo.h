/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef READ_ECHO_H
#define READ_ECHO_H

#include "ndn_handle_common.h"

class read_echo{
public:
  read_echo(Face* face, storage_handle* p_handle)
    : face_(face)
    , p_handle_(p_handle)
  { 
  }
  
  // onInterest. Read the name from database
  void onInterest(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void onRegisterFailed(const Name& prefix, const std::string& reason);

  void readListen(const Name& prefix);
  const RegisteredPrefixId *id_;

private:
  KeyChain keyChain_;
  Face* face_;
  Buffer ndndId_;

  storage_handle* p_handle_;
};

#endif