//*********************************************************
//	Read_ParkRide.cpp - Read the Park and Ride File
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Read_ParkRide
//---------------------------------------------------------

void TransitNet::Read_ParkRide (void)
{
	int node_field, capacity_field, hourly_field, daily_field;
	int dir, node, capacity, hourly, daily, lot_offset, min_length, offset, type;
	bool walk_flag, stop_flag;
	String notes;

	Int_Map_Itr map_itr;
	Int_Map *stop_list;
	Dir_Data *dir_ptr, *best_dir;
	Link_Data *link_ptr;
	Parking_Nest park_nest;
	Parking_Data park_rec;

	//---- read the park and ride file ----

	Show_Message (String ("Reading %s -- Record") % parkride_file.File_Type ());
	Set_Progress ();
	Print (1);

	node_field = parkride_file.Required_Field ("NODE");
	capacity_field = parkride_file.Optional_Field ("CAPACITY", "PARKING", "SPACE", "SPACES");
	hourly_field = parkride_file.Optional_Field ("HOURLY", "RATE");
	daily_field = parkride_file.Optional_Field ("DAILY", "MAXIMUM");
	parkride_file.Notes_Field (parkride_file.Optional_Field ("NOTES"));

	lot_offset = Round (Internal_Units (30, METERS));
	min_length = lot_offset * 3 / 2;

	while (parkride_file.Read ()) {
		Show_Progress ();

		node = parkride_file.Get_Integer (node_field);
		if (node == 0) continue;

		map_itr = node_map.find (node);
		if (map_itr == node_map.end ()) {
			Warning (String ("Park&Ride Node %d was Not Found in the Node File") % node);
			parking_warnings++;
			continue;
		}
		node = map_itr->second;

		capacity = parkride_file.Get_Integer (capacity_field);
		hourly = parkride_file.Get_Integer (hourly_field);
		daily = parkride_file.Get_Integer (daily_field);

		notes = parkride_file.Notes ();
		if (notes.empty ()) notes = "Park&Ride Lot";

		//---- select the best link ----

		offset = 0;
		best_dir = 0;
		walk_flag = stop_flag = false;

		for (dir = node_list [node]; dir >= 0; dir = dir_list [dir]) {
			dir_ptr = &dir_array [dir];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Length () < min_length) continue;

			type = link_ptr->Type ();
			if (type == FREEWAY || type == BRIDGE || type == TUNNEL || type == RAIL) continue;

			if (!Use_Permission (link_ptr->Use (), CAR)) continue;

			stop_list = &dir_stop_array [dir];

			map_itr = stop_list->begin ();

			if (map_itr != stop_list->end ()) {
				if (best_dir == 0 || !stop_flag || map_itr->first < offset ||
					(!walk_flag && Use_Permission (link_ptr->Use (), WALK))) {

					best_dir = dir_ptr;
					offset = map_itr->first;
					walk_flag = Use_Permission (link_ptr->Use (), WALK);
					stop_flag = true;
				}
			} else if (best_dir == 0 || (!walk_flag && Use_Permission (link_ptr->Use (), WALK))) {
				best_dir = dir_ptr;
				offset = lot_offset;
				walk_flag = Use_Permission (link_ptr->Use (), WALK);
				stop_flag = false;
			}
		}
		if (best_dir == 0) {
			Warning (String ("Park&Ride Node %d did not have Auto Access") % node);
			parking_warnings++;
			continue;
		}
		nparkride++;

		//---- insert the parking record ----

		park_rec.Clear ();

		park_rec.Parking (++max_parking);
		park_rec.Link_Dir (best_dir->Link_Dir ());
		if (offset < lot_offset) offset = lot_offset;
		park_rec.Offset (offset);
		park_rec.Type (PARKRIDE);

		park_nest.Clear ();

		park_nest.Use (CAR);
		park_nest.Start (Model_Start_Time ());
		park_nest.End (Model_End_Time ());
		park_nest.Space (capacity);
		park_nest.Time_In (0);
		park_nest.Time_Out (0);
		park_nest.Hourly (hourly);
		park_nest.Daily (daily);

		park_rec.push_back (park_nest);

		parking_map.insert (Int_Map_Data (park_rec.Parking (), (int) parking_array.size ()));
		parking_array.push_back (park_rec);
		nparking++;
	}
	End_Progress ();

	Print (1, "Number of Park and Ride Records = ") << nparkride;
}
