#ifndef WRITE_ECHO_H
#define WRITE_ECHO_H

#include "ndn_handle_common.h"

class write_echo{
public:
  write_echo(Face face, storage_handle* p_handle, CommandInterestValidator validator);
  
  // onInterest.
  void operator()(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void operator()(const Name& prefix, const std::string& reason);
  void validated(const shared_ptr<const Interest>& interest);
  void validationFailed(const shared_ptr<const Interest>& interest);
private:
  Face face_;
  CommandInterestValidator validator_;
  int validres;

  storage_handle* p_handle_;
};

#endif