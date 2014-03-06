/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * @author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 * See COPYING for copyright and distribution information.
 */

// correct way to include NDN-CPP headers
#include <ndn-cpp-dev/face.hpp>
#include <ndn-cpp-dev/security/key-chain.hpp>
#include <ndn-cpp-dev/helpers/command-interest-generator.hpp>
#include <iostream>

#include "../helper/repocommandparameter.h"
//#include "face.hpp"

using namespace std;
using namespace ndn;


class Producer
{
public:
  Producer(Face* face, KeyChain& keyChain)
  : face_(face)
  , keyChain_(keyChain)
  {
  }

  void
  onInterest(const Name& name, const Interest& interest)
  {
    cout<<"onInterest"<<endl;
    std::cout << "<< I: " << interest << std::endl;
    
    ndn::Data data(ndn::Name(interest.getName()));
    //cout<<<<interest.getName()<<endl;
    cout<<"data.getName(): "<<data.getName()<<endl;
    data.setFreshnessPeriod(1000); // 10 sec

    data.setContent((const uint8_t*)"HELLO KITTY", sizeof("HELLO KITTY"));

    keyChain_.sign(data);

    cout << ">> D: " << data.getName().toUri() << std::endl;
    face_->put(data);
  }


  void
  onRegisterFailed (const ndn::Name& prefix, const std::string& reason)
  {
    std::cerr << "ERROR: Failed to register prefix in local hub's daemon (" << reason << ")" << std::endl;
    face_->shutdown ();
  }
  
  void
  listen()
  {
    face_->setInterestFilter("/a/b/c/d",
                            func_lib::bind(&Producer::onInterest, this, _1, _2),
                            func_lib::bind(&Producer::onRegisterFailed, this, _1, _2));
    cout<<"setInterestFilter: /a/b/c/d"<<endl;
  }

private:
  Face* face_;
  KeyChain keyChain_;

  Buffer ndndId_;
};
void
onData(ndn::Face &face,
       const ndn::Interest& interest, ndn::Data& data)
{
  cout<<"onData"<<endl;
  std::cout << "I: " << interest.toUri() << std::endl;
  std::cout << "D: " << data.getName().toUri() << std::endl;


}

void
onTimeout(ndn::Face &face,
          const ndn::Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}

int main()
{
  try {
    CommandInterestGenerator generator;
    //ndn::Interest i(ndn::Name("/a/b/c/e/a/b/c/d/7"));
    Name name("/a/b/c/insert");
    
    repocommandparameter rpara;
    rpara.setStartBlockId(0);
    rpara.setEndBlockId(100);

    rpara.setName(Name("/a/b/c/d/8"));

    name.append(rpara.wireEncode());
    
    ndn::Interest i(name);
    KeyChain keyChain;

    generator.generateWithIdentity(i, keyChain.getDefaultIdentity());

    ndn::Face face;
    face.expressInterest(i,
                          ndn::bind(onData, boost::ref(face), _1, _2),
                          ndn::bind(onTimeout, boost::ref(face), _1));
    
    Producer producer(&face, keyChain);
    producer.listen();
    face.processEvents();
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
