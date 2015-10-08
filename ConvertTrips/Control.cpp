//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ConvertTrips::Program_Control (void)
{
	int i, num_file, lvalue, mode;
	bool factor_flag;
	String key, format;
	Int_Map_Stat map_stat;

	File_Group group_rec;
	File_Group_Itr group;

	factor_flag = false;

	//---- open demand and network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	//---- set the file points ----

	skim_flag = System_File_Flag (SKIM);
	skim_file = (Skim_File *) System_File_Base (SKIM);

	trip_file = (Trip_File *) System_File_Base (NEW_TRIP);
	household_file = (Household_File *) System_File_Base (NEW_HOUSEHOLD);
	vehicle_file = (Vehicle_File *) System_File_Base (NEW_VEHICLE);

	zone_loc_flag = Zone_Loc_Flag ();

	//---- check for copy commands ----

	if (System_File_Flag (TRIP)) {
		trip_file->Clear_Fields ();
		trip_file->Replicate_Fields (System_File_Base (TRIP));
		trip_file->Write_Header ();
	}
	if (System_File_Flag (HOUSEHOLD)) {
		household_file->Clear_Fields ();
		household_file->Replicate_Fields (System_File_Base (HOUSEHOLD));
		household_file->Write_Header ();
	}
	if (System_File_Flag (VEHICLE)) {
		vehicle_file->Clear_Fields ();
		vehicle_file->Replicate_Fields (System_File_Base (VEHICLE));
		vehicle_file->Write_Header ();
	}
	if (System_File_Flag (NEW_PROBLEM)) {
		problem_flag = true;
		problem_file = (Problem_File *) System_File_Handle (NEW_PROBLEM);
		problem_file->Router_Data ();
	}

	//---- random number seeds ----
	
	random_org.Seed (random.Seed () + 1);
	random_des.Seed (random.Seed () + 2);
	random_tod.Seed (random.Seed () + 3);
	random_stop.Seed (random.Seed () + 4);

	Print (2, String ("%s Control Keys:") % Program ());	

	//---- get the first household number ----
	
	hhold_id = Get_Control_Integer (FIRST_HOUSEHOLD_NUMBER);

	//---- get the additional travel time ----

	additional_time = Get_Control_Time (ADDITIONAL_TRAVEL_TIME);

	//---- get the tour file ----

	key = Get_Control_String (TOUR_DATA_FILE);

	if (!key.empty ()) {
		if (Check_Control_Key (TOUR_DATA_FORMAT)) {
			tour_file.Dbase_Format (Get_Control_String (TOUR_DATA_FORMAT));
		}
		tour_file.Open (Project_Filename (key));
		tour_flag = true;

		//---- set tour field numbers ----

		Print (1);

		//---- tour household field ----

		key = Get_Control_String (TOUR_HOUSEHOLD_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.HHold_Field (key);
		} else {
			lvalue = tour_file.HHold_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Household Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour person field ----

		key = Get_Control_String (TOUR_PERSON_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Person_Field (key);
		} else {
			lvalue = tour_file.Person_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Person Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour number field ----

		key = Get_Control_String (TOUR_NUMBER_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Tour_Field (key);
		} else {
			lvalue = tour_file.Tour_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Number Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour purpose field ----

		key = Get_Control_String (TOUR_PURPOSE_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Purpose_Field (key);
		} else {
			lvalue = tour_file.Purpose_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Purpose Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour mode field ----

		key = Get_Control_String (TOUR_MODE_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Mode_Field (key);
		} else {
			lvalue = tour_file.Mode_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Mode Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}
		
		//---- tour origin field ----

		key = Get_Control_String (TOUR_ORIGIN_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Origin_Field (key);
		} else {
			lvalue = tour_file.Origin_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Origin Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour destination field ----

		key = Get_Control_String (TOUR_DESTINATION_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Dest_Field (key);
		} else {
			lvalue = tour_file.Dest_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Destination Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour outbound stop field ----

		key = Get_Control_String (TOUR_STOP_OUT_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Stop_Out_Field (key);
		} else {
			lvalue = tour_file.Stop_Out_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Stop Out Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour inbound stop field ----

		key = Get_Control_String (TOUR_STOP_IN_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Stop_In_Field (key);
		} else {
			lvalue = tour_file.Stop_In_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Stop In Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- tour start time ----

		key = Get_Control_String (TOUR_START_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Start_Field (key);
		} else {
			lvalue = tour_file.Start_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Start Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}
		
		//---- tour return time ----

		key = Get_Control_String (TOUR_RETURN_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Return_Field (key);
		} else {
			lvalue = tour_file.Return_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Return Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}	

		//---- tour group field ----

		key = Get_Control_String (TOUR_GROUP_FIELD);
		if (!key.empty ()) {
			lvalue = tour_file.Group_Field (key);
		} else {
			lvalue = tour_file.Group_Field ();
		}
		if (lvalue >= 0) {
			Print (1, "Tour Group Field = ") << tour_file.Field (lvalue)->Name ();
			Print (0, ", Number = ") << lvalue;
		}

		//---- get the tour group script -----

		key = Get_Control_String (TOUR_GROUP_SCRIPT);

		if (!key.empty ()) {
			group_script.File_Type ("Tour Group Script");
			group_script.Open (Project_Filename (key));
			script_flag = true;
		}
	}

	//---- write the report names ----

	List_Reports ();

	//---- get the number of files ----

	num_file = Highest_Control_Group (TIME_DISTRIBUTION_FILE, 0);

	if (num_file == 0) {
		Error ("No Trip Groups were Found");
	}
	file_group.reserve (num_file);
	group_index.assign ((num_file + 1), -1);

	//---- process each file ----

	for (i=1; i <= num_file; i++) {
		key = Get_Control_String (TIME_DISTRIBUTION_FILE, i);
		if (key.empty ()) continue;

		key = Get_Control_String (TRIP_TABLE_FILE, i);
		if (key.empty () && !tour_flag) {
			Warning (String ("Trip Table File #%d was Not Found") % i);
			continue;
		}

		if (!Break_Check (16)) {
			Print (1);
		}
		group_index [i] = (int) file_group.size ();

		file_group.push_back (group_rec);
		group = --file_group.end ();

		group->Group (i);

		//---- get the trip file ----

		if (!key.empty ()) {
			if (Check_Control_Key (TRIP_TABLE_FORMAT, i)) {
				format = Get_Control_String (TRIP_TABLE_FORMAT, i);
			} else {
				format.clear ();
			}
			group->Open_Trips (key, format);

			//---- get the trip scaling factor ----

			group->Scaling_Factor (Get_Control_Double (TRIP_SCALING_FACTOR, i));
		}

		//---- get the time distribution file ----

		key = Get_Control_String (TIME_DISTRIBUTION_FILE, i);

		if (Check_Control_Key (TIME_DISTRIBUTION_FORMAT, i)) {
			format = Get_Control_String (TIME_DISTRIBUTION_FORMAT, i);
		} else {
			format.clear ();
		}
		group->Open_Diurnal (key, format);

		//---- get the time distribution script -----

		group->Diurnal_Script (Get_Control_String (TIME_DISTRIBUTION_SCRIPT, i));

		//---- get the trip time field ----

		key = Get_Control_Text (TIME_DISTRIBUTION_FIELD, i);

		if (!key.empty ()) {
			if (!group->Time_Field (key)) {
				Error (String ("Trip Time Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (group->Time_Field () + 1);
		}

		//---- get the time distribution type ----

		key = Get_Control_Text (TIME_DISTRIBUTION_TYPE, i);

		if (!key.empty ()) {
			group->Time_Point (Time_Point_Code (key));
			if (key [0] >= '0' && key [0] <= '9') {
				Print (0, String (" (%s)") % Time_Point_Code ((Time_Point_Type) group->Time_Point ()));
			}
		}

		//---- get the time period range ----

		key = Get_Control_Text (TIME_PERIOD_RANGE, i);

		if (!key.empty ()) {
			if (!group->Set_Periods (key)) {
				Error (String ("Time Period Range %s is Illogical") % key);
			}
		}

		//---- get the time schedule constraint ----
		
		key = Get_Control_Text (TIME_SCHEDULE_CONSTRAINT, i);

		if (!key.empty ()) {
			group->Constraint (Constraint_Code (key));
			if (key [0] >= '0' && key [0] <= '9') {
				Print (0, String (" (%s)") % Constraint_Code ((Constraint_Type) group->Constraint ()));
			}
		}

		//---- get the origin weight ----

		key = Get_Control_Text (ORIGIN_WEIGHT_FIELD, i);

		if (!key.empty ()) {
			lvalue = System_File_Base (LOCATION)->Field_Number (key);

			if (lvalue < 0) {
				Error (String ("Origin Weight Field %s was Not Found in the Location File") % key);
			}
			group->Org_Wt (lvalue);
			Print (0, ", Number = ") << (lvalue + 1);
		} else {
			group->Org_Wt (-1);
			Print (1, "No Origin Weight Field --> Equal Weights");
		}

		//---- get the destination weight ----

		key = Get_Control_Text (DESTINATION_WEIGHT_FIELD, i);

		if (!key.empty ()) {
			lvalue = System_File_Base (LOCATION)->Field_Number (key);

			if (lvalue < 0) {
				Error (String ("Destination Weight Field %s was Not Found in the Location File") % key);
			}
			group->Des_Wt (lvalue);
			Print (0, ", Number = ") << (lvalue + 1);
		} else {
			group->Des_Wt (-1);
			Print (1, "No Destination Weight Field --> Equal Weights");
		}

		//---- get the distance weight ----

		group->Dist_Wt (Get_Control_Flag (DISTANCE_WEIGHT_FLAG, i));

		//---- get the stop weight ----

		key = Get_Control_Text (STOP_WEIGHT_FIELD, i);

		if (!key.empty ()) {
			lvalue = System_File_Base (LOCATION)->Field_Number (key);

			if (lvalue < 0) {
				Error (String ("Stop Weight Field %s was Not Found in the Location File") % key);
			}
			group->Stop_Wt (lvalue);
			Print (0, ", Number = ") << (lvalue + 1);
		} else {
			group->Stop_Wt (-1);
			if (tour_flag) Print (1, "No Stop Weight Field --> Equal Weights");
		}

		//---- get the traveler type script -----

		group->Traveler_Script (Get_Control_String (TRAVELER_TYPE_SCRIPT, i));

		//---- get the traveler type ----

		group->Type (Get_Control_Integer (TRAVELER_TYPE_CODE, i));

		//---- get the trip purpose ----

		group->Purpose (Get_Control_Integer (TRIP_PURPOSE_CODE, i));

		//---- get the travel mode ----

		key = Get_Control_Text (TRAVEL_MODE_CODE, i);

		if (!key.empty ()) {
			group->Mode (Mode_Code (key));
			if (key [0] >= '0' && key [0] <= '9') {
				Print (0, String (" (%s)") % Mode_Code ((Mode_Type) group->Mode ()));
			}
		}

		//---- get the return trip flag ----

		group->Return_Flag (Get_Control_Flag (RETURN_TRIP_FLAG, i));

		//---- get the activity duration ----

		group->Duration (Get_Control_Time (ACTIVITY_DURATION, i));

		if (group->Return_Flag () && group->Duration () == 0) {
			Error ("Activity Duration is needed for Return Trip processing");
		}

		//---- get the stop duration ----

		group->Stop_Time (Get_Control_Time (TRIP_STOP_DURATION, i));

		if (tour_flag && !group->Trip_Flag() && group->Stop_Time () == 0) {
			Error ("Stop Duration is needed for Tour processing");
		}

		//---- get the trip priority code ----

		key = Get_Control_Text (TRIP_PRIORITY_CODE, i);

		if (!key.empty ()) {
			group->Priority (Priority_Code (key));
			if (key [0] >= '0' && key [0] <= '9') {
				Print (0, String (" (%s)") % Priority_Code ((Priority_Type) group->Priority ()));
			}
		}

		//---- get the vehicle type ----

		group->Veh_Type (Get_Control_Integer (VEHICLE_TYPE_CODE, i));

		//---- get the vehicle passengers ----

		if (Check_Control_Key (VEHICLE_PASSENGERS, i)) {
			group->Passengers (Get_Control_Integer (VEHICLE_PASSENGERS, i));
		} else {
			mode = group->Mode ();
			if (mode == HOV2_MODE || mode == TAXI_MODE || mode == KNR_OUT_MODE || mode == KNR_IN_MODE) {
				group->Passengers (1);
			} else if (mode == HOV3_MODE) {
				group->Passengers (2);
			} else if (mode == HOV4_MODE) {
				group->Passengers (3);
			} else {
				group->Passengers (0);
			}
		}

		//---- average speed ----

		group->Speed (Round (Get_Control_Double (AVERAGE_TRAVEL_SPEED, i)));

		//---- get the trip factor file ----

		key = Get_Control_String (TRIP_FACTOR_FILE, i);

		if (!key.empty ()) {
			if (Check_Control_Key (TRIP_FACTOR_FORMAT, i)) {
				format = Get_Control_String (TRIP_FACTOR_FORMAT, i);
			} else {
				format.clear ();
			}
			group->Open_Factors (key, format);
			factor_flag = true;
		}

		//---- set the field map ----

		if (group->Org_Wt () > 0) {
			lvalue = (int) field_map.size ();

			map_stat = field_map.insert (Int_Map_Data (group->Org_Wt (), lvalue));

			if (!map_stat.second) {
				lvalue = map_stat.first->second;
			}
			group->Org_Wt (lvalue);
		}
		if (group->Des_Wt () > 0) {
			lvalue = (int) field_map.size ();

			map_stat = field_map.insert (Int_Map_Data (group->Des_Wt (), lvalue));

			if (!map_stat.second) {
				lvalue = map_stat.first->second;
			}
			group->Des_Wt (lvalue);
		}
		if (group->Stop_Wt () > 0) {
			lvalue = (int) field_map.size ();

			map_stat = field_map.insert (Int_Map_Data (group->Stop_Wt (), lvalue));

			if (!map_stat.second) {
				lvalue = map_stat.first->second;
			}
			group->Stop_Wt (lvalue);
		}
	}
	Write (2, "Highest Trip Group Number = ") << num_file;
	Write (1, "Number of Trip Table Groups = ") << file_group.size ();

	if (field_map.size () > 0) {
		Print (2, "Number of Unique Activity Location Weight Fields = ") << field_map.size ();
	}

	//---- create new time distribution file ----

	key = Get_Control_String (NEW_TIME_DISTRIBUTION_FILE);

	if (!key.empty ()) {
		diurnal_file.Create (Project_Filename (key));
		diurnal_flag = true;
	}

	Show_Message (1);

	//---- read the time equiv ----

	if (tour_flag) {
		time_flag = Time_Equiv_Flag ();

		if (time_flag) {
			time_equiv.Read (Report_Flag (TIME_EQUIV));
		} else {
			Write (2, "Tour Periods are Hour-Based");
			key ("1:00");
			tour_periods.Increment (key);
		}
	}

	//---- read the zone equiv ----

	if (factor_flag) {
		equiv_flag = Zone_Equiv_Flag ();

		if (equiv_flag) {
			zone_equiv.Read (Report_Flag (ZONE_EQUIV));
		} else {
			Write (2, "Trip Factor File is Zone-Based");
		}
	}
}
