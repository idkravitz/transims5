//*********************************************************
//	Read_Problems.cpp - Read the Problem File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Problems
//---------------------------------------------------------

void Data_Service::Read_Problems (void)
{
	int part, num, num_rec, part_num, first;
	Problem_File *file = (Problem_File *) System_File_Handle (PROBLEM);

	Problem_Data problem_rec;

	Initialize_Problems (*file);
	num_rec = first = 0;

	//---- check the partition number ----

	if (file->Part_Flag () && First_Partition () != file->Part_Number ()) {
		file->Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}

		//---- store the trip data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			problem_rec.Clear ();

			if (Get_Problem_Data (*file, problem_rec, part_num)) {
				file->Add_Trip (problem_rec.Household (), problem_rec.Person (), problem_rec.Tour ());
				problem_array.push_back (problem_rec);
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = (int) problem_array.size ();

	if (num && num != num_rec) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (PROBLEM);
}

//---------------------------------------------------------
//	Initialize_Problems
//---------------------------------------------------------

void Data_Service::Initialize_Problems (Problem_File &file)
{
	Required_File_Check (file, LOCATION);
	//Required_File_Check (file, VEHICLE);
	
	int percent = System_Data_Reserve (PROBLEM);

	if (problem_array.capacity () == 0 && percent > 0) {
		int num = file.Estimate_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			problem_array.reserve (num);
			if (num > (int) problem_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Problem_Data
//---------------------------------------------------------

bool Data_Service::Get_Problem_Data (Problem_File &file, Problem_Data &problem_rec, int partition)
{
	int hhold, lvalue, link, dir, offset, dir_index, lane;
	Int_Map_Itr map_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	Link_Data *link_ptr;

	//---- check the household id ----

	hhold = file.Household ();
	if (hhold < 1) return (false);

	problem_rec.Household (hhold);

	problem_rec.Person (file.Person ());
	problem_rec.Tour (MAX (file.Tour (), 1));
	problem_rec.Trip (file.Trip ());

	problem_rec.Start (file.Start ());
	problem_rec.End (file.End ());

	//---- convert the origin ----

	lvalue = file.Origin ();
	if (lvalue >= 0) {
		map_itr = location_map.find (lvalue);

		if (map_itr == location_map.end ()) {
			Warning (String ("Problem %d Origin %d was Not Found") % Progress_Count () % lvalue);
			return (false);
		}
		problem_rec.Origin (map_itr->second);
	}

	//---- convert the destination ----

	lvalue = file.Destination ();
	if (lvalue > 0) {
		map_itr = location_map.find (lvalue);

		if (map_itr == location_map.end ()) {
			Warning (String ("Problem %d Destination %d was Not Found") % Progress_Count () % lvalue);
			return (false);
		}
		problem_rec.Destination (map_itr->second);
	}
	lvalue = file.Vehicle ();

	if (file.Version () <= 40) {
		lvalue = Fix_Vehicle_ID (lvalue);
	}
	if (lvalue > 0) {
		if (System_Data_Flag (VEHICLE)) {
			veh_index.Household (hhold);
			veh_index.Vehicle (lvalue);

			veh_itr = vehicle_map.find (veh_index);
			if (veh_itr == vehicle_map.end ()) {
				Warning (String ("Trip %d Vehicle %d was Not Found") % Progress_Count () % lvalue);
				return (false);
			}
			lvalue = veh_itr->second;
		}
	} else {
		lvalue = -1;
	}
	problem_rec.Vehicle (lvalue);

	if (file.Version () <= 40) {
		problem_rec.Mode (Trip_Mode_Map (file.Mode ()));
	} else {
		problem_rec.Mode (file.Mode ());
	}
	problem_rec.Purpose (file.Purpose ());
	problem_rec.Constraint (file.Constraint ());
	problem_rec.Passengers (file.Passengers ());

	problem_rec.Problem (file.Problem ());
	problem_rec.Survey (file.Survey ());
	problem_rec.Time (file.Time ());

	//---- check/convert the link number and direction ----

	link = file.Link ();

	if (link > 0) {
		dir = file.Dir ();
		offset = Round (file.Offset ());

		link_ptr = Set_Link_Direction (file, link, dir, offset);

		if (link_ptr == 0) return (false);

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
		} else {
			dir_index = link_ptr->AB_Dir ();
		}
		if (dir_index < 0) {
			Warning (String ("Problem %d Link %d Direction %s was Not Found") % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
		}
		problem_rec.Dir_Index (dir_index);
		problem_rec.Offset (offset);

		lane = file.Lane ();

		if (file.Version () > 40) {
			if (lane == 0) {
				lane = -1;
			} else {
				lane = Convert_Lane_ID (dir_index, lane);
			}
		}
		problem_rec.Lane (lane);
	}

	lvalue = file.Route ();

	if (lvalue > 0) {
		if (System_File_Flag (TRANSIT_ROUTE)) {
			map_itr = line_map.find (lvalue);

			if (map_itr == line_map.end ()) {
				Warning (String ("Problem %d Route %d was Not Found") % Progress_Count () % lvalue);
				return (false);
			}
			problem_rec.Route (map_itr->second);
		} else {
			problem_rec.Route (lvalue);
		}
	}
	if (file.Partition_Flag ()) {
		problem_rec.Partition (file.Partition ());
	} else {
		problem_rec.Partition (partition);
	}	
	problem_rec.Notes (file.Notes ());

	return (true);
}
