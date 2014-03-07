#include "write_echo.h"


write_echo::write_echo(Face* face, storage_handle* p_handle, repovalidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , validator_(validator)
    , retrytime_(TIMEOUTRETRY)
    , credit_(DEFAULTCREDIT)
  {
    noendTimeout_ = boost::posix_time::millisec(NOENDTIMEOUT);
  }
// Interest.
void write_echo::onInterest(const Name& prefix, const Interest& interest) {
  validator_.validate(interest, bind(&write_echo::validated, this, _1), bind(&write_echo::validationFailed, this, _1));
  
  repocommandparameter rpara;
  rpara.wireDecode(interest.getName().get(prefix.size()).blockFromValue());
  Name name = rpara.getName();
  cout<<"name:"<<name<<endl;

  cout<<"startBlockId: "<<rpara.getStartBlockId()<<endl;
  cout<<"hasStartBlockId: "<<rpara.hasStartBlockId()<<endl;
  cout<<"endBlockId: "<<rpara.getEndBlockId()<<endl;
  cout<<"hasEndBlockId: "<<rpara.hasEndBlockId()<<endl;

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
      Selectors selectors;
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
            boost::random::mt19937_64 gen(std::time(0));
            boost::random::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);
            uint64_t processId = dist(gen);

            cout<<"processId: "<<processId<<endl;
            repocommandresponse mapresponse;
            mapresponse.setStatusCode(100);
            mapresponse.setProcessId(processId);
            mapresponse.setInsertNum(0);
            mapresponse.setStartBlockId(startBlockId);
            mapresponse.setEndBlockId(endBlockId);
            processMap.insert(pair<uint64_t, repocommandresponse>(processId, mapresponse));

            Data rdata(interest.getName());
            cout<<interest.getName()<<endl;
            rdata.setContent(mapresponse.wireEncode());
            keyChain_.sign(rdata);
            face_->put(rdata);

            segInit(processId, rpara);
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
          boost::random::mt19937_64 gen(std::time(0));
          boost::random::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);
          uint64_t processId = dist(gen);

          cout<<"processId: "<<processId<<endl;
          repocommandresponse mapresponse;
          mapresponse.setStatusCode(100);
          mapresponse.setProcessId(processId);
          mapresponse.setInsertNum(0);
          mapresponse.setStartBlockId(rpara.getStartBlockId());
          processMap.insert(pair<uint64_t, repocommandresponse>(processId, mapresponse));

          Data rdata(interest.getName());
          cout<<interest.getName()<<endl;
          rdata.setContent(mapresponse.wireEncode());
          keyChain_.sign(rdata);
          face_->put(rdata);

          segInit(processId, rpara);
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
  cout<<"onData"<<endl;
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

void write_echo::onSegData(ndn::Face &face, const Interest& interest, Data& data, uint64_t processId)
{
  cout << "I: " << interest.toUri() << endl;
  cout << "D: " << data.getName().toUri() << endl;
  //retrieve the process from the responsemap
  map<uint64_t ,repocommandresponse>::iterator it;
  it = processMap.find(processId);
  repocommandresponse mapresponse;
  if(it == processMap.end()){
    cout<<"no such processId: "<<processId;
    return;
  }else{
    mapresponse = it->second;
  }
  //refresh endBlockId
  name::Component finalBlockId = data.getFinalBlockId();

  if(!finalBlockId.empty()){
    uint64_t final = finalBlockId.toSeqNum();
    if(final < mapresponse.getEndBlockId()){
      mapresponse.setEndBlockId(final);
    }
  }
  

  //insert data
  cout << "start to insert"<<endl;
  if(p_handle_->insert_data(interest, data) == 1){
    mapresponse.setInsertNum(mapresponse.getInsertNum() + 1);
  }
  cout << "end of insert"<<endl;

  it->second = mapresponse;

  segOnDataControl(processId, interest);
}

void write_echo::onTimeout(ndn::Face &face, const ndn::Interest& interest)
{
  std::cout << "Timeout" << std::endl;
}

void write_echo::onSegTimeout(ndn::Face &face, const Interest& interest, uint64_t processId){
  std::cout << "SegTimeout" << std::endl;

  segOnTimeoutControl(processId, interest);
}

