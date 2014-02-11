#include "read_echo.h"

// Interest.
void read_echo::operator()
(const Name& prefix, const Interest& interest) {
  std::cout << "<< I: " << interest.getName() << std::endl;

  Name name = Name(interest.getName());
  Data data = Data(interest.getName());

  p_handle_->check_data(interest, data);

  //keyChain_.sign(data);
  
  std::cout << ">> D: "<<"size: "<<data.wireEncode().size()<<endl;
  cout<<">> D: data: "<<data.wireEncode().wire()<<endl;
  face_.put(data);
}

// onRegisterFailed.
void read_echo::operator()(const Name& prefix, const std::string& reason){
  std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
  face_.shutdown();
}