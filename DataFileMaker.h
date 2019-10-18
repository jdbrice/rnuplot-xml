#ifndef DATA_FILE_MAKER_H
#define DATA_FILE_MAKER_H

#include "TH3.h"
#include "TH2.h"
#include "TH1.h"

#include "gnuplot_i.hpp"

#include "loguru.h"

class DataFileMaker {
	public:

		DataFileMaker() {}
		~DataFileMaker() {}

		std::vector<std::string> tmpfile_list; 
		static size_t tmpfile_count_max;
		static size_t tmpfile_count;



		// master convert (handles all histogram obects)
		string convert( TH1 * h, string format, string fn = "", bool binary = false ) {

			std::ofstream tmp;
			std::string name;
			if ( fn == "" )
				name = create_tmpfile(tmp);
			else {
				tmp.open (fn, std::ofstream::out );
				name = fn;
			}
			if (name == "")
				return "";

			if ( nullptr != static_cast<TH3*>( h ) && static_cast<TH3*>( h )->GetZaxis()->GetNbins() > 1 ){
				convert_TH3( tmp, static_cast<TH3*>( h ), format );
			} else if ( nullptr != static_cast<TH2*>( h ) && static_cast<TH3*>( h )->GetYaxis()->GetNbins() > 1 ){
				convert_TH2( tmp, static_cast<TH2*>( h ), format );
			} else {
				convert_TH1( tmp, h, format );
			}


			tmp.flush();
			tmp.close();
			return name; // return the filename
		}


		void convert_TH1( std::ofstream &tmp, TH1*h, string format, bool binary = false ){
			LOG_SCOPE_FUNCTION( INFO );

			if ( "" == format ){
				format = "x y xlow xhigh ylow yhigh";
				LOG_F( INFO, "No format provided, default is: %s", format.c_str() );
			}

			vector<double> x, y, x_low, x_high, y_low, y_high;
			for ( int i = 1; i < h->GetNbinsX() + 1; i++ ){
				double vx = h->GetBinCenter( i );
				double vy = h->GetBinContent( i );
				double vey = h->GetBinError( i );

				x.push_back( vx );
				x_low.push_back( h->GetBinLowEdge(i) );
				x_high.push_back( h->GetBinLowEdge(i) + h->GetBinWidth(i) );
				y.push_back( vy );
				y_low.push_back( vy - vey );
				y_high.push_back( vy + vey );
			}

			assert( y.size() == x.size() );
			assert( x_low.size() == x.size() );
			assert( x_high.size() == x.size() );
			assert( y_low.size() == x.size() );
			assert( y_high.size() == x.size() );


			for (unsigned int i = 0; i < x.size(); i++){

				// NOW parse the format string and push data out in that order
				stringstream sstr( format );
				string token = "";
				string sep = "";
				while( std::getline(sstr, token, ' ') ){

					if ( "x" == token )
						tmp << sep << x[i];
					if ( "xlow" == token )
						tmp << sep << x_low[i];
					if ( "xhigh" == token )
						tmp << sep << x_high[i];
					if ( "y" == token )
						tmp << sep << y[i];
					if ( "ylow" == token )
						tmp << sep << y_low[i];
					if ( "yhigh" == token )
						tmp << sep << y_high[i];
					if ( "dx" == token )
						tmp << sep << (x_high[i] - x[i]);
					if ( "dy" == token )
						tmp << sep << (y_high[i] - y[i]);

					sep = " ";
				}

				tmp << std::endl;
			}

		} // convert_TH1

		void convert_TH2( std::ofstream &tmp, TH2*h, string format, bool binary = false ){
			LOG_SCOPE_FUNCTION( INFO );

			if ( "" == format ){
				format = "x y z";
				LOG_F( INFO, "No format provided, default is: %s", format.c_str() );
			}

			
			for ( int ix = 1; ix < h->GetNbinsX(); ix++ ){
				for ( int iy = 1; iy < h->GetNbinsY(); iy++ ){
					
					double vx = h->GetXaxis()->GetBinCenter( ix );
					double vy = h->GetYaxis()->GetBinCenter( iy );
					double vz = h->GetBinContent( ix, iy );
					double vez = h->GetBinError( ix, iy );
					double vxlow = h->GetXaxis()->GetBinLowEdge(ix);
					double vxhigh = vxlow + h->GetXaxis()->GetBinWidth(ix);

					double vylow = h->GetYaxis()->GetBinLowEdge(iy);
					double vyhigh = vylow + h->GetYaxis()->GetBinWidth(iy);

					// NOW parse the format string and push data out in that order
					stringstream sstr( format );
					string token = "";
					string sep = "";
					while( std::getline(sstr, token, ' ') ){

						if ( "x" == token )
							tmp << sep << vx;
						if ( "xlow" == token )
							tmp << sep << vxlow;
						if ( "xhigh" == token )
							tmp << sep << vxhigh;
						if ( "y" == token )
							tmp << sep << vy;
						if ( "ylow" == token )
							tmp << sep << vylow;
						if ( "yhigh" == token )
							tmp << sep << vyhigh;
						if ( "z" == token )
							tmp << sep << vz;
						if ( "zlow" == token )
							tmp << sep << vz-vez;
						if ( "zhigh" == token )
							tmp << sep << vz+vez;
						if ( "dx" == token )
							tmp << sep << (vxhigh - vx);
						if ( "dy" == token )
							tmp << sep << (vyhigh - vy);
						if ( "dz" == token )
							tmp << sep << (vez);

						sep = " ";
					}

					tmp << std::endl;
				}
				tmp << std::endl;
			}

			return;

		} // convert_TH2

