#include "read_echo.h"

// Interest.
void read_echo::onInterest(const Name& prefix, const Interest& interest) {
  std::cout << "<< I: " << interest.getName() << std::endl;

  Name name = Name(interest.getName());
  Data data = Data(interest.getName());

  p_handle_->check_data(interest, data);

  //keyChain_.sign(data);
  
  std::cout << ">> D: "<<"size: "<<data.wireEncode().size()<<endl;
  cout<<">> D: data: "<<data.wireEncode().wire()<<endl;
  face_->put(data);
}

// onRegisterFailed.
void read_echo::onRegisterFailed(const Name& prefix, const std::string& reason){
  std::cerr << "ERROR: Failed to register prefix in local hub's daemon" << std::endl;
  face_->shutdown();
}

void read_echo::readListen(const Name& prefix){
  (*face_).setInterestFilter(prefix,
                            func_lib::bind(&read_echo::onInterest, this, _1, _2),
                            func_lib::bind(&read_echo::onRegisterFailed, this, _1, _2));
}