/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef REPO_COMMAND_PARAMETER_HPP
#define REPO_COMMAND_PARAMETER_HPP

#include <ndn-cpp-dev/encoding/encoding-buffer.hpp>
#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/selectors.hpp>
#include "repo-tlv.hpp"
using namespace ndn; 

class RepoCommandParameter {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };
  RepoCommandParameter(){
    hasStartBlockId_ = 0;
    hasEndBlockId_ = 0;
    hasProcessId_ = 0;
  }
  RepoCommandParameter(const Block& block)
  {
    hasStartBlockId_ = 0;
    hasEndBlockId_ = 0;
    hasProcessId_ = 0;
    wireDecode(block);
  }
  
  const Name&
  getName () const
  {
    return name_;
  }
  
  RepoCommandParameter&
  setName (const Name& name)
  {
    name_ = name;
    wire_.reset ();
    return *this;
  }

  const Selectors&
  getSelectors () const
  {
    return selectors_;
  }
  
  RepoCommandParameter&
  setSelectors (const Selectors& selectors)
  {
    selectors_ = selectors;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasSelectors() const
  {
    return !selectors_.empty();
  }

  uint64_t 
  getStartBlockId () const
  {
    return startBlockId_;
  }

  RepoCommandParameter&
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
  
  RepoCommandParameter&
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

  bool inline
  hasProcessId() const
  {
    return hasProcessId_;
  }  

  RepoCommandParameter&
  setProcessId (uint64_t processId)
  {
    processId_  = processId;
    hasProcessId_ = 1;
    wire_.reset ();
    return *this;
  }

  template<bool T>
  size_t
  wireEncode(EncodingImpl<T> &block) const;
  
  const Block&
  wireEncode () const;
  
  void 
  wireDecode (const Block &wire);

private:

  Name name_;
  Selectors selectors_;
  uint64_t startBlockId_;
  uint64_t endBlockId_;
  uint64_t processId_;

  bool hasStartBlockId_;
  bool hasEndBlockId_;
  bool hasProcessId_;

  mutable Block wire_;
};

template<bool T>
inline size_t
RepoCommandParameter::wireEncode(EncodingImpl<T>& blk) const
{
  size_t total_len = 0;
  size_t var_len = 0;
//prepare name
  total_len += getName().wireEncode(blk);

//prepare selectors
  if (!getSelectors().empty())
  {
    total_len += getSelectors().wireEncode(blk);
  }

//prepare startblockid
  if(hasStartBlockId_){
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
  var_len = blk.prependNonNegativeInteger (processId_);
  total_len += var_len;
  total_len += blk.prependVarNumber (var_len);
  total_len += blk.prependVarNumber (tlv_repo::ProcessId);

//prepare RepoCommandParameter
  total_len += blk.prependVarNumber (total_len);
  total_len += blk.prependVarNumber (tlv_repo::RepoCommandParameter);
  return total_len;
}

inline const Block&
RepoCommandParameter::wireEncode () const
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
RepoCommandParameter::wireDecode (const Block &wire)
{

  wire_ = wire;

  wire_.parse ();

  if (wire_.type() != tlv_repo::RepoCommandParameter)
    throw Error("Requested decoding of RepoCommandParameter, but Block is of different type");

  // Name
  name_.wireDecode(wire_.get(tlv_repo::Name));  

  // Selectors
  Block::element_const_iterator val = wire_.find(tlv_repo::Selectors);
  if (val != wire_.elements_end())
  {
    selectors_.wireDecode(*val);
  }
  else
    selectors_ = Selectors();

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

}

inline std::ostream&
operator << (std::ostream &os, const RepoCommandParameter &option)
{
  os << "ForwardingEntry(";
  
  // Name
  os << "Name: " << option.getName () << ", ";

  // StartBlockId
  os << "StartBlockId: " << option.getStartBlockId () << ", ";

  // EndBlickId
  os << "EndBlockId: " << option.getEndBlockId ();

  //ProcessId
  os << "ProcessId: " << option.getProcessId ();

  os << ")";
  return os;
}

#endif //RepoCommandParameter_H
