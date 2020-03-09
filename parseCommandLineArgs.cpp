#include <tuple>

std::tuple<bool, double> parseCommandLineArgs(int argc, char* argv[])
{
	if (argc < 2) { return {true, 6.0}; }
	
}