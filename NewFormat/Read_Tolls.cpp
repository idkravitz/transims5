//*********************************************************
//	Read_Toll.cpp - read the toll file
//*********************************************************

#include "NewFormat.hpp"
#include "Lane_Use_Data.hpp"

//---------------------------------------------------------
//	Read_Tolls
//---------------------------------------------------------

void NewFormat::Read_Tolls (void)
{
	int link, dir, dir_index, first_toll, low, high;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Lane_Use_Data lane_use_rec, *lane_use_ptr;

	first_toll = (int) lane_use_array.size ();

	//---- process the toll file ----

	Show_Message (String ("Reading %s -- Record") % toll_file.File_Type ());
	Set_Progress ();

	while (toll_file.Read ()) {
		Show_Progress ();

		//---- check/convert the link number and direction ----
		
		link = toll_file.Link ();
		dir = toll_file.Dir ();

		link_ptr = Set_Link_Direction (toll_file, link, dir);

		if (link_ptr == 0) continue;

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
		} else {
			dir_index = link_ptr->AB_Dir ();
		}
		if (dir_index < 0) {
			Error (String ("Toll %d Link Direction %s was Not Found") % Progress_Count () % ((dir) ? "BA" : "AB"));
		}

		dir_ptr = &dir_array [dir_index];

		Convert_Lane_Range (dir_ptr, 0, low, high);

		lane_use_rec.Low_Lane (low);
		lane_use_rec.High_Lane (high);

		lane_use_rec.Dir_Index (dir_index);
		lane_use_rec.Use (toll_file.Use ());
		lane_use_rec.Start (toll_file.Start ());
		lane_use_rec.End (toll_file.End ());
		if (lane_use_rec.End () == 0) lane_use_rec.End (Model_End_Time ());
		lane_use_rec.Toll (Round (toll_file.Toll ()));
		lane_use_rec.Notes (toll_file.Notes ());

		lane_use_array.push_back (lane_use_rec);
	}
	End_Progress ();
	toll_file.Close ();

	Print (2, String ("Number of %s Records = %d") % toll_file.File_Type () % Progress_Count ());

	//---- update the directional link list ---- 

	for (dir = (int) lane_use_array.size () - 1; dir >= first_toll; dir--) {
		lane_use_ptr = &lane_use_array [dir];
		dir_ptr = &dir_array [lane_use_ptr->Dir_Index ()];

		lane_use_ptr->Next_Index (dir_ptr->First_Lane_Use ());
		dir_ptr->First_Lane_Use (dir);
	}
}
