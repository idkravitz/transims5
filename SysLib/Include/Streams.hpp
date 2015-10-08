//*********************************************************
//	Streams.hpp - custom stream processing
//*********************************************************

#ifndef STREAMS_HPP
#define STREAMS_HPP

#include "APIDefs.hpp"

#include <time.h>

#include <string>
#include <fstream>
#include <iostream>
using namespace std;

//---------------------------------------------------------
//	Message_Stream - output message processing
//---------------------------------------------------------

class SYSLIB_API Message_Stream
{
public:
	Message_Stream (bool quiet = false)              { Quiet (quiet); }

	Message_Stream & Message (int num_lines, const char *text);
	
	Message_Stream & Message (const char *text)      { if (!quiet) cout << "\n\t" << text << flush; return (Active (!quiet)); }
	Message_Stream & Message (int num_lines)         { if (!quiet) while (num_lines-- > 0) cout << endl; return (Active (!quiet)); }

	Message_Stream & operator<< (int right)          { if (active) cout << right << flush; return (*this); } 
	Message_Stream & operator<< (short right)        { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (double right)       { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (bool right)         { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (char right)         { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (const char *right)  { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (size_t right)       { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (void *right)        { if (active) cout << right << flush; return (*this); }
	Message_Stream & operator<< (string right)       { if (active) cout << right << flush; return (*this); }

	bool Quiet (void)                                { return (quiet); }
	void Quiet (bool flag)                           { quiet = flag; }
	
	bool Active (void)                               { return (active); }
	Message_Stream & Active (bool flag)              { active = flag; return (*this); }

private:
	bool quiet, active;
};

//---------------------------------------------------------
//	Print_Stream - print output processing
//---------------------------------------------------------

class SYSLIB_API Print_Stream
{
public:
	Print_Stream (int lines = 65, int width = 95);

	Print_Stream & Print (int num_lines, const char *text);
	Print_Stream & Print (int num_lines);

	Print_Stream & operator<< (int right)          { if (Open ()) report << right; return (*this); } 
	Print_Stream & operator<< (short right)        { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (double right)       { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (bool right)         { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (char right)         { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (char *right)        { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (const char *right)  { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (size_t right)       { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (void *right)        { if (Open ()) report << right; return (*this); }
	Print_Stream & operator<< (string right)       { if (Open ()) report << right; return (*this); }

	bool Open (string &report_name, bool report_flag);
	bool Open (void)                               { return (report.is_open () && active); }
	void Close (void)                              { report.close (); }

	void Page_Size (int width, int lines)          { page_width = width; page_lines = (lines > 0) ? lines : 0x7FFFFFFF; }
	int  Page_Lines (void)                         { return (page_lines); }
	int  Page_Width (void)                         { return (page_width); }

	bool Break_Check (int num_lines);
	bool Page_Check (int new_lines);
	void New_Page (void);
	void Flush (void)                              { if (Open ()) report << flush; }
	
	bool Active (void)                             { return (active); }
	Print_Stream & Active (bool flag)              { active = flag; return (*this); }

	ofstream * Report (void)                       { return ((Open ()) ? &report : 0); }

private:
	ofstream report;

	bool active;
	int lines, page, page_width, page_lines;
};

//---------------------------------------------------------
//	Write_Stream - write output processing
//---------------------------------------------------------

class SYSLIB_API Write_Stream
{
public:
	Write_Stream (void)                            { message = 0; print = 0; active = true; }

	void Initialize (Message_Stream *msg, Print_Stream *prn)
	                                               { message = msg; print = prn; }

	Write_Stream & Write (int num_lines, const char *text);
	Write_Stream & Write (int num_lines);

	Write_Stream & operator<< (int right)          { if (Ok ()) { *message << right; *print << right; } return (*this); } 
	Write_Stream & operator<< (short right)        { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (double right)       { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (bool right)         { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (char right)         { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (char *right)        { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (const char *right)  { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (size_t right)       { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (void *right)        { if (Ok ()) { *message << right; *print << right; } return (*this); }
	Write_Stream & operator<< (string right)       { if (Ok ()) { *message << right; *print << right; } return (*this); }
	
	bool Active (void)                             { return (active); }
	Write_Stream & Active (bool flag)              { active = flag; return (*this); }

private:
	bool Ok (void)                                 { return (message != 0 && print != 0 && active); }

	bool active;
	Message_Stream *message;
	Print_Stream *print;
};

//---------------------------------------------------------
//	XML_Stream - xml output processing
//---------------------------------------------------------

class SYSLIB_API XML_Stream
{
public:
	XML_Stream (void)                            {}
	~XML_Stream (void)                           { Close (); }

	XML_Stream & XML (int num_lines, const char *text);

	XML_Stream & operator<< (int right)          { if (Open ()) xml << right; return (*this); } 
	XML_Stream & operator<< (short right)        { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (double right)       { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (bool right)         { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (char right)         { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (char *right)        { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (const char *right)  { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (size_t right)       { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (void *right)        { if (Open ()) xml << right; return (*this); }
	XML_Stream & operator<< (string right)       { if (Open ()) xml << right; return (*this); }

	bool Open (string report_name);
	bool Open (void)                             { return (xml.is_open ()); }
	void Close (void);
	void Flush (void)                            { if (Open ()) xml << flush; }

	ofstream * XML (void)                        { return ((Open ()) ? &xml : 0); }

private:
	ofstream xml;
};

#endif
