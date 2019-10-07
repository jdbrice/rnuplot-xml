#ifndef DATA_FILE_MAKER_H
#define DATA_FILE_MAKER_H

#include "TH3.h"
#include "TH2.h"
#include "TH1.h"

#include "gnuplot_i.hpp"

class DataFileMaker {
	public:

		DataFileMaker() {}
		~DataFileMaker() {}

		//------------------------------------------------------------------------------
		//
		// Opens a temporary file
		//
		std::string Gnuplot::create_tmpfile(std::ofstream &tmp)
		{

		#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
			char name[] = "gnuplotiXXXXXX"; //tmp file in working directory
		#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
			char name[] = "/tmp/gnuplotiXXXXXX"; // tmp file in /tmp
		#endif

			//
			// check if maximum number of temporary files reached
			//
			if (Gnuplot::tmpfile_num == GP_MAX_TMP_FILES - 1)
			{
				std::ostringstream except;
				except << "Maximum number of temporary files reached (" 
					   << GP_MAX_TMP_FILES << "): cannot open more files" << std::endl;

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
			Gnuplot::tmpfile_num++;

			return name;
		}

};

#endif