//*********************************************************
//	Read_Access_Links.cpp - read the access link file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Access_Links
//---------------------------------------------------------

void Data_Service::Read_Access_Links (void)
{
	Access_File *file = (Access_File *) System_File_Handle (ACCESS_LINK);
	
	int num;
	Int_Map_Stat map_stat;
	Access_Data access_rec;

	//---- store the access link data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	Initialize_Access (*file);

	while (file->Read ()) {
		Show_Progress ();

		access_rec.Clear ();

		if (Get_Access_Data (*file, access_rec)) {
			map_stat = access_map.insert (Int_Map_Data (access_rec.Link (), (int) access_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Link Number = ") << access_rec.Link ();
				continue;
			} else {
				access_array.push_back (access_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) access_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (ACCESS_LINK);
}

//---------------------------------------------------------
//	Initialize_Access
//---------------------------------------------------------

void Data_Service::Initialize_Access (Access_File &file)
{
	Required_File_Check (file, PARKING);
	Required_File_Check (file, LOCATION);
	
	int percent = System_Data_Reserve (ACCESS_LINK);

	if (access_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () <= 40) {
			num = (int) (num / 3);
		}
		if (num > 1) {
			access_array.reserve (num);
			if (num > (int) access_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool Data_Service::Get_Access_Data (Access_File &file, Access_Data &access_rec)
{
	int lvalue, index, type;
	Int_Map_Itr map_itr;

	bool stop_flag = System_Data_Flag (TRANSIT_STOP);

	//---- set the end type ----
	
	access_rec.From_Type (file.From_Type ());
	access_rec.To_Type (file.To_Type ());

	if ((access_rec.From_Type () == STOP_ID || access_rec.To_Type () == STOP_ID) 
		&& !stop_flag) return (false);

	//---- check/convert the from id ----

	lvalue = file.From_ID ();
	if (lvalue == 0) return (false);

	type = access_rec.From_Type ();
	index = -1;

	if (type == LOCATION_ID) {
		map_itr = location_map.find (lvalue);
		if (map_itr != location_map.end ()) index = map_itr->second;
	} else if (type == PARKING_ID) {
		map_itr = parking_map.find (lvalue);
		if (map_itr != parking_map.end ()) index = map_itr->second;
	} else if (type == STOP_ID) {
		map_itr = stop_map.find (lvalue);
		if (map_itr != stop_map.end ()) index = map_itr->second;
	} else if (type == NODE_ID) {
		map_itr = node_map.find (lvalue);
		if (map_itr != node_map.end ()) index = map_itr->second;
	} else {
		return (false);
	}
	if (index < 0) {
		if (file.Version () > 40 || !(type == LOCATION_ID && access_rec.To_Type () == STOP_ID)) {
			Warning (String ("Access Link From_ID %d was Not Found in the %s file") % lvalue % ID_Code ((ID_Type) type));
		}
		return (false);
	}
	access_rec.From_ID (index);

	//---- check/convert the to id ----

	lvalue = file.To_ID ();
	type = access_rec.To_Type ();
	index = -1;

	if (type == LOCATION_ID) {
		map_itr = location_map.find (lvalue);
		if (map_itr != location_map.end ()) index = map_itr->second;
	} else if (type == PARKING_ID) {
		map_itr = parking_map.find (lvalue);
		if (map_itr != parking_map.end ()) index = map_itr->second;
	} else if (type == STOP_ID) {
		map_itr = stop_map.find (lvalue);
		if (map_itr != stop_map.end ()) index = map_itr->second;
	} else if (type == NODE_ID) {
		map_itr = node_map.find (lvalue);
		if (map_itr != node_map.end ()) index = map_itr->second;
	} else {
		return (false);
	}
	if (index < 0) {
		if (file.Version () > 40 || !(type == LOCATION_ID && access_rec.From_Type () == STOP_ID)) {
			Warning (String ("Access Link To_ID %d was Not Found in the %s file") % lvalue % ID_Code ((ID_Type) type));
		}
		return (false);
	}
	access_rec.To_ID (index);

	//----- optional fields ----

	access_rec.Time (file.Time ());
	access_rec.Cost (file.Cost ());

	if (file.Version () <= 40) {
		index = (int) access_array.size () - 1;
		if (index >= 0) {

			//---- check for twoway access links ----

			Access_Data *access_ptr = &access_array [index];
			if (access_ptr->From_ID () == access_rec.To_ID () && 
				access_ptr->From_Type () == access_rec.To_Type () &&
				access_ptr->To_ID () == access_rec.From_ID () && 
				access_ptr->To_Type () == access_rec.From_Type () &&
				access_ptr->Time () == access_rec.Time () && 
				access_ptr->Cost () == access_rec.Cost ()) {

				access_ptr->Dir (0);
				return (false);
			}
		}
		access_rec.Dir (1);

		//----- check for access on the same link ----

		if (access_rec.Cost () == 0 && 
			(access_rec.From_Type () == PARKING_ID || access_rec.To_Type () == PARKING_ID) &&
			(access_rec.From_Type () == LOCATION_ID || access_rec.To_Type () == LOCATION_ID)) {
			
			Parking_Data *parking_ptr;
			Location_Data *location_ptr;

			if (access_rec.From_Type () == LOCATION_ID) {
				location_ptr = &location_array [access_rec.From_ID ()];
			} else {
				location_ptr = &location_array [access_rec.To_ID ()];
			}
			if (access_rec.From_Type () == PARKING_ID) {
				parking_ptr = &parking_array [access_rec.From_ID ()];
			} else {
				parking_ptr = &parking_array [access_rec.To_ID ()];
			}
			if (location_ptr->Link_Dir () == parking_ptr->Link_Dir ()) {
				if (abs (location_ptr->Offset () - parking_ptr->Offset ()) < Round (50)) return (false);
			}
		}
	} else {
		access_rec.Dir (file.Dir ());
	}
	access_rec.Link (file.Link ());
	if (access_rec.Link () == 0) {
		access_rec.Link ((int) access_array.size () + 1);
	}
	access_rec.Notes (file.Notes ());

	return (true);
}
