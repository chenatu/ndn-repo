#include "write_echo.h"


write_echo::write_echo(Face face, storage_handle* p_handle)
    : face_(face)
    , keyChain_()
    , p_handle_(p_handle)
  {
    responseinfo.setContentType(0);
    responseinfo.setFreshnessPeriod(1000);
  }
// Interest.
void write_echo::operator()
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
void write_echo::operator()(const ptr_lib::shared_ptr<const Name>& prefix){
  std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
  face_.shutdown();
}