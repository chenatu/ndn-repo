#include "read_echo.h"

// Interest.
void read_echo::operator()
(const ptr_lib::shared_ptr<const Name>& prefix, const ptr_lib::shared_ptr<const Interest>& interest) {
  std::cout << "<< I: " << interest->getName() << std::endl;

  Name name = Name(interest->getName());
  Data data;

  p_handle_->check_data(name, data);

  //keyChain_.sign(data);
  
  std::cout << ">> D: " << data << std::endl;
  face_.put(data);
}

// onRegisterFailed.
void read_echo::operator()(const ptr_lib::shared_ptr<const Name>& prefix){
  std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
  face_.shutdown();
}