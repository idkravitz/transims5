//*********************************************************
//	Streams.cpp - custom stream processing
//*********************************************************

#include "Streams.hpp"
#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Message_Stream - Message
//---------------------------------------------------------

Message_Stream & Message_Stream::Message (int num_lines, const char *text) 
{	
	if (!quiet) {
		if (num_lines > 0) {
			while (num_lines-- > 0) cout << endl;
			cout << '\t';
		}
		cout << text << flush;
	}
	return (Active (!quiet));
}

//---------------------------------------------------------
//	Print_Stream - constructor
//---------------------------------------------------------

Print_Stream::Print_Stream (int max_lines, int max_width)
{
	Page_Size (max_width, max_lines);
	lines = 0;
	page = 1;
	active = true;
}

//---------------------------------------------------------
//	Open
//---------------------------------------------------------

bool Print_Stream::Open (string &report_name, bool report_flag) 
{
	if (report_flag) {
		report.open (report_name.c_str (), ios_base::out | ios_base::app);
	} else {
		report.open (report_name.c_str (), ios_base::out);
	}
	return (Open ());
}

//---------------------------------------------------------
//	Print_Stream - Print
//---------------------------------------------------------

Print_Stream & Print_Stream::Print (int num_lines, const char *text) 
{ 
	if (Open ()) {
		Page_Check (num_lines);

		//---- add the new line commands ----

		if (num_lines > 0) {
			while (num_lines-- > 0) {
				report << endl;
			}
			report << '\t';
		}

		//---- sent the message to the print file ----

		if ((int) strlen (text) > page_width) {
			report << string (text, page_width);
		} else {
			report << text;
		}
	}
	return (Active (true));
}

Print_Stream & Print_Stream::Print (int num_lines) 
{
	if (Open ()) {
		Page_Check (num_lines);

		while (num_lines-- > 0) {
			report << endl;
		}
	}
	return (Active (true));
}

//---------------------------------------------------------
//	Break_Check
//---------------------------------------------------------

bool Print_Stream::Break_Check (int num_lines)
{
	if (lines + num_lines > page_lines) {
		New_Page ();
		return (true);
	} else {
		return (false);
	}
}

//---------------------------------------------------------
//	Page_Check
//---------------------------------------------------------

bool Print_Stream::Page_Check (int new_lines) 
{ 
	lines += new_lines;

	if (lines > page_lines) {
		New_Page ();
		lines += new_lines;
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	New_Page
//---------------------------------------------------------

void Print_Stream::New_Page (void) 
{
	//---- print the system header ----

	if (Open ()) {
		page++;
		lines = 3;

		time_t ltime;
		ltime = time (&ltime);

		report << String ("\f\t%s\n\t%s  %s  page %d\n") % exe->Title () %
			exe->c_time (&ltime) % exe->Program () % page;

		//---- print the user header ----

		if (exe->Header_Number () != 0) exe->Page_Header ();
	}
}

//---------------------------------------------------------
//	Write_Stream - Write
//---------------------------------------------------------

Write_Stream & Write_Stream::Write (int num_lines, const char *text) 
{ 
	message->Message (num_lines, text);
	print->Print (num_lines, text);
	return (Active (true));
}

Write_Stream & Write_Stream::Write (int num_lines) 
{
	message->Message (num_lines);
	print->Print (num_lines);
	return (Active (true));
}

//---------------------------------------------------------
//	XML_Stream
//---------------------------------------------------------

bool XML_Stream::Open (string report_name) 
{
	if (report_name.empty ()) return (false);

	xml.open (report_name.c_str (), ios_base::out);

	if (Open ()) {
		xml << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << endl << "<TRANSIMS>";
		return (true);
	} else {
		return (false);
	}
}

void XML_Stream::Close (void)
{
	if (Open ()) {
		xml << endl << "</TRANSIMS>" << endl;
		xml.close ();
	}
}

XML_Stream & XML_Stream::XML (int num_lines, const char *text) 
{ 
	if (Open ()) {
		if (num_lines > 0) {
			while (num_lines-- > 0) {
				xml << endl;
			}
		}
		xml << text;
	}
	return (*this);
}
