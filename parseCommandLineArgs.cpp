#include <tuple>
#include <cstdlib>
#include <iostream>
#include <lyra/lyra.hpp>


std::tuple<bool, double> parseCommandLineArgs(int argc, const char** argv)
{
	bool isFullScreen = true;
	double timer = 60.0;
	bool show_help = false;

	auto cli
		= lyra::help(show_help).description("Play video with glitch filters, cl args for fullscreen and a run timer")
		| lyra::opt(isFullScreen)
			["-f"]["--fullscreen"]("Use '-f' or '--fullscreen' to start in exclusive fullscreen")
		| lyra::opt(timer, "timer")
			["-t"]["--timer"]("Use '-t 60' or '--timer 120' to set a timer for the app in seconds");
	
	auto result = cli.parse({ argc, argv });

	

	if(!result)
	{
		std::cerr << "Error in command line: " << result.errorMessage() << " to see the help type -?" << std::endl;
		std::cerr << cli << "\n"; // <1>
		std::exit(1);
	}

	// Show the help when asked for.
	if (show_help) // <2>
	{
		std::cout << cli << "\n";
		std::exit(0);
	}
	return { isFullScreen, timer };
	
}
