#include "write_echo.h"

// Interest.
void write_echo::operator()
(const ptr_lib::shared_ptr<const Name>& prefix, const ptr_lib::shared_ptr<const Interest>& interest) {
  ++responseCount_;
    
  // Make and sign a Data packet.
  Data data(interest->getName());
  string content(string("Echo ") + interest->getName().toUri());
  data.setFreshnessPeriod(1000);
  data.setContent((const uint8_t *)&content[0], content.size());

  keyChain_.sign(data);

  // Put data on wire
  face_.put(data);

  // Unregister prefix to ensure that the processing thread finishes after Data
  // packet is send out to the forwarder
  face_.unsetInterestFilter(id_);
}

// onRegisterFailed.
void write_echo::operator()(const ptr_lib::shared_ptr<const Name>& prefix){
  ++responseCount_;
  cout << "Register failed for prefix " << prefix->toUri() << endl;
}