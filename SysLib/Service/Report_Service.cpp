//*********************************************************
//	Report_Service.cpp - general reporting services
//*********************************************************

#include "Report_Service.hpp"

#include "Execution_Service.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Report_Service - constructor
//---------------------------------------------------------

Report_Service::Report_Service (void) : Message_Service ()
{
	header = max_problems = 0;
	first_part = last_part = -1;
	parameter_flag = false;
	partition_flag = false;
	xml_flag = xml_help = false;
	write.Initialize (&message, &print);
	Reset_Problems ();
}

//---------------------------------------------------------
//	Open_Report
//---------------------------------------------------------

void Report_Service::Open_Report (string report_dir, string report_name, bool report_flag) 
{
	size_t index;

	//---- construct defaule report file name ----

	if (report_name.empty ()) {
		report_name = Control_File ();

		index = report_name.find ('.', report_name.length () - 4);

		if (index != report_name.npos) {
			report_name.erase (index);
		}

		if (first_part >= 0) {
			report_name += String ("_%d") % first_part;
			if (last_part > first_part && last_part < 1000) {
				report_name += String ("-%d") % last_part;
			}
		} else if (parameter_flag && !parameter.empty ()) {
			index = parameter.find ('.');

			if (index != parameter.npos) {
				string param = parameter.substr (0, index);

				index = parameter.find_last_of ('.');
				param += "-" + parameter.substr (index + 1);
				report_name += "_" + param;
			} else {
				report_name += "_" + parameter;
			}
		} else if (Slave ()) {
			report_name += (String ("_%d") % MPI_Rank ());
		}
		report_name += ".prn";

	} else if (first_part >= 0) {
		index = report_name.find ('.', report_name.length () - 4);

		if (index != report_name.npos) {
			if (last_part > first_part && last_part < 1000) {
				report_name.insert (index, String ("_%d-%d") % first_part % last_part);
			} else {
				report_name.insert (index, String ("_%d") % first_part);
			}
		}
	} else if (parameter_flag && !parameter.empty ()) {
		string param ("_");

		index = parameter.find ('.');

		if (index != parameter.npos) {
			param += parameter.substr (0, index);

			index = parameter.find_last_of ('.');
			param += "-" + parameter.substr (index + 1);
		} else {
			param += parameter;
		}
		index = report_name.find ('.', report_name.length () - 4);

		if (index != report_name.npos) {
			report_name.insert (index, param);
		} else {
			report_name += param;
		}
	} else if (Slave ()) {
		report_name += (String ("_%d") % MPI_Rank ());
	}

	//---- open report file ----

	report_dir = Replace_Directory (report_dir, report_name);

	if (!print.Open (report_dir, report_flag)) {
		File_Error ("Opening Report File", report_dir);
	}

	//---- print the banner ----

	Page_Check (12);

	Banner (print.Report ());

	Write (1, "Control File = ") << Control_File ();

	if (Single_Partition ()) {
		Show_Message (2, "Partition Number = ") << First_Partition ();
	} else if (Partition_Range ()) {
		Show_Message (2, "Partition Range = ") << First_Partition () << ".." << Last_Partition ();
	}
	Show_Message (1);
	Print (1, "Report File  = ") << report_dir << ((report_flag) ? " (Append)" : " (Create)"); 
	Print (2, Title ());
	Print (1);
	Flush ();
}

//---------------------------------------------------------
//	Open_XML
//---------------------------------------------------------

