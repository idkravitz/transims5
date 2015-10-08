//*********************************************************
//	Write_Volume.cpp - write the output volume file
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Write_Volume
//---------------------------------------------------------

void Validate::Write_Volume (void)
{
	int i, num, nvol, dir_index;
	double volume;
	bool save;

	Link_Data *link_ptr;
	Int_Map_Itr itr;
	Node_Data *node_ptr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;

	nvol = 0;

	Show_Message (String ("Writing %s -- Record") % output_file.File_Type ());
	Set_Progress ();

	//---- define the output fields ----

	output_file.Copy_Periods (sum_periods);
	output_file.Num_Decimals (0);
	output_file.Data_Units (Performance_Units_Map (FLOW_DATA));

	if (!output_file.Create_Fields ()) {
		Error ("Creating New Volume Fields");
	}
	if (!output_file.Write_Header ()) {
		Error ("Creating New Volume Header");
	}
	num = sum_periods.Num_Periods ();

	//---- process each link in sorted order ----

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		Show_Progress ();

		save = false;
		output_file.Zero_Fields ();

		link_ptr = &link_array [itr->second];

		//---- process the AB direction ----

		dir_index = link_ptr->AB_Dir ();

		if (dir_index >= 0) {
			for (i=0; i < num; i++) {
				array_ptr = &link_delay_array [i];
				flow_ptr = &array_ptr->at (dir_index);

				volume = flow_ptr->Flow ();

				if (volume > 0) {
					output_file.Data_AB (i, volume);
					save = true;
				}
			}
		}

		//---- process the BA direction ----

		dir_index = link_ptr->BA_Dir ();

		if (dir_index >= 0) {
			for (i=0; i < num; i++) {
				array_ptr = &link_delay_array [i];
				flow_ptr = &array_ptr->at (dir_index);

				volume = flow_ptr->Flow ();

				if (volume > 0) {
					output_file.Data_BA (i, volume);
					save = true;
				}
			}
		}

		//---- save non-zero records ----

		if (save) {
			nvol++;

			output_file.Link (link_ptr->Link ());

			node_ptr = &node_array [link_ptr->Anode ()];
			output_file.Anode (node_ptr->Node ());

			node_ptr = &node_array [link_ptr->Bnode ()];
			output_file.Bnode (node_ptr->Node ());

			if (!output_file.Write ()) {
				Error (String ("Writing the %s") % output_file.File_Type ());
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % output_file.File_Type () % nvol);
	Print (1, "Number of Time Periods = ") << output_file.Num_Periods ();

	output_file.Close ();
}
