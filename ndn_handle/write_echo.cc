#include "write_echo.h"


write_echo::write_echo(Face* face, storage_handle* p_handle, repovalidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , validator_(validator)
  {
    
  }
// Interest.
void write_echo::onInterest(const Name& prefix, const Interest& interest) {
  validator_.validate(interest, bind(&write_echo::validated, this, _1), bind(&write_echo::validationFailed, this, _1));
  if(validres_ == 1){
    //Should first check whether this name exists
    //Generate response and put
    int checkres = p_handle_->check_name(interest.getName().getPrefix(-4).getSubName(prefix.size()));
    ControlResponse response;
    
    Data rdata(interest.getName());
    cout<<interest.getName()<<endl;
    //The data doest not exist, can be inserted
    if(checkres == 0){
      response.setCode(200);
      rdata.setContent(response.wireEncode());
      keyChain_.sign(rdata);
      face_->put(rdata);
      cout<<"repo data put"<<rdata.getName()<<endl;
      
      //Check whether segmented
      if(1){

      }else{
        //if just one to one
        cout<<"one to one"<<endl;
        Interest i;
        i.setName(interest.getName().getPrefix(-4).getSubName(prefix.size()));
        face_->expressInterest(i, 
          bind(&write_echo::onData, this, boost::ref(*face_), _1, _2), 
          bind(&write_echo::onTimeout, this, boost::ref(*face_), _1));
        cout<<"repo interest express"<<i.getName()<<endl;
      }
      //Gernerate interest to get data and insert into repo
      
    //The data exists, can not be insterted, just return a positive response, do not do anything
    }else if(checkres == 1){
      response.setCode(404);
      rdata.setContent(response.wireEncode());
      keyChain_.sign(rdata);
      face_->put(rdata);
      cout<<"repo data put"<<rdata.getName()<<endl;
    }
  }else if(validres_ == 0){
    ControlResponse response;
    response.setCode(403);
    Data rdata(interest.getName());
    cout<<interest.getName()<<endl;
    rdata.setContent(response.wireEncode());
    keyChain_.sign(rdata);
    face_->put(rdata);
  }
}

// onRegisterFailed.
void write_echo::onRegisterFailed(const Name& prefix, const std::string& reason){

}

// onInterest for insert check.
void write_echo::onCheckInterest(const Name& prefix, const Interest& interest){

}
  
// onRegisterFailed for insert.
void write_echo::onCheckRegisterFailed(const Name& prefix, const std::string& reason){

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

void write_echo::listen(const Name& prefix){
    (*face_).setInterestFilter(prefix,
                            func_lib::bind(&write_echo::onInterest, this, _1, _2),
                            func_lib::bind(&write_echo::onRegisterFailed, this, _1, _2));
}