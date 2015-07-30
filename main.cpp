#include <iostream>
#include <string>
using namespace std;

#include "CameraMonitor.h"

int main(int argc, char** argv)
{
	string config_file = "default.xml";
	if(argc>=2)
		config_file = string(argv[1]);

	CameraMonitor monitor(config_file);
	monitor.work();

	return 0;
}