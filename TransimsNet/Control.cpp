//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransimsNet::Program_Control (void)
{
	int i, j, k, l, num, low1, high1, low2, high2, low3, high3, type, length, number, index;
	int use, time_in, time_out, hourly, daily;
	Dtime start, end;
	double d1, d2;
	String key, text;
	Strings parts;
	Str_Itr str_itr;
	bool first;

	Pocket_Warrant pocket_rec, *pocket_ptr;
	Pocket_Index pocket_index;
	Pocket_Warrant_Map_Stat pocket_stat;

	Control_Warrant control_rec;
	Control_Index control_index;
	Control_Warrant_Map_Stat control_stat;

	Access_Warrant access_rec;
	Access_Index access_index;
	Access_Warrant_Map_Stat access_stat;

	Units_Type units;
	Parking_Details detail_rec;
	Street_Parking street_rec;

	//---- open network files ----

	Data_Service::Program_Control ();

	short_length = Internal_Units (short_length, METERS);

	//---- set the file flags ----

	shape_flag = System_File_Flag (NEW_SHAPE);
	zone_flag = System_File_Flag (ZONE);
	zout_flag = System_File_Flag (NEW_ZONE);
	turn_flag = System_File_Flag (TURN_PENALTY);

	type = low1 = high1 = low2 = high2 = low3 = high3 = 0;
	
	proj_service.Read_Control ();

	//---- set the zone fields ----

	if (zout_flag) {
		if (!zone_flag) {
			Print (2, "Zone Centroids Copied from Node File");
		} else {
			Zone_File *file = (Zone_File *) System_File_Handle (ZONE);
			Zone_File *new_file = (Zone_File *) System_File_Handle (NEW_ZONE);

			new_file->Clear_Fields ();
			new_file->Replicate_Fields (file);
			new_file->Write_Header ();
		}
	}

	Print (2, String ("%s Control Keys:") % Program ());	

	//---- default link setback ----

	link_setback = Round (Get_Control_Double (DEFAULT_LINK_SETBACK));

	//---- default location setback ----

	loc_setback = Round (Get_Control_Double (DEFAULT_LOCATION_SETBACK));

	//---- get the maximum connection angle ----

	max_angle = Get_Control_Integer (MAXIMUM_CONNECTION_ANGLE);

	max_angle = compass.Num_Points () * max_angle / 360;

	//---- uturn flag ----

	uturn_flag = Get_Control_Flag (ADD_UTURN_TO_DEAD_END_LINKS);

	//---- signal id as node id ----

	signal_id_flag = Get_Control_Flag (SIGNAL_ID_AS_NODE_ID);

	//---- external zone range ----

	key = Get_Control_Text (EXTERNAL_ZONE_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		ext_zone_range.Add_Ranges (key);
		ext_zone_flag = true;
	}

	//---- external station offset ----

	external_offset = Round (Get_Control_Integer (EXTERNAL_STATION_OFFSET));

	//---- open the zone boundary file ----

	key = Get_Control_String (ZONE_BOUNDARY_FILE);

	if (!key.empty ()) {
		Print (1);
		boundary_flag = true;
		boundary_file.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

		boundary_file.File_Type ("Zone Boundary File");
		boundary_file.File_ID ("Zone");

		boundary_file.Open (Project_Filename (key));

		if (Check_Control_Key (ZONE_FIELD_NAME)) {
			key = Get_Control_Text (ZONE_FIELD_NAME);

			zone_field = boundary_file.Field_Number (key);
			Print (0, "  (field=") << zone_field << ")";
		} else {
			zone_field = boundary_file.Required_Field (Get_Range_String (ZONE_FIELD_NAME));
		}
		if (zone_field == 0) {
			Error (String ("Zone Field Name %s was Not Found in the Boundary File") % key);
		}
		Print (1);
	}

	//---- get the pocket lane warrants ----

	num = Highest_Control_Group (POCKET_LANE_WARRANT, 0);
	if (num > 0) Print (1);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (POCKET_LANE_WARRANT, i);

		if (key.empty ()) continue;

		Print (1, Current_Label ()) << " = ";
		text = key;
		text.Parse (parts, COMMA_DELIMITERS);
		length = 0;
		number = 1;
	
		for (j=0, str_itr = parts.begin (); str_itr != parts.end (); str_itr++, j++) {
			switch (j) {
				case 0:		//---- from facility type ----
					if (!Type_Range (*str_itr, FACILITY_CODE, low1, high1)) goto pocket_error;
					break;
				case 1:		//---- to facility types ----
					if (!Type_Range (*str_itr, FACILITY_CODE, low2, high2)) goto pocket_error;
					break;
				case 2:		//---- area types ----
					if (str_itr->Equals ("ALL")) {
						low3 = 0;
						high3 = 100;
					} else {
						if (!str_itr->Range (d1, d2)) goto pocket_error;
						low3 = (int) d1;
						high3 = (int) d2;
					}
					break;
				case 3:		//---- pocket type ----
					type = Pocket_Code (*str_itr);
					break;
				case 4:		//---- pocket length ----
					text = *str_itr;
					units = Parse_Units (text);
					if (units == NO_UNITS) units = METERS;
					length = Round (Internal_Units (text.Double (), units));
					break;
				case 5:		//---- number of lanes ----
					number = str_itr->Integer ();
					break;
				default:
					goto pocket_error;
					break;
			}
		}
		if (low1 > high1 || low2 > high2 || low3 > high3) goto pocket_error;
		if (length <= 0 || number < 1 || number > 4) goto pocket_error;

		//---- print the codes ----
					
		Print (0, Facility_Code ((Facility_Type) low1));
		if (low1 != high1) Print (0, "..") << Facility_Code ((Facility_Type) high1);
		Print (0, ", ") << Facility_Code ((Facility_Type) low2);
		if (low2 != high2) Print (0, "..") << Facility_Code ((Facility_Type) high2);
		Print (0, ", ") << low3;
		if (low3 != high3) Print (0, "..") << high3;
		Print (0, ", ") << Pocket_Code ((Pocket_Type) type);

		units = (Metric_Flag ()) ? METERS : FEET;
		text ("%.1lf") % External_Units (UnRound (length), units);
		text += " ";
		text += Units_Code (units);
		text.To_Lower ();

		Print (0, ", ") << text;
		Print (0, ", ") << number << " lane" << ((number > 1) ? "s" : "");

		//---- save the warrant data and index map ----

		pocket_rec.Clear ();

		switch (type) {
			case LEFT_TURN:
				pocket_rec.Left_Length (length);
				pocket_rec.Left_Lanes (number);
				break;
			case RIGHT_TURN:
				pocket_rec.Right_Length (length);
				pocket_rec.Right_Lanes (number);
				break;
			case RIGHT_MERGE:
				pocket_rec.Merge_Length (length);
				pocket_rec.Merge_Lanes (number);
				break;
			default:
				break;
		}
		index = (int) pocket_warrants.size ();
		first = true;

		for (j=low1; j <= high1; j++) {
			pocket_index.From_Type (j);

			for (k=low2; k <= high2; k++) {
				pocket_index.To_Type (k);

				for (l=low3; l <= high3; l++) {
					pocket_index.Area_Type (l);

					pocket_stat = pocket_warrant_map.insert (Pocket_Warrant_Map_Data (pocket_index, index));

					if (!pocket_stat.second) {
						pocket_ptr = &pocket_warrants [pocket_stat.first->second];

						switch (type) {
							case LEFT_TURN:
								pocket_ptr->Left_Length (length);
								pocket_ptr->Left_Lanes (number);
								break;
							case RIGHT_TURN:
								pocket_ptr->Right_Length (length);
								pocket_ptr->Right_Lanes (number);
								break;
							case RIGHT_MERGE:
								pocket_ptr->Merge_Length (length);
								pocket_ptr->Merge_Lanes (number);
								break;
							default:
								break;
						}
					} else if (first) {
						pocket_warrants.push_back (pocket_rec);
						first = false;
					}
				}
			}
		}
	}

	//---- get the traffic control warrants ----

	num = Highest_Control_Group (TRAFFIC_CONTROL_WARRANT, 0);
	if (num > 0) Print (1);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (TRAFFIC_CONTROL_WARRANT, i);

		if (key.empty ()) continue;
		
		Print (1, Current_Label ()) << " = ";
		text = key;
		text.Parse (parts, COMMA_DELIMITERS);
		length = link_setback;
		number = 1;
	
		for (j=0, str_itr = parts.begin (); str_itr != parts.end (); str_itr++, j++) {
			switch (j) {
				case 0:		//---- primary facility type ----
					if (!Type_Range (*str_itr, FACILITY_CODE, low1, high1)) goto traffic_error;
					break;
				case 1:		//---- secondary facility types ----
					if (!Type_Range (*str_itr, FACILITY_CODE, low2, high2)) goto traffic_error;
					break;
				case 2:		//---- area types ----
					if (str_itr->Equals ("ALL")) {
						low3 = 0;
						high3 = 100;
					} else {
						if (!str_itr->Range (d1, d2)) goto traffic_error;
						low3 = (int) d1;
						high3 = (int) d2;
					}
					break;
				case 3:		//---- control type ----
					type = Control_Code (*str_itr);
					break;
				case 4:		//---- intersection setback ----
					text = *str_itr;
					units = Parse_Units (text);
					if (units == NO_UNITS) units = METERS;
					length = Round (Internal_Units (text.Double (), units));
					break;
				case 5:		//---- signal group ----
					number = str_itr->Integer ();
					break;
				default:
					goto traffic_error;
					break;
			}
		}
		if (low1 > high1 || low2 > high2 || low3 > high3) goto traffic_error;
		if (length < 0 || length > Round (Internal_Units (30, METERS))) goto traffic_error;
		if (number < 0) goto traffic_error;

		//---- print the codes ----
					
		Print (0, Facility_Code ((Facility_Type) low1));
		if (low1 != high1) Print (0, "..") << Facility_Code ((Facility_Type) high1);
		Print (0, ", ") << Facility_Code ((Facility_Type) low2);
		if (low2 != high2) Print (0, "..") << Facility_Code ((Facility_Type) high2);
		Print (0, ", ") << low3;
		if (low3 != high3) Print (0, "..") << high3;
		Print (0, ", ") << Control_Code ((Control_Type) type);

		units = (Metric_Flag ()) ? METERS : FEET;
		text ("%.1lf") % External_Units (UnRound (length), units);
		text += " ";
		text += Units_Code (units);
		text.To_Lower ();

		Print (0, ", ") << text << ", " << number;

		//---- save the warrant data and index map ----

		index = (int) control_warrants.size ();

		control_rec.Control_Type (type);
		control_rec.Setback (length);
		control_rec.Group (number);

		control_warrants.push_back (control_rec);

		for (j=low1; j <= high1; j++) {
			control_index.Primary (j);

			for (k=low2; k <= high2; k++) {
				control_index.Secondary (k);

				for (l=low3; l <= high3; l++) {
					control_index.Area_Type (l);

					control_stat = control_warrant_map.insert (Control_Warrant_Map_Data (control_index, index));

					if (!control_stat.second) {
						control_stat.first->second = index;
					}
				}
			}
		}
	}

	//---- get the facility access warrants ----

	num = Highest_Control_Group (FACILITY_ACCESS_WARRANT, 0);
	if (num > 0) Print (1);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (FACILITY_ACCESS_WARRANT, i);

		if (key.empty ()) continue;

		Print (1, Current_Label ()) << " = ";
		text = key;
		text.Parse (parts, COMMA_DELIMITERS);
		type = loc_setback;
		length = Round (Internal_Units (100, METERS));
		number = 3;
		low1 = high1 = low2 = 0;
		high2 = 10;

		for (j=0, str_itr = parts.begin (); str_itr != parts.end (); str_itr++, j++) {
			switch (j) {
				case 0:		//---- facility type ----
					if (!Type_Range (*str_itr, FACILITY_CODE, low1, high1)) goto access_error;
					break;
				case 1:		//---- area types ----
					if (str_itr->Equals ("ALL")) {
						low2 = 0;
						high2 = 100;
					} else {
						if (!str_itr->Range (d1, d2)) goto access_error;
						low2 = (int) d1;
						high2 = (int) d2;
					}
					break;
				case 2:		//---- location setback ----
					text = *str_itr;
					units = Parse_Units (text);
					if (units == NO_UNITS) units = METERS;
					type = Round (Internal_Units (text.Double (), units));
					break;
				case 3:		//---- minimum split length ----
					text = *str_itr;
					units = Parse_Units (text);
					if (units == NO_UNITS) units = METERS;
					length = Round (Internal_Units (text.Double (), units));
					break;
				case 4:		//---- maximum number of points ----
					number = str_itr->Integer ();
					break;
				default:
					goto access_error;
					break;
			}
		}

		//---- check the values ---

		if (low1 > high1 || low2 > high2) goto access_error;
		if (type < 1 || type > Round (Internal_Units (100, METERS))) {
			Write (1, "Setback ") << UnRound (type) << " is Out of Range (1.." << Internal_Units (100, METERS) << ")";
			goto access_error;
		}
		if (length < Round (Internal_Units (40, METERS)) || length > Round (Internal_Units (4000, METERS))) {
			Write (1, "Split Length ") << UnRound (length) << " is Out of Range (" << Internal_Units (40, METERS) << ".." << Internal_Units (4000, METERS) << ")";
			goto access_error;
		}
		if (number < 1 || number > 20) goto access_error;

		//---- print the codes ----
					
		Print (0, Facility_Code ((Facility_Type) low1));
		if (low1 != high1) Print (0, "..") << Facility_Code ((Facility_Type) high1);
		Print (0, ", ") << low2;
		if (low2 != high2) Print (0, "..") << high2;

		units = (Metric_Flag ()) ? METERS : FEET;
		text ("%.1lf") % External_Units (UnRound (type), units);
		text += " ";
		text += Units_Code (units);
		text.To_Lower ();

		Print (0, ", ") << text;

		text ("%.1lf") % External_Units (UnRound (length), units);
		text += " ";
		text += Units_Code (units);
		text.To_Lower ();

		Print (0, ", ") << text;
		Print (0, ", ") << number << " point" << ((number > 1) ? "s" : "");

		//---- save the warrant data and index map ----

		index = (int) access_warrants.size ();

		access_rec.Setback (type);
		access_rec.Min_Length (length);
		access_rec.Max_Points (number);

		access_warrants.push_back (access_rec);

		for (j=low1; j <= high1; j++) {
			access_index.Facility (j);

			for (k=low2; k <= high2; k++) {
				access_index.Area_Type (k);

				access_stat = access_warrant_map.insert (Access_Warrant_Map_Data (access_index, index));

				if (!access_stat.second) {
					access_stat.first->second = index;
				}
			}
		}
	}

	//---- get the parking details warrants ----

	num = Highest_Control_Group (PARKING_DETAILS_WARRANT, 0);
	details_flag = (num > 0);
	if (details_flag) Print (1);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (PARKING_DETAILS_WARRANT, i);

		if (key.empty ()) continue;

		Print (1, Current_Label ()) << " = ";
		text = key;
		text.Parse (parts, COMMA_DELIMITERS);

		use = time_in = time_out = hourly = daily = low1 = high1 = 0;
		start = end = 0;

		for (j=0, str_itr = parts.begin (); str_itr != parts.end (); str_itr++, j++) {
			switch (j) {
				case 0:		//---- area types ----
					if (str_itr->Equals ("ALL")) {
						low1 = 0;
						high1 = 100;
					} else {
						if (!str_itr->Range (d1, d2)) goto parking_error;
						low1 = (int) d1;
						high1 = (int) d2;
					}
					break;
				case 1:		//---- time range ----
					if (str_itr->Equals ("ALL")) {
						start = Model_Start_Time ();
						end = Model_End_Time ();
					} else {
						if (!str_itr->Range (d1, d2)) goto parking_error;
						start.Hours (d1);
						end.Hours (d2);
					}
					break;
				case 2:		//---- use ----
					use = Use_Code (*str_itr);
					break;
				case 3:		//---- time in ----
					time_in = str_itr->Integer ();
					break;
				case 4:		//---- time out ----
					time_out = str_itr->Integer ();
					break;
				case 5:		//---- hourly ----
					hourly = str_itr->Integer ();
					break;
				case 6:		//---- daily ----
					daily = str_itr->Integer ();
					break;
				default:
					goto parking_error;
					break;
			}
		}

		//---- check the values ---

		if (low1 > high1 || start > end) goto parking_error;
		if (time_in < 0 || time_out < 0 || hourly < 0 || daily < 0) goto parking_error;

		//---- print the codes ----
					
		Print (0, "") << low1;
		if (low1 != high1) Print (0, "..") << high1;

		Print (0, ", ") << start.Time_String () << ".." << end.Time_String ();
		Print (0, ", ") << Use_Code (use);
		Print (0, ", ") << time_in << " seconds";
		Print (0, ", ") << time_out << " seconds";
		Print (0, ", ") << hourly << " cents";
		Print (0, ", ") << daily << " cents";

		//---- save the parking details ----

		detail_rec.Area_Type1 (low1);
		detail_rec.Area_Type2 (high1);
		detail_rec.Start (start);
		detail_rec.End (end);
		detail_rec.Time_In (Round (time_in));
		detail_rec.Time_Out (Round (time_out));
		detail_rec.Hourly (hourly);
		detail_rec.Daily (daily);

		parking_details.push_back (detail_rec);
	}

	//---- get the street parking warrants ----

	num = Highest_Control_Group (STREET_PARKING_WARRANT, 0);
	street_flag = (num > 0);
	if (street_flag) Print (1);

	for (i=1; i <= num; i++) {
		key = Get_Control_String (STREET_PARKING_WARRANT, i);

		if (key.empty ()) continue;

		Print (1, Current_Label ()) << " = ";
		text = key;
		text.Parse (parts, COMMA_DELIMITERS);

		low1 = high1 = low2 = high2 = 0;
		start = end = 0;

		for (j=0, str_itr = parts.begin (); str_itr != parts.end (); str_itr++, j++) {
			switch (j) {
				case 0:		//---- facility type ----
					if (!Type_Range (*str_itr, FACILITY_CODE, low1, high1)) goto street_error;
					break;
				case 1:		//---- area types ----
					if (str_itr->Equals ("ALL")) {
						low2 = 0;
						high2 = 100;
					} else {
						if (!str_itr->Range (d1, d2)) goto street_error;
						low2 = (int) d1;
						high2 = (int) d2;
					}
					break;
				case 2:		//---- time range ----
					if (str_itr->Equals ("ALL")) {
						start = Model_Start_Time ();
						end = Model_End_Time ();
					} else {
						if (!str_itr->Range (d1, d2)) goto street_error;
						start.Hours (d1);
						end.Hours (d2);
					}
					break;
				default:
					goto street_error;
					break;
			}
		}

		//---- check the values ---

		if (low1 > high1 || low2 > high2 || start > end) goto street_error;

		//---- print the codes ----

		Print (0, Facility_Code ((Facility_Type) low1));
		if (low1 != high1) Print (0, "..") << Facility_Code ((Facility_Type) high1);
		Print (0, ", ") << low2;
		if (low2 != high2) Print (0, "..") << high2;					
		Print (0, ", ") << start.Time_String () << ".." << end.Time_String ();

		//---- save the street parking ----

		index = (int) street_parking.size ();

		street_rec.Clear ();
		street_rec.Start (start);
		street_rec.End (end);

		street_parking.push_back (street_rec);

		for (j=low1; j <= high1; j++) {
			for (k=low2; k <= high2; k++) {

				street_park_map.insert (Int2_Map_Data (Int2_Key (j, k), index));
			}
		}
	}
	Print (1);

	//---- update node range ----

	key = Get_Control_Text (UPDATE_NODE_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		update_node_range.Add_Ranges (key);
		update_node_flag = true;
	}

	//---- update link range ----

	key = Get_Control_Text (UPDATE_LINK_RANGE);

	if (!key.empty () && !key.Equals ("ALL")) {
		update_link_range.Add_Ranges (key);
		update_link_flag = true;
	}

	//---- update node file ----

	key = Get_Control_String (UPDATE_NODE_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Update Node File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			update_node_range.Add_Ranges (key);
		}
		file.Close ();
		update_node_flag = true;
	}

	//---- update link file ----

	key = Get_Control_String (UPDATE_LINK_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Update Link File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			update_link_range.Add_Ranges (key);
		}
		file.Close ();
		update_link_flag = true;
	}

	//---- delete node range ----

	key = Get_Control_Text (DELETE_NODE_RANGE);

	if (!key.empty () && !key.Equals ("NONE")) {
		delete_node_range.Add_Ranges (key);
		delete_node_flag = true;
	}

	//---- delete link range ----

	key = Get_Control_Text (DELETE_LINK_RANGE);

	if (!key.empty () && !key.Equals ("NONE")) {
		delete_link_range.Add_Ranges (key);
		delete_link_flag = true;
	}

	//---- delete node file ----

	key = Get_Control_String (DELETE_NODE_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Delete Node File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			delete_node_range.Add_Ranges (key);
		}
		file.Close ();
		delete_node_flag = true;
	}

	//---- delete link file ----

	key = Get_Control_String (DELETE_LINK_FILE);

	if (!key.empty ()) {
		Db_File file;

		file.File_Type ("Delete Link File");
		file.Open (Project_Filename (key));

		while (file.Read ()) {
			key = file.Record_String ();
			if (key.Integer () == 0) continue;
			delete_link_range.Add_Ranges (key);
		}
		file.Close ();
		delete_link_flag = true;
	}

	//---- link detail file ----

	key = Get_Control_String (LINK_DETAIL_FILE);

	if (!key.empty ()) {
		Print (1);
		detail_file.File_Type ("Link Detail File");
		detail_file.Open (Project_Filename (key));

		link_detail_flag = true;
	}
	return;

pocket_error:
	Error (String ("Pocket Lane Warrant Error\n\t\t%s") % key);
	return;

traffic_error:
	Error (String ("Traffic Control Warrant Error\n\t\t%s") % key);
	return;

access_error:
	Error (String ("Facility Access Warrant Error\n\t\t%s") % key);
	return;

parking_error:
	Error (String ("Parking Details Warrant Error\n\t\t%s") % key);
	return;

street_error:
	Error (String ("Street Parking Warrant Error\n\t\t%s") % key);
	return;
} 
