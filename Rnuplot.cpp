


#include "Rnuplot.h"
#include "ChainLoader.h"
#include "XmlHistogram.h"
#include "Utils.h"

#include "TLatex.h"
#include "THStack.h"
#include "TKey.h"
#include "TList.h"
#include "TStyle.h"
#include "TColor.h"
#include "TTree.h"
#include "TString.h"

#include <thread>

#include "loguru.h"

#include <iostream>

//  for exec
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
//


// #include "DataFileMaker.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
size_t DataFileMaker::tmpfile_count_max = 27;
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
size_t DataFileMaker::tmpfile_count_max = 64;
#endif
size_t DataFileMaker::tmpfile_count = 0;


inline bool exists_test (const std::string& name) {
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}



void Rnuplot::init(){
	LOG_SCOPE_FUNCTION(INFO);
	LOG_F( INFO, "Rnuplot Starting" );
	string logfile = config[ "Log:url" ];
	if ( config.exists( "Log:url" ) ){
		LOG_F( INFO, "Writing log to %s", logfile.c_str() );
		loguru::add_file(logfile.c_str(), loguru::Truncate, loguru::Verbosity_MAX);
	} else {
		LOG_F( INFO, "Not writing logfile");
	}

	

	// handle_map[ "TCanvas"      ] = &Rnuplot::exec_TCanvas;
	handle_map[ "TFile"         ] = &Rnuplot::exec_TFile;
	handle_map[ "TFileOut"         ] = &Rnuplot::exec_TFileOut;
	handle_map[ "Equation"     ] = &Rnuplot::exec_Equation;
	handle_map[ "E"            ] = &Rnuplot::exec_Equation;
	handle_map[ "Latex"        ] = &Rnuplot::exec_Latex;
	// handle_map[ "Script"       ] = &Rnuplot::exec_Script;
	handle_map[ "Command"         ] = &Rnuplot::exec_Command;
	handle_map[ "C"         ] = &Rnuplot::exec_Command;
	handle_map[ "Plot"         ] = &Rnuplot::exec_Plot;
	handle_map[ "Histogram"    ] = &Rnuplot::exec_Histogram;
	handle_map[ "H"            ] = &Rnuplot::exec_Histogram;
	handle_map[ "Legend"            ] = &Rnuplot::exec_Legend;
	handle_map[ "Gnuplot"    ] = &Rnuplot::exec_Gnuplot;
	handle_map[ "Data"            ] = &Rnuplot::exec_Data;

	/*** Transforms ***/
	handle_map[ "Transforms"   ] = &Rnuplot::exec_Transforms;
	handle_map[ "Transform"   ] = &Rnuplot::exec_Transforms;

	handle_map[ "Projection"   ] = &Rnuplot::exec_transform_Projection;
	handle_map[ "ProjectionX"  ] = &Rnuplot::exec_transform_ProjectionX;
	handle_map[ "ProjectionY"  ] = &Rnuplot::exec_transform_ProjectionY;
	handle_map[ "FitSlices"    ] = &Rnuplot::exec_transform_FitSlices;
	handle_map[ "FitSlice "    ] = &Rnuplot::exec_transform_FitSlices;
	handle_map[ "MultiAdd"     ] = &Rnuplot::exec_transform_MultiAdd;
	handle_map[ "Add"          ] = &Rnuplot::exec_transform_Add;
	handle_map[ "Divide"       ] = &Rnuplot::exec_transform_Divide;
	handle_map[ "Difference"   ] = &Rnuplot::exec_transform_Difference;
	handle_map[ "Rebin"        ] = &Rnuplot::exec_transform_Rebin;
	handle_map[ "Scale"        ] = &Rnuplot::exec_transform_Scale;
	handle_map[ "Normalize"    ] = &Rnuplot::exec_transform_Normalize;
	handle_map[ "Draw"         ] = &Rnuplot::exec_transform_Draw;
	handle_map[ "Clone"        ] = &Rnuplot::exec_transform_Clone;
	handle_map[ "Smooth"       ] = &Rnuplot::exec_transform_Smooth;
	handle_map[ "CDF"          ] = &Rnuplot::exec_transform_CDF;
	handle_map[ "Style"        ] = &Rnuplot::exec_transform_Style;
	handle_map[ "SetBinError"  ] = &Rnuplot::exec_transform_SetBinError;
	handle_map[ "BinLabels"    ] = &Rnuplot::exec_transform_BinLabels;
	handle_map[ "Sumw2"        ] = &Rnuplot::exec_transform_Sumw2;
	handle_map[ "ProcessLine"  ] = &Rnuplot::exec_transform_ProcessLine;
	handle_map[ "Assign"       ] = &Rnuplot::exec_transform_Assign;
	handle_map[ "Format"       ] = &Rnuplot::exec_transform_Format;
	handle_map[ "Print"        ] = &Rnuplot::exec_transform_Print;
	handle_map[ "Proof"        ] = &Rnuplot::exec_transform_Proof;
	handle_map[ "List"         ] = &Rnuplot::exec_transform_List;


	Gnuplot::set_terminal_std("qt");

} // init

