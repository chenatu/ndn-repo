/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#ifndef DeleteEcho_H
#define DeleteEcho_H

#include "ndn-handle-common.hpp"

class DeleteEcho{
public:
  DeleteEcho(Face* face, StorageHandle* p_handle, RepoCommandValidator validator);
  
  // onInterest.
  void 
  onInterest(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed.
  void 
  onRegisterFailed(const Name& prefix, const std::string& reason);
  void 
  validated(const shared_ptr<const Interest>& interest);
  // onInterest for insert check.
  void 
  onCheckInterest(const Name& prefix, const Interest& interest);
  // onRegisterFailed for insert.
  void 
  onCheckRegisterFailed(const Name& prefix, const std::string& reason);
  void 
  validationFailed(const shared_ptr<const Interest>& interest);

  void 
  deleteListen(const Name& prefix);


private:
  Face *face_;
  RepoCommandValidator validator_;
  KeyChain keyChain_;
  int validres_;

  StorageHandle* p_handle_;
};

#endif