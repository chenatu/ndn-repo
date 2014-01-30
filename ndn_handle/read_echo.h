#ifndef READ_ECHO_H
#define READ_ECHO_H

#include "ndn_handle_common.h"

class read_echo{
public:
  read_echo(Face face, storage_handle* p_handle)
    : face_(face)
    , keyChain_()
    , p_handle_(p_handle)
  { 
  }
  
  // onInterest. Read the name from database
  void operator()(const ptr_lib::shared_ptr<const Name>& prefix, const ptr_lib::shared_ptr<const Interest>& interest);
  
  // onRegisterFailed.
  void operator()(const ptr_lib::shared_ptr<const Name>& prefix);
  const RegisteredPrefixId *id_;

private:
  KeyChain keyChain_;
  Face face_;
  Buffer ndndId_;

  storage_handle* p_handle_;
};

#endif