#ifndef DELETE_ECHO_H
#define DELETE_ECHO_H

#include "ndn_handle_common.h"

class delete_echo{
public:
  delete_echo(Face* face, storage_handle* p_handle, repovalidator validator);
  
  // onInterest.
  void operator()(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void operator()(const Name& prefix, const std::string& reason);
  void validated(const shared_ptr<const Interest>& interest);
  void validationFailed(const shared_ptr<const Interest>& interest);
private:
  Face *face_;
  repovalidator validator_;
  KeyChain keyChain_;
  int validres_;

  storage_handle* p_handle_;
};

#endif