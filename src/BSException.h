#ifndef BUSINESS_EXCEPTION
#define BUSINESS_EXCEPTION
#include "CAUtilityInternal.h"
namespace BSERRDEF{
enum class BSError{
    NOERROR = 0,
    CONFIG_LOADFAILURE=300,
    CONFIG_NOTRUNK,
    CONFIG_NOGROUPS,
    CONFIG_NOGROUP,
    CONFIG_NOAPP,
    


};
class BSException{
    BSException()=default;
    virtual ~BSException()=default;
    public:
static std::string getBSerrorstr(BSError err);
        
};

};

#endif