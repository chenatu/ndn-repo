/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */
#include "delete-echo.hpp"


DeleteEcho::DeleteEcho(Face* face, StorageHandle* p_handle, RepoCommandValidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , validator_(validator)
  {
    
  }
// Interest.
void 
DeleteEcho::onInterest(const Name& prefix, const Interest& interest) {
  cout<<"call DeleteEcho"<<endl;
  validator_.validate(interest, bind(&DeleteEcho::validated, this, _1), bind(&DeleteEcho::validationFailed, this, _1));
  RepoCommandParameter rpara;
  rpara.wireDecode(interest.getName().get(prefix.size()).blockFromValue());
  Name name = rpara.getName();
  //cout<<"name:"<<name<<endl;
  if(validres_ == 1){    
    if(rpara.hasSelectors()){
      //cout<<"hasSelectors"<<endl;
      if(rpara.hasStartBlockId() || rpara.hasEndBlockId()){
        //has selector and blockid return 402
        RepoCommandResponse response;
        cout<<"402"<<endl;
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
        uint64_t deleteNum = 0;
        Selectors selectors = rpara.getSelectors();
        vector<Name> vname;
        p_handle_->check_name_any(name, selectors, vname);
        vector<Name>::iterator it;
        int i = 0;
        //cout<<"vname size: "<<vname.size()<<endl;
        for(it = vname.begin(), i = 0; it < vname.end(); it++, i++){
          //cout<<"seg: "<<i<<endl;
          //cout<<"name: "<<*it<<endl;
          if(p_handle_->delete_data(*it) == 1){
            deleteNum++;
          }
        }
        //cout<<"Selectors Delete Complete"<<endl;
        //All data has been deleted, return 200
        RepoCommandResponse response;
        response.setStatusCode(200);
        Data rdata(interest.getName());
        if(rpara.hasProcessId()){
          response.setProcessId(rpara.getProcessId());
        }
        response.setDeleteNum(deleteNum);
        //cout<<interest.getName()<<endl;
        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
        return;
      }
    }else{
      if(rpara.hasEndBlockId()){
        if(!rpara.hasStartBlockId())
          rpara.setStartBlockId(0);
        uint64_t startBlockId = rpara.getStartBlockId();
        uint64_t endBlockId = rpara.getEndBlockId();
        if(startBlockId <= endBlockId){
          Name tmpname;
          Selectors selectors;
          uint64_t deleteNum = 0;
          for(int i = startBlockId; i <= endBlockId; i++){
            tmpname.wireDecode(name.wireEncode());
            tmpname.appendSegment(i);
            //cout<<"seg:"<<i<<endl;
            if(p_handle_->delete_data(tmpname) == 1){
              deleteNum++;
            }
          }
          //All the data deleted, return 200
          //cout<<"seg delete"<<endl;
          RepoCommandResponse response;
          response.setStatusCode(200);
          Data rdata(interest.getName());
          if(rpara.hasProcessId()){
            response.setProcessId(rpara.getProcessId());
          }
          response.setDeleteNum(deleteNum);
          rdata.setContent(response.wireEncode());
          keyChain_.sign(rdata);
          face_->put(rdata);
          return;
        }else{
          RepoCommandResponse response;
          cout<<"403"<<endl;
          response.setStatusCode(403);
          Data rdata(interest.getName());
          if(rpara.hasProcessId()){
            response.setProcessId(rpara.getProcessId());
          }
          rdata.setContent(response.wireEncode());
          keyChain_.sign(rdata);
          face_->put(rdata);
          return;
        }
      }else{
        if(rpara.hasStartBlockId()){
          Name tmpname;
          uint64_t deleteNum = 0;
          uint64_t startBlockId = rpara.getStartBlockId();
          uint64_t i = startBlockId;
          while(1){
            tmpname.wireDecode(name.wireEncode());
            tmpname.appendSegment(i);
            //cout<<"seg:"<<i<<endl;
            if(p_handle_->delete_data(tmpname) != 1){
              break;
            }
            deleteNum++;
          }
          RepoCommandResponse response;
          response.setStatusCode(200);
          cout<<"200 no end delete"<<endl;
          Data rdata(interest.getName());
          if(rpara.hasProcessId()){
            response.setProcessId(rpara.getProcessId());
          }
          response.setDeleteNum(deleteNum);
          rdata.setContent(response.wireEncode());
          keyChain_.sign(rdata);
          face_->put(rdata);
          return;
        }else{
          //cout<<"one to one delete"<<endl;
          uint64_t deleteNum = 0;
          if(p_handle_->delete_data(name) == 1){
            deleteNum == 1;
          }
          RepoCommandResponse response;
          response.setStatusCode(200);
          Data rdata(interest.getName());
          if(rpara.hasProcessId()){
            response.setProcessId(rpara.getProcessId());
          }
          response.setDeleteNum(deleteNum);
          rdata.setContent(response.wireEncode());
          keyChain_.sign(rdata);
          face_->put(rdata);
          return;
        }
      }
    }    
  }else if(validres_ == 0){
    RepoCommandResponse response;
    response.setStatusCode(401);
    Data rdata(interest.getName());
    if(rpara.hasProcessId()){
      response.setProcessId(rpara.getProcessId());
    }
    response.setDeleteNum(0);
    cout<<interest.getName()<<endl;
    rdata.setContent(response.wireEncode());
    keyChain_.sign(rdata);
    face_->put(rdata);
    return;
  }
  cout<<"no condition"<<endl;
}

// onRegisterFailed.
void 
DeleteEcho::onRegisterFailed(const Name& prefix, const std::string& reason){

}

// onInterest for insert check.
void 
DeleteEcho::onCheckInterest(const Name& prefix, const Interest& interest){

}
  
// onRegisterFailed for insert.
void 
DeleteEcho::onCheckRegisterFailed(const Name& prefix, const std::string& reason){

}


void 
DeleteEcho::validated(const shared_ptr<const Interest>& interest){
  cout<<"validated"<<endl;
  validres_ = 1;
}

void 
DeleteEcho::validationFailed(const shared_ptr<const Interest>& interest){
  cout<<"unvalidated"<<endl;
  validres_ = 0;
}

void 
DeleteEcho::deleteListen(const Name& prefix){
    (*face_).setInterestFilter(prefix,
                            func_lib::bind(&DeleteEcho::onInterest, this, _1, _2),
                            func_lib::bind(&DeleteEcho::onRegisterFailed, this, _1, _2));
}