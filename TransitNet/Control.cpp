//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransitNet::Program_Control (void)
{
	int i, low, high;
	String key;
	Strings parts;
	Str_Itr str_itr;

	//---- check for a park & ride file ----

	if (Check_Control_Key (PARK_AND_RIDE_FILE)) {
		parkride_flag = true;
		System_File_True (PARKING);
		System_File_True (NEW_PARKING);
	} else {
		System_Option_False (PARKING);
		System_Option_False (NEW_PARKING);
	}

	//---- open network files ----

	Data_Service::Program_Control ();

	access_flag = System_File_Flag (NEW_ACCESS_LINK);

	//---- set processing flags ----

	Link_File *link_file = (Link_File *) System_File_Handle (LINK);

	at_flag = !link_file->Area_Type_Flag () && System_File_Flag (ZONE);

	Route_Nodes_File *nodes_file = (Route_Nodes_File *) System_File_Handle (ROUTE_NODES);

	dwell_flag = nodes_file->Dwell_Flag ();
	time_flag = nodes_file->Time_Flag ();
	speed_flag = nodes_file->Speed_Flag ();

	//---- open the park and ride file ----

	if (parkride_flag) {
		key = Get_Control_String (PARK_AND_RIDE_FILE);

		parkride_file.File_Type ("Park and Ride File");

		parkride_file.Open (Project_Filename (key));
	}

	//---- get the min spacing length ----
	
	Get_Control_List_Groups (STOP_SPACING_BY_AREA_TYPE, min_stop_spacing);

	//---- get the stop facility type range ----

	key = Get_Control_Text (STOP_FACILITY_TYPE_RANGE);

	if (key.empty ()) {
		key = "PRINCIPAL..FRONTAGE";
	}
	key.Parse (parts, COMMA_DELIMITERS);

	for (str_itr = parts.begin (); str_itr != parts.end (); str_itr++) {

		if (!Type_Range (*str_itr, FACILITY_CODE, low, high)) {
			Error (String ("Facility Type Range %s is Illegal") % *str_itr);
		}
		for (i=low; i <= high; i++) {
			facility_flag [i] = true;
		}
	}

	//---- get the time periods ----

	key = Get_Control_Text (TRANSIT_TIME_PERIODS);

	if (!key.empty ()) {
		schedule_periods.Add_Breaks (key);
	}
	num_periods = schedule_periods.Num_Periods ();

	Route_Nodes_File *fh = (Route_Nodes_File *) System_File_Handle (ROUTE_NODES);

	if (fh->Num_Periods () != num_periods) {
		Error (String ("Transit Time Periods = %d do not match Route Nodes Periods = %d") % num_periods % fh->Num_Periods ());
	}

	//---- set the time factors ----

	Get_Control_List_Groups (TRANSIT_TRAVEL_TIME_FACTORS, time_factor);

	//---- minimum dwell time ----

	min_dwell = Get_Control_Time (MINIMUM_DWELL_TIME);

	//---- intersection stop type ----

	stop_type = Stop_Code (Get_Control_Text (INTERSECTION_STOP_TYPE));

	//---- intersection stop offset ----

	stop_offset = Round (Get_Control_Double (INTERSECTION_STOP_OFFSET));

	if (stop_type == FARSIDE && stop_offset < Round (Internal_Units (10, METERS))) {
		Warning ("Farside Stops and Stop Offset are Incompatible");
	}

	//---- write the report names ----

	List_Reports ();

	//---- read the zone equiv ----

	equiv_flag = Zone_Equiv_Flag ();

	if (equiv_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
} 
