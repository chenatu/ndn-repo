/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2013 Regents of the University of California.
 * See COPYING for copyright and distribution information.
 */

#ifndef REPO_VALIDATOR_H
#define REPO_VALIDATOR_H

#include <ndn-cpp-dev/security/validator.hpp>
#include <ndn-cpp-dev/security/identity-certificate.hpp>
#include <ndn-cpp-dev/security/sec-rule-specific.hpp>

using namespace ndn;

class RepoCommandValidator : public Validator {
public:
  enum {
    POS_SIG_VALUE = -1,
    POS_SIG_INFO = -2,
    POS_RANDOM_VAL = -3,
    POS_TIMESTAMP = -4,

    GRACE_INTERVAL = 3000 // ms
  };

  RepoCommandValidator(int64_t graceInterval = GRACE_INTERVAL) 
  { m_graceInterval = (graceInterval < 0 ? GRACE_INTERVAL : graceInterval); }

  virtual
  ~RepoCommandValidator() {}

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
  {
    onValidationFailed(data.shared_from_this(), "No policy for data checking");
  }
  
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
RepoCommandValidator::addInterestRule(const std::string& regex, const IdentityCertificate& certificate)
{
  Name keyName = IdentityCertificate::certificateNameToPublicKeyName(certificate.getName());
  addInterestRule(regex, keyName, certificate.getPublicKeyInfo());
}

inline void
RepoCommandValidator::addInterestRule(const std::string& regex, const Name& keyName, const PublicKey& publicKey)
{
  m_trustAnchorsForInterest[keyName] = publicKey;
  shared_ptr<Regex> interestRegex = make_shared<Regex>(regex);
  shared_ptr<Regex> signerRegex = Regex::fromName(keyName, true);
  m_trustScopeForInterest.push_back(SecRuleSpecific(interestRegex, signerRegex));
}

inline void
RepoCommandValidator::checkPolicy (const Interest& interest, 
                                       int stepCount, 
                                       const OnInterestValidated &onValidated, 
                                       const OnInterestValidationFailed &onValidationFailed,
                                       std::vector<shared_ptr<ValidationRequest> > &nextSteps)
{
  const Name& interestName = interest.getName();
  
  //Prepare 
  if (interestName.size() < 4)
    return onValidationFailed(interest.shared_from_this(), 
                              "Interest is not signed: " + interest.getName().toUri());
  
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
    return onValidationFailed(interest.shared_from_this(), 
                              "Signer cannot be authorized for the command: " + keyName.toUri());

  //Check if timestamp is valid
  uint64_t timestamp = interestName.get(POS_TIMESTAMP).toNumber();
  uint64_t current = static_cast<uint64_t>(time::now()/1000000);
  std::map<Name, uint64_t>::const_iterator timestampIt = m_lastTimestamp.find(keyName);
  if(timestampIt == m_lastTimestamp.end())
    {
      if(timestamp > (current + m_graceInterval) || (timestamp + m_graceInterval) < current)
        return onValidationFailed(interest.shared_from_this(), 
                                  "The command is not in grace interval: " + interest.getName().toUri());
    }
  else 
    {
      if(m_lastTimestamp[keyName] >= timestamp)
        return onValidationFailed(interest.shared_from_this(), 
                                  "The command is outdated: " + interest.getName().toUri());
    }

  //Check signature
  if(!Validator::verifySignature(interestName.wireEncode().value(),
                                 interestName.wireEncode().value_size() - interestName[-1].size(),
                                 sig, m_trustAnchorsForInterest[keyName]))
    return onValidationFailed(interest.shared_from_this(), 
                              "Signature cannot be validated: " + interest.getName().toUri());

  //Update timestamp
  m_lastTimestamp[keyName] = timestamp;

  return onValidated(interest.shared_from_this());
}

#endif 
