/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef REPOCOMMANDPARAMETER_H
#define REPOCOMMANDPARAMETER_H

#include <ndn-cpp-dev/encoding/encoding-buffer.hpp>
#include <ndn-cpp-dev/encoding/tlv-nfd.hpp>
#include <ndn-cpp-dev/name.hpp>
#include <ndn-cpp-dev/selectors.hpp>


using namespace ndn;  

namespace tlv_repo{
enum {
  // ControlResponse
  RepoCommandParameter = 201,
  Name = 7, // Name
  Selectors = 9,
  StartBlockId = 204,
  EndBlockId = 205,
  ProcessId =206
};
} //tlv_repo


class repocommandparameter {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };
  repocommandparameter(){
    hasStartBlockId_ = 0;
    hasEndBlockId_ = 0;
  }
  repocommandparameter(const Block& block)
  {
    startBlockId_ = 0;
    endBlockId_ = 0;
    wireDecode(block);
  }
  
  const Name&
  getName () const
  {
    return name_;
  }
  
  repocommandparameter&
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
  
  repocommandparameter&
  setSelectors (const Selectors& selectors)
  {
    selectors_ = selectors;
    wire_.reset ();
    return *this;
  }

  bool inline
  hasSelectors() const
  {
    return selectors_.empty();
  }

  uint64_t 
  getStartBlockId () const
  {
    return startBlockId_;
  }
  
  repocommandparameter&
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
  
  repocommandparameter&
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
  
  repocommandparameter&
  setEProcessId (uint64_t processId)
  {
    processId_  = processId;
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

  mutable Block wire_;
};

template<bool T>
inline size_t
repocommandparameter::wireEncode(EncodingImpl<T>& blk) const
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
repocommandparameter::wireEncode () const
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
repocommandparameter::wireDecode (const Block &wire)
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

  // EndBlockId
  val = wire_.find(tlv_repo::ProcessId);
  if (val != wire_.elements_end())
  {
    processId_ = readNonNegativeInteger(*val);
  }

}

inline std::ostream&
operator << (std::ostream &os, const repocommandparameter &option)
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

#endif //REPOCOMMANDPARAMETER_H
