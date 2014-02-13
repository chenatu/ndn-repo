#include "write_echo.h"


write_echo::write_echo(Face* face, storage_handle* p_handle, KeyChain& keyChain, CommandInterestValidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , keyChain_(keyChain)
    , validator_(validator)
  {
    
  }
// Interest.
void write_echo::operator()(const Name& prefix, const Interest& interest) {
  validator_.validate(interest, bind(&write_echo::validated, this, _1), bind(&write_echo::validationFailed, this, _1));
  if(validres_ == 1){
    //Generate response and put
    ControlResponse response;
    response.setCode(200);
    Data rdata(interest.getName());
    cout<<interest.getName()<<endl;
    rdata.setContent(response.wireEncode());
    keyChain_.sign(rdata);
    face_->put(rdata);
    cout<<"repo data put"<<rdata.getName()<<endl;
    //Gernerate interest to get data and insert into repo
    Interest i;
    i.setName(interest.getName().getPrefix(-4).getSubName(prefix.size()));
    face_->expressInterest(i, 
      bind(&write_echo::onData, this, boost::ref(*face_), _1, _2), 
      bind(&write_echo::onTimeout, this, boost::ref(*face_), _1));
    cout<<"repo interest express"<<i.getName()<<endl;
  }else if(validres_ == 0){

  }
}

// onRegisterFailed.
void write_echo::operator()(const Name& prefix, const std::string& reason){

}

void write_echo::validated(const shared_ptr<const Interest>& interest){
  cout<<"validated"<<endl;
  validres_ = 1;
}

void write_echo::validationFailed(const shared_ptr<const Interest>& interest){
  cout<<"unvalidated"<<endl;
  validres_ = 0;
}

void write_echo::onData(ndn::Face &face, const ndn::Interest& interest, ndn::Data& data)
{
  cout << "I: " << interest.toUri() << endl;
  cout << "D: " << data.getName().toUri() << endl;
  cout << "start to insert"<<endl;
  p_handle_->insert_data(interest, data);
  cout << "end of insert"<<endl;
}

void write_echo::onTimeout(ndn::Face &face, const ndn::Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}
