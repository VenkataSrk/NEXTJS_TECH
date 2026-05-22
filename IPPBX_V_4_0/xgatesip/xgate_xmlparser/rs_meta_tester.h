/*
 * ******************************************************************************
 * Vectone Mobile
 * @file rs_meta_tester.h
 *
 * @brief This is sample tester for RS Metadata XML Parser/Encoder header file.
 *
 * @author Narrain prithvi Dharuman
 * ******************************************************************************
 */ 
#ifndef RS_XML_TESTER
#define RS_XML_TESTER
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "xGateRSXMLParserUtil.h"
#include "xGateRSXMLParser.h"

class xmltester{
public:
    xmltester();
    ~xmltester();
    void starttest(const char* payload);
};
#endif
