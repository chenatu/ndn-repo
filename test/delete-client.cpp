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
#include "../helpers/repo-command-parameter.hpp"
#include <iostream>
//#include "face.hpp"

using namespace std;
using namespace ndn;


void
onData(ndn::Face &face,
       const ndn::Interest& interest, ndn::Data& data)
{
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
    Name name("/a/b/c/delete");
    RepoCommandParameter rpara;
    rpara.setName("/a/b/c/d/8");
    Selectors selectors;
    selectors.setMinSuffixComponents(0);
    rpara.setSelectors(selectors);

    /*rpara.setStartBlockId(5);
    rpara.setEndBlockId(9);*/

    name.append(rpara.wireEncode());
    Interest i(name);
    KeyChain keyChain;

    generator.generateWithIdentity(i, keyChain.getDefaultIdentity());

    i.setInterestLifetime(10000);
    ndn::Face face;
    face.expressInterest(i,
                          ndn::bind(onData, boost::ref(face), _1, _2),
                          ndn::bind(onTimeout, boost::ref(face), _1));
    face.processEvents();
  }
  catch(std::exception &e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}
