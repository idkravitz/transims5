//*********************************************************
//	Write_Vol_Cnt.cpp - write the volume count file
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Write_Volume_Count
//---------------------------------------------------------

void Validate::Write_Volume_Count (void)
{
	int i, num, dir_index, nrec;
	double volume, count, diff;
	bool save;

	Link_Data *link_ptr;
	Int_Map_Itr itr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;

	nrec = 0;

	Show_Message (String ("Writing %s -- Record") % vol_cnt_file.File_Type ());
	Set_Progress ();

	//---- define the output fields ----

	vol_cnt_file.Copy_Periods (sum_periods);
	vol_cnt_file.Num_Decimals (0);
	vol_cnt_file.Join_Flag (true);
	vol_cnt_file.Difference_Flag (true);
	vol_cnt_file.Data_Units (Performance_Units_Map (FLOW_DATA));

	if (!vol_cnt_file.Create_Fields ()) {
		Error ("Creating New Volume Count Fields");
	}
	if (!vol_cnt_file.Write_Header ()) {
		Error ("Creating New Volume Count Header");
	}
	num = sum_periods.Num_Periods ();

	//---- process each link in sorted order ----

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		Show_Progress ();

		link_ptr = &link_array [itr->second];

		//---- process the AB direction ----

		dir_index = link_ptr->AB_Dir ();

		if (dir_index >= 0) {
			vol_cnt_file.Zero_Fields ();
			save = false;

			//---- process each period ----

			for (i=0; i < num; i++) {
				array_ptr = &link_count_array [i];
				flow_ptr = &array_ptr->at (dir_index);
				count = flow_ptr->Flow ();
				if (count > 0.0) save = true;

				array_ptr = &link_delay_array [i];
				flow_ptr = &array_ptr->at (dir_index);
				volume = flow_ptr->Flow ();

				diff = volume - count;

				vol_cnt_file.Data (i, volume);
				vol_cnt_file.Data2 (i, count);
				vol_cnt_file.Diff (i, diff);
			}

			if (save) {
				nrec++;

				vol_cnt_file.Link_Dir (link_ptr->Link () * 2 + 0);
				vol_cnt_file.Link (link_ptr->Link ());
				vol_cnt_file.Dir (0);

				if (!vol_cnt_file.Write ()) {
					Error (String ("Writing %s") % vol_cnt_file.File_Type ());
				}
			}
		}

		//---- process the BA direction ----

		dir_index = link_ptr->BA_Dir ();

		if (dir_index >= 0) {
			vol_cnt_file.Zero_Fields ();
			save = false;

			//---- process each period ----

			for (i=0; i < num; i++) {
				array_ptr = &link_count_array [i];
				flow_ptr = &array_ptr->at (dir_index);
				count = flow_ptr->Flow ();
				if (count > 0.0) save = true;

				array_ptr = &link_delay_array [i];
				flow_ptr = &array_ptr->at (dir_index);
				volume = flow_ptr->Flow ();

				diff = volume - count;

				vol_cnt_file.Data (i, volume);
				vol_cnt_file.Data2 (i, count);
				vol_cnt_file.Diff (i, diff);
			}

			if (save) {
				nrec++;

				vol_cnt_file.Link_Dir (link_ptr->Link () * 2 + 1);
				vol_cnt_file.Link (link_ptr->Link ());
				vol_cnt_file.Dir (1);

				if (!vol_cnt_file.Write ()) {
					Error (String ("Writing %s") % vol_cnt_file.File_Type ());
				}
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % vol_cnt_file.File_Type () % nrec);
	Print (1, "Number of Time Periods = ") << vol_cnt_file.Num_Periods ();
	vol_cnt_file.Close ();
}
