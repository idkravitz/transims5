//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ConvertTrips::Execute (void)
{
	File_Group_Itr group;

	//---- compile user scripts ----

	Compile_Scripts ();

	//---- read the network ----

	Data_Service::Execute ();

	//---- generate the zone map ----

	Zone_Location ();

	//---- check the parking lots ----

	Parking_Check ();

	//---- process group data ----

	Show_Message (1);

	for (group = file_group.begin (); group != file_group.end (); group++) {
		group->Read_Diurnal ();
		if (group->Factor_Flag ()) group->Read_Factors (zone_equiv);

		if (group->Mode () == TRANSIT_MODE || group->Mode () == RIDE_MODE || 
			group->Mode () == WALK_MODE || group->Mode () == BIKE_MODE) continue;

		if (veh_type_map.find (group->Veh_Type ()) == veh_type_map.end ()) {
			Error (String ("Vehicle Type %d was Not Found in the Vehicle Type Flle") % group->Veh_Type ());
		}
	}
	if (hhold_id <= max_hh_in) hhold_id = 100 * ((max_hh_in / 100) + 1);

	//---- process the tour file ----

	if (tour_flag) {
		Tour_Processing ();
	}
	if (hhold_id <= max_hh_in) hhold_id = 100 * ((max_hh_in / 100) + 1);

	//---- process trip files ----

	for (group = file_group.begin (); group != file_group.end (); group++) {
		if (group->Trip_Flag ()) {
			if (group->Mode () >= 0 && group->Mode () < MAX_MODE && !select_mode [group->Mode ()]) continue;
			if (select_purposes && !purpose_range.In_Range (group->Purpose ())) continue;

			Read_Trip_Table (group);
		}
	}

	//---- new diurnal file ----

	if (diurnal_flag) {
		Diurnal_Results ();
	}

	//---- summarize the results ----

	if (trip_copy > 0 || hhold_copy > 0 || veh_copy > 0) {
		Break_Check (5);

		Write (2, "Number of Trip Records Copied      = ") << trip_copy;
		Write (1, "Number of Household Records Copied = ") << hhold_copy;
		Write (1, "Number of Vehicle Records Copied   = ") << veh_copy;
	}

	Break_Check (10);

	Write (2, "Total Number of Tours Read     = ") << tot_tours;
	Write (1, "Total Number of Trips Read     = ") << tot_trips;
	Write (1, "Total Number of Trips Written  = ") << new_trips;
	if (tot_add > 0.5) {
		Write (1, String ("Total Number of Trips Added    = %.0lf") % tot_add);
	}
	if (tot_del > 0.5) {
		Write (1, String ("Total Number of Trips Deleted  = %.0lf") % tot_del);
	}
	Write (1, "Number of Trips Not Allocated  = ") << tot_errors;

	Write (2, "Number of Households Generated = ") << new_hholds;
	Write (1, "Number of Persons Generated    = ") << new_persons;
	Write (1, "Number of Vehicles Generated   = ") << new_vehicles;

	if (problem_flag) {
		if (problem_file->Num_Trips () > 0) {
			problem_file->Print_Summary ();
		}
	}

	//---- end the program ----

	Report_Problems (tot_trips);
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void ConvertTrips::Page_Header (void)
{
	switch (Header_Number ()) {
		case GROUP_SCRIPT:			//---- Tour Group Script ----
			Print (1, "Tour Group Script");
			Print (1);
			break;
		case GROUP_STACK:			//---- Tour Group Stack ----
			Print (1, "Tour Group Stack");
			Print (1);
			break;
		case DIURNAL_SCRIPT:		//---- Time Distribution Script ----
			Print (1, "Time Distribution Script #") << group_number;
			Print (1);
			break;
		case DIURNAL_STACK:			//---- Time Distribution Stack ----
			Print (1, "Time Distribution Stack #") << group_number;
			Print (1);
			break;
		case TRAVELER_SCRIPT:		//---- Traveler Type Script ----
			Print (1, "Traveler Type Script #") << group_number;
			Print (1);
			break;
		case TRAVELER_STACK:		//---- Traveler Type Stack ----
			Print (1, "Traveler Type Stack #") << group_number;
			Print (1);
			break;
		default:
			break;
	}
}
