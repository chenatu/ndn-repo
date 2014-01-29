#include "read_echo.h"

// Interest.
void read_echo::operator()
(const ptr_lib::shared_ptr<const Name>& prefix, const ptr_lib::shared_ptr<const Interest>& interest) {
  std::cout << "<< I: " << *interest << std::endl;

  ndn::Data data(ndn::Name(interest->getName()).append("testApp").appendVersion());
  data.setFreshnessPeriod(1000); // 10 sec

  data.setContent((const uint8_t*)"HELLO KITTY", sizeof("HELLO KITTY"));

  keyChain_.sign(data);
  
  std::cout << ">> D: " << data << std::endl;
  face_.put(data);
}

// onRegisterFailed.
void read_echo::operator()(const ptr_lib::shared_ptr<const Name>& prefix){
  std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
  face_.shutdown();
}