void Rnuplot::exec_node( string _path ){
	LOG_SCOPE_FUNCTION(INFO);
	if ( false == config.exists( _path ) )
		return;

	string tag = config.tagName( _path );
	LOG_F( 1, "exec_node( %s ), tag = %s", _path.c_str(), tag.c_str() );

	if ( 0 == handle_map.count( tag ) ){
		LOG_F( ERROR, "No Handler for %s", tag.c_str() );
		return;
	}

	exec( tag, _path );
} // exec_node

string Rnuplot::random_string( size_t length ){
	auto randchar = []() -> char
	{
		const char charset[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[ rand() % max_index ];
	};
	string str(length,0);
	generate_n( str.begin(), length, randchar );
	return str;
}

void Rnuplot::make(){
	LOG_SCOPE_FUNCTION(INFO);

	// Load data first
	exec_children( "", "TFile" );
	// exec_children( "", "Data" );
	
	vector<string> dpaths = config.childrenOf( "", "TFile" );
	if ( dpaths.size() > 0 && dataFiles.size() == 0 && dataChains.size() == 0 ){
		LOG_F( WARNING, "No valid data files found" );
	}

	vector<string> paths = config.childrenOf( "", "ExportConfig" );
	for ( string p : paths ){
		exec_node( p );
	}

	// if a TFile node is given for writing output, then execute it
	exec_node( "TFileOut" );

	// Top level nodes
	vector<string> tlp = { "Data", "Gnuplot", "Script", "Plot", "Loop", "RangeLoop", "Transforms", "Transform", "Latex" };
	vector<string> known_but_not_processed = { "TFile", "arg", "argc", "jobIndex" };
	paths = config.childrenOf( "", 1 );
	for ( string p : paths ){
		string tag = config.tagName( p );
		if ( std::find( tlp.begin(), tlp.end(), tag ) != tlp.end() ){
			exec_node( p );
		} else {
			if ( std::find( known_but_not_processed.begin(), known_but_not_processed.end(), tag ) == known_but_not_processed.end() ){
				LOG_F( WARNING, "Found unrecognized node = %s, not processed here", tag.c_str() );
			}
		}
	}

	// Write data out if requested
	if ( dataOut && dataOut->IsOpen() ){
		dataOut->Write();
		dataOut->Close();
		LOG_F( INFO, "Write to %s completed, file closed", config.getString( "TFileOut:url" ).c_str() );
	}

} // make

void Rnuplot::loadDataFile( string _path ){
	LOG_SCOPE_FUNCTION(INFO);
	if ( config.exists( _path + ":name" ) && config.exists( _path + ":url" )  ){
		string name = config.getXString( _path+":name" );
		string url = config.getXString( _path+":url" );

		LOG_F( 1, "Data[%s] = %s", name.c_str(), url.c_str() );
		// LOG_S( INFO ) <<  "Data name=" << name << " @ " << url ;

		TFile * f = new TFile( url.c_str() );
		if ( false == f->IsOpen() ){
			LOG_F( ERROR, "%s cannot be opened", url.c_str() );
			return;
		}

		dataFiles[ name ] = f;
		LOG_F( INFO, "Data[%s] = %s", name.c_str(), url.c_str() );

		// if ( config.getBool( _path + ":inline", false ) )
		// 	inlineDataFile( _path, f );

	} else if ( config.exists( _path + ":name" ) ){
		string name = config.getXString( _path+":name" );
		string fname = "tmp_" + name + ".root";
		TFile *f = new TFile( fname.c_str(), "RECREATE" );
		f->cd();
		vector<string> paths = config.childrenOf( _path, "HistogramData" );
		for ( string p : paths ){

			TH1 * _h = XmlHistogram::fromXml( config, p );
			_h->Write();
			LOG_F( INFO, "Making %s = %p", p.c_str(), _h );
			dataFiles[ name ] = f;
		}
	}
} // loadDataFiles

void Rnuplot::exec_TFileOut( string _path ){
	LOG_SCOPE_FUNCTION(INFO);
	string url = config.get<string>( _path + ":url" );
	string mode = config.get<string>( _path + ":mode", "RECREATE" );
	LOG_F( INFO, "Opening %s in %s (:mode)", url.c_str(), mode.c_str() );
	dataOut = new TFile( url.c_str(), mode.c_str() );
}

int Rnuplot::numberOfData() {
	LOG_SCOPE_FUNCTION(INFO);
	return dataFiles.size() + dataChains.size();
} // numberOfData

void Rnuplot::loadChain( string _path ){
	LOG_SCOPE_FUNCTION(INFO);
	string name     = config.getXString( _path + ":name" );
	string treeName = config.getXString( _path + ":treeName" );
	string url      = config.getXString( _path + ":url" );
	int maxFiles    = config.getInt( _path + ":maxFiles", -1 );
	int index       = config.getInt( _path + ":index", -1 );
	int splitBy     = config.getInt( _path + ":splitBy", 50 );

	dataChains[ name ] = new TChain( treeName.c_str() );
	
	if ( url.find( ".lis" ) != std::string::npos ){
		if ( index >= 0 ){
			ChainLoader::loadListRange( dataChains[ name ], url, index, splitBy );
			LOG_F( INFO, "Loading index=%d, splitBy=%d", index, splitBy );
		}else 
			ChainLoader::loadList( dataChains[ name ], url, maxFiles );
	} else 
		ChainLoader::load( dataChains[ name ], url, maxFiles );
	// LOG_S(INFO) << "Loaded TTree [name=" << quote(name) << "] from url: " << url;
	int nFiles = 0;
	if ( dataChains[name]->GetListOfFiles() )
		nFiles = dataChains[name]->GetListOfFiles()->GetEntries();
	// LOG_S(INFO) << "Chain has " << nFiles << plural( nFiles, " file", " files" );
} // loadChain

void Rnuplot::loadData(){
	LOG_SCOPE_FUNCTION(INFO);
	vector<string> data_nodes = config.childrenOf( nodePath, "Data" );
	
	LOG_F( INFO, "Found %lu data nodes", data_nodes.size() );
	for ( string path : data_nodes ){
		if ( config.exists( path + ":name" ) && !config.exists( path + ":treeName" ) ){
			loadDataFile( path );
		} else if ( config.exists( path + ":name" ) && config.exists( path + ":treeName" )){
			loadChain( path );
		}
	}

	if ( data_nodes.size() <= 0 ){
		LOG_F(WARNING, "No data nodes found!" );
	}
} // loadData


TObject* Rnuplot::findObject( string _path ){
	LOG_SCOPE_FUNCTION(INFO);

	string data = config.getXString( _path + ":data", config.getXString( _path + ":data" ) );
	string name = config.getXString( _path + ":name", config.getXString( _path + ":name" ) );

	if ( "" == data && name.find( "/" ) != string::npos ){
		data = dataOnly( name );
		//INFOC( "data from name " << quote( data ) );
		name = nameOnly( name );
	}

	//INFO( classname(), "Looking for [" << name << "] in " << data << "=(" << dataFiles[data] <<")" );
	// first check for a normal histogram from a root file
	if ( dataFiles.count( data ) > 0 && dataFiles[ data ] ){
		LOG_F( INFO, "Lokking in %s", data.c_str() );
		if ( nullptr == dataFiles[ data ]->Get( name.c_str() ) ) return nullptr;
		TObject * obj = (TH1*)dataFiles[ data ]->Get( name.c_str() );
		return obj;
	}

	// finally look for histos we made and named in the ttree drawing
	if ( globalHistos.count( name ) > 0 && globalHistos[ name ] ){
		LOG_F( INFO, "Looking in globalHistos" );
		return globalHistos[ name ];
	}

	if ( globalGraphs.count( name ) > 0 && globalGraphs[ name ] ){
		LOG_F( INFO, "Looking in globalGraphs" );
		return globalGraphs[ name ];
	}

	LOG_F( WARNING, "Object [%s/%s] was not found in any source", data.c_str(), name.c_str() );
	return nullptr;
} // findObject

TH1 *Rnuplot::findHistogram( string _data, string _name, string _path, int iHist ){
	LOG_SCOPE_FUNCTION(INFO);
	LOG_F( 1, "_data=%s, _name=%s", _data.c_str(), _name.c_str() );

	string data = _data;
	string name = _name;

	if ( "" == data && name.find( "/" ) != string::npos ){
		data = dataOnly( name );
		LOG_F( INFO, "Setting data to %s", data.c_str()  );
		name = nameOnly( name );
	}

	// finally look for histos we made and named in the ttree drawing
	if ( globalHistos.count( name ) > 0 && globalHistos[ name ] ){
		LOG_F( INFO, "Found histogram in mem pool [%s]", name.c_str() );
		return globalHistos[ name ];
	}

	LOG_F( 1, "data=%s, name=%s, dataFiles.size()=%lu", data.c_str(), name.c_str(), dataFiles.size() );
	if ( "" == data && dataFiles.size() >= 1 ){
		data = dataFiles.begin()->first;
		LOG_F( 1, "data was not set -> setting to %s", data.c_str()  );
	}

	//INFO( classname(), "Looking for [" << name << "] in " << data << "=(" << dataFiles[data] <<")" );
	// first check for a normal histogram from a root file
	if ( dataFiles.count( data ) > 0 && dataFiles[ data ] ){


		TH1 * h = (TH1*)dataFiles[ data ]->Get( name.c_str() );
		if ( nullptr != h ){
			h = (TH1*)h->Clone( (string("hist_") + h->GetName() ).c_str() );
			if ( config.getBool( _path + ":setdir", true ) ){} 
			else 
				h->SetDirectory(0);
			//INFO( classname(), "Found Histogram [" << name << "]= " << h << " in data file " << data );
			return h;
		}
	}

	// look for a dataChain with the name
	if ( dataChains.count( data ) > 0 && dataChains[ data ] ){
		TH1 * h = makeHistoFromDataTree( _path, iHist );
		return h;
	}

	// finally look for histos we made and named in the ttree drawing
	if ( globalHistos.count( name ) > 0 && globalHistos[ name ] ){	
		return globalHistos[ name ];
	}

	return nullptr;
} // findHistogram

TH1* Rnuplot::findHistogram( string _path, int iHist, string _mod ){
	LOG_SCOPE_FUNCTION(INFO);
	LOG_F( 1, "_path=%s, iHist=%d, _mod=%s", _path.c_str(), iHist, _mod.c_str() );

	string data = config.getXString( _path + ":data" + _mod, config.getXString( _path + ":data" ) );
	string name = config.getXString( _path + ":name" + _mod, config.getXString( _path + ":name" + _mod ) );

	return findHistogram( data, name, _path, iHist );
} //findHistogram

TH1* Rnuplot::makeHistoFromDataTree( string _path, int iHist ){
	LOG_SCOPE_FUNCTION(INFO);

	string data = config.getXString( _path + ":data" );
	string hName = config.getXString( _path + ":name" );

	if ( "" == data && hName.find( "/" ) != string::npos ){
		data = dataOnly( hName );
		hName = nameOnly( hName );
	}

	if ( "" == data && dataChains.size() == 1 ){
		data = dataChains.begin()->first;
	} else if ( "" == data ){
		LOG_F( ERROR, "Must specify the data source" );
		return nullptr;
	}
	
	TChain * chain = dataChains[ data ];
	if ( nullptr == chain ){
		LOG_F( ERROR, "Chain is null" );
		return nullptr; 
	}
	
	LOG_F( INFO, "Using name=%s, data=%s", quote(hName).c_str(), quote(data).c_str() );

	string title = config.getXString( _path + ":title" );
	string drawCmd = config.getXString( _path + ":draw" ) + " >> " + hName;
	string selectCmd = config.getXString( _path + ":select" );
	string drawOpt = config.getString( _path + ":opt" );


	// If a title is not given then set the x, y, and z axis titles based on draw command
	if ( !config.exists( _path + ":title" ) ){
		string rdraw = config.getXString( _path + ":draw" );
		size_t n = std::count(rdraw.begin(), rdraw.end(), ':');
		if ( 0 == n ){
			title = ";" + rdraw + "; dN/d(" + rdraw + ")";
		} else if ( 1 == n ){
			size_t p1 = rdraw.find(':');
			string yt = rdraw.substr( 0, p1 );
			string xt = rdraw.substr( p1+1 );
			title = ";" + xt + ";" + yt;
		} else if ( 2 == n ){
			size_t p1 = rdraw.find(':');
			size_t p2 = rdraw.find(':', p1+1);
			string zt = rdraw.substr( 0, p1 );
			string yt = rdraw.substr( p1+1, p2 );
			string xt = rdraw.substr( p2+1 );
			title = ";" + xt + ";" + yt + ";" + zt;
		}
	}
	
	// if bins are given lets assume we need to make the histo first
	if ( config.exists( _path + ":bins_x" ) ){
		HistoBins bx( config, config.getXString( _path + ":bins_x" ) );
		HistoBins by( config, config.getXString( _path + ":bins_y" ) );
		HistoBins bz( config, config.getXString( _path + ":bins_z" ) );
		HistoBook::make( "D", hName, title, bx, by, bz );
		LOG_F( INFO, "x=%s, y=%s, z=%s", bx.toString().c_str(), by.toString().c_str(), bz.toString().c_str() );
	}

	long N = std::numeric_limits<long>::max();
	if ( config.exists( _path + ":N" ) ){
		N = config.get<long>( _path + ":N" );
		// LOG_S(INFO) << "TTree->Draw( " << quote(drawCmd) << ", " << quote(selectCmd) << ", " << quote(drawOpt) << ", " << N << " );";
	} else {
		// LOG_S(INFO) << "TTree->Draw( " << quote(drawCmd) << ", " << quote(selectCmd) << ", " << quote(drawOpt) << " );";
	}

	chain->Draw( drawCmd.c_str(), selectCmd.c_str(), drawOpt.c_str(), N );
	TH1 *h = (TH1*)gPad->GetPrimitive( hName.c_str() );

	if ( config.exists( _path +":after_draw" ) ){
		string cmd = ".x " + config[_path+":after_ draw"] + "( " + h->GetName() + " )";
		LOG_F( INFO, "Executing: %s", cmd.c_str()  );
		// gROOT->ProcessLine( cmd.c_str() );
		gROOT->LoadMacro( config[_path+":after_draw"].c_str() );
		gROOT->ProcessLine( "after_draw()" );
	}	

	globalHistos[ hName ] = h;

	return h;
} // makeHistoFromDataTree




void Rnuplot::exec_children( string _path ){
	LOG_SCOPE_FUNCTION( 2 );
	vector<string> paths = config.childrenOf( _path, 1 );
	for ( string p : paths ){
		exec_node( p );
	}
} //exec_children

void Rnuplot::exec_children( string _path, string tag_type ){
	LOG_SCOPE_FUNCTION( 2 );
	vector<string> paths = config.childrenOf( _path, 1 );
	// DLOG( "exec children of %s, where tag = %s", _path.c_str(), tag_type.c_str() );
	for ( string p : paths ){
		string tag = config.tagName( p );

		if ( tag_type == tag )
			exec_node( p );
	}
} //exec_children

void Rnuplot::exec_TFile( string _path ){
	LOG_SCOPE_FUNCTION( 2 );
	if ( config.exists( _path + ":name" ) && !config.exists( _path + ":treeName" ) ){
		loadDataFile( _path );
	} else if ( config.exists( _path + ":name" ) && config.exists( _path + ":treeName" )){
		loadChain( _path );
	}
} // exec_Data

void Rnuplot::exec_Equation( string _path ){
	LOG_SCOPE_FUNCTION( INFO );

		gnuplot.cmd( current_out );

		gnuplot.set_style( "lines" );
		gnuplot.set_samples( 1000 );
		string formula = config.get<string>( _path + ":f", "x" );
		if ( config.exists( _path + ":x-range" ) ){
			vector<string> xr = config.getVector<string>( _path + ":x-range" );
			gnuplot.cmd( TString::Format( "set xrange[%s:%s]", xr[0].c_str(), xr[1].c_str() ).Data() );
		}
		if ( config.exists( _path + ":y-range" ) ){
			vector<string> xr = config.getVector<string>( _path + ":y-range" );
			gnuplot.cmd( TString::Format( "set yrange[%s:%s]", xr[0].c_str(), xr[1].c_str() ).Data() );
		}
		if ( config.exists( _path + ":with" ) ){
			LOG_F( INFO, "Setting style = %s", config.get<string>( _path + ":with" ).c_str() );
			gnuplot.set_style( config.get<string>( _path + ":with" ) );
		}	
		gnuplot.plot_equation(formula,config.get<string>( _path + ":t", formula ));

}


void Rnuplot::exec_Plot( string _path ) {
	LOG_SCOPE_FUNCTION( INFO );
	// DLOG( "Makign Plot[%s] @ %s", config.getString( _path+":name", "" ).c_str(), _path.c_str() );

	gnuplot.reset_plot();
	string terminal = config.get<string>( _path + ":terminal", "postscript eps enhanced defaultplex leveldefault color colortext dashlength 1.0 linewidth 1.0 butt noclip nobackground palfuncparam 2000,0.003 size 7.00in, 6.00in \"Helvetica\" 20  fontscale 1.3" );
	gnuplot.cmd( "set terminal " + terminal );
	
	string url = config.get<string>( _path + ":url" );
	current_out = "set output \"" + url + "\"";
	if ( "" == url ) 
		current_out = "set output";

	// guess terminal if not given and EPS is not good choice
	auto index_last_dot = url.find_last_of(".");
	string ext = url.substr( index_last_dot + 1 );
	// check for a few common endings (TODO, move into function)
	// TODO: handle to_lowercase
	if ( config.exists( _path + ":terminal" ) == false ){
		if ( "pdf" == ext ){
			gnuplot.cmd( "set terminal pdf size 7.00in, 6.00in" );
		} else if ( "eps" == ext ){
			gnuplot.cmd("set terminal postscript eps enhanced defaultplex leveldefault color colortext dashlength 1.0 linewidth 1.0 butt noclip nobackground palfuncparam 2000,0.003 size 7.00in, 6.00in \"Helvetica\" 20  fontscale 1.3");
		} else if ( "png" == ext ){
			gnuplot.cmd("set terminal png size 7.00in, 6.00in");
		}
	}

	// // Make the legend if it is given
	exec_children( _path, "Legend" );
	
	vector<string> tlp = { "Equation", "H", "E", "Histogram", "C", "Command" };
	vector<string> known_but_not_processed = { "Export", "Legend", "Axes" };
	vector<string> paths = config.childrenOf( _path, 1 );
	for ( string p : paths ){
		string tag = config.tagName( p );
		if ( std::find( tlp.begin(), tlp.end(), tag ) != tlp.end() ){
			exec_node( p );
		} else {
			if ( std::find( known_but_not_processed.begin(), known_but_not_processed.end(), tag ) == known_but_not_processed.end() ){
				LOG_F( WARNING, "Found unrecognized node inside <Plot/>, node = %s", tag.c_str() );
			}
		}
	}

	// Make LAtex on global scope for ease
	// exec_children( "", "TLatex" );

	if  ("" == url){
		cout << endl << "Press ENTER to continue..." << endl;
		cin.get();
	}
		
	
	
	// // Export the Plot if desired
	// exec_children( _path, "Export" );
} // exec_Plot

void Rnuplot::exec_Command( string _path ){
	gnuplot.cmd( config.get<string>( _path ) );
}


void Rnuplot::exec_Legend( string _path ){
	LOG_SCOPE_FUNCTION( INFO );
	if ( config.get<bool>( _path + ":visible", true ) == false ){
		gnuplot.cmd( "set nokey" );
	}

	gnuplot.cmd( "set key noopaque enhanced" );

	string pp = config.oneOf( _path + ":p", _path + ":pos", _path + ":position" );
	if ( config.exists( pp ) ){
		string kp = config.get<string>( pp );
		gnuplot.cmd( "set key " + kp );
	}
}

void Rnuplot::exec_Histogram( string _path ){

	LOG_SCOPE_FUNCTION( INFO );
	gnuplot.cmd( current_out );

	gnuplot.set_grid();
	gnuplot.set_style("points");

	string name = config.getXString( _path + ":name" );
	string data = config.getXString( _path + ":data" );
	string fqn = fullyQualifiedName( data, name );

	TH1* h = findHistogram( _path, 0 );

	if ( nullptr == h ) {
		LOG_F( WARNING, "Could not find Histogram @ %s (fqn=\"%s\")", _path.c_str(), fqn.c_str() );
		return;
	}

	float x_min = 100000, x_max = -1000;
	float y_min = 1000, y_max = -10000;

	vector<double> x, y, ey;
	for ( int i = 1; i < h->GetNbinsX(); i++ ){
		double vx = h->GetBinCenter( i );
		double vy = h->GetBinContent( i );
		double vey = h->GetBinError( i );

		x.push_back( vx );
		y.push_back( vy );
		ey.push_back( vey );

		if ( vx > x_max ) x_max = vx;
		if ( vy > y_max ) y_max = vy;

		if ( vx < x_min ) x_min = vx;
		if ( vy < y_min ) y_min = vy;
	}

	if (gnuplot.nplots == 0){
		gnuplot.cmd( TString::Format( "set xrange[%f:%f]", x_min, x_max ).Data() );
		gnuplot.cmd( TString::Format( "set yrange[%f:%f]", y_min, y_max ).Data() );
	}


	if ( config.exists( _path + ":x-range" ) ){
		vector<string> xr = config.getVector<string>( _path + ":x-range" );
		gnuplot.cmd( TString::Format( "set xrange[%s:%s]", xr[0].c_str(), xr[1].c_str() ).Data() );
	}
	if ( config.exists( _path + ":y-range" ) ){
		vector<string> xr = config.getVector<string>( _path + ":y-range" );
		gnuplot.cmd( TString::Format( "set yrange[%s:%s]", xr[0].c_str(), xr[1].c_str() ).Data() );
	}

	if ( config.exists( _path + ":style" ) ){
		gnuplot.set_style( config.get<string>( _path + ":style", "points" ) );
	}
	if ( config.exists( _path + ":with" ) ){
		gnuplot.set_style( config.get<string>( _path + ":with", "points" ) );
	}

	gnuplot.plot_xy(x, y, config.get<string>( _path + ":title" ));

	// gnuplot.plot_xy_err(x, y, ey, config.get<string>( _path + ":title" ));

}

void Rnuplot::exec_Gnuplot( string _path ){
	LOG_SCOPE_FUNCTION( INFO );

	//  get inline BUT means escape chars :(
	string lines = config.get<string>( _path );

	if ( config.exists( _path + ":script" ) ){
		// is this a file ?
		if ( exists_test( config.get<string>( _path + ":script" ) ) ){
			string gnuscript = config.get<string>( _path + ":script" );
			LOG_F( INFO, "Reading gnuplot script from file : %s", gnuscript.c_str() );
			ifstream gnuscriptf( gnuscript.c_str() );
			string aline = "";
			while (std::getline(gnuscriptf, aline)){
				lines += aline + "\n";
			}
		} else { // inline (probably to allow escape characters)
			lines = config.get<string>( _path + ":script" );
		}
	}


	LOG_F( INFO, "LINES \n%s", lines.c_str() );

	for ( auto dfn : gnudf ){
		stringstream cmd;
		cmd << dfn.first << " = \"" << dfn.second << "\"";
		gnuplot.cmd( cmd.str() );
	}

	std::istringstream iss(lines);

	for (std::string line; std::getline(iss, line); )
	{
		string rline = line;
		// for (auto dfn : gnudf ){
		// 	if ( replace(rline, dfn.first, "\"" + dfn.second + "\"" ) ) {
		// 		LOG_F( INFO, "%s --> %s = %s", dfn.first.c_str(), dfn.second.c_str(), rline.c_str() );
		// 	}
		// }

		gnuplot.cmd( rline );
	}

	if ( config.get<bool>( _path + ":pause", false ) == true ){
		cout << "NOW in gnuplot terminal. Press q and ENTER to continue..." << endl;
		string input = "";
		getline(cin,input);
		if ( "q" != input && "exit" != input ){
			gnuplot.cmd( input );
		}
		// cout << "USER: " << input << endl;
		while ( "q" != input && "exit" != input ){
			getline(cin,input);
			// cout << "USER: " << input << endl;
			if ( "q" != input && "exit" != input ){
				gnuplot.cmd( input );
			}
		}
		
	}
	

	gnuplot.cmd( "set output" ); // flush output!

}

void Rnuplot::exec_Data( string _path ){
	LOG_SCOPE_FUNCTION( INFO );
	if ( config.exists(_path + ":name" ) == false ){
		LOG_F( ERROR, "Must provide name for data file" );
		return;
	}

	TH1* h = findHistogram( "", config.get<string>( _path+":from" ) );

	if ( nullptr == h ) {
		LOG_F( WARNING, "Could not find Histogram @ %s", _path.c_str() );
		return;
	}

	string dfvn = config.get<string>( _path +":name" );

	string dfn = "";
	if ( true == config.get<bool>( _path + ":local", false ) ){
		dfn = dfvn + ".dat";
	}

	dfn = datafilemaker.convert( h, config.get<string>( _path + ":format" ), dfn );


	// vector<double> x, y, x_low, x_high, y_low, y_high;
	// for ( int i = 1; i < h->GetNbinsX(); i++ ){
	// 	double vx = h->GetBinCenter( i );
	// 	double vy = h->GetBinContent( i );
	// 	double vey = h->GetBinError( i );

	// 	x.push_back( vx );
	// 	x_low.push_back( h->GetBinLowEdge(i) );
	// 	x_high.push_back( h->GetBinLowEdge(i) + h->GetBinWidth(i) );
	// 	y.push_back( vy );
	// 	y_low.push_back( vy - vey );
	// 	y_high.push_back( vy + vey );
	// }

	// string dfn = "";

	// if ( true == config.get<bool>( _path + ":local", false ) ){
	// 	dfn = gnuplot.df( dfvn, x, y, x_low, x_high, y_low, y_high );
	// } else {
	// 	dfn = gnuplot.df( x, y, x_low, x_high, y_low, y_high );
	// }

	// if ( "" == dfn ){
	// 	LOG_F( ERROR, "Cannot make data file from %s", config.get<string>( _path+":from" ).c_str() );
	// }

	gnudf[ dfvn ] = dfn;
	LOG_F( INFO, "Data File [%s] = %s", dfvn.c_str(), dfn.c_str());
}

void Rnuplot::exec_Loop( string _path ){
	LOG_SCOPE_FUNCTION( INFO );
	vector<string> states;
	string var = "state";
	string v = "%s";


	/***************************************/
	/* LOOP over predefined states */
	if ( config.exists( _path + ":states" ) )
		states = config.getStringVector( _path +":states" );

	if ( config.exists( _path + ":var" ) )
		var = config.getString( _path + ":var" );


	/***************************************/
	/* LOOP as an incrementation */
	// support arange
	if ( states.size() == 0 && config.exists( _path + ":arange" ) ){
		vector<float> pars = config.getFloatVector( _path + ":arange" );

		if ( pars.size() >= 2 ){
			float step = 1.0;
			if ( pars.size() > 2  )
				step = pars[2];
			LOG_SCOPE_F( INFO, "Loop arange (%f, %f, %f)", pars[0], pars[1], step );
			for ( float fv = pars[0]; fv < pars[1] - step/2.; fv+=step ){
				states.push_back( ts( fv ) );
			}
		}
	}

	// support linspace
	if ( states.size() == 0 && config.exists( _path + ":linspace" ) ){
		vector<float> pars = config.getFloatVector( _path + ":linspace" );
		LOG_SCOPE_F( INFO, "Loop linspace (%f, %f, %f)", pars[0], pars[1], pars[2] );
		if ( pars.size() > 2  ){
			for ( int i = 0; i < (int)pars[2]+1; i++ ){
				float fv = pars[0] + ((pars[1] - pars[0]) / ((int)pars[2])) * i;
				states.push_back( ts( fv ) );
			}
		}
	}

	/***************************************/
	/* LOOP as a RANGE */
	// suport a range 
	if ( config.tagName( _path ) == "RangeLoop" ){

		if ( !config.exists( _path + ":vmin" ) ) { LOG_F( WARNING, "<RangeLoop vmin vmax min max width/>"  ); }
		if ( !config.exists( _path + ":vmax" ) ) { LOG_F( WARNING, "<RangeLoop vmin vmax min max width/>"  ); }
		if ( !config.exists( _path + ":min" ) ) { LOG_F( WARNING, "<RangeLoop vmin vmax min max width/>"  ); }
		if ( !config.exists( _path + ":max" ) ) { LOG_F( WARNING, "<RangeLoop vmin vmax min max width/>"  ); }



		string vmin = config.get<string>( _path + ":vmin" );
		string vmax = config.get<string>( _path + ":vmax" );
		string indexName = config.get<string>( _path + ":index", vmin+"_i" );

		HistoBins bx;
		bx.load( config, _path );
		if ( 0 == bx.nBins() ){
			LOG_F( WARNING, "Cannot make x bins" );
		}

		LOG_F( INFO, "<RangLoop vmin=%s vmax=%s", vmin.c_str(), vmax.c_str() );
		LOG_F( INFO, "%s", bx.toString().c_str() );
		LOG_F( INFO, "</RangLoop>" );

		for ( int i = 0; i < bx.bins.size()-1; i++ ){
			float va = bx.bins[i];
			float vb = bx.bins[i+1];
			// DLOG( "Executing range loop %s = %s", var.c_str(), state.c_str() );
			config.set( vmin, ts(va) );
			config.set( vmax, ts(vb) );

			LOG_F( INFO, "Executing Range Loop [%s = %d] (%s=%f, %s=%f)", indexName.c_str(), i, vmin.c_str(), va, vmax.c_str(), vb );

			config.set( indexName, ts(i) );

			exec_children( _path );
		}
		return;
	}
	
	

	/***************************************/
	/* LOOP as a GLOB */
	if ( config.exists( _path + ":glob" ) ){
		vector<string> gstates = glob( config.get<string>( _path + ":glob" ) );
		// append these to the states to loop over
		for ( string &s : gstates ){
			states.push_back( s );
		}
	}


	// Used for Transforms or organization
	if ( states.size() == 0 ){
		LOG_SCOPE_F( INFO, "Scope at %s", _path.c_str() );
		// LOG_F( INFO, "Executing Scope" );
		exec_children( _path );
	} else {
		LOG_SCOPE_F( INFO, "Loop at %s", _path.c_str() );
		int i = 0;
		string indexName = config.get<string>( _path + ":index", var + "_i" );
		for ( string state : states ){
			LOG_F( INFO, "Executing loop %s[%s = %d] = %s", var.c_str(), indexName.c_str(), i, state.c_str() );
			string value = state;
			config.set( var, value );
			config.set( indexName, ts(i) );
			// vector<string> paths = config.childrenOf( _path, 1 );
			exec_children( _path );
			i++;
		} // loop on states
	}

	

} // exec_Loop

vector<string> Rnuplot::glob( string query ){
	LOG_SCOPE_FUNCTION( INFO );
	vector<string> names;
	
	// allow prefix of type
	//example "TH1:test*" will glob all names like test* that are TH1 (TH1D, TH1F etc)
	size_t typePos = query.find( ":" );
	string type = "";
	if ( typePos != string::npos ){
		type = query.substr( 0, typePos);
		query = query.substr( typePos+1 );
		LOG_F( INFO, "Query: %s, type: %s", query.c_str(), type.c_str() );
	}


	// TODO add suffix support
	size_t pos = query.find( "*" );

	if ( pos != string::npos ){
		LOG_F( INFO, "WILDCARD FOUND at %lu ", pos );
		string qc = query.substr( 0, pos );
		LOG_F(INFO, "query compare : %s ", qc.c_str()  );
		// TODO add support for others in data file... 
		for ( auto kv : globalHistos ){
			string oType = kv.second->ClassName();
			LOG_F( INFO, "[%s] = %s", kv.first.c_str(), oType.c_str() );
			if ( kv.first.substr( 0, pos ) == qc && typeMatch( kv.second, type ) ){
				names.push_back( kv.first );
				LOG_F( INFO, "Query MATCH: %s", kv.first.c_str() );
			}
		}

		// now try data files:
		for ( auto df : dataFiles ){
			auto objs = dirMap( df.second);
			for ( auto kv : objs ){
				LOG_F( INFO, "[%s]=%s, typeMatch=%s", kv.first.c_str(), kv.second->ClassName(), bts( typeMatch( kv.second, type ) ).c_str() );
				if ( kv.first.substr( 0, pos ) == qc && typeMatch( kv.second, type ) ){
					names.push_back( df.first + "/" + kv.first );
				}
			}
		} // loop dataFiles
		




	} else {

	}
	return names;
}

bool Rnuplot::typeMatch( TObject *obj, string type ){
	if ( nullptr == obj ) return false;
	if ( "" == type ) return true;
	string objType = obj->ClassName();
	if ( objType.substr( 0, type.length() ) == type ) return true;
	return false;
}

map<string, TObject*> Rnuplot::dirMap( TDirectory *dir, string prefix, bool dive ) {
	LOG_SCOPE_FUNCTION(INFO);

	map<string, TObject*> mp;
	
	if ( nullptr == dir ) return mp;
	
	TList* list = dir->GetListOfKeys() ;
	if ( !list ) return mp;

	TIter next(list) ;
	TKey* key ;
	TObject* obj;
	while ( (key = (TKey*)next()) ) {
		obj = key->ReadObj() ;
		string key = prefix + obj->GetName();
		mp[ key ] = obj;
		if ( dive && 0 == strcmp( "TDirectoryFile", obj->ClassName() ) ){
			auto m = dirMap( (TDirectory*)obj, key + "/" );
			mp.insert( m.begin(), m.end() );
		}
	}
	return mp;
}



std::string cmdexec(const char* cmd) {
	LOG_SCOPE_FUNCTION( INFO );
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}


void Rnuplot::exec_Latex( string _path ){
	LOG_SCOPE_FUNCTION( INFO );

	string figure_filename = config.get<string>( _path + ":from" );
	size_t lastindex = figure_filename.find_last_of("."); 
	string figure_basename = figure_filename.substr(0, lastindex);
	LOG_F( INFO, "figure (tex file) : %s", figure_filename.c_str() );
	LOG_F( INFO, "basename : %s", figure_filename.c_str() );

	if ( false == exists_test( figure_filename ) ){
		LOG_F( ERROR, "LATEX file=%s not found, cannot process. Use epslatex terminal", figure_filename.c_str() );
		return;
	}
	
	auto cmd_str = ("pdflatex " + config.get<string>( _path + ":from" )).c_str();
	LOG_F( INFO, "EXECUTE: %s", cmd_str );
	
	// run pdflatex
	cmdexec( ("pdflatex " + config.get<string>( _path + ":from" )).c_str() );


	if ( config.get<bool>( _path +":clean", true ) ){
		LOG_F( INFO, "Cleaning intermediate files. If you want to keep them (to see log etc.) use <Latex clean=\"false\" />" );
		std::remove( (figure_basename + ".aux").c_str() );
		std::remove( (figure_basename + ".log").c_str() );
		std::remove( (figure_basename + "-inc-eps-converted-to.pdf").c_str() );
		std::remove( (figure_basename + "-inc.eps").c_str() ); 
	}

}