bool Report_Service::Open_XML (string report_dir, string report_name) 
{
	if (xml.Open ()) return (true);

	if (report_name.empty ()) {
		report_name = Control_File ();

		if (report_name.empty ()) {
			report_name = Program ();
		}
	}
	size_t index = report_name.find ('.', report_name.length () - 4);

	if (index != report_name.npos) {
		report_name.erase (index);
	}
	if (first_part >= 0) {
		report_name += String ("_%d") % first_part;
		if (last_part > first_part && last_part < 1000) {
			report_name += String ("-%d") % last_part;
		}
	} else if (parameter_flag && !parameter.empty ()) {
		index = parameter.find ('.');

		if (index != parameter.npos) {
			string param = parameter.substr (0, index);

			index = parameter.find_last_of ('.');
			param += "-" + parameter.substr (index + 1);
			report_name += "_" + param;
		} else {
			report_name += "_" + parameter;
		}
	}
	report_name += ".xml";

	report_dir = Replace_Directory (report_dir, report_name);

	return (xml.Open (report_dir));
}

//---------------------------------------------------------
//	Exit_Stat
//---------------------------------------------------------

void Report_Service::Exit_Stat (Exit_Code stat) 
{
	if (XML_Flag ()) {
		xml.Close ();
	}
	if (print.Open ()) {
		if (stat == CANCEL) {
			Write_Time (2, "Process Terminated by the User");
		} else if (stat == FATAL) {
			Write_Time (2, "Process Terminated Due to Errors");
		} else if (stat == WARNING) {
			Write_Time (2, String ("Process Terminated with %d Warnin%s") % Warnings () % ((Warnings () > 1) ? "gs" : "g"));
		} else if (Warnings () > 0) {
			stat = WARNING;
			Write_Time (2, String ("Process Complete with %d Warnin%s") % Warnings () % ((Warnings () > 1) ? "gs" : "g"));
		} else {
			Write_Time (2, "Process Complete");	
		}
		Print (1);
		print.Close ();
	}
	Message_Service::Exit_Stat (stat);
}

//---------------------------------------------------------
//	Error
//---------------------------------------------------------

void Report_Service::Error (const char *text) 
{ 
	if (!text || text [0] == '\0') Exit_Stat (FATAL);

	//---- send the message to the report file ----

	if (print.Open ()) {
		Page_Check (3);
		time_t ltime;
		time (&ltime);

		Print (0, "\n\n\t") << c_time (&ltime);
		Print (0, "\n\tError: ") << text;
		Flush ();
	}

	//---- send the message to the screen ----

	Show_Error (text);
}

void Report_Service::Mem_Error (const char *type)
{
	Error (String ("Insufficient Memory for %s Data") % type); 
}

//---------------------------------------------------------
//	Warning
//---------------------------------------------------------

Write_Stream & Report_Service::Warning (const char *text) 
{ 
	if (!text || text [0] == '\0') Exit_Stat (FATAL);

	//---- check the warning message count ----

	Warnings (Warnings () + 1);

	if (Warnings () > Max_Warnings ()) {
		if (Warning_Flag ()) {
			Error (String ("Maximum Warning Messages %d Exceeded") % Max_Warnings ());
		}
		return (Active (false));
	}
	return (Write (1, "Warning: ") << text);
}

//---------------------------------------------------------
//	Print_Filename
//---------------------------------------------------------

void Report_Service::Print_Filename (int num_lines, string label, string filename) 
{
	if (label.empty () || filename.empty () || !print.Open ()) return;

	size_t len, max_len;
	string text;

	len = filename.length ();
	max_len = (int) (Page_Width () - label.length () - 14);

	if (len > max_len && max_len > 0) {
		text = filename.substr (len - max_len);

		text [0] = text [1] = text [2] = '.';
	} else {
		text = filename;
	}
	Print (num_lines, label) << " = " << text;
}

//---------------------------------------------------------
//	File_Error
//---------------------------------------------------------

void Report_Service::File_Error (string label, string text) 
{ 
	if (label.empty () || text.empty ()) {
		Exit_Stat (FATAL);
	}
	Print (2, "Error: ") << label;
	Print (1, "\t\"") << text << "\"";

	Show_Error (String ("%s\n\t\t\"%s\"") % label % text);
}

//---------------------------------------------------------
//	Write_Time
//---------------------------------------------------------

