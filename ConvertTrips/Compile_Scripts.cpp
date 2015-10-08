//*********************************************************
//	Compile_Scripts.cpp - compile file group scripts
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Compile_Scripts
//---------------------------------------------------------

void ConvertTrips::Compile_Scripts (void)
{
	File_Group_Itr group;
	Db_Base_Array files;
	bool first;

	//---- compile the group scripts ----

	if (script_flag) {
		if (Report_Flag (GROUP_SCRIPT)) {
			Header_Number (GROUP_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		script_base.File_ID ("Group");
		script_base.File_Type ("Tour Group");
		script_base.Dbase_Format (BINARY);
		script_base.File_Access (MODIFY);

		script_base.Add_Field ("GROUP_OUT", DB_INTEGER, sizeof (short), NO_UNITS, true);
		script_base.Add_Field ("GROUP_IN", DB_INTEGER, sizeof (short), NO_UNITS, true);
		script_base.Add_Field ("DIURNAL_OUT", DB_INTEGER, sizeof (short), NO_UNITS, true);;
		script_base.Add_Field ("DIURNAL_IN", DB_INTEGER, sizeof (short), NO_UNITS, true);;
		script_base.Add_Field ("MODE_OUT", DB_INTEGER, sizeof (short), NO_UNITS, true);
		script_base.Add_Field ("MODE_IN", DB_INTEGER, sizeof (short), NO_UNITS, true);
		script_base.Add_Field ("WORK_CODE", DB_INTEGER, sizeof (short), NO_UNITS, true);

		files.push_back (&tour_file);
		files.push_back (&script_base);

		if (!user_group.Initialize (files, random.Seed ()) || !user_group.Compile (group_script, Report_Flag (GROUP_SCRIPT))) {
			Error ("Compiling Tour Group Script");
		}
		if (Report_Flag (GROUP_STACK)) {
			Header_Number (GROUP_STACK);
			user_group.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- compile the diurnal scripts ----

	for (group = file_group.begin (); group != file_group.end (); group++) {
		if (group->Diurnal_Script () != 0) {
			if (Report_Flag (DIURNAL_SCRIPT)) {
				group_number = group->Group ();

				Header_Number (DIURNAL_SCRIPT);

				if (!Break_Check (10)) {
					Print (1);
					Page_Header ();
				}
			}
			if (!group->Compile_Diurnal (Report_Flag (DIURNAL_SCRIPT))) {
				Error ("Compiling Time Distribution Script");
			}
			if (Report_Flag (DIURNAL_STACK)) {
				Header_Number (DIURNAL_STACK);

				group->Diurnal_Stack (false);
			}
			Header_Number (0);
		}
	}

	//---- compile the traveler scripts ----

	first = true;

	for (group = file_group.begin (); group != file_group.end (); group++) {
		if (group->Traveler_Script () != 0) {
			group_number = group->Group ();
			if (Report_Flag (TRAVELER_SCRIPT)) {
				Header_Number (TRAVELER_SCRIPT);

				if (!Break_Check (10)) {
					Print (1);
					Page_Header ();
				}
			}
			if (first) {
				first = false;

				traveler_file.File_ID ("Traveler");
				traveler_file.File_Type ("Traveler Attributes");
				traveler_file.File_Access (MODIFY);
				traveler_file.Dbase_Format (BINARY);

				traveler_file.Add_Field ("GROUP", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("ORG", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("DES", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("START", DB_DOUBLE, sizeof (float), HOURS, true);
				traveler_file.Add_Field ("MODE", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("PURPOSE", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("DURATION", DB_DOUBLE, sizeof (float), HOURS, true);	
				traveler_file.Add_Field ("CONSTRAINT", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("PRIORITY", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("VEH_TYPE", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("PASSENGERS", DB_INTEGER, sizeof (int), NO_UNITS, true);
				traveler_file.Add_Field ("TYPE", DB_INTEGER, sizeof (int), NO_UNITS, true);

				group_fld = traveler_file.Field_Number ("GROUP");
				org_fld = traveler_file.Field_Number ("ORG");
				des_fld = traveler_file.Field_Number ("DES");
				start_fld = traveler_file.Field_Number ("START");
				mode_fld = traveler_file.Field_Number ("MODE");
				purpose_fld = traveler_file.Field_Number ("PURPOSE");
				duration_fld = traveler_file.Field_Number ("DURATION");
				constraint_fld = traveler_file.Field_Number ("CONSTRAINT");
				priority_fld = traveler_file.Field_Number ("PRIORITY");
				veh_type_fld = traveler_file.Field_Number ("VEH_TYPE");
				passengers_fld = traveler_file.Field_Number ("PASSENGERS");
				type_fld = traveler_file.Field_Number ("TYPE");
			}
			if (!group->Compile_Traveler (&traveler_file, Report_Flag (TRAVELER_SCRIPT))) {
				Error ("Compiling Traveler Type Script");
			}
			if (Report_Flag (TRAVELER_STACK)) {
				Header_Number (TRAVELER_STACK);

				group->Traveler_Stack (false);
			}
			Header_Number (0);
		}
	}

	//---- set the random number seeds ----

	for (group = file_group.begin (); group != file_group.end (); group++) {
		group->Random_Seed (random.Seed () + group->Group ());
	}
}
