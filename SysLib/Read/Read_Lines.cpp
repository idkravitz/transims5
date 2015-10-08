//*********************************************************
//	Read_Lines.cpp - read the transit route file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Lines
//---------------------------------------------------------

void Data_Service::Read_Lines (void)
{
	Line_File *file = (Line_File *) System_File_Handle (TRANSIT_ROUTE);

	int i, num;
	bool keep_flag;
	Int_Map_Stat map_stat;
	Line_Data line_rec;

	//---- store the route data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Lines (*file);

	while (file->Read (false)) {
		Show_Progress ();

		line_rec.Clear ();

		keep_flag = Get_Line_Data (*file, line_rec);

		num = file->Num_Nest ();
		if (num > 0) line_rec.reserve (num);

		for (i=1; i <= num; i++) {
			if (!file->Read (true)) {
				Error (String ("Number of Stop Records for Route %d") % file->Route ());
			}
			Show_Progress ();

			Get_Line_Data (*file, line_rec);
		}
		if (keep_flag) {
			map_stat = line_map.insert (Int_Map_Data (line_rec.Route (), (int) line_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Route Number = ") << line_rec.Route ();
			} else {
				line_array.push_back (line_rec);

				line_array.Add_Route_Stops ((int) line_rec.size ());
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) line_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (TRANSIT_ROUTE);
}

//---------------------------------------------------------
//	Initialize_Lines
//---------------------------------------------------------

void Data_Service::Initialize_Lines (Line_File &file)
{
	Required_File_Check (file, TRANSIT_STOP);
	if (file.Version () > 40) {
		Required_File_Check (file, VEHICLE_TYPE);
	}
	int percent = System_Data_Reserve (TRANSIT_ROUTE);

	if (line_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records () / 25;

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			line_array.reserve (num);
			if (num > (int) line_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Line_Data
//---------------------------------------------------------

bool Data_Service::Get_Line_Data (Line_File &file, Line_Data &line_rec)
{
	int route;

	//---- process a header line ----

	if (!file.Nested ()) {
		Int_Map_Itr map_itr;

		route = file.Route ();
		if (route == 0) return (false);
		line_rec.Route (route);

		if (file.Stops () < 2) return (false);

		line_rec.Mode (file.Mode ());
		line_rec.Type (file.Type ());
		line_rec.Name (file.Name ());
		line_rec.Notes (file.Notes ());

		if (file.Version () > 40) {
			map_itr = veh_type_map.find (line_rec.Type ());
			if (map_itr == veh_type_map.end ()) {
				Warning (String ("Transit Route %d Vehicle Type %d was Not Found") % route % line_rec.Type ());
			} else {
				line_rec.Type (map_itr->second);
			}
		}
		return (true);
	}

	//---- process a stop record ----

	int zone, stop;
	Line_Stop line_stop;
	Int_Map_Itr itr;

	route = line_rec.Route ();
	stop = file.Stop ();

	itr = stop_map.find (stop);
	if (itr == stop_map.end ()) {
		Warning (String ("Transit Stop %d on Route %d was Not Found") % stop % route);
		return (false);
	}
	line_stop.Stop (itr->second);
	zone = file.Zone ();

	if (Num_Fare_Zones () > 0 && (zone < 0 || zone > Num_Fare_Zones ())) {
		Warning (String ("Transit Zone %d on Route %d is Out of Range (1..%d)") % zone % route % Num_Fare_Zones ());
	}
	line_stop.Zone (zone);
	line_stop.Time_Flag ((file.Time_Flag () > 0));
	
	line_rec.push_back (line_stop);
	return (true);
}
