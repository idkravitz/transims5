//*********************************************************
//	Read_Flow.cpp - read the volume/count files
//*********************************************************

#include "Validate.hpp"

//---------------------------------------------------------
//	Read_Flow
//---------------------------------------------------------

void Validate::Read_Flow (Link_Data_File *file, bool type)
{
	int i, link, anode, bnode, an, bn, ab_dir, ba_dir, num, nmap, p1, p2, periods;
	Dtime low, high;
	Integers volume_map;
	Doubles volume;

	Link_Data *link_ptr;
	Flow_Time_Array array_rec, *array_ptr;
	Flow_Time_Data data_rec, *flow_ptr;
	Flow_Time_Period_Array *flow_array;
	Int_Map_Itr map_itr;

	if (type) {
		flow_array = &link_delay_array;
	} else {
		flow_array = &link_count_array;
	}
	periods = sum_periods.Num_Periods ();

	if (flow_array->size () == 0) {
		num = (int) dir_array.size () + Num_Lane_Use_Flows ();

		if (num > 0) {
			flow_array->assign (periods, array_rec);

			for (i=0; i < periods; i++) {
				array_ptr = &flow_array->at (i);
				array_ptr->assign (num, data_rec);
			}
		}
	}

	//---- read the volume file ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();

	//---- build the volume map ----

	nmap = file->Num_Periods ();

	volume_map.assign (nmap, 0);
	volume.assign (periods, 0.0);

	for (i=0; i < nmap; i++) {
		file->Period_Range (i, low, high);

		p1 = sum_periods.Period (low);
		p2 = sum_periods.Period (high - 1);

		volume_map [i] = sum_periods.Period ((low + high) / 2);

		if (p1 != p2 || p1 != volume_map [i]) {
			Db_Field *field = file->Field (file->AB (i));
			Error (String ("Link Data Field (%s) does not fit the Time Period") % field->Name ());
		}
	}
	ab_dir = ba_dir = -1;

	//---- read the count data ----

	while (file->Read ()) {
		Show_Progress ();

		link = file->Link ();
		if (link == 0) continue;

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Warning (String ("Link %d was Not Found in the Link File") % link);
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		anode = file->Anode ();
		bnode = file->Bnode ();

		if (anode != 0 && bnode != 0) {
			map_itr = node_map.find (anode);
			if (map_itr == node_map.end ()) {
				Warning (String ("Link %d Anode %d was Not Found in the Node File") % link % anode);
				continue;
			}
			an = map_itr->second;

			map_itr = node_map.find (bnode);
			if (map_itr == node_map.end ()) {
				Warning (String ("Link %d Bnode %d was Not Found in the Node File") % link % bnode);
				continue;
			}
			bn = map_itr->second;

			if (an == link_ptr->Anode () && bn == link_ptr->Bnode ()) {
				ab_dir = link_ptr->AB_Dir ();
				ba_dir = link_ptr->BA_Dir ();
			} else if (bn == link_ptr->Anode () && an == link_ptr->Bnode ()) {
				ab_dir = link_ptr->BA_Dir ();
				ba_dir = link_ptr->AB_Dir ();
			} else {
				Warning (String ("Anode %d or Bnode %d were Not Found on Link %d") % anode % bnode % link);
				continue;
			}
		} else {
			ab_dir = link_ptr->AB_Dir ();
			ba_dir = link_ptr->BA_Dir ();
		}

		//---- add the counts to the directional links ----

		if (ab_dir >= 0) {
			volume.assign (periods, 0.0);

			for (i=0; i < nmap; i++) {
				if (volume_map [i] >= 0) {
					volume [volume_map [i]] += file->Data_AB (i);
				}
			}
			for (i=0; i < periods; i++) {
				array_ptr = &flow_array->at (i);
				flow_ptr = &array_ptr->at (ab_dir);
				flow_ptr->Combine_Flow (volume [i]);
			}
		}
		if (ba_dir >= 0) {
			volume.assign (periods, 0.0);

			for (i=0; i < nmap; i++) {
				if (volume_map [i] >= 0) {
					volume [volume_map [i]] += file->Data_BA (i);
				}
			}
			for (i=0; i < periods; i++) {
				array_ptr = &flow_array->at (i);
				flow_ptr = &array_ptr->at (ba_dir);
				flow_ptr->Combine_Flow (volume [i]);
			}
		}
	}
	End_Progress ();
	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	file->Close ();
}

