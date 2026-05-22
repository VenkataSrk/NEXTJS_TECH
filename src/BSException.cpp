#include "BSException.h"

        std::string BSERRDEF::BSException::getBSerrorstr(BSERRDEF::BSError err){
            std::string ret;
            switch(err){
                case BSERRDEF::BSError::CONFIG_LOADFAILURE:
                ret = "BSError::CONFIG_LOADFAILURE";
                break;
                case BSERRDEF::BSError::CONFIG_NOTRUNK:
                ret = "BSError::CONFIG_NOTRUNK";
                break;
                case BSERRDEF::BSError::CONFIG_NOGROUPS:
                ret = "BSError::CONFIG_NOGROUPS";
                break;
                case BSERRDEF::BSError::CONFIG_NOGROUP:
                ret = "BSError::CONFIG_NOGROUP";
                break;
                case BSERRDEF::BSError::CONFIG_NOAPP:
                ret = "BSError::CONFIG_NOAPP";
                break;
                case BSERRDEF::BSError::NOERROR:
                ret = "BSError::NOERROR";
                break;
                default:
                ret = "";
                break;

            }
            return ret;
        }