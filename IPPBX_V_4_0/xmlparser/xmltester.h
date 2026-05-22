#ifndef XML_TESTER
#define XML_TESTER
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "./xGateXMLParser.h"
class xmltester{
public:
xmltester();
~xmltester();
void starttest(const char* payload);
void testencoder();
};
#endif
