 //*********************************************************
//	Execution_Service.cpp - general execution class
//*********************************************************

#include "Execution_Service.hpp"

#include "String.hpp"
#include "Db_File.hpp"

#include <new>
using namespace std;

Execution_Service *exe = 0;

//-----------------------------------------------------------
//	Insufficient_Memory
//-----------------------------------------------------------

void Insufficient_Memory (void)
{
	exe->Show_Error (String ("%s: Insufficient Memory") % exe->Program ());
	throw bad_alloc ();
}

//---------------------------------------------------------
//	Execution_Service - constructor
//---------------------------------------------------------

Execution_Service::Execution_Service (void) : Control_Service ()
{
	Control_Key exe_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TITLE, "TITLE", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ REPORT_DIRECTORY, "REPORT_DIRECTORY", LEVEL0, OPT_KEY, PATH_KEY, "", "", NO_HELP },
		{ REPORT_FILE, "REPORT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", "[report_directory]filename[_partition][.prn]", NO_HELP },
		{ REPORT_FLAG, "REPORT_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PAGE_LENGTH, "PAGE_LENGTH", LEVEL0, OPT_KEY, INT_KEY, "65", ">= 0", NO_HELP },
		{ PROJECT_DIRECTORY, "PROJECT_DIRECTORY", LEVEL0, OPT_KEY, PATH_KEY, "", "", NO_HELP },
		{ DEFAULT_FILE_FORMAT, "DEFAULT_FILE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TIME_OF_DAY_FORMAT, "TIME_OF_DAY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ MODEL_START_TIME, "MODEL_START_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0:00", ">= 0 [seconds], 0.0 [hours], 0:00", NO_HELP },
		{ MODEL_END_TIME, "MODEL_END_TIME", LEVEL0, OPT_KEY, TIME_KEY, "24:00", "> [model_start_time]", NO_HELP },
		{ MODEL_TIME_INCREMENT, "MODEL_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
		{ UNITS_OF_MEASURE, "UNITS_OF_MEASURE", LEVEL0, OPT_KEY, TEXT_KEY, "METRIC", "METRIC, ENGLISH", NO_HELP },
		{ RANDOM_NUMBER_SEED, "RANDOM_NUMBER_SEED", LEVEL0, OPT_KEY, INT_KEY, "0", SEED_RANGE, NO_HELP },
		{ MAX_WARNING_MESSAGES, "MAX_WARNING_MESSAGES", LEVEL0, OPT_KEY, INT_KEY, "100000", ">= 0", NO_HELP },
		{ MAX_WARNING_EXIT_FLAG, "MAX_WARNING_EXIT_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ MAX_PROBLEM_COUNT, "MAX_PROBLEM_COUNT", LEVEL0, OPT_KEY, INT_KEY, "0", ">= 0", NO_HELP },
		{ NUMBER_OF_THREADS, "NUMBER_OF_THREADS", LEVEL0, OPT_KEY, INT_KEY, "1", "1..64", NO_HELP },
		END_CONTROL
	};

	Key_List (exe_keys);
	Service_Level (EXECUTION_SERVICE);

	random_seed = 0;
	num_threads = 1;
	thread_flag = false;
	report_flag = false;
	mpi_flag = false;
	debug = false;

	report_name.reserve (10);
	report_number.reserve (10);

	start_time = 0;
	end_time = Round (MIDNIGHT);

	Notes_Name_Flag (false);
	
	exe = this;
}

//-----------------------------------------------------------
//	Key_List - set the control key list
//-----------------------------------------------------------

void Execution_Service::Key_List (Control_Key keys []) 
{
	for (int i=0; keys [i].code != 0; i++) {
		Add_Control_Key (keys [i]);
	}
}

//---------------------------------------------------------
//	Ignore_Keys - set keys to ignore
//---------------------------------------------------------

void Execution_Service::Ignore_Keys (int *keys)
{
	if (keys == 0) return;

	for (int i=0; keys [i] != 0; i++) {
		Ignore_Control_Key (keys [i]);
	}
}

//-----------------------------------------------------------
//	Start_Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Start_Execution (int commands, char *command []) 
{
	Program_Status (RUNNING);

#ifdef MAKE_MPI
	int value;
	MPI_Init (&commands, &command);
	MPI_Comm_rank (MPI_COMM_WORLD, &value);
	MPI_Rank (value);
	MPI_Comm_size (MPI_COMM_WORLD, &value);
	MPI_Size (value);
#elif defined (BOOST_MPI)
	mpi::environment env (commands, command);
	MPI_Rank (world.rank ());
	MPI_Size (world.size ());
#endif
	if (Slave ()) {
		if (!Enable_MPI ()) Exit_Stat (DONE);
		Quiet (true);
#ifdef DEBUG_KEYS
		Active (true);
#else
		Active (false);
#endif
	}
	if (Command_Line (commands, command)) {
		if (Master () && !Enable_MPI () && MPI_Size () > 0) {
			String message (Program ());
			message += " is Not MPI Enabled";
			Warning (message);
			Write (1);
		}
		if (Master () && Enable_MPI () && MPI_Size () < 2) {
			Error ("MPI Version Requires at least Two Processors");
			Exit_Stat (FATAL);
		}
		if (report_flag) Read_Reports ();

		set_new_handler (Insufficient_Memory);

		Program_Control ();

		if (Master ()) {
			if (report_flag) {
				List_Reports ();
			} else if (!Highest_Control_Group (PROGRAM_REPORTS, 0)) {
				Print (1);
				Flush ();
			}
		}
		Execute ();
	}
	return (Return_Code ());
}

//-----------------------------------------------------------
//	Start_Execution - run the program 
//-----------------------------------------------------------

int Execution_Service::Start_Execution (Strings keys) 
{
	Str_Itr key_itr;
	char *config_file;

	Program_Status (RUNNING);
	
	Show_Banner ();

	//---- check for a config file ----

	config_file = get_env ("TRANSIMS_CONFIG_FILE");

	if (config_file != 0) {
		if (!Read_Control_File (config_file)) {
			return (Return_Code ());
		}
	}

	//---- process the control keys ----
	
	for (key_itr = keys.begin (); key_itr != keys.end (); key_itr++) {
		Process_Control_String (key_itr->c_str ());
	}

	//---- open the output report ----

	Title (Get_Control_String (TITLE));

	Open_Report (Get_Control_String (REPORT_DIRECTORY), Get_Control_String (REPORT_FILE), Set_Control_Flag (REPORT_FILE));
	
	if (report_flag) Read_Reports ();
		
	set_new_handler (Insufficient_Memory);

	//---- start processing ----

	Program_Control ();

	if (report_flag) {
		List_Reports ();
	} else if (!Highest_Control_Group (PROGRAM_REPORTS, 0)) {
		Print (1);
		Flush ();
	}
	Execute ();

	return (Return_Code ());
}

//---------------------------------------------------------
//	Command_Line
//---------------------------------------------------------

bool Execution_Service::Command_Line (int commands, char *command [])
{
	bool banner_flag, syntax_flag, help_flag, command_flag, key_flag, param_flag, control_flag, exit_flag, doc_flag, user_flag;
	String message;
	char *config_file;

	banner_flag = syntax_flag = help_flag = command_flag = key_flag = param_flag = control_flag = exit_flag = doc_flag = user_flag = false;

	if (commands < 2) {
		Show_Banner ();
		Show_Syntax ();
		banner_flag = syntax_flag = true;

		Control_File (Get_Control_File ());
		if (Control_File ().empty ()) return (false);

	} else {

		//---- process the command line -----

		for (int i=1; i < commands; i++) {
			if (command [i][0] == '-') {

				if (command [i][1] == 'Q' || command [i][1] == 'q') {
					if (Master ()) Quiet (true);
				} else if (command [i][1] == 'H' || command [i][1] == 'h') {
					if (Master ()) Syntax_Help ();
					help_flag = syntax_flag = banner_flag = true;
				} else if (command [i][1] == 'P' || command [i][1] == 'p') {
					if (Master ()) Pause (true);
				} else if (command [i][1] == 'N' || command [i][1] == 'n') {
					if (Master ()) No_Pause (true);
				} else if (command [i][1] == 'D' || command [i][1] == 'd' ||
					command [i][1] == 'B' || command [i][1] == 'b') {	//---- version 4.0 compatibility ----
					if (Master ()) Detail (true);
				} else if (command [i][1] == 'K' || command [i][1] == 'k') {
					if (Master ()) key_flag = true;
				} else if (command [i][1] == 'X' || command [i][1] == 'x') {
					if (Master ()) {
						XML_Flag (true);
						if (command [i][2] == 'H' || command [i][2] == 'h') {
							XML_Help_Flag (true);
						}
					}
				} else if (command [i][1] == 'C' || command [i][1] == 'c') {
					if (Master ()) {
						control_flag = true;
						if (command [i][2] == 'X' || command [i][2] == 'x') {
							exit_flag = true;
						}
					}
				} else if (command [i][1] == 'R' || command [i][1] == 'r') {
					if (Master ()) {
						doc_flag = true;
						if (command [i][2] == 'U' || command [i][2] == 'u') {
							user_flag = true;
						}
					}
				} else {
					message ("Unrecognized Command Line Option = ");
					message += command [i];
					goto error_message;
				}
			} else if (Control_File ().empty ()) {
				Control_File (message (command [i]));
			} else if (Enable_Partitions ()) {
				if (First_Partition () >= 0) {
					message ("Too Many Partition Parameters");
					goto error_message;
				}
				int low, high;
				message = command [i];
				message.Range (low, high);
				if (low < 0 || low > high || high > 999) {
					if (low == high) {
						message ("Partition Number %d is Out of Range (0..999)") % low;
					} else {
						message ("Partition Range %d..%d is Out of Range (0..999)") % low % high;
					}
				}
				First_Partition (low); 
				Last_Partition (high);
			} else if (Enable_Parameter ()) {
				if (!Parameter ().empty ()) {
					message ("Too Many Parameters");
					goto error_message;
				} else {
					Parameter (command [i]);
					param_flag = true;
				}
			} else {
				command_flag = true;
			}
		}
		if (Control_File ().empty ()) {
			if (XML_Flag ()) XML_Open ();
			if (control_flag) Write_Control_File ();
			if (doc_flag) Document (user_flag);
			if (help_flag) {
				if (command_flag) {
					message = Program ();
					message += " has Too Many Parameters";
					Show_Warning (message);
				}
				Pause (true);
				Exit_Stat (DONE);
				return (false);
			} else if (XML_Flag () || control_flag || doc_flag) {
				Exit_Stat (DONE);
				return (false);
			}
			message ("No Control File");
			goto error_message;
		}
		if (!banner_flag) {
			Show_Banner ();
			banner_flag = true;
		}
	}

	//---- check for a config file ----

	config_file = get_env ("TRANSIMS_CONFIG_FILE");

	if (config_file != 0) {
		if (!Read_Control_File (config_file)) {
			message ("Reading Configuration File: \"%s\"") % config_file;
			goto error_message;
		}
	}

	//---- read the control file ----

	if (!Read_Control_File (Control_File ())) {
		if (control_flag) {
			Write_Control_File ();
			Exit_Stat (DONE);
			return (false);
		} else {
			message ("Reading Control File: \"%s\"") % Control_File ();
			goto error_message;
		}
	}
	if (control_flag) Write_Control_File ();
	if (XML_Flag ()) XML_Open ();
	if (doc_flag) Document (user_flag);

	if (exit_flag) {
		Exit_Stat (DONE);
		return (false);
	}

	//---- open the output report ----

	Title (Get_Control_String (TITLE));

#ifdef DEBUG_KEYS
	{
#else
	if (Master ()) {
#endif
		Open_Report (Get_Control_String (REPORT_DIRECTORY), Get_Control_String (REPORT_FILE), Set_Control_Flag (REPORT_FILE));
	}
	
	//---- show parameter warning ----

	if (command_flag) {
		message (Program ()) += " has Too Many Parameters";
		Warning (message);
		Write (1);
	}

	//---- show unrecognized control key warnings ----

	if (key_flag) {
		Show_Extra_Keys ();
	}
	if (!Check_Keys ()) {
		Error ("Required Control Keys were Not Found");
	}
	return (true);

error_message: 
	if (Master ()) {
		Quiet (false);
		if (!banner_flag) Show_Banner ();
		if (!syntax_flag) Show_Syntax ();
		Show_Error (message);
	}
	return (false);
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Execution_Service::Program_Control (void) 
{
	String key;
	Dtime increment;

	//---- override default page length ----

	if (Check_Control_Key (PAGE_LENGTH)) {
		int len = Get_Control_Integer (PAGE_LENGTH);
		Page_Size (Page_Width (), len);
	}

	//---- project directory ----

	key = Get_Control_String (PROJECT_DIRECTORY);

	if (!key.empty ()) {
		Print_Filename (1, "Project Directory", key);
		Project_Directory (key);
	}

	//---- default file format ----

	Default_Format (Format_Code (Get_Control_Text (DEFAULT_FILE_FORMAT)));

	//---- time of day format ----

	Time_Format (Time_Code (Get_Control_Text (TIME_OF_DAY_FORMAT)));

	//---- model start time ----

	Model_Start_Time (Get_Control_Text (MODEL_START_TIME));

	//---- model end time ----

	Model_End_Time (Get_Control_Text (MODEL_END_TIME));

	//---- model time increment ----

	increment = Get_Control_Time (MODEL_TIME_INCREMENT);

	time_periods.Set_Periods (increment, Model_Start_Time (), Model_End_Time ());

	//---- units of measure ----

	Metric_Flag (Measure_Code (Get_Control_Text (UNITS_OF_MEASURE)) == METRIC);

	//---- random number seed ----

	if (Control_Key_Status (RANDOM_NUMBER_SEED)) {
		key = Get_Control_String (RANDOM_NUMBER_SEED);

		if (!key.empty ()) {
			if (key.length () > 9) key.erase (9);
			random.Seed (key.Integer ());
		}
		random_seed = random.Seed ();
		Output_Control_Label (RANDOM_NUMBER_SEED) << random_seed;
	}

	//---- warning messages ----

	if (Control_Key_Status (MAX_WARNING_MESSAGES)) {
		Max_Warnings (Get_Control_Integer (MAX_WARNING_MESSAGES));
	}
	Warning_Flag (Get_Control_Flag (MAX_WARNING_EXIT_FLAG));

	//---- problem count ----

	Max_Problems (Get_Control_Integer (MAX_PROBLEM_COUNT));

	//---- number of threads ----

	num_threads = Get_Control_Integer (NUMBER_OF_THREADS);

	if (num_threads > 1) {
		if (!Enable_Threads ()) {
			key (Program ()) += " is Not Thread Enabled";
			Warning (key);
			Write (1);
			num_threads = 1;
		}
	}
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Execution_Service::Execute (void) 
{
	Show_Error (String ("%s is Not Yet Implemented") % Program ());
}

//-----------------------------------------------------------
//	Show_Syntax - show the program syntax
//-----------------------------------------------------------

void Execution_Service::Show_Syntax (void) 
{
	if (Enable_Partitions ()) {
		Show_Message (String ("Syntax is %s [-flag] [control_file] [partition]\n") % Program ());
	} else if (Enable_Parameter ()) {
		Show_Message (String ("Syntax is %s [-flag] [control_file] [parameter]\n") % Program ());
	} else {
		Show_Message (String ("Syntax is %s [-flag] [control_file]\n") % Program ());
	}
}

//-----------------------------------------------------------
//	Syntax_Help - show the program syntax and keys
//-----------------------------------------------------------

void Execution_Service::Syntax_Help (void) 
{
	bool quiet_flag = Quiet ();

	Report_Name_Itr itr;

	Show_Banner ();
	Show_Syntax ();
	Show_Message (2, "Optional Flags:");
	Show_Message ("  -H[elp]     = show program syntax and control keys");
	Show_Message ("  -C[ontrol]  = create/update a default control file");
	Show_Message ("  -K[eyCheck] = list unrecognized control file keys");
	Show_Message ("  -P[ause]    = pause before exiting");
	Show_Message ("  -N[oPause]  = never pause before exiting");
	Show_Message ("  -Q[uiet]    = execute without screen messages");
	Show_Message ("  -D[etail]   = execute with detailed status messages");
	Show_Message ("  -X[ML]      = write an XML file with control keys");
	Show_Message ("  -R[eport]   = write control keys and report names"); 

	Show_Program_Keys ();

	//---- list the report options ----

	if (report_name.size () > 0) {
		Show_Message (2, "Report Options:");

		for (itr = report_name.begin (); itr != report_name.end (); itr++) {
			string message = "  ";
			Show_Message (message += itr->name);
		}
	}
	Show_Message (1);
	Quiet (quiet_flag);
}

//-----------------------------------------------------------
//	Document - write the program syntax and keys
//-----------------------------------------------------------

void Execution_Service::Document (bool user_flag) 
{
	Report_Name_Itr itr;

	String text = Program ();
	text += ".doc";

	FILE *file = f_open (text, "wt");
	if (file == 0) return;

	fprintf (file, "%s (%d.%d.%d)\n", Program (), (VERSION_CODE / 10), (VERSION_CODE % 10), Version ());

	fprintf (file, "Syntax:\n\t%s [-flag] [control_file]", Program ());
	if (Enable_Partitions ()) {
		fprintf (file, " [partition]");
	} else if (Enable_Parameter ()) {
		fprintf (file, " [parameter]");
	}
	fprintf (file, "\n\nOptional Flags:");
	fprintf (file, "\n\t-H[elp]\tshow program syntax and control keys");
	fprintf (file, "\n\t-C[ontrol]\tcreate/update a default control file");
	fprintf (file, "\n\t-K[eyCheck]\tlist unrecognized control file keys");
	fprintf (file, "\n\t-P[ause]\tpause before exiting");
	fprintf (file, "\n\t-N[oPause]\tnever pause before exiting");
	fprintf (file, "\n\t-Q[uiet]\texecute without screen messages");
	fprintf (file, "\n\t-D[etail]\texecute with detailed status messages");
	fprintf (file, "\n\t-X[ML]\twrite an XML file with control keys");
	fprintf (file, "\n\t-R[eport]\twrite control keys and report names"); 

	Document_Controls (file, user_flag);

	//---- list the report options ----

	if (report_name.size () > 0) {
		fprintf (file, "\n\nReport Options:");

		for (itr = report_name.begin (); itr != report_name.end (); itr++) {
			fprintf (file, "\n\t%s", itr->name.c_str ());
		}
	}
}

//-----------------------------------------------------------
//	XML_Open
//-----------------------------------------------------------

void Execution_Service::XML_Open (void) 
{
	XML_Flag (Open_XML (Get_Control_String (REPORT_DIRECTORY), Get_Control_String (REPORT_FILE)));

	if (XML_Flag ()) {
		XML (1, String ("<PROGRAM NAME=\"%s\" ") % Program ());
		XML (0, String ("VERSION=\"%d.%d.%d\" ") % (VERSION_CODE / 10) % (VERSION_CODE % 10) % Version ());
		XML (0, String ("COPYRIGHT=\"%s\" ") % CopyRight ());
		if (Enable_Partitions ()) {
			XML (0, "PARTITIONS=\"TRUE\" />");
		} else if (Enable_Parameter ()) {
			XML (0, "PARAMETERS=\"TRUE\" />");
		} else {
			XML (0, "/>");
		}
		XML_Controls ();
		if (XML_Help_Flag ()) {
			XML_Help ();
		}
	}
}

//-----------------------------------------------------------
//	XML_Help
//-----------------------------------------------------------

void Execution_Service::XML_Help (void) 
{
	if (!XML_Flag () || help_set.size () == 0) return;

	//---- get the help file name ----

	char *help_file = get_env ("TRANSIMS_HELP_FILE");

	if (help_file == 0) {
		Warning ("TRANSIMS_HELP_FILE Environment Variable was Not Found");
		return;
	}

	//---- open the help file ----

	Db_File file;
	bool message_flag = Send_Messages ();
	
	Send_Messages (false);

	file.File_Type ("Help File");
	file.File_Format (UNFORMATED);
	file.Clean_Flag (false);

	if (!file.Open (help_file)) {
		Send_Messages (message_flag);
		Warning (String ("Help File \"%s\" was Not Found") % help_file);
		return;
	}
	Send_Messages (message_flag);

	int line, code;
	bool flag = false;
	String text;

	XML (1, "<HELP_CODES>");
	line = 1;

	//---- read the help file ----

	while (file.Read ()) {
		text = file.Record_String ();

		if (text.Starts_With ("HELP_CODE=")) {
			if (flag) {
				XML (1, "</HELP>");
			}
			text.erase (0, 10);
			code = text.Integer ();

			Int_Set_Itr itr = help_set.find (code);
			flag = (itr != help_set.end ());

			if (flag) {
				XML (1, String ("<HELP CODE=\"%d\" >") % code);
				line = 1;
			}
		} else if (flag) {
			XML (1, String ("<LINE NUM=\"%d\" TEXT=\"%s\" />") % line % text);
			line++;
		}

	}
	if (flag) {
		XML (1, "</HELP>");
	}
	XML (1, "</HELP_CODES>");
	file.Close ();
}

//-----------------------------------------------------------
//	Report_List - set the report list
//-----------------------------------------------------------

void Execution_Service::Report_List (const char *names []) 
{
	int i;
	Report_Name rec;
	Report_Name_Itr itr;

	if (report_name.capacity () == 0) {
		report_name.reserve (10);
	}
	rec.flag = false;

	for (i=0; names [i][0] != '\0'; i++) {
		rec.name = names [i];

		report_name.push_back (rec);
	}

	//---- add the report key ----

	if (i > 0) {
		String key = Program ();
		key.To_Upper ();
		key += "_REPORT";

		Control_Key report_key [] = { //--- code, key, level, status, type, default, range, help ----
			{ PROGRAM_REPORTS, "", LEVEL1, OPT_KEY, TEXT_KEY, "", "program report name", REPORT_HELP },
			END_CONTROL
		};
		report_key [0].key = key;

		Key_List (report_key);
		report_flag = true;
	}
}

//---------------------------------------------------------
//	Read_Reports
//---------------------------------------------------------

void Execution_Service::Read_Reports (void) 
{
	int i, num, num_reports;
	String key;

	Report_Name_Itr itr;
	Report_Number report;

	//---- get the number of selected reports ----

	num_reports = Highest_Control_Group (PROGRAM_REPORTS, 0);
	
	if (num_reports <= 0) {
		report_flag = false;
		return;
	}
	report_flag = true;
	report_number.reserve (num_reports);

	for (i=1; i <= num_reports; i++) {
		key = Get_Control_String (PROGRAM_REPORTS, i);

		if (key.empty ()) continue;

		report.type = 0;
		report.data = 0.0;

		//---- identify the report ----

		num = 1;

		for (itr = report_name.begin (); itr != report_name.end (); itr++, num++) {
			if (itr->name.Equals (key)) {
				itr->flag = true;
				report.type = num;
				break;
			} else {

				//---- check for a variable length report label ----

				size_t index = itr->name.find_last_of ('*');

				if (index != string::npos) {
					String temp = itr->name.substr (0, index);

					if (key.Starts_With (temp)) {
						itr->flag = true;
						report.type = num;
						temp = key.substr (index);
						report.data = temp.Double ();
						break;
					}
				}
			}
		}

		//---- check for unrecognized report types ----

		if (report.type == 0) {
			Error (String ("Unrecognized Report Type = %s") % key);
		} else {
			report_number.push_back (report);
		}
	}
}

//-----------------------------------------------------------
//	List_Reports - print the report keys
//-----------------------------------------------------------

void Execution_Service::List_Reports (void) 
{
	int i, offset, num_reports;
	string key;
	bool first;

	if (!report_flag) return;
	report_flag = false;

	//---- get the number of selected reports ----

	num_reports = Highest_Control_Group (PROGRAM_REPORTS, 0);
	offset = 0;

	if (num_reports > 0) {
		first = true;

		for (i=1; i <= num_reports; i++) {
			key = Get_Control_String (PROGRAM_REPORTS, i);

			if (key.empty ()) continue;

			if (first) {
				Print (2, Program ()) << " Reports: ";
				first = false;
				offset = (int) strlen (Program ()) + 10;
			} else {
				Print (1, String ("%*c") % offset % BLANK);
			}
			Print (0, String ("%2d. ") % i) << key;
		}
	}
	Print (1);
	Flush ();
}

//-----------------------------------------------------------
//	Report_Flag - check report type flag
//-----------------------------------------------------------

bool Execution_Service::Report_Flag (int type) 
{
	if (type > (int) report_name.size ()) return (false);
	return (report_name [type - 1].flag);
}

//-----------------------------------------------------------
//	Report_Type - get the report type
//-----------------------------------------------------------

int Execution_Service::Report_Type (int number) 
{
	if (number > (int) report_number.size ()) return (0);
	return (report_number [number - 1].type);
}

//-----------------------------------------------------------
//	Report_Data - get the report data
//-----------------------------------------------------------

double Execution_Service::Report_Data (int number) 
{
	if (number > (int) report_number.size ()) return (0.0);
	if (number == 0) return (report_num_itr->data);
	return (report_number [number - 1].data);
}

//-----------------------------------------------------------
//	First_Report - get the first report type
//-----------------------------------------------------------

int Execution_Service::First_Report (void) 
{
	report_num_itr = report_number.begin ();

	if (report_num_itr != report_number.end ()) {
		return (report_num_itr->type);
	} else {
		return (0);
	}
}

//-----------------------------------------------------------
//	Next_Report - get the next report type
//-----------------------------------------------------------

int Execution_Service::Next_Report (void) 
{
	if (++report_num_itr != report_number.end ()) {
		return (report_num_itr->type);
	} else {
		return (0);
	}
}

//-----------------------------------------------------------
//	Project_Directory
//-----------------------------------------------------------

void Execution_Service::Project_Directory (string dir) 
{
	project_directory = dir;
	Fix_Directory (project_directory);
}


