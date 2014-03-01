#include "delete_echo.h"


delete_echo::delete_echo(Face* face, storage_handle* p_handle, repovalidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , validator_(validator)
  {
    
  }
// Interest.
void delete_echo::onInterest(const Name& prefix, const Interest& interest) {
  cout<<"call delete_echo"<<endl;
  validator_.validate(interest, bind(&delete_echo::validated, this, _1), bind(&delete_echo::validationFailed, this, _1));
  repocommandparameter rpara;
  rpara.wireDecode(interest.getName().get(prefix.size()).blockFromValue());
  Name name = rpara.getName();
  cout<<"name:"<<name<<endl;
  if(validres_ == 1){
    //Should first check whether this name exists
    //Generate response and put

    
    if(rpara.hasSelectors()){
      if(rpara.hasStartBlockId() || rpara.hasEndBlockId()){
        //has selector and blockid return 402
        repocommandresponse response;
        response.setStatusCode(402);
        Data rdata(interest.getName());
        if(rpara.hasProcessId()){
          response.setProcessId(rpara.getProcessId());
        }
        cout<<interest.getName()<<endl;

        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
        return;
      }else{
        //choose data with selector and delete it
      }
    }else{
      if(rpara.hasEndBlockId()){
        if(!rpara.hasStartBlockId())
          rpara.setStartBlockId(0);
        uint64_t startBlockId = rpara.getStartBlockId();
        uint64_t endBlockId = rpara.getEndBlockId();
        if(startBlockId <= endBlockId){

        }else{
          repocommandresponse response;
          response.setStatusCode(403);
          Data rdata(interest.getName());
          if(rpara.hasProcessId()){
            response.setProcessId(rpara.getProcessId());
          }
          cout<<interest.getName()<<endl;
          rdata.setContent(response.wireEncode());
          keyChain_.sign(rdata);
          face_->put(rdata);
        }
      }else{

      }
    }
    
  }else if(validres_ == 0){
    repocommandresponse response;
    response.setStatusCode(401);
    Data rdata(interest.getName());
    if(rpara.hasProcessId()){
      response.setProcessId(rpara.getProcessId());
    }
    cout<<interest.getName()<<endl;
    rdata.setContent(response.wireEncode());
    keyChain_.sign(rdata);
    face_->put(rdata);
  }
}

// onRegisterFailed.
void delete_echo::onRegisterFailed(const Name& prefix, const std::string& reason){

}

// onInterest for insert check.
void delete_echo::onCheckInterest(const Name& prefix, const Interest& interest){

}
  
// onRegisterFailed for insert.
void delete_echo::onCheckRegisterFailed(const Name& prefix, const std::string& reason){

}


void delete_echo::validated(const shared_ptr<const Interest>& interest){
  cout<<"validated"<<endl;
  validres_ = 1;
}

void delete_echo::validationFailed(const shared_ptr<const Interest>& interest){
  cout<<"unvalidated"<<endl;
  validres_ = 0;
}

void delete_echo::deleteListen(const Name& prefix){
    (*face_).setInterestFilter(prefix,
                            func_lib::bind(&delete_echo::onInterest, this, _1, _2),
                            func_lib::bind(&delete_echo::onRegisterFailed, this, _1, _2));
}