#include "delete_echo.h"


delete_echo::delete_echo(Face* face, storage_handle* p_handle, repovalidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , validator_(validator)
  {
    
  }
// Interest.
void delete_echo::operator()(const Name& prefix, const Interest& interest) {
  cout<<"call delete_echo"<<endl;
  validator_.validate(interest, bind(&delete_echo::validated, this, _1), bind(&delete_echo::validationFailed, this, _1));
  if(validres_ == 1){
    //Should first check whether this name exists
    //Generate response and put
    int res = p_handle_->check_name(interest.getName().getPrefix(-4).getSubName(prefix.size()));
    repocommandresponse response;
    
    Data rdata(interest.getName());
    cout<<interest.getName()<<endl;
    //The data exists, can be deleted
    if(res == 1){
      cout<<"before delete"<<endl;
      res = p_handle_->delete_data(interest, interest.getName().getPrefix(-4).getSubName(prefix.size()));
      cout<<"end delete"<<endl;
      if(res == 1){
        response.setStatusCode(200);
        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
        cout<<"repo data put"<<rdata.getName()<<endl;
      }else if(res == 0){
        //delete error
        response.setStatusCode(404);
        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
        cout<<"repo data put"<<rdata.getName()<<endl;
      }
    //The data doest not exit, can not be deleted, return 404 code
    }else if(res == 0){
      cout<<"The data doest not exit, can not be deleted, return 404 code"<<endl;
      response.setStatusCode(404);
      cout<<"----0----"<<endl;
      rdata.setContent(response.wireEncode());
      cout<<"----1----"<<endl;
      keyChain_.sign(rdata);
      cout<<"----2----"<<endl;
      face_->put(rdata);
      cout<<"repo data put"<<rdata.getName()<<endl;
    }
  }else if(validres_ == 0){
    repocommandresponse response;
    response.setStatusCode(403);
    Data rdata(interest.getName());
    cout<<interest.getName()<<endl;
    rdata.setContent(response.wireEncode());
    keyChain_.sign(rdata);
    face_->put(rdata);
  }
}

// onRegisterFailed.
void delete_echo::operator()(const Name& prefix, const std::string& reason){

}

void delete_echo::validated(const shared_ptr<const Interest>& interest){
  cout<<"validated"<<endl;
  validres_ = 1;
}

void delete_echo::validationFailed(const shared_ptr<const Interest>& interest){
  cout<<"unvalidated"<<endl;
  validres_ = 0;
}

