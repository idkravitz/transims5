//*********************************************************
//	Read_Activity.cpp - read the version 4 activity file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Read_Activity
//---------------------------------------------------------

void NewFormat::Read_Activity (void)
{
	int lvalue, hhold, person, purpose, location, last_hh, last_per, last_tour, last_trip;
	Int_Map_Itr map_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Trip_Data trip_rec;

	Trip_File *out_file = (Trip_File *) System_File_Handle (NEW_TRIP);

	//---- process the person file ----

	Show_Message (String ("Reading %s -- Record") % activity_file.File_Type ());
	Set_Progress ();

	last_hh = last_per = last_tour = last_trip = 0;

	while (activity_file.Read ()) {
		Show_Progress ();

		hhold = activity_file.Household ();

		//---- check the household id ----

		hhold = activity_file.Household ();
		if (hhold < 1) continue;

		person = activity_file.Person ();
		purpose = activity_file.Purpose ();

		lvalue = activity_file.Location ();
		map_itr = location_map.find (lvalue);

		if (map_itr == location_map.end ()) {
			Warning (String ("Activity %d Location %d was Not Found") % Progress_Count () % lvalue);
			continue;
		}
		location = map_itr->second;

		if (hhold != last_hh || person != last_per) {
			last_hh = hhold;
			last_per = person;
			last_tour = 1;
			last_trip = 1;
			trip_rec.Household (hhold);
			trip_rec.Person (person);
			trip_rec.Tour (last_tour);
			trip_rec.Trip (last_trip);
			trip_rec.Origin (location);
			trip_rec.Start ((activity_file.End_Min () + activity_file.End_Max () + 1) / 2);
			continue;
		}
		trip_rec.Destination (location);
		trip_rec.End ((activity_file.Start_Min () + activity_file.Start_Max () + 1) / 2);
		trip_rec.Duration ((activity_file.Time_Min () + activity_file.Time_Max () + 1) / 2);

		lvalue = activity_file.Vehicle ();

		if (activity_file.Version () <= 40) {
			lvalue = Fix_Vehicle_ID (lvalue);
		}
		if (lvalue > 0) {
			veh_index.Household (hhold);
			veh_index.Vehicle (lvalue);

			veh_itr = vehicle_map.find (veh_index);
			if (veh_itr == vehicle_map.end ()) {
				Warning (String ("Activity %d Vehicle %d was Not Found") % Progress_Count () % lvalue);
				continue;
			}
			lvalue = veh_itr->second;
		} else {
			lvalue = -1;
		}
		trip_rec.Vehicle (lvalue);

		if (activity_file.Version () <= 40) {
			trip_rec.Mode (Trip_Mode_Map (activity_file.Mode ()));
		} else {
			trip_rec.Mode (activity_file.Mode ());
		}
		trip_rec.Purpose (purpose);
		trip_rec.Constraint (activity_file.Constraint ());
		trip_rec.Passengers (activity_file.Passengers ());

		trip_rec.Type (0);
		trip_rec.Partition (0);
		//trip_rec.Notes (activity_file.Notes ());

		num_new_act += Put_Trip_Data (*out_file, trip_rec);

		if (purpose == 0) {
			last_tour++;
			last_trip = 0;
			trip_rec.Tour (last_tour);
		}
		last_trip++;
		trip_rec.Trip (last_trip);
		trip_rec.Origin (location);
		trip_rec.Start ((activity_file.End_Min () + activity_file.End_Max () + 1) / 2);
	}
	End_Progress ();
	activity_file.Close ();

	Print (2, String ("Number of %s Records = %d") % activity_file.File_Type () % Progress_Count ());
}
