/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef REPOVALIDATOR_H
#define REPOVALIDATOR_H

#include <ndn-cpp-dev/security/validator.hpp>
#include <ndn-cpp-dev/security/identity-certificate.hpp>
#include <ndn-cpp-dev/security/sec-rule-specific.hpp>

using namespace ndn;

class repovalidator : public Validator
{
public:
  enum {
    POS_SIG_VALUE = -1,
    POS_SIG_INFO = -2,
    POS_RANDOM_VAL = -3,
    POS_TIMESTAMP = -4,

    GRACE_INTERVAL = 3000 // ms
  };

  repovalidator(int64_t graceInterval = GRACE_INTERVAL) 
  { m_graceInterval = (graceInterval < 0 ? GRACE_INTERVAL : graceInterval); }

  virtual
  ~repovalidator() {}

  inline void
  addInterestRule(const std::string& regex, const IdentityCertificate& certificate);

  inline void
  addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey);

protected:
  virtual void
  checkPolicy (const Data& data, 
               int stepCount, 
               const OnDataValidated &onValidated, 
               const OnDataValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps)
  { onValidationFailed(data.shared_from_this()); }
  
  virtual void
  checkPolicy (const Interest& interest, 
               int stepCount, 
               const OnInterestValidated &onValidated, 
               const OnInterestValidationFailed &onValidationFailed,
               std::vector<shared_ptr<ValidationRequest> > &nextSteps);
private:
  int64_t m_graceInterval; //ms
  std::map<Name, PublicKey> m_trustAnchorsForInterest;
  std::list<SecRuleSpecific> m_trustScopeForInterest;
  std::map<Name, uint64_t> m_lastTimestamp;
};

inline void
repovalidator::addInterestRule(const std::string& regex, const IdentityCertificate& certificate)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());
  addInterestRule(regex, keyName, certificate.getPublicKeyInfo());
}

inline void
repovalidator::addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey)
{
  m_trustAnchorsForInterest[keyName] = publicKey;
  shared_ptr<Regex> interestRegex = make_shared<Regex>(regex);
  shared_ptr<Regex> signerRegex = Regex::fromName(keyName, true);
  m_trustScopeForInterest.push_back(SecRuleSpecific(interestRegex, signerRegex));
}

inline void
repovalidator::checkPolicy (const Interest& interest, 
                                       int stepCount, 
                                       const OnInterestValidated &onValidated, 
                                       const OnInterestValidationFailed &onValidationFailed,
                                       std::vector<shared_ptr<ValidationRequest> > &nextSteps)
{
  try
    {
      const Name& interestName = interest.getName();

      if (interestName.size() < 4)
        return onValidationFailed(interest.shared_from_this());

      Signature signature(interestName[POS_SIG_INFO].blockFromValue(), 
                          interestName[POS_SIG_VALUE].blockFromValue());
    
      SignatureSha256WithRsa sig(signature);
      const Name& keyLocatorName = sig.getKeyLocator().getName();
      Name keyName = IdentityCertificate::certificateNameToPublicKeyName(keyLocatorName);

      //Check if command is in the trusted scope
      bool inScope = false;  
      for(std::list<SecRuleSpecific>::iterator scopeIt = m_trustScopeForInterest.begin();
          scopeIt != m_trustScopeForInterest.end();
          ++scopeIt)
        {
          if(scopeIt->satisfy(interestName, keyName))
            {
              inScope = true;
              break;
            }
        }
      if(inScope == false)
        {
          onValidationFailed(interest.shared_from_this());
          return;
        }

      //Check if timestamp is valid
      uint64_t timestamp = interestName.get(POS_TIMESTAMP).toNumber();
      uint64_t current = static_cast<uint64_t>(time::now()/1000000);
      std::map<Name, uint64_t>::const_iterator timestampIt = m_lastTimestamp.find(keyName);
      if(timestampIt == m_lastTimestamp.end())
        {
          if(timestamp > (current + m_graceInterval) || (timestamp + m_graceInterval) < current)
            {
              onValidationFailed(interest.shared_from_this());
              return;
            }
        }
      else if(m_lastTimestamp[keyName] >= timestamp)
        {
          onValidationFailed(interest.shared_from_this());
          return;
        }

      if(!Validator::verifySignature(interestName.wireEncode().value(),
                                     interestName.wireEncode().value_size() - interestName[-1].size(),
                                     sig, m_trustAnchorsForInterest[keyName]))
        {
          onValidationFailed(interest.shared_from_this());
          return;
        }

      m_lastTimestamp[keyName] = timestamp;
      onValidated(interest.shared_from_this());
      return;

    }
  catch(...)
    {
      onValidationFailed(interest.shared_from_this());
    }
}
#endif
