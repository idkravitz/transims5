//*********************************************************
//	Read_Dir_Data.cpp - Read the Directional Data File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Read_Dir_Data
//---------------------------------------------------------

void LinkData::Read_Dir_Data (void)
{
	int i, anode, bnode, dir;
	double volume, speed;

	Data *data_ptr;
	Int2_Map_Itr map_itr;
	Int_Itr int_itr;
	Dbl_Itr dbl_itr;

	//---- read the directional link file ----

	Show_Message (String ("Reading %s -- Record") % dir_file.File_Type ());
	Set_Progress ();

	while (dir_file.Read ()) {
		Show_Progress ();

		if (output_flag) {
			new_dir_file.Copy_Fields (dir_file);
			new_dir_file.Put_Field (0, 0);
			new_dir_file.Put_Field (1, 0);
		}
		anode = dir_file.Get_Integer (from_field);
		bnode = dir_file.Get_Integer (to_field);

		//---- find the link ID ----

		map_itr = data_map.find (Int2_Key (anode, bnode));

		if (map_itr == data_map.end ()) {
			map_itr = data_map.find (Int2_Key (bnode, anode));

			if (map_itr == data_map.end ()) {
				Write (1, String ("Link %d-%d was Not Found in the Link Node List File") % anode % bnode);
				if (output_flag) new_dir_file.Write ();
				continue;
			}
			dir = 1;
		} else {
			dir = 0;
		}
		data_ptr = &data_array [map_itr->second];

		if (output_flag) {
			new_dir_file.Put_Field (0, data_ptr->link);
			new_dir_file.Put_Field (1, dir);
			new_dir_file.Write ();
		}

		//---- update the link record ----

		if (data_flag) {
			if (dir) {
				data_ptr->num_ba++;
			} else {
				data_ptr->num_ab++;
			}

			for (i=0; i < num_fields; i++) {
				volume = 1;
				speed = 0;

				if (two_way_flag) {
					if (volume_flag) volume = dir_file.Get_Double (vol_ab_fields [i]);
					if (speed_flag) speed = dir_file.Get_Double (spd_ab_fields [i]) * volume;
				} else {
					if (volume_flag) volume = dir_file.Get_Double (vol_fields [i]);
					if (speed_flag) speed = dir_file.Get_Double (spd_fields [i]) * volume;
				}
				if (dir) {
					data_ptr->volume_ba [i] += volume;
					if (speed_flag) data_ptr->speed_ba [i] += speed;
				} else {
					data_ptr->volume_ab [i] += volume;
					if (speed_flag) data_ptr->speed_ab [i] += speed;
				}

				if (two_way_flag) {
					if (volume_flag) volume = dir_file.Get_Double (vol_ba_fields [i]);
					if (speed_flag) speed = dir_file.Get_Double (spd_ba_fields [i]) * volume;

					if (dir == 0) {
						data_ptr->volume_ba [i] += volume;
						if (speed_flag) data_ptr->speed_ba [i] += speed;
					} else {
						data_ptr->volume_ab [i] += volume;
						if (speed_flag) data_ptr->speed_ab [i] += speed;
					}
				}
			}
		}
	}
	End_Progress ();
	ndir = Progress_Count ();

	dir_file.Close ();
}
