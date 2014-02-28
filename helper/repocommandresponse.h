/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef REPOCOMMANDRESPONSE_HPP
#define REPOCOMMANDRESPONSE_HPP

#include <ndn-cpp-dev/encoding/block.hpp>
#include <ndn-cpp-dev/encoding/tlv-nfd.hpp>
#include "repo_tlv.h"

/**
 * @brief Class defining abstraction of repocommandresponse for NFD Control Protocol
 *
 * @see http://redmine.named-data.net/projects/nfd/wiki/ControlCommand
 */


class repocommandresponse {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };
  repocommandresponse(){
    hasStartBlockId_ = 0;
    hasEndBlockId_ = 0;
    hasProcessId_ = 0;
    hasInsertNum_ = 0;
    hasDeleteNum_ = 0;
  }
  repocommandresponse(const Block& block)
  {
    startBlockId_ = 0;
    endBlockId_ = 0;
    hasProcessId_ = 0;
    hasInsertNum_ = 0;
    hasDeleteNum_ = 0;
    wireDecode(block);
  }
  

  uint64_t 
  getStartBlockId () const
  {
    return startBlockId_;
  }
  
  repocommandresponse&
  setStartBlockId (uint64_t startBlockId)
  {
    startBlockId_  = startBlockId;
    hasStartBlockId_ = 1;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasStartBlockId() const
  {
    return hasStartBlockId_;
  }

  uint64_t 
  getEndBlockId () const
  {
    return endBlockId_;
  }
  
  repocommandresponse&
  setEndBlockId (uint64_t endBlockId)
  {
    endBlockId_  = endBlockId;
    hasEndBlockId_ = 1;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasEndBlockId() const
  {
    return hasEndBlockId_;
  }


  uint64_t 
  getProcessId () const
  {
    return processId_;
  }
  
  repocommandresponse&
  setProcessId (uint64_t processId)
  {
    processId_  = processId;
    hasProcessId_ = 1;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasProcessId() const
  {
    return hasProcessId_;
  }

  uint64_t
  getStatusCode () const
  {
    return statusCode_;
  }
  
  repocommandresponse&
  setStatusCode (uint64_t statusCode)
  {
    statusCode_  = statusCode;
    hasStatusCode_ = 1;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasStatusCode() const
  {
    return hasStatusCode_;
  }

  uint64_t 
  getInsertNum () const
  {
    return insertNum_;
  }
  
  repocommandresponse&
  setInsertNum (uint64_t insertNum)
  {
    insertNum_  = insertNum;
    hasInsertNum_ = 1;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasInsertNum() const
  {
    return hasInsertNum_;
  }

  uint64_t 
  getDeleteNum () const
  {
    return deleteNum_;
  }
  
  repocommandresponse&
  setDeleteNum (uint64_t deleteNum)
  {
    deleteNum_  = deleteNum;
    hasDeleteNum_ = 1;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasDeleteNum() const
  {
    return hasDeleteNum_;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;
  
  const Block&
  wireEncode () const;
  
  void 
  wireDecode (const Block &wire);

private:

  uint64_t statusCode_;
  uint64_t startBlockId_;
  uint64_t endBlockId_;
  uint64_t processId_;
  uint64_t insertNum_;
  uint64_t deleteNum_;


  bool hasStartBlockId_;
  bool hasEndBlockId_;
  bool hasProcessId_;
  bool hasInsertNum_;
  bool hasDeleteNum_;
  bool hasStatusCode_;

  mutable Block wire_;
};

template<bool T>
inline size_t
repocommandresponse::wireEncode(EncodingImpl<T>& blk) const
{
  size_t total_len = 0;
  size_t var_len = 0;

//prepare startblockid
  if(hasEndBlockId_){
    var_len = blk.prependNonNegativeInteger (startBlockId_);
    total_len += var_len;
    total_len += blk.prependVarNumber (var_len);
    total_len += blk.prependVarNumber (tlv_repo::StartBlockId);
  }

//prepare endblockid
  if(hasEndBlockId_){
    var_len = blk.prependNonNegativeInteger (endBlockId_);
    total_len += var_len;
    total_len += blk.prependVarNumber (var_len);
    total_len += blk.prependVarNumber (tlv_repo::EndBlockId);
  }

//prepare processid
  if(hasProcessId_){
    var_len = blk.prependNonNegativeInteger (processId_);
    total_len += var_len;
    total_len += blk.prependVarNumber (var_len);
    total_len += blk.prependVarNumber (tlv_repo::ProcessId);
  }

  if(hasStatusCode_){
    var_len = blk.prependNonNegativeInteger (statusCode_);
    total_len += var_len;
    total_len += blk.prependVarNumber (var_len);
    total_len += blk.prependVarNumber (tlv_repo::StatusCode);
  }

  if(hasInsertNum_){
    var_len = blk.prependNonNegativeInteger (insertNum_);
    total_len += var_len;
    total_len += blk.prependVarNumber (var_len);
    total_len += blk.prependVarNumber (tlv_repo::InsertNum);
  }

  if(hasDeleteNum_){
    var_len = blk.prependNonNegativeInteger (deleteNum_);
    total_len += var_len;
    total_len += blk.prependVarNumber (var_len);
    total_len += blk.prependVarNumber (tlv_repo::DeleteNum);
  }

//prepare repocommandresponse
  total_len += blk.prependVarNumber (total_len);
  total_len += blk.prependVarNumber (tlv_repo::RepoCommandResponse);
  return total_len;
}

inline const Block&
repocommandresponse::wireEncode () const
{
  if (wire_.hasWire ())
    return wire_;

  EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);
  
  EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  wire_ = buffer.block();
  return wire_;
}

inline void 
repocommandresponse::wireDecode (const Block &wire)
{

  wire_ = wire;

  wire_.parse ();

  Block::element_const_iterator val;

  if (wire_.type() != tlv_repo::RepoCommandResponse)
    throw Error("Requested decoding of repocommandresponse, but Block is of different type");

  // StartBlockId
  val = wire_.find(tlv_repo::StartBlockId);
  if (val != wire_.elements_end())
  {
    hasStartBlockId_ = 1;
    startBlockId_ = readNonNegativeInteger(*val);
  }

  // EndBlockId
  val = wire_.find(tlv_repo::EndBlockId);
  if (val != wire_.elements_end())
  {
    hasEndBlockId_ = 1;
    endBlockId_ = readNonNegativeInteger(*val);
  }

  // ProcessId
  val = wire_.find(tlv_repo::ProcessId);
  if (val != wire_.elements_end())
  {
    hasProcessId_ = 1;
    processId_ = readNonNegativeInteger(*val);
  }

  // StatusCode
  val = wire_.find(tlv_repo::StatusCode);
  if (val != wire_.elements_end())
  {
    hasStatusCode_ = 1;
    statusCode_ = readNonNegativeInteger(*val);
  }

  // InsertNum
  val = wire_.find(tlv_repo::InsertNum);
  if (val != wire_.elements_end())
  {
    hasInsertNum_ = 1;
    insertNum_ = readNonNegativeInteger(*val);
  }

  // DeleteNum
  val = wire_.find(tlv_repo::DeleteNum);
  if (val != wire_.elements_end())
  {
    hasDeleteNum_ = 1;
    deleteNum_ = readNonNegativeInteger(*val);
  }

}

inline std::ostream&
operator << (std::ostream &os, const repocommandresponse &option)
{
  os << "ForwardingEntry(";
  
  //ProcessId
  os << "ProcessId: " << option.getProcessId ();

  os << ")";
  return os;
}

#endif //repocommandresponse_H
