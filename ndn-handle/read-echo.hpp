/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef ReadEcho_H
#define ReadEcho_H

#include "ndn-handle-common.hpp"

class ReadEcho{
public:
  ReadEcho(Face* face, StorageHandle* p_handle)
    : face_(face)
    , p_handle_(p_handle)
  { 
  }
  
  // onInterest. Read the name from database
  void 
  onInterest(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void 
  onRegisterFailed(const Name& prefix, const std::string& reason);

  void 
  readListen(const Name& prefix);
  const RegisteredPrefixId *id_;

private:
  KeyChain keyChain_;
  Face* face_;
  Buffer ndndId_;

  StorageHandle* p_handle_;
};

#endif