#include "WordParser.h"

using namespace std;

void example(){
	std::string ab = "Good&&Name";
	std::string cd = CreateSafeName(ab);
	std::cout<<"CHECK "<< cd<<std::endl;

}
