#include "loguru.cpp"

// RooBarb
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// #include "gnuplot_v.hpp"

#include "VegaXmlPlotter.h"


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
 #include <conio.h>   //for getch(), needed in wait_for_key()
 #include <windows.h> //for Sleep()
 void sleep(int i) { Sleep(i*1000); }
#endif

void wait_for_key ()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)  // every keypress registered, also arrow keys
    cout << endl << "Press any key to continue..." << endl;

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    _getch();
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    cout << endl << "Press ENTER to continue..." << endl;

    std::cin.clear();
    std::cin.ignore(std::cin.rdbuf()->in_avail());
    std::cin.get();
#endif
    return;
}


int main( int argc, char* argv[] ) {

	loguru::init(argc, argv);
	TaskFactory::registerTaskRunner<VegaXmlPlotter>( "VegaXmlPlotter" );
	TaskEngine engine( argc, argv, "VegaXmlPlotter" );


	



	// wait_for_key();


	return 0;
}