void write_echo::writeListen(const Name& prefix){
    (*face_).setInterestFilter(prefix,
                            func_lib::bind(&write_echo::onInterest, this, _1, _2),
                            func_lib::bind(&write_echo::onRegisterFailed, this, _1, _2));
}

void write_echo::segInit(uint64_t processId, repocommandparameter rpara){
  creditMap_.insert(pair<uint64_t, int>(processId, 0));
  map<uint64_t, int> mapretry;

  Name name = rpara.getName();

  uint64_t startBlockId = rpara.getStartBlockId();

  Name tmpname;
  Interest i;
  uint64_t j;

  uint64_t initend = credit_;
  if(rpara.hasEndBlockId()){
    if((rpara.getEndBlockId() - rpara.getStartBlockId()) < credit_) {
      initend = rpara.getEndBlockId() - rpara.getStartBlockId();
    }
  }else{
    // set noendtimeout timer
    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::local_time();
    noendTimeoutMap_.insert(pair<uint64_t, boost::posix_time::ptime>(processId, t1));
  }
  for(j = startBlockId; j <= initend; j++){
    tmpname.wireDecode(name.wireEncode());
    tmpname.appendSegment(j);
    i.setName(tmpname);
    cout<<"seg:"<<j<<endl;
    face_->expressInterest(i, 
      bind(&write_echo::onSegData, this, boost::ref(*face_), _1, _2, processId), 
      bind(&write_echo::onSegTimeout, this, boost::ref(*face_), _1, processId));
    mapretry.insert(pair<uint64_t, int>(j, 0));
  }

  retryMap_.insert(pair<uint64_t, map<uint64_t, int> >(processId, mapretry));
  queue<uint64_t> nextSegQueue;
  nextSegQueue.push(credit_ + 1);
  nextSegQueueMap_.insert(pair<uint64_t, queue<uint64_t> >(processId, nextSegQueue));
  nextSegMap_.insert(pair<uint64_t, uint64_t>(processId, credit_ + 1));

}

