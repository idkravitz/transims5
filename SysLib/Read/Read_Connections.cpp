//*********************************************************
//	Read_Connections.cpp - read the connection file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Connections
//---------------------------------------------------------

void Data_Service::Read_Connections (void)
{
	Connect_File *file = (Connect_File *) System_File_Handle (CONNECTION);

	int num, in, out;
	Int2_Map_Stat map_stat;
	Connect_Data connect_rec;

	//---- store the lane connectivity data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Connects (*file);
	
	num = 0;

	while (file->Read ()) {
		Show_Progress ();

		connect_rec.Clear ();

		if (Get_Connect_Data (*file, connect_rec)) {
			in = connect_rec.Dir_Index ();
			out = connect_rec.To_Index ();

			map_stat = connect_map.insert (Int2_Map_Data (Int2_Key (in, out), (int) connect_array.size ()));

			if (!map_stat.second) {
				Dir_Data *dir_ptr;
				Link_Data *link_ptr;

				dir_ptr = &dir_array [in];
			
				link_ptr = &link_array [dir_ptr->Link ()];
				in = link_ptr->Link ();

				dir_ptr = &dir_array [out];

				link_ptr = &link_array [dir_ptr->Link ()];
				out = link_ptr->Link ();

				Warning (String ("Duplicate Link Connection = %d to %d") % in % out);
				continue;
			} else {
				connect_array.push_back (connect_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) connect_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) {
		System_Data_True (CONNECTION);

		Connect_Data *connect_ptr;
		Dir_Data *dir_ptr;

		//---- create directional link list ---- 

		while (num--) {
			connect_ptr = &connect_array [num];

			dir_ptr = &dir_array [connect_ptr->Dir_Index ()];
			connect_ptr->Next_To (dir_ptr->First_Connect_To ());
			dir_ptr->First_Connect_To (num);

			dir_ptr = &dir_array [connect_ptr->To_Index ()];
			connect_ptr->Next_From (dir_ptr->First_Connect_From ());
			dir_ptr->First_Connect_From (num);
		}
	}
}

//---------------------------------------------------------
//	Initialize_Connects
//---------------------------------------------------------

void Data_Service::Initialize_Connects (Connect_File &file)
{
	Required_File_Check (file, LINK);
	if (file.Version () <= 40) {
		Required_File_Check (file, POCKET);
	}
	int percent = System_Data_Reserve (CONNECTION);

	if (connect_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () <= 40) {
			num = (int) (num / 2.25);
		}
		if (num > 1) {
			connect_array.reserve (num);
			if (num > (int) connect_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Connect_Data
//---------------------------------------------------------

bool Data_Service::Get_Connect_Data (Connect_File &file, Connect_Data &connect_rec)
{
	int link, dir, node, dir_index, to_index, lanes, low, high;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;

	//---- check/convert the link number and direction ----
	
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
		Warning (String ("Connection %d Link %d Direction %s was Not Found") % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	connect_rec.Dir_Index (dir_index);

	//---- convert the to-link number ----

	link = file.To_Link ();

	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("Connection %d Link %d was Not Found") % Progress_Count () % link);
		return (false);
	}
	link = map_itr->second;

	link_ptr = &link_array [link];

	if (link_ptr->Anode () == node) {
		to_index = link_ptr->AB_Dir ();
	} else if (link_ptr->Bnode () == node) {
		to_index = link_ptr->BA_Dir ();
	} else {
		Node_Data *ptr = &node_array [node];
		Warning (String ("Connection %d Node %d is Not on Link %d") % Progress_Count () % 
			ptr->Node () % link_ptr->Link ());
		return (false);
	}
	connect_rec.To_Index (to_index);

	//---- lane numbers ----

	lanes = file.Lanes ();

	if (file.Version () <= 40 && lanes > 0) {
		low = lanes - 1;
		high = file.In_High () - 1;
		if (high < low) high = low;
	} else {
		Convert_Lane_Range (dir_index, lanes, low, high);
	}
	connect_rec.Low_Lane (low);
	connect_rec.High_Lane (high);

	lanes = file.To_Lanes ();

	if (file.Version () <= 40 && lanes > 0) {
		low = lanes - 1;
		high = file.Out_High () - 1;
		if (high < low) high = low;
	} else {
		Convert_Lane_Range (to_index, lanes, low, high);
	}
	connect_rec.To_Low_Lane (low);
	connect_rec.To_High_Lane (high);

	//---- optional fields ----
	
	connect_rec.Type (file.Type ());
	connect_rec.Penalty (file.Penalty ());
	connect_rec.Speed (file.Speed ());
	connect_rec.Capacity (file.Capacity ());
	connect_rec.Notes (file.Notes ());

	//---- check for duplicate records ----

	if (file.Version () <= 40) {
		Int2_Map_Itr map2_itr;

		//max_in = dir_ptr->Thru () + dir_ptr->Left () + dir_ptr->Right ();
		//in_bear = dir_ptr->Out_Bearing ();

		//max_out = dir_ptr->Thru () + dir_ptr->Left () + dir_ptr->Right ();
		//out_bear = dir_ptr->In_Bearing ();

		//---- read and save the data ----

		//int num, node_num, link, dir, in_link, out_link, max_in, max_out, in_bear, out_bear;
		//int in_low, in_high, out_low, out_high, type, penalty, speed, capacity, change;
		//if (type == 0 && compass.Num_Points () > 0) {
		//	change = compass.Change (in_bear, out_bear) * 360 / compass.Num_Points ();

		//	if (change >= -45 && change <= 45) {
		//		type = THRU;
		//	} else if (change < -165 || change > 165) {
		//		type = UTURN;
		//	} else if (change < 0) {
		//		type = LEFT;
		//	} else {
		//		type = RIGHT;
		//	}
		//}

		map2_itr = connect_map.find (Int2_Key (connect_rec.Dir_Index (), connect_rec.To_Index ()));

		if (map2_itr != connect_map.end ()) {
			Connect_Data *connect_ptr;

			//---- update optional fields ----

			connect_ptr = &connect_array [map2_itr->second];

			//---- merge the entry lane codes ----

			if (connect_rec.Low_Lane () < connect_ptr->Low_Lane ()) {
				connect_ptr->Low_Lane (connect_rec.Low_Lane ());
			}
			if (connect_rec.High_Lane () > connect_ptr->High_Lane ()) {
				connect_ptr->High_Lane (connect_rec.High_Lane ());
			}

			//---- merge the exit lane codes ----
			
			if (connect_rec.To_Low_Lane () < connect_ptr->To_Low_Lane ()) {
				connect_ptr->To_Low_Lane (connect_rec.To_Low_Lane ());
			}
			if (connect_rec.To_High_Lane () > connect_ptr->To_High_Lane ()) {
				connect_ptr->To_High_Lane (connect_rec.To_High_Lane ());
			}

			//---- merge the optional data ----

			if (connect_rec.Type () > connect_ptr->Type ()) {
				connect_ptr->Type (connect_rec.Type ());
			}
			if (connect_rec.Penalty () > connect_ptr->Penalty ()) {
				connect_ptr->Penalty (connect_rec.Penalty ());
			}
			if (connect_rec.Speed () > connect_ptr->Speed ()) {
				connect_ptr->Speed (connect_rec.Speed ());
			}
			connect_ptr->Capacity (connect_ptr->Capacity () + connect_rec.Capacity ());
			return (false);
		}
	}
	return (true);
}
