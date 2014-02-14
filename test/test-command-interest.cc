#include <ndn-cpp-dev/helper/command-interest-generator.hpp>
#include <ndn-cpp-dev/helper/command-interest-validator.hpp>
#include <ndn-cpp-dev/util/random.hpp>
#include <iostream>

using namespace std;
using namespace ndn;

class TestCore
{
public:
  TestCore()
    : m_validity(false)
  {}
  
  void
  validated(const shared_ptr<const Interest>& interest)
  { m_validity = true; }

  void
  validationFailed(const shared_ptr<const Interest>& interest)
  { m_validity = false; }

  void
  reset()
  { m_validity = false; }

  bool m_validity;
};

int main(){
	KeyChain keyChain;
	Name identity("/TestCommandInterest/Validation");
	Name certName;

	certName = keyChain.createIdentity(identity);

	TestCore core;
	CommandInterestGenerator generator;
	CommandInterestValidator validator;

	validator.addInterestRule("^<TestCommandInterest><Validation>", *keyChain.getCertificate(certName));

	//Test a legitimate command
	shared_ptr<Interest> commandInterest1 = make_shared<Interest>("/TestCommandInterest/Validation/Command1");
	generator.generateWithIdentity(*commandInterest1, identity);
	validator.validate(*commandInterest1,
		bind(&TestCore::validated, &core, _1),
		bind(&TestCore::validationFailed, &core, _1));

	if(core.m_validity){
		cout<<"legitimate"<<endl;
	}
	/*//Test an outdated command
	core.reset();
	shared_ptr<Interest> commandInterest2 = make_shared<Interest>("/TestCommandInterest/Validation/Command2");
	int64_t timestamp = time::now() / 1000000;
	timestamp -= 5000;
	commandInterest2->getName().append(name::Component::fromNumber(timestamp)).append(name::Component::fromNumber(random::generateWord64()));
	keyChain.signByIdentity(*commandInterest2, identity);
	validator.validate(*commandInterest2,
		bind(&TestCore::validated, &core, _1),
		bind(&TestCore::validationFailed, &core, _1));
	if(!core.m_validity){
		cout<<"outdated"<<endl;
	}
	//Test an unauthorized command
	Name identity2("/TestCommandInterest/Validation2");
	Name certName2;
	certName2 = keyChain.createIdentity(identity2);
  
	shared_ptr<Interest> commandInterest3 = make_shared<Interest>("/TestCommandInterest/Validation/Command3");
	generator.generateWithIdentity(*commandInterest3, identity2);
	validator.validate(*commandInterest3,
		bind(&TestCore::validated, &core, _1),
		bind(&TestCore::validationFailed, &core, _1));

	if(!core.m_validity){
		cout<<"unauthorized"<<endl;
	}


	//Test another unauthorized command
	shared_ptr<Interest> commandInterest4 = make_shared<Interest>("/TestCommandInterest/Validation2/Command");
	generator.generateWithIdentity(*commandInterest4, identity);
	validator.validate(*commandInterest4,
		bind(&TestCore::validated, &core, _1),
		bind(&TestCore::validationFailed, &core, _1));
  	if(!core.m_validity)
  		cout<<"unauthorized"<<endl;*/
	return 1;
}
