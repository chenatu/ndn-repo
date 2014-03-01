#ifndef WRITE_ECHO_H
#define WRITE_ECHO_H

#include "ndn_handle_common.h"

class write_echo{
public:
  write_echo(Face* face, storage_handle* p_handle, repovalidator validator);
  
  // onInterest for insert.
  void onInterest(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed for insert.
  void onRegisterFailed(const Name& prefix, const std::string& reason);


  // onInterest for insert check.
  void onCheckInterest(const Name& prefix, const Interest& interest);
  
  // onRegisterFailed for insert.
  void onCheckRegisterFailed(const Name& prefix, const std::string& reason);

  void validated(const shared_ptr<const Interest>& interest);

  void validationFailed(const shared_ptr<const Interest>& interest);
  // Ask for Data insertation and when data comes
  void onData(ndn::Face &face, const Interest& interest, Data& data, uint64_t processId);
  // Ask for Segmented Data insertation and when data comes
  void onSegData(ndn::Face &face, const Interest& interest, Data& data, uint64_t processId);
  // Ask for Data insertation and when data timeout
  void onTimeout(ndn::Face &face, const Interest& interest);
  // Ask for Data when EndBlockId is null

  void writeListen(const Name& prefix);
  map<uint64_t, repocommandresponse> processMap;
private:
  Face* face_;
  repovalidator validator_;
  KeyChain keyChain_;
  int validres_;
  storage_handle* p_handle_;
};

#endif