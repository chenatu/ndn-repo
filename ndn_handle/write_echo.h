#ifndef WRITE_ECHO_H
#define WRITE_ECHO_H

#include "ndn_handle_common.h"

class write_echo{
public:
  write_echo(Face face, storage_handle* p_handle)
    : face_(face)
    , keyChain_()
    , p_handle_(p_handle)
  { 
  }
  
  // onInterest.
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