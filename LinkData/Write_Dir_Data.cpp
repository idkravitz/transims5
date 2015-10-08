//*********************************************************
//	Write_Dir_Data.cpp - Write the Directional Data File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Write_Dir_Data
//---------------------------------------------------------

void LinkData::Write_Dir_Data (void)
{
	int i;
	double vol_ab, vol_ba, spd_ab, spd_ba;

	Data_Itr data_itr;

	vol_ab = vol_ba = spd_ab = spd_ba = 0;

	Show_Message (String ("Writing %s -- Record") % data_file.File_Type ());
	Set_Progress ();

	for (data_itr = data_array.begin (); data_itr != data_array.end (); data_itr++) {
		Show_Progress ();

		if (data_itr->num_ab == 0 && data_itr->num_ba == 0) continue;

		if (custom_flag) {
			data_file.Put_Field (0, data_itr->link);
			data_file.Put_Field (1, data_itr->anode);
			data_file.Put_Field (2, data_itr->bnode);
		} else {
			data_file.Link (data_itr->link);
			data_file.Anode (data_itr->anode);
			data_file.Bnode (data_itr->bnode);
		}
		for (i=0; i < num_fields; i++) {
			if (volume_flag) {
				if (data_itr->num_ab > 0) {
					vol_ab = data_itr->volume_ab [i] / data_itr->num_ab;
				} else {
					vol_ab = 0.0;
				}
				if (data_itr->num_ba > 0) {
					vol_ba = data_itr->volume_ba [i] / data_itr->num_ba;
				} else {
					vol_ba = 0.0;
				}
			}
			if (speed_flag) {
				if (data_itr->volume_ab [i] > 0) {
					spd_ab = data_itr->speed_ab [i] / data_itr->volume_ab [i];
				} else {
					spd_ab = 0.0;
				}
				if (data_itr->volume_ba [i] > 0) {
					spd_ba = data_itr->speed_ba [i] / data_itr->volume_ba [i];
				} else {
					spd_ba = 0.0;
				}
			}
			if (custom_flag) {
				if (volume_flag) {
					data_file.Put_Field (ab_vol_fields [i], vol_ab);
					data_file.Put_Field (ba_vol_fields [i], vol_ba);
				}
				if (speed_flag) {
					data_file.Put_Field (ab_spd_fields [i], spd_ab);
					data_file.Put_Field (ba_spd_fields [i], spd_ba);
				}
			} else {
				data_file.Data_AB (i, ((volume_flag) ? vol_ab : spd_ab));
				data_file.Data_BA (i, ((volume_flag) ? vol_ba : spd_ba));
			}
		}
		if (!data_file.Write ()) {
			Error ("Writing Link Data File");
		}
		nlink++;
	}
	End_Progress ();

	data_file.Close ();
}