void write_echo::segOnDataControl(uint64_t processId, const Interest& interest){
  cout<<"segOnDataControl: "<<processId<<endl;
  repocommandresponse mapresponse;
  map<uint64_t ,repocommandresponse>::iterator pit;
  pit = processMap.find(processId);
  if(pit == processMap.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }else{
    mapresponse = pit->second;
  }

  int mapcredit = 0;
  map<uint64_t, int>::iterator cit;
  cit = creditMap_.find(processId);
  if(cit == creditMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }else{
    mapcredit = cit->second;
  }

  int nextSeg = 0;
  map<uint64_t, uint64_t>::iterator nit;
  nit = nextSegMap_.find(processId);
  if(nit == nextSegMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }else{
    nextSeg = nit->second;
  }

  map<uint64_t, queue<uint64_t> >::iterator qit;
  qit = nextSegQueueMap_.find(processId);
  if(qit == nextSegQueueMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }


  //just use rit as the map
  map<uint64_t, map<uint64_t, int> >::iterator rit;
  rit = retryMap_.find(processId);
  if(rit == retryMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }

  //check whether notime timeout
  if(!mapresponse.hasEndBlockId()){
    map<uint64_t, boost::posix_time::ptime>::iterator tit;
    tit = noendTimeoutMap_.find(processId);
    if(tit == noendTimeoutMap_.end()){
      cout<<"no such processId: "<<processId<<endl;
      return;
    }
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration diff = now - tit->second;
    if(diff > noendTimeout_){
      cout<<"noendtimeout: "<<processId<<endl;
      processMap.erase(pit);
      nextSegQueueMap_.erase(qit);
      nextSegMap_.erase(nit);
      retryMap_.erase(rit);
      creditMap_.erase(cit);
      noendTimeoutMap_.erase(tit);
      return;
    }
  }

  //check whether this process has total ends, if ends, remove control info from the maps
  if(mapresponse.hasEndBlockId()){
    uint64_t totalNum = mapresponse.getEndBlockId() - mapresponse.getStartBlockId() + 1;
    if(mapresponse.getInsertNum() >= totalNum){
      cout<<"process end 1: "<<processId<<endl;
      processMap.erase(pit);
      nextSegQueueMap_.erase(qit);
      nextSegMap_.erase(nit);
      retryMap_.erase(rit);
      creditMap_.erase(cit);

      map<uint64_t, boost::posix_time::ptime>::iterator tit;
      tit = noendTimeoutMap_.find(processId);
      if(tit != noendTimeoutMap_.end()){
        noendTimeoutMap_.erase(tit);
      }
      return;
    }
  }

  mapcredit++;
  if(mapcredit++ > 0){
    //whether sent queue empty
    if(qit->second.empty()){
      cit->second = mapcredit;
    }else{
      //pop the queue
      uint64_t sendingSeg = qit->second.front();
      qit->second.pop();
      if(sendingSeg > mapresponse.getEndBlockId()){
        //do not do anything
        cit->second = mapcredit;
        return;
      }else{
        //check whether this is retransmit data;
        uint64_t fetchedSeg = interest.getName().get(interest.getName().size() - 1).toSeqNum();
        //Timeout iterator
        map<uint64_t, int>::iterator oit;
        int retrytime = 0;
        oit = rit->second.find(fetchedSeg);
        if(oit == rit->second.end()){
          cout<<"no such fetchedSeg, something is wrong"<<processId<<endl;
          processMap.erase(pit);
          nextSegQueueMap_.erase(qit);
          nextSegMap_.erase(nit);
          retryMap_.erase(rit);
          creditMap_.erase(cit);

          map<uint64_t, boost::posix_time::ptime>::iterator tit;
          tit = noendTimeoutMap_.find(processId);
          if(tit != noendTimeoutMap_.end()){
            noendTimeoutMap_.erase(tit);
          }
          return;
        }else{
          //find this fetched data, remove it from this map
          rit->second.erase(oit);
          //express the interest of the top of the queue
          Interest i;
          Name fetchName(interest.getName().getPrefix(-1));
          fetchName.appendSegment(sendingSeg);
          i.setName(fetchName);
          face_->expressInterest(i,
            bind(&write_echo::onSegData, this, boost::ref(*face_), _1, _2, processId),
            bind(&write_echo::onSegTimeout, this, boost::ref(*face_), _1, processId));
          cout<<"sent seg: "<<sendingSeg<<endl;
          //found the retry map, if found, plus retry time, if not found, insert in the retry time
          oit = rit->second.find(sendingSeg);
          if(oit == rit->second.end()){
            //not found
            rit->second.insert(pair<uint64_t, int>(sendingSeg, 0));
          }else{
            //found
            oit->second = oit->second + 1;
          }
          //increase the next seg and put it into the queue
          nextSeg++;
          if(nextSeg <= mapresponse.getEndBlockId()){
            nit->second = nextSeg;
            qit->second.push(nextSeg);
          }
          mapcredit--;
          cit->second = mapcredit;
        }
      }
    }
  }else{
    return;
  }

}

