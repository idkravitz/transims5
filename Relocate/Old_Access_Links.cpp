//*********************************************************
//	Old_Access_Links.cpp - read the old access link file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Old_Access_Links
//---------------------------------------------------------

void Relocate::Old_Access_Links (void)
{
	int num, lvalue, index, type;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Access_Data access_rec;

	//---- store the access link data ----

	Show_Message (String ("Reading %s -- Record") % old_acc_file.File_Type ());
	Set_Progress ();

	num = old_acc_file.Num_Records ();
	access_array.reserve (num);

	while (old_acc_file.Read ()) {
		Show_Progress ();

		access_rec.Clear ();

		//---- set the end type ----
	
		access_rec.From_Type (old_acc_file.From_Type ());
		access_rec.To_Type (old_acc_file.To_Type ());

		if (access_rec.From_Type () == STOP_ID || access_rec.To_Type () == STOP_ID) continue;

		//---- check/convert the from id ----

		lvalue = old_acc_file.From_ID ();
		if (lvalue == 0) continue;

		type = access_rec.From_Type ();
		index = -1;

		if (type == LOCATION_ID) {
			map_itr = old_loc_map.find (lvalue);
			if (map_itr != old_loc_map.end ()) index = map_itr->second;
		} else if (type == PARKING_ID) {
			map_itr = old_park_map.find (lvalue);
			if (map_itr != old_park_map.end ()) index = map_itr->second;
		} else {
			continue;
		}
		if (index < 0) {
			Warning (String ("Access Link From_ID %d was Not Found in the %s file") % lvalue % ID_Code ((ID_Type) type));
			continue;
		}
		access_rec.From_ID (index);

		//---- check/convert the to id ----

		lvalue = old_acc_file.To_ID ();
		type = access_rec.To_Type ();
		index = -1;

		if (type == LOCATION_ID) {
			map_itr = old_loc_map.find (lvalue);
			if (map_itr != old_loc_map.end ()) index = map_itr->second;
		} else if (type == PARKING_ID) {
			map_itr = old_park_map.find (lvalue);
			if (map_itr != old_park_map.end ()) index = map_itr->second;
		} else {
			continue;
		}
		if (index < 0) {
			Warning (String ("Access Link To_ID %d was Not Found in the %s file") % lvalue % ID_Code ((ID_Type) type));
			continue;
		}
		access_rec.To_ID (index);

		//----- optional fields ----

		access_rec.Time (old_acc_file.Time ());
		access_rec.Cost (old_acc_file.Cost ());
		access_rec.Dir (old_acc_file.Dir ());

		access_rec.Link (old_acc_file.Link ());
		if (access_rec.Link () == 0) {
			access_rec.Link ((int) old_acc_array.size () + 1);
		}
		map_stat = old_acc_map.insert (Int_Map_Data (access_rec.Link (), (int) old_acc_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Link Number = ") << access_rec.Link ();
		} else {
			old_acc_array.push_back (access_rec);
		}
	}
	End_Progress ();
	old_acc_file.Close ();

	Print (2, String ("Number of %s Records = %d") % old_acc_file.File_Type () % Progress_Count ());

	num = (int) old_acc_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % old_acc_file.File_ID () % num);
	}
}

