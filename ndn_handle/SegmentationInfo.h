/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef SEGMENTATIONINFO_H
#define SEGMENTATIONINFO_H

#include <ndn-cpp-dev/encoding/encoding-buffer.hpp>
#include <ndn-cpp-dev/encoding/tlv-nfd.hpp>

namespace tlv_repo{
enum {
  // ControlResponse
  SegmentationInfo = 201,
  StartBlockId = 202,
  EndBlockId = 203,
};
} //tlv_repo


class SegmentationInfo {
public:
  struct Error : public Tlv::Error { Error(const std::string &what) : Tlv::Error(what) {} };

  SegmentationInfo(const Block& block)
  {
    wireDecode(block);
  }
  
  uint64_t 
  getStartBlockId () const
  {
    return startBlockId_;
  }
  
  SegmentationInfo&
  setStartBlockId (uint64_t startBlockId)
  {
    startBlockId_  = startBlockId;
    wire_.reset ();
    return *this;
  }

  uint64_t 
  getEndBlockId () const
  {
    return endBlockId_;
  }
  
  SegmentationInfo&
  setEndBlockId (uint64_t endBlockId)
  {
    endBlockId_  = endBlockId;
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
  uint64_t startBlockId_;
  uint64_t endBlockId_;

  mutable Block wire_;
};

template<bool T>
inline size_t
SegmentationInfo::wireEncode(EncodingImpl<T>& blk) const
{
  size_t total_len = 0;
  size_t var_len = 0;
//prepare startblockid
  var_len = blk.prependNonNegativeInteger (startBlockId_);
  total_len += var_len;
  total_len += blk.prependVarNumber (var_len);
  total_len += blk.prependVarNumber (tlv_repo::StartBlockId);

//prepare endblockid
  var_len = blk.prependNonNegativeInteger (endBlockId_);
  total_len += var_len;
  total_len += blk.prependVarNumber (var_len);
  total_len += blk.prependVarNumber (tlv_repo::EndBlockId);

//prepare segmentationinfo
  total_len += blk.prependVarNumber (total_len);
  total_len += blk.prependVarNumber (tlv_repo::SegmentationInfo);
  return total_len;
}

inline const Block&
SegmentationInfo::wireEncode () const
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
SegmentationInfo::wireDecode (const Block &wire)
{

  wire_ = wire;

  if (wire_.type() != tlv_repo::SegmentationInfo)
    throw Error("Requested decoding of SegmentationInfo, but Block is of different type");
  
  wire_.parse ();

  // StartBlockId
  Block::element_const_iterator val = wire_.find(tlv_repo::StartBlockId);
  if (val != wire_.elements_end())
  {
    startBlockId_ = readNonNegativeInteger(*val);
  }

  // EndBlockId
  val = wire_.find(tlv_repo::EndBlockId);
  if (val != wire_.elements_end())
  {
    endBlockId_ = readNonNegativeInteger(*val);
  }

}

inline std::ostream&
operator << (std::ostream &os, const SegmentationInfo &option)
{
  os << "ForwardingEntry(";
  
  // StartBlockId
  os << "StartBlockId: " << option.getStartBlockId () << ", ";

  // EndBlickId
  os << "EndBlockId: " << option.getEndBlockId ();

  os << ")";
  return os;
}

#endif //SEGMENTATIONINFO_H