		void convert_TH3( std::ofstream &tmp, TH3*h, string format, bool binary = false ){
			LOG_SCOPE_FUNCTION( INFO );
		}




		//------------------------------------------------------------------------------
		//
		// check if file exists
		// from gnuplot
		//
		bool file_exists(const std::string &filename, int mode)
		{
			if ( mode < 0 || mode > 7)
			{
				throw std::runtime_error("In function \"DataFileMaker::file_exists\": mode\
						has to be an integer between 0 and 7");
				return false;
			}

			// int _access(const char *path, int mode);
			//  returns 0 if the file has the given mode,
			//  it returns -1 if the named file does not exist or is not accessible in 
			//  the given mode
			// mode = 0 (F_OK) (default): checks file for existence only
			// mode = 1 (X_OK): execution permission
			// mode = 2 (W_OK): write permission
			// mode = 4 (R_OK): read permission
			// mode = 6       : read and write permission
			// mode = 7       : read, write and execution permission
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
			if (_access(filename.c_str(), mode) == 0)
		#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
			if (access(filename.c_str(), mode) == 0)
		#endif
			{
				return true;
			}
			else
			{
				return false;
			}
			return false;
		}

		bool file_available(const std::string &filename){
			std::ostringstream except;
			if( DataFileMaker::file_exists(filename,0) ) // check existence
			{
				if( !(DataFileMaker::file_exists(filename,4)) ){// check read permission
					except << "No read permission for File \"" << filename << "\"";
					throw GnuplotException( except.str() );
					return false;
				}
			}
			else{
				except << "File \"" << filename << "\" does not exist";
				throw GnuplotException( except.str() );
				return false;
			}
			return false;
		}


		//------------------------------------------------------------------------------
		//
		// Opens a temporary file
		// taken directly from gnuplot_i
		//
		std::string create_tmpfile(std::ofstream &tmp)
		{

		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
			char name[] = "gnuplotiXXXXXX"; //tmp file in working directory
		#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
			char name[] = "/tmp/gnuplotiXXXXXX"; // tmp file in /tmp
		#endif

			//
			// check if maximum number of temporary files reached
			//
			if (DataFileMaker::tmpfile_count == DataFileMaker::tmpfile_count_max - 1)
			{
				std::ostringstream except;
				except << "Maximum number of temporary files reached (" 
					   << DataFileMaker::tmpfile_count_max << "): cannot open more files" << std::endl;

				throw GnuplotException( except.str() );
				return "";
			}

			// int mkstemp(char *name);
			// shall replace the contents of the string pointed to by "name" by a unique
			// filename, and return a file descriptor for the file open for reading and 
			// writing.  Otherwise, -1 shall be returned if no suitable file could be 
			// created.  The string in template should look like a filename with six 
			// trailing 'X' s; mkstemp() replaces each 'X' with a character from the 
			// portable filename character set.  The characters are chosen such that the
			// resulting name does not duplicate the name of an existing file at the 
			// time of a call to mkstemp()


			//
			// open temporary files for output
			//
		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
			if (_mktemp(name) == NULL)
		#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
			if (mkstemp(name) == -1)
		#endif
			{
				std::ostringstream except;
				except << "Cannot create temporary file \"" << name << "\"";
				throw GnuplotException(except.str());
				return "";
			}

			tmp.open(name);
			if (tmp.bad())
			{
				std::ostringstream except;
				except << "Cannot create temporary file \"" << name << "\"";
				throw GnuplotException(except.str());
				return "";
			}

			//
			// Save the temporary filename
			//
			tmpfile_list.push_back(name);
			DataFileMaker::tmpfile_count++;

			return name;
		}

};



#endif