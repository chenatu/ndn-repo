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
  
  repocommandparameter rpara;
  rpara.wireDecode(interest.getName().get(prefix.size()).blockFromValue());
  Name name = rpara.getName();
  cout<<"name:"<<name<<endl;
  cout<<"hasStartBlockId"<<rpara.hasStartBlockId()<<endl;
  cout<<"hasEndBlockId"<<rpara.hasEndBlockId()<<endl;
  if(validres_ == 1){
    if(!rpara.hasStartBlockId() && !rpara.hasEndBlockId()){
      // No Segment, just use selectors to fetch the data; one to one

      //The command is OK. can start to fetch the data
      ControlResponse response;
      response.setCode(100);
      Data rdata(interest.getName());
      cout<<interest.getName()<<endl;
      rdata.setContent(response.wireEncode());
      keyChain_.sign(rdata);
      face_->put(rdata);

      cout<<"one to one"<<endl;
      Interest i;
      i.setName(name);
      i.setSelectors(rpara.getSelectors());
      face_->expressInterest(i, 
        bind(&write_echo::onData, this, boost::ref(*face_), _1, _2), 
        bind(&write_echo::onTimeout, this, boost::ref(*face_), _1));
      cout<<"repo interest express"<<i.getName()<<endl;
    }else{
      //segmented
      if(rpara.hasSelectors()){
        //has selectors, return  402
        ControlResponse response;
        response.setCode(402);
        Data rdata(interest.getName());
        cout<<interest.getName()<<endl;
        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
      }else{
        if(rpara.hasEndBlockId()){
          //normal fetch segment
          if(!rpara.hasStartBlockId())
            rpara.setStartBlockId(0);
        }else{
          //no EndBlockId, so fetch FindalBlockId in data, if timeout, stop
        }
      }
    }
   
  }else if(validres_ == 0){
    ControlResponse response;
    response.setCode(401);
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