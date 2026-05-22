#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
//#define RAPIDXML_NO_EXCEPTIONS
#include "../rapidxml-1.13/rapidxml.hpp"
#include "../rapidxml-1.13/rapidxml_utils.hpp"

using namespace rapidxml;
using namespace std;
#if defined(RAPIDXML_NO_EXCEPTIONS)
void rapidxml::parse_error_handler(const char* what, void* where) {
    printf("Parse error(@%p): %s\n", where, what);
    std::abort();
}
#endif
xml_node<>* GetChildNode(xml_node<>*root,std::string childName){

for (xml_node<> *node = root->first_node(); node; node = node->next_sibling()){
if(node)
 if(childName == node->name())
   return node;
}
return NULL;
}

int main(void)
{
	cout << "Parsing my beer journal..." << endl;
std::string xmlDoc ="<?xml version=\"1.0\" encoding=\"utf-8\"?><conference-info confid=\"b7956f34-108e-4e2b-a56d-2dda1e673f2b\" state=\"start\"><host-info entity=\"100@node1.vectone.com\" confdisplayname=\"morning meeting\"></host-info><users count=\"1\"><user entity=\"100@node1.vectone.com\" type=\"host\" index=\"0\" callcount=\"2\"><call-info index=\"0\"><call_status status=\"joining\" /><display-text value=\"\" /><call-id id=\"1c0b82d4-5a9c-4ee2-b204-9b429dc6242d\" /><to sipuri=\"102@node1.vectone.com\" tagid=\"9b429dc6242d\" /><from sipuri=\"100@node1.vectone.com\"  tagid=\"b2041c0b82d4-5a9c-4ee2\" /></call-info><call-info index=\"1\"><call_status status=\"joining\" /><display-text value=\"\" /><call-id id=\"3c0b82d4-5a9c-4ee2-b204-9b429dc6242d\" /><to sipuri=\"101@node1.vectone.com\"  tagid=\"20b429dc6242d\" /><from sipuri=\"100@node1.vectone.com\" tagid=\"21041c0b82d4-5a9c-4ee2\" /></call-info></user></users></conference-info>";
std::string xmlDoc2 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><document><conference-info attribute=\"0\" /><conference-info attribute=\"1\"><conference-info attribute=\"2\" more_attribute=\"child\" /><conference-info attribute=\"3\" more_attribute=\"sibling\" /></conference-info></document>";
std::string xmlDoc3 = "<?xml version=\"1.0\" encoding=\"utf-8\"?><conference-info><conference-info confid=\"b7956f34-108e-4e2b-a56d-2dda1e673f2b\" state=\"start\" /><node attribute=\"0\" />   <node attribute=\"1\">      <node attribute=\"2\" more_attribute=\"child\" />      <node attribute=\"3\" more_attribute=\"sibling\" />   </node>   </conference-info>";
	xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile ("./ad-hoc.xml");
        std::stringstream buffer;
//	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
buffer << theFile.rdbuf();
//	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
std::string content =  xmlDoc;
printf( "XML String %s \n",content.c_str()	);
try{
doc.parse<0>(&xmlDoc[0]);
}
catch (const std::runtime_error& e)
    {
        std::cerr << "Runtime error was: " << e.what() << std::endl;
	exit(1);
    }
    catch (const rapidxml::parse_error& e)
    {
        std::cerr << "Parse error was: " << e.what() << std::endl;
	exit(1);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error was: " << e.what() << std::endl;
	exit(1);
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred." << std::endl;
	exit(1);
    }
	// Find our root node
	//root_node = doc.first_node("conference-info");
	root_node = doc.first_node();
        printf("Root node name %s \n", root_node->name());
xml_node<>*child =  GetChildNode(root_node,"host-info");
if(child){
printf("Child Name is %s \n",child->name());
xml_node<>*subnode =  GetChildNode(root_node,"users");
if(subnode){
printf("Sub Node of child %s \n",subnode->name());
xml_node<>*leaf = GetChildNode(subnode,"user");
if(leaf){
printf("Leaf node of sub node %s \n",leaf->name());
for(xml_node<>*steam = leaf->first_node(); steam; steam = steam->next_sibling()){
if(steam){
printf("Steam Names are %s \n", steam->name());
for(xml_node<>*last = steam->first_node(); last; last = last->next_sibling()){
if(last)
 printf("last nodes are %s \t", last->name());
}
}
}
}
}
}
	// Iterate over the brewerys
/*	for (xml_node<> * brewery_node = root_node->first_node(); brewery_node; brewery_node = brewery_node->next_sibling())
	{
	    printf("I have visited child name %s \n ", brewery_node->name());
//	    	brewery_node->first_attribute("entity")->value());
	    //	brewery_node->first_attribute("attribute")->value());
	    	//brewery_node->first_attribute("confdisplayname")->value());
	    	//brewery_node->first_attribute("more_attribute")->value());
            // Interate over the beers
	    }*/
	    cout << endl;
	
}
