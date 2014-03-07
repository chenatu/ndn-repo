/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef DELETE_ECHO_H
#define DELETE_ECHO_H

#include "ndn_handle_common.h"

class delete_echo{
public:
  delete_echo(Face* face, storage_handle* p_handle, repovalidator validator);
  
  // onInterest.
  void onInterest(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void onRegisterFailed(const Name& prefix, const std::string& reason);
  void validated(const shared_ptr<const Interest>& interest);
  // onInterest for insert check.
  void onCheckInterest(const Name& prefix, const Interest& interest);
  // onRegisterFailed for insert.
  void onCheckRegisterFailed(const Name& prefix, const std::string& reason);
  void validationFailed(const shared_ptr<const Interest>& interest);

  void deleteListen(const Name& prefix);


private:
  Face *face_;
  repovalidator validator_;
  KeyChain keyChain_;
  int validres_;

  storage_handle* p_handle_;
};

#endif