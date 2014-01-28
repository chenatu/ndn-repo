#ifndef WRITE_ECHO_H
#define WRITE_ECHO_H

#include "ndn_handle_common.h"

class write_echo{
public:
  write_echo(KeyChainImpl<SecPublicInfoMemory, SecTpmMemory> &keyChain, Face &face)
    : keyChain_(keyChain)
    , face_(face)
    , responseCount_(0)
  { 
  }
  
  // onInterest.
  void operator()(const ptr_lib::shared_ptr<const Name>& prefix, const ptr_lib::shared_ptr<const Interest>& interest);
  
  // onRegisterFailed.
  void operator()(const ptr_lib::shared_ptr<const Name>& prefix);

  KeyChainImpl<SecPublicInfoMemory, SecTpmMemory> &keyChain_;
  Face &face_;
  int responseCount_;
  const RegisteredPrefixId *id_;
};

#endif