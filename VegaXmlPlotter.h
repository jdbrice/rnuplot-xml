#ifndef VEGA_XML_PLOTTER_H
#define VEGA_XML_PLOTTER_H

#include "TaskRunner.h"
#include "RooPlotLib.h"
#include "HistoBook.h"
#include "XmlPad.h"
#include "XmlCanvas.h"

using namespace jdb;

#include <map>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>

using namespace std;

#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TChain.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TApplication.h"
#include "TColor.h"
#include "TDirectory.h"
#include "TImage.h"

#include "gnuplot_i.hpp"





class VegaXmlPlotter : public TaskRunner
{
protected:

	bool initializedGROOT = false;

	typedef void (VegaXmlPlotter::*MFP)(string);
	std::map <string, MFP> handle_map;


	map<string, TH1*> histos;
	map<string, TGraph*> graphs;
	map<string, TF1*> funcs;
	TH1 * current_frame = nullptr;

	XmlCanvas * xcanvas;

	map<string, string> gnudf;
	Gnuplot gnuplot;
	size_t nplots;
	string current_out;

public:

	virtual const char* classname() const { return "VegaXmlPlotter"; }
	VegaXmlPlotter() {}
	~VegaXmlPlotter() {}

	virtual void init();
	virtual void make();

	// vector<shared_ptr<Gnuplot>> gplots;

	virtual void exec_node( string _path );
	virtual void exec_children( string _path );
	virtual void exec_children( string _path, string tag_type );
	virtual void exec_Loop( string _path );
	// virtual void exec_TCanvas( string _path );
	virtual void exec_Command( string _path );
	virtual void exec_TFile( string _path );
	virtual void exec_TFileOut( string _path );
	virtual void exec_Data( string _path );
	virtual void exec_Equation( string _path );
	virtual void exec_Plot( string _path );
	virtual void exec_Histogram( string _path );
	virtual void exec_Legend( string _path );
	virtual void exec_Latex( string _path );

	virtual void exec_Gnuplot( string _path );

	bool replace(std::string& str, const std::string& from, const std::string& to) {
				size_t start_pos = str.find(from);
				if(start_pos == std::string::npos)
								return false;
				str.replace(start_pos, from.length(), to);
				return true;
	}


	// gnu plot...
	void plot_Equation( string _path );
	// virtual void exec_Pad( string _path );
	// virtual void exec_Axes( string _path );
	// virtual void exec_Export( string _path );
	// virtual void exec_ExportConfig( string _path );
	// virtual void exec_StatBox( string _path );
	
	// virtual void exec_Graph( string _path );
	// virtual void exec_TF1( string _path );
	// virtual void exec_TLine( string _path );
	// virtual void exec_Rect( string _path );
	// virtual void exec_Ellipse( string _path );
	// virtual void exec_TLatex( string _path );
	// virtual void exec_TLegend( string _path );
	// virtual void exec_Canvas( string _path );
	// virtual void exec_Clear( string _path );
	// virtual void exec_Margins( string _path );
	// virtual void exec_TFile( string _path );
	// virtual void exec_Script( string _path );
	// virtual void exec_Palette( string _path );
	// virtual void exec_ColorAxis( string _path );

	// Transforms
	virtual void exec_Transforms( string _path );
	virtual void exec_transform_Projection( string _path );
	virtual void exec_transform_ProjectionX( string _path);
	virtual void exec_transform_ProjectionY( string _path);
	virtual void exec_transform_FitSlices( string _path);
	virtual void exec_transform_MultiAdd( string _path);
	virtual void exec_transform_Add( string _path);
	virtual void exec_transform_Divide( string _path);
	virtual void exec_transform_Difference( string _path);
	virtual void exec_transform_Rebin( string _path);
	virtual void exec_transform_Scale( string _path);
	virtual void exec_transform_Normalize( string _path);
	virtual void exec_transform_Draw( string _path);
	virtual void exec_transform_Clone( string _path );
	virtual void exec_transform_Smooth( string _path );
	virtual void exec_transform_CDF( string _path );
	virtual void exec_transform_Style( string _path );
	virtual void exec_transform_SetBinError( string _path );
	virtual void exec_transform_BinLabels( string _path );
	virtual void exec_transform_Sumw2( string _path );
	virtual void exec_transform_ProcessLine( string _path );
	virtual void exec_transform_Assign( string _path );
	virtual void exec_transform_Format( string _path );
	virtual void exec_transform_Print( string _path );
	virtual void exec_transform_Proof( string _path );
	virtual void exec_transform_List( string _path );


