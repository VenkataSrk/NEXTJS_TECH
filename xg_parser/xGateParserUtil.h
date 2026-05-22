#ifndef XGATE_PARSER_UTILITY
#define XGATE_PARSER_UTILITY
#include <string>
#include <iostream>
#include <vector>
namespace parser{
  enum XPARSER_ERROR{
    RETURN_FAIL=0,
    RETURN_SUCCESS,
    PARSER_ERROR,
    RUNTIME_ERROR,
    STANDARD_ERROR,
    UNKNOWN_ERROR
  };
};

#endif
