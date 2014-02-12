#include "write_echo.h"


write_echo::write_echo(Face face, storage_handle* p_handle, CommandInterestValidator validator)
    : face_(face)
    , p_handle_(p_handle)
    , validator_(validator)
  {
    
  }
// Interest.
void write_echo::operator()
(const Name& prefix, const Interest& interest) {
  validator_.validate(interest,
           bind(&write_echo::validated, this, _1),
           bind(&write_echo::validationFailed, this, _1));
}

// onRegisterFailed.
void write_echo::operator()(const Name& prefix, const std::string& reason){

}

void write_echo::validated(const shared_ptr<const Interest>& interest){
  validres = 1;
}

void write_echo::validationFailed(const shared_ptr<const Interest>& interest){
  validres = 0;
}