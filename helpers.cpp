#include "helpers.h"

void textDump(std::string output){
	std::ofstream myfile (LOGFILE, std::ios::out | std::ios::app); 
	if (myfile.is_open())
	{
		myfile << output;
		myfile << "\n";
		myfile.close();
	}
}

float randb(float a, float b){
	float ret = ((float) rand()) / (float) RAND_MAX;
	return a + ret*(b-a);
}