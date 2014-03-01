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

  if(validres_ == 1){
    if(!rpara.hasStartBlockId() && !rpara.hasEndBlockId()){
      // No Segment, just use selectors to fetch the data; one to one

      //The command is OK. can start to fetch the data
      //random processid
      boost::random::mt19937_64 gen;
      boost::random::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);
      uint64_t processId = dist(gen);

      repocommandresponse response;
      response.setStatusCode(100);
      response.setProcessId(processId);
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
        bind(&write_echo::onData, this, boost::ref(*face_), _1, _2, processId), 
        bind(&write_echo::onTimeout, this, boost::ref(*face_), _1));
      cout<<"repo interest express"<<i.getName()<<endl;
      repocommandresponse mapresponse;
      mapresponse.setStatusCode(100);
      mapresponse.setProcessId(processId);
      mapresponse.setInsertNum(0);
      processMap.insert(pair<uint64_t, repocommandresponse>(processId,mapresponse));
    }else{
      //segmented
      cout<<"segmented"<<endl;

      if(rpara.hasSelectors()){
        //has selectors, return  402
        cout<<"has selectors"<<endl;
        repocommandresponse response;
        response.setStatusCode(402);
        Data rdata(interest.getName());
        cout<<interest.getName()<<endl;
        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
        return;
      }else{
        if(rpara.hasEndBlockId()){
          //normal fetch segment
          if(!rpara.hasStartBlockId()){
            rpara.setStartBlockId(0);
          }


          uint64_t startBlockId = rpara.getStartBlockId();
          uint64_t endBlockId = rpara.getEndBlockId();
          cout<<"startBlockId: "<<startBlockId<<endl;
          cout<<"endBlockId: "<<endBlockId<<endl;
          if(startBlockId <= endBlockId){           
            boost::random::mt19937_64 gen;
            boost::random::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);
            uint64_t processId = dist(gen);
            repocommandresponse response;
            response.setStatusCode(100);
            response.setStartBlockId(startBlockId);
            response.setEndBlockId(endBlockId);
            response.setProcessId(processId);
            Data rdata(interest.getName());
            cout<<interest.getName()<<endl;
            rdata.setContent(response.wireEncode());
            keyChain_.sign(rdata);
            face_->put(rdata);

            repocommandresponse mapresponse;
            mapresponse.setStatusCode(100);
            mapresponse.setProcessId(processId);
            mapresponse.setInsertNum(0);
            mapresponse.setStartBlockId(startBlockId);
            mapresponse.setEndBlockId(endBlockId);
            processMap.insert(pair<uint64_t, repocommandresponse>(processId,mapresponse));

            Name tmpname;
            Interest i;
            uint64_t j;
            for(j = startBlockId; j <= endBlockId; j++){
              tmpname.wireDecode(name.wireEncode());
              tmpname.appendSegment(j);
              i.setName(tmpname);
              cout<<"seg:"<<j<<endl;
              face_->expressInterest(i, 
                bind(&write_echo::onSegData, this, boost::ref(*face_), _1, _2, processId), 
                bind(&write_echo::onTimeout, this, boost::ref(*face_), _1));
            }
          }else{
            cout<<"start > end"<<endl;
            repocommandresponse response;
            response.setStatusCode(403);
            Data rdata(interest.getName());
            cout<<interest.getName()<<endl;
            rdata.setContent(response.wireEncode());
            keyChain_.sign(rdata);
            face_->put(rdata);
            return;
          }

        }else{
          //no EndBlockId, so fetch FindalBlockId in data, if timeout, stop
        }
      }
    }
   
  }else if(validres_ == 0){
    repocommandresponse response;
    response.setStatusCode(401);
    Data rdata(interest.getName());
    cout<<interest.getName()<<endl;
    rdata.setContent(response.wireEncode());
    keyChain_.sign(rdata);
    face_->put(rdata);
    return;
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

void write_echo::onData(ndn::Face &face, const ndn::Interest& interest, ndn::Data& data, uint64_t processId)
{
  cout << "I: " << interest.toUri() << endl;
  cout << "D: " << data.getName().toUri() << endl;
  map<uint64_t ,repocommandresponse>::iterator it;;
  it = processMap.find(processId);
  repocommandresponse mapresponse;
  if(it == processMap.end()){
    cout<<"no such processId: "<<processId;
    return;
  }else{
    mapresponse = it->second;
  }
  if(mapresponse.getInsertNum() == 0){
    cout << "start to insert"<<endl;
    p_handle_->insert_data(interest, data);
    cout << "end of insert"<<endl;
  }
  processMap.erase(it);
}

void write_echo::onSegData(ndn::Face &face, const ndn::Interest& interest, ndn::Data& data, uint64_t processId)
{
  cout << "I: " << interest.toUri() << endl;
  cout << "D: " << data.getName().toUri() << endl;
  map<uint64_t ,repocommandresponse>::iterator it;;
  it = processMap.find(processId);
  repocommandresponse mapresponse;
  if(it == processMap.end()){
    cout<<"no such processId: "<<processId;
    return;
  }else{
    mapresponse = it->second;
  }

  uint64_t totalNum = mapresponse.getEndBlockId() - mapresponse.getStartBlockId() + 1;
  if(mapresponse.getInsertNum() == 0){
    cout << "start to insert"<<endl;
    p_handle_->insert_data(interest, data);
    cout << "end of insert"<<endl;
  }

  uint64_t insertNum = mapresponse.getInsertNum() + 1;

  if(insertNum < totalNum){
    mapresponse.setInsertNum(insertNum);
  }else{
    processMap.erase(it);
  }

}

void write_echo::onTimeout(ndn::Face &face, const ndn::Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}

void write_echo::writeListen(const Name& prefix){
    (*face_).setInterestFilter(prefix,
                            func_lib::bind(&write_echo::onInterest, this, _1, _2),
                            func_lib::bind(&write_echo::onRegisterFailed, this, _1, _2));
}