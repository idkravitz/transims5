//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void FileFormat::Program_Control (void)
{
	int i, j, num, field;
	String key, name, buf;
	Db_Header *fh, *new_fh;
	Field_Type type = DB_INTEGER;
	double size;
	bool flag;

	//---- create the network files ----

	Execution_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- initialize the file list ----

	num_files = Highest_Control_Group (DATA_FILE, 0);

	if (num_files == 0) {
		Error ("No Data File Keys were Found");
	}
	file = new Db_Header * [num_files];
	new_file = new Db_Header * [num_files];

	memset (file, '\0', num_files * sizeof (Db_Header *));
	memset (new_file, '\0', num_files * sizeof (Db_Header *));

	//---- open each file ----

	for (i=1; i <= num_files; i++) {

		//---- open the input file ----

		key = Get_Control_String (DATA_FILE, i);

		if (key.empty ()) continue;
	
		Print (1);
		fh = file [i-1] = new Db_Header ();

		fh->File_Type (String ("Data File #%d") % i);
		fh->File_ID (String ("Data%d") % i);

		//---- get the file format ----

		if (Check_Control_Key (DATA_FORMAT, i)) {
			fh->Dbase_Format (Get_Control_String (DATA_FORMAT, i));
		}
		fh->Open (Project_Filename (key));

		if (fh->Nest_Flag ()) {
			if (fh->Num_Nest_Field () < 0) {
				Error ("A Nest Count field is Required for Nested Files");
			}
		}

		//---- open the output file ----

		Print (1);
		key = Get_Control_String (NEW_DATA_FILE, i);

		if (key.empty ()) {
			Error (String ("New Data File %d is Required") % i);
		}
		new_fh = new_file [i-1] = new Db_Header ();

		new_fh->File_Type (String ("New Data File #%d") % i);
		new_fh->File_ID (String ("NewData%d") % i);

		//---- get the file format ----

		if (Check_Control_Key (NEW_DATA_FORMAT, i)) {
			new_fh->Dbase_Format (Get_Control_String (NEW_DATA_FORMAT, i));
		}
		new_fh->Nest (fh->Nest ());
		new_fh->Create (Project_Filename (key));

		//---- copy existing fields ----

		flag = Get_Control_Flag (COPY_EXISTING_FIELDS, i);

		if (flag) {
			new_fh->Replicate_Fields (fh);
		}
		if (Get_Control_Flag (NEW_FILE_HEADER, i)) {
			new_fh->Header_Lines (fh->Header_Lines ());
			new_fh->Header_Record (fh->Header_Record ());
		} else {
			new_fh->Header_Lines (0);
		}

		//---- get the new data fields ----

		num = Highest_Control_Group (NEW_DATA_FIELD, i, 0);

		if (num == 0) {
			if (!flag) {
				Error ("No New Data Fields");
			}
		} else {
			Print (1);

			for (j=1; j <= num; j++) {
				key = Get_Control_Text (NEW_DATA_FIELD, i, j);
				if (key.empty ()) continue;

				key.Split (name, ",");
				if (name.empty ()) {
					Error (String ("New Data Field %d-%d is Improperly Specified") %i % j);
				}
				field = new_fh->Field_Number (name);
				if (field >= 0) {
					Error (String ("New Data Field %s already exists as Field %d") % name % field);
				}
				key.Split (buf, ",");
				if (buf.empty () || buf.Starts_With ("I")) {
					type = DB_INTEGER;
				} else if (buf.Starts_With ("D") || buf.Starts_With ("R")) {
					type = DB_DOUBLE;
				} else if (buf.Starts_With ("S") || buf.Starts_With ("C")) {
					type = DB_STRING;
				} else {
					Error (String ("New Data Field %d-%d is Improperly Specified") %i % j);
				}
				key.Split (buf, ",");
				if (buf.empty ()) {
					if (type == DB_DOUBLE) {
						size = 10.2;
					} else {
						size = 10.0;
					}
				} else {
					size = buf.Double ();
				}
				new_fh->Add_Field (name, type, size);
			}
		}
		new_fh->Write_Header ();		

		data_rec.push_back (fh);
		data_rec.push_back (new_fh);
	}

	//---- read conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (!key.empty ()) {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (key));
	}

	//---- read report types ----

	List_Reports ();

	if (script_flag) {
		Show_Message ("Compiling Conversion Script");

		if (Report_Flag (PRINT_SCRIPT)) {
			Header_Number (PRINT_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		program.Initialize (data_rec, random.Seed () + 1);

		if (!program.Compile (program_file, Report_Flag (PRINT_SCRIPT))) {
			Error ("Compiling Conversion Script");
		}
		if (Report_Flag (PRINT_STACK)) {
			Header_Number (PRINT_STACK);

			program.Print_Commands (false);
		}
		Header_Number (0);
		Write (1);
	}
}