void write_echo::segOnTimeoutControl(uint64_t processId, const Interest& interest){
  cout<<"segOnTimeoutControl: "<<processId<<endl;
  repocommandresponse mapresponse;
  map<uint64_t ,repocommandresponse>::iterator pit;
  pit = processMap.find(processId);
  if(pit == processMap.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }else{
    mapresponse = pit->second;
  }

  int mapcredit = 0;
  map<uint64_t, int>::iterator cit;
  cit = creditMap_.find(processId);
  if(cit == creditMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }else{
    mapcredit = cit->second;
  }

  int nextSeg = 0;
  map<uint64_t, uint64_t>::iterator nit;
  nit = nextSegMap_.find(processId);
  if(nit == nextSegMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }else{
    nextSeg = nit->second;
  }

  map<uint64_t, queue<uint64_t> >::iterator qit;
  qit = nextSegQueueMap_.find(processId);
  if(qit == nextSegQueueMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }

  //just use rit as the map
  map<uint64_t, map<uint64_t, int> >::iterator rit;
  rit = retryMap_.find(processId);
  if(rit == retryMap_.end()){
    cout<<"no such processId: "<<processId<<endl;
    return;
  }

  uint64_t timeoutSeg = interest.getName().get(interest.getName().size() - 1).toSeqNum();

  cout<<"timeoutSeg: "<<timeoutSeg<<endl;
  map<uint64_t, int>::iterator oit;
  int retrytime = 0;
  oit = rit->second.find(timeoutSeg);
  if(oit == rit->second.end()){
    cout<<"no such timeoutSeg, something is wrong"<<processId<<endl;;
    processMap.erase(pit);
    nextSegQueueMap_.erase(qit);
    nextSegMap_.erase(nit);
    retryMap_.erase(rit);
    creditMap_.erase(cit);
    return;
  }else{
    //check the retry time. If retry out of time, fail the process. if not, plus
    retrytime = oit->second;
    if(retrytime >= retrytime_){
      //fail this process
      cout<<"Retry timeout: "<<processId<<endl;
      processMap.erase(pit);
      nextSegQueueMap_.erase(qit);
      nextSegMap_.erase(nit);
      retryMap_.erase(rit);
      creditMap_.erase(cit);
      return;
    }else{
      //Reput it in the queue, retrytime++
      retrytime++;
      oit->second = retrytime;
      qit->second.push(timeoutSeg);
      if(mapcredit <= 0){
        cout<<"timeout credit fails ends: "<<processId<<endl;
        processMap.erase(pit);
        nextSegQueueMap_.erase(qit);
        nextSegMap_.erase(nit);
        retryMap_.erase(rit);
        creditMap_.erase(cit);
        return;
      }else{
        Interest i;
        Name timeoutName(interest.getName().getPrefix(-1));
        timeoutName.appendSegment(timeoutSeg);
        i.setName(timeoutName);
        face_->expressInterest(i,
          bind(&write_echo::onSegData, this, boost::ref(*face_), _1, _2, processId),
          bind(&write_echo::onSegTimeout, this, boost::ref(*face_), _1, processId));
        mapcredit--;
        cit->second = mapcredit;
      }
    }
  }
}

void write_echo::onCheckInterest(const Name& prefix, const Interest& interest){
  validator_.validate(interest, bind(&write_echo::validated, this, _1), bind(&write_echo::validationFailed, this, _1));
  
  repocommandparameter rpara;
  rpara.wireDecode(interest.getName().get(prefix.size()).blockFromValue());
  Name name = rpara.getName();
  cout<<"name:"<<name<<endl;
  if(validres_ == 1){
    if(rpara.hasProcessId()){
      uint64_t processId = rpara.getProcessId();
      repocommandresponse mapresponse;
      map<uint64_t ,repocommandresponse>::iterator pit;
      pit = processMap.find(processId);
      if(pit == processMap.end()){
        cout<<"no such processId: "<<processId<<endl;
        repocommandresponse response;
        response.setStatusCode(404);
        Data rdata(interest.getName());
        cout<<interest.getName()<<endl;
        rdata.setContent(response.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);
        return;
      }else{
        mapresponse = pit->second;
        mapresponse.setStatusCode(300);
        Data rdata(interest.getName());
        cout<<interest.getName()<<endl;
        rdata.setContent(mapresponse.wireEncode());
        keyChain_.sign(rdata);
        face_->put(rdata);

        //check if noendtimeout
        if(!mapresponse.hasEndBlockId()){
          map<uint64_t, boost::posix_time::ptime>::iterator tit;
          tit = noendTimeoutMap_.find(processId);
          if(tit == noendTimeoutMap_.end()){
            cout<<"no such processid"<<processId<<endl;
            return;
          }

          //refresh the time
          boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
          tit->second = now;
        }
      }
    }else{
      repocommandresponse response;
      response.setStatusCode(404);
      Data rdata(interest.getName());
      cout<<interest.getName()<<endl;
      rdata.setContent(response.wireEncode());
      keyChain_.sign(rdata);
      face_->put(rdata);
    }
  }else{
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

void write_echo::writeCheckListen(const Name& prefix){
  (*face_).setInterestFilter(prefix,
                            func_lib::bind(&write_echo::onCheckInterest, this, _1, _2),
                            func_lib::bind(&write_echo::onRegisterFailed, this, _1, _2));
}