	virtual bool exec( string tag, string _path ){
		bool e = handle_map.count( tag ) > 0;
		if ( false == e ) return false;
		// LOG_F( INFO, "exec( %s @ %s )", tag.c_str(), _path.c_str() );
		MFP fp = handle_map[ tag ];
		(this->*fp)( _path );
		return true;
	}


	string random_string( size_t length );

	// shared_ptr<HistoBook> book;
	map<string, TFile*> dataFiles;
	map<string, TChain *> dataChains;
	map<string, TH1 * > globalHistos;
	map<string, TGraph * > globalGraphs;

	// bool initializedGROOT = false;

	TFile * dataOut = nullptr;
	virtual void loadDataFile( string _path );
	virtual int numberOfData();
	virtual void loadChain( string _path );
	virtual void loadData();

	virtual TObject* findObject( string _data );
	virtual TH1* findHistogram( string _data, string _name, string _path ="", int iHist=-1 );
	virtual TH1* findHistogram( string _path, int iHist, string _mod="" );
	virtual TH1* makeHistoFromDataTree( string _path, int iHist );
	// virtual void positionOptStats( string _path, TPaveStats * st );

	// virtual TCanvas* makeCanvas( string _path );

	int color( string _color ){
		if ( _color[0] == '#' && _color.size() == 7 ){
			return TColor::GetColor( _color.c_str() );
		} else if ( _color[0] == '#' && _color.size() == 4 ){
			string colstr = "#";
			colstr += _color[1];
			colstr += _color[1];
			colstr += _color[2];
			colstr += _color[2];
			colstr += _color[3];
			colstr += _color[3];
			return TColor::GetColor( colstr.c_str() );
		}
		return atoi( _color.c_str() );
	}

	string nameOnly( string fqn ){
		if ( fqn.find( "/" ) != string::npos )
			return fqn.substr( fqn.find( "/" ) + 1 );
		else 
			return fqn;
	} 
	string dataOnly( string fqn ){
		if ( fqn.find( "/" ) != string::npos )
			return fqn.substr( 0, fqn.find( "/" ) );
		else 
			return "";
	} 
	string fullyQualifiedName( string _data, string _name ){
		if ( "" == _data )
			return _name;
		return _data + "/" + _name;
	}


	bool typeMatch( TObject *obj, string type );
	vector<string> glob( string query );
	map<string, TObject*> dirMap( TDirectory *dir, string prefix ="", bool dive = true );

	int getProjectionBin( string _path, TH1 * _h, string _axis="x", string _n="1", int _def = -1 ){
		LOG_F( 1, "getProjectionBin( _path=%s, axis=%s, n=%s, default=%d", _path.c_str(), _axis.c_str(), _n.c_str(), _def );
		if ( nullptr == _h ) return 1;

		TAxis *axis = _h->GetXaxis();
		if ( "y" == _axis || "Y" == _axis ) axis = _h->GetYaxis();
		if ( "z" == _axis || "Z" == _axis ) axis = _h->GetZaxis();

		string binAttr = _path + ":" + _axis + "b" + _n;
		string valAttr = _path + ":" + _axis + _n;


		int b = config.getInt( binAttr, _def );
		LOG_F( 1, "Checking for bin value @ %s = %d (default=%d)", binAttr.c_str(), b, _def );
		if ( config.exists( valAttr ) ){
			double v = config.getDouble( valAttr, _def );
			b = axis->FindBin( v );
			LOG_F( 1, "Checking for user value @ %s = %f, bin=%d (default=%d)", valAttr.c_str(), v, b, _def );
		} else {
			LOG_F( 1, "bin values DNE @ %s", valAttr.c_str() );
		}
		return b;
	}

	bool axesRangeGiven( string _path, string _axis ){
		string n = "1";
		string binAttr = _path + ":" + _axis + "b" + n;
		string valAttr = _path + ":" + _axis + n;

		if ( false == config.exists( binAttr ) && false == config.exists( valAttr ) )
			return false;
		n = "2";
		binAttr = _path + ":" + _axis + "b" + n;
		valAttr = _path + ":" + _axis + n;
		if ( false == config.exists( binAttr ) && false == config.exists( valAttr ) )
			return false;
		return true;
	}

	string underscape( string in ){
		std::replace( in.begin(), in.end(), '/', '_' );
		return in;
	}

protected:
	
};



#endif