void Report_Service::Write_Time (int num_lines, const char *text)
{
	if (!text || text [0] == '\0') Exit_Stat (FATAL);

	time_t ltime;
	time (&ltime);

	int duration = (int) (ltime - StartTime ());
	int hour = duration / 3600;
	int second = duration % 3600;
	int minute = second / 60;
	second = second % 60;
	
	Write (num_lines, String ("%s -- %s (%d:%02d:%02d)") % c_time (&ltime) % text % hour % minute % second);
	Flush ();
}

//---------------------------------------------------------
//      Fix_Directory
//---------------------------------------------------------

string& Report_Service::Fix_Directory (string &dir)
{
	size_t len = dir.length () - 1;

	if (len > 0) {
#ifdef _WIN32
		if (dir [len] != '\\' && dir [len] != '/') {
			dir += '\\';
		}
#else
		if (dir [len] != '/') {
			dir += "/";
		}
#endif
	}
	return (dir);
}

//---------------------------------------------------------
//      Replace_Directory
//---------------------------------------------------------

string Report_Service::Replace_Directory (string dir, string filename)
{
	if (dir.empty ()) return (filename);
	dir = Fix_Directory (dir);

	size_t index = filename.find_last_of ("\\/");

	if (index != string::npos) {
		filename = filename.substr (index+1);
	}
	return (dir + filename);
}

//---------------------------------------------------------
//	Set_Problem
//---------------------------------------------------------

int Report_Service::Set_Problem (Problem_Type problem)
{
	if (problem > TOTAL_PROBLEM && problem < MAX_PROBLEM) {
		num_problems [problem]++;
		num_problems [0]++;
	}
	if ((num_problems [0] > max_problems && max_problems > 0) || problem == MAX_PROBLEM) {
		Report_Problems ();
		Error ("Maximum Number of Problems Exceeded");
	}
	return (problem);
}

//---------------------------------------------------------
//	Report_Problems
//---------------------------------------------------------

void Report_Service::Report_Problems (int total_records, bool show_flag)
{
	int i, num;
	double percent;
	String text;

	if (num_problems [0] == 0) return;

	percent = 100.0 / num_problems [0];

	for (i=1, num=2; i < MAX_PROBLEM; i++) {
		if (num_problems [i] > 0) num++;
	}
	Break_Check (num);
	
	text ("Total Number of Problems = %d") % num_problems [0];
	if (total_records > 0) {
		text += String (" (%.1lf%%)") % (100.0 * num_problems [0] / total_records) % FINISH;
	}
	if (show_flag) {
		Write (2, text);
	} else {
		Print (2, text);
	}
	for (i=1; i < MAX_PROBLEM; i++) {
		num = num_problems [i];

		if (num > 0) {
			text ("Number of %s (#%d) Problems = %d (%.1lf%%)") % 
				Problem_Code ((Problem_Type) i) % i % num % (num * percent) % FINISH;

			if (show_flag) {
				Write (1, text);
			} else {
				Print (1, text);
			}
		}
	}
}

//---------------------------------------------------------
//	Add_Problems
//---------------------------------------------------------

void Report_Service::Add_Problems (int *problem_array)
{
	for (int i=0; i < MAX_PROBLEM; i++) {
		num_problems [i] += problem_array [i];
	}
}

//---------------------------------------------------------
//	Partition_Index
//---------------------------------------------------------

int Report_Service::Partition_Index (int number)
{
	if (Partition_Range ()) {
		if (number < first_part || number > last_part) return (-1);
		number -= first_part;
	} else if (Single_Partition ()) {
		if (number != first_part) return (-1);
		number = 0;
	}
	return (number);
}

//---------------------------------------------------------
//	Partition_Number
//---------------------------------------------------------

int Report_Service::Partition_Number (int index)
{
	if (Partition_Range ()) {
		index += first_part;
		if (index > last_part) return (-1);
	} else if (Single_Partition ()) {
		index += first_part;
	}
	return (index);
}

