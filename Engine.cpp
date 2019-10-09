#include "loguru.cpp"

// RooBarb
#include "XmlConfig.h"
#include "TaskEngine.h"
using namespace jdb;

// #include "gnuplot_v.hpp"

#include "Rnuplot.h"

int main( int argc, char* argv[] ) {

	loguru::init(argc, argv);
	TaskFactory::registerTaskRunner<Rnuplot>( "Rnuplot" );
	TaskEngine engine( argc, argv, "Rnuplot" );
	return 0;
}
