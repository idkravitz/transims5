//*********************************************************
//	FileFormat.cpp - convert files to a new format
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	FileFormat constructor
//---------------------------------------------------------

FileFormat::FileFormat (void) : Execution_Service ()
{
	Program ("FileFormat");
	Version (1);
	Title ("File Format Conversion");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DATA_FILE, "DATA_FILE", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_DATA_FILE, "NEW_DATA_FILE", LEVEL1, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DATA_FORMAT, "NEW_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL1, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ NEW_FILE_HEADER, "NEW_FILE_HEADER", LEVEL1, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ NEW_DATA_FIELD, "NEW_DATA_FIELD", LEVEL2, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Key_List (keys);
	Report_List (reports);

	num_files = 0;
	file = new_file = 0;
	script_flag = false;
}

//---------------------------------------------------------
//	FileFormat destructor
//---------------------------------------------------------

FileFormat::~FileFormat (void)
{
	if (num_files > 0) {
		for (int i=0; i < num_files; i++) {
			if (file [i] != 0) {
				delete file [i];
			}
			if (new_file [i] != 0) {
				delete new_file [i];
			}
		}
		delete [] file;
		delete [] new_file;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	FileFormat *exe = new FileFormat ();

	return (exe->Start_Execution (commands, control));
}

