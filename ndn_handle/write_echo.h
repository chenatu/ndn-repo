#ifndef WRITE_ECHO_H
#define WRITE_ECHO_H

#include "ndn_handle_common.h"

class write_echo{
public:
  write_echo(Face face, storage_handle* p_handle);
  
  // onInterest.
  void operator()(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void operator()(const Name& prefix, const std::string& reason);

private:
  KeyChain keyChain_;
  Face face_;
  Buffer ndndId_;

  Data responseinfo;

  storage_handle* p_handle_;
};

#endif