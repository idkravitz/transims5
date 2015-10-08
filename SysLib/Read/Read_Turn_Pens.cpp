//*********************************************************
//	Read_Turn_Pens.cpp - read the turn penalty file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Turn_Pens
//---------------------------------------------------------

void Data_Service::Read_Turn_Pens (void)
{
	Turn_Pen_File *file = (Turn_Pen_File *) System_File_Handle (TURN_PENALTY);

	int num;
	Turn_Pen_Data turn_rec;

	//---- store the turn data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Turn_Pens (*file);

	while (file->Read ()) {
		Show_Progress ();

		turn_rec.Clear ();

		if (Get_Turn_Pen_Data (*file, turn_rec)) {
			turn_pen_array.push_back (turn_rec);
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) turn_pen_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) {
		System_Data_True (TURN_PENALTY);

		Turn_Pen_Data *turn_ptr;
		Dir_Data *dir_ptr;

		//---- create directional link list ---- 

		while (num--) {
			turn_ptr = &turn_pen_array [num];

			dir_ptr = &dir_array [turn_ptr->Dir_Index ()];
			turn_ptr->Next_To (dir_ptr->First_Turn_To ());
			dir_ptr->First_Turn_To (num);

			dir_ptr = &dir_array [turn_ptr->To_Index ()];
			turn_ptr->Next_From (dir_ptr->First_Turn_From ());
			dir_ptr->First_Turn_From (num);
		}
	}
}

//---------------------------------------------------------
//	Initialize_Turn_Pens
//---------------------------------------------------------

void Data_Service::Initialize_Turn_Pens (Turn_Pen_File &file)
{
	Required_File_Check (file, LINK);
	if (file.Veh_Type_Flag ()) Required_File_Check (file, VEHICLE_TYPE);

	int percent = System_Data_Reserve (TURN_PENALTY);
	
	if (turn_pen_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			turn_pen_array.reserve (num);
			if (num > (int) turn_pen_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Turn_Pen_Data
//---------------------------------------------------------

bool Data_Service::Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &turn_rec)
{
	int link, dir, node, dir_index, to_index, low, high, penalty;
	Int_Map_Itr map_itr;

	//---- set the entry and exit link directions ----

	if (file.Node_Based ()) {
		Int2_Map_Itr map2_itr;

		//---- node-based method ----

		node = file.Dir ();
		if (node == 0) return (false);

		map_itr = node_map.find (node);

		if (map_itr == node_map.end ()) {
			Warning (String ("Turn Penalty Node %d was Not Found") % node);
			return (false);
		}

		//---- entry link ----

		dir = file.In_Node ();

		map2_itr = ab_map.find (Int2_Key (dir, node));

		if (map2_itr == ab_map.end ()) {
			Warning (String ("A Link between %d and %d was Not Found") % dir % node);
			return (false);
		}
		dir_index = map2_itr->second;

		//---- exit link ----

		dir = file.Out_Node ();

		map2_itr = ab_map.find (Int2_Key (node, dir));

		if (map2_itr == ab_map.end ()) {
			Warning (String ("A Link between %d and %d was Not Found") % node % dir);
			return (false);
		}
		to_index = map2_itr->second;

	} else {

		Link_Data *link_ptr;

		//---- convert the link direction ----
		
		link = file.Link ();
		dir = file.Dir ();

		link_ptr = Set_Link_Direction (file, link, dir);

		if (link_ptr == 0) return (false);

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
			node = link_ptr->Anode ();
		} else {
			dir_index = link_ptr->AB_Dir ();
			node = link_ptr->Bnode ();
		}
		if (dir_index < 0) {
			Warning (String ("Turn Penalty %d Link %d Direction %s was Not Found") % Progress_Count () % 
				link_ptr->Link () % ((dir) ? "BA" : "AB"));
			return (false);
		}

		//---- convert the to-link number ----

		link = file.To_Link ();

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Warning (String ("Turn Penalty %d Link %d was Not Found") % Progress_Count () % link);
			return (false);
		}
		link = map_itr->second;

		link_ptr = &link_array [link];

		to_index = -1;

		if (link_ptr->Anode () == node) {
			to_index = link_ptr->AB_Dir ();
		} else if (link_ptr->Bnode () == node) {
			to_index = link_ptr->BA_Dir ();
		} else {
			Node_Data *ptr = &node_array [node];
			Warning (String ("Turn Penalty %d Node %d is Not on Link %d") % Progress_Count () % ptr->Node () % link_ptr->Link ());
			return (false);
		}
		if (to_index < 0) {
			Node_Data *ptr = &node_array [node];
			Warning (String ("Turn Penalty %d Link %d does not Leave Node %d") % Progress_Count () % link_ptr->Link () % ptr->Node ());
			return (false);
		}
	}
	turn_rec.Dir_Index (dir_index);
	turn_rec.To_Index (to_index);

	//---- convert the vehicle type range ----

	low = file.Min_Veh_Type ();
	high = file.Max_Veh_Type ();

	if (low > 0) {
		map_itr = veh_type_map.find (low);
		if (map_itr == veh_type_map.end ()) {
			Warning (String ("Turn Penalty Vehicle Type %d was Not Found") % low);
			low = -1;
		} else {
			low = map_itr->second;
		}
	} else {
		low = -1;
	}
	if (high > 0) {
		map_itr = veh_type_map.find (high);
		if (map_itr == veh_type_map.end ()) {
			Warning (String ("Turn Penalty Vehicle Type %d was Not Found") % high);
			high = -1;
		} else {
			high = map_itr->second;
		}
	} else {
		high = -1;
	}
	if (low > high) {
		if (high == -1) {
			high = low;
		} else {
			Warning (String ("Turn Penalty Vehicle Type Range %d-%d is Illegal") % file.Min_Veh_Type () % file.Max_Veh_Type ());
			high = low;
		}
	}
	turn_rec.Min_Veh_Type (low);
	turn_rec.Max_Veh_Type (high);

	//---- optional fields ----

	turn_rec.Start (file.Start ());
	turn_rec.End (file.End ());
	if (turn_rec.End () == 0) turn_rec.End (Model_End_Time ());

	turn_rec.Use (file.Use ());

	penalty = file.Penalty ();
	if (penalty < 0) penalty = 0;
	turn_rec.Penalty (penalty);

	turn_rec.Notes (file.Notes ());

	return (true);
}
