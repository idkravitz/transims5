//*********************************************************
//	Sum_Snapshot.cpp - summarize snapshot file
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Sum_Snapshot
//---------------------------------------------------------

void ArcSnapshot::Sum_Snapshot (void)
{
	int period, last_period, link, dir, cell, lane, occupancy, *data, record;
	double offset;
	Dtime time, last_time;

	Db_Header *file;
	Link_Data *link_ptr;
	Cell_Data *cell_ptr;
	Int_Map_Itr map_itr;

	if (occupancy_flag) {
		file = &occupancy_file;
	} else {
		file = &snapshot_file;
	}

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	Print (1);

	//---- read each snapshot record ----

	last_time = -1;
	last_period = -1;

	while (file->Read ()) {
		Show_Progress ();

		//---- get the time data ----

		if (occupancy_flag) {
			time = occupancy_file.End ();
		} else {
			time = snapshot_file.Time ();
		}
		if (time > max_time) break;
		period = sum_periods.Period (time);

		//---- process the period change ----

		if (period != last_period) {
			if (last_period > 0) {
				End_Progress ();
				record = Progress_Count ();

				if (method == MAXIMUM) {
					Maximum_Check ();
				}
				Write_Snapshot (last_period);

				Show_Message (String ("Reading %s -- Record") % file->File_Type ());
				Set_Progress ();
				Show_Progress (record);

				last_time = -1;
			}
			last_period = period;
		}
		if (period == 0) continue;

		//---- process a time change ----

		if (time != last_time) {
			if (last_time >= 0 && method == MAXIMUM) {
				Maximum_Check ();
			}
			last_time = time;
		}

		//---- store the record ----

		file->Get_Field (link_field, link);
		file->Get_Field (dir_field, dir);

		if (link < 0) {
			link = -link;
			dir = 1;
		}
		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) continue;

		link_ptr = &link_array [map_itr->second];

		if (link_ptr->Use () == 0) continue;

		if (file->LinkDir_Type () == LINK_NODE) {
			if (dir == link_ptr->Anode ()) {
				dir = 0;
			} else if (dir == link_ptr->Bnode ()) {
				dir = 1;
			} else {
				Warning (String ("Snapshot Node %d not on Link %d") % dir % link);
				continue;
			}
		}
		if (dir == 1) {
			cell_ptr = &cell_array [link_ptr->BA_Dir ()];
		} else {
			cell_ptr = &cell_array [link_ptr->AB_Dir ()];
		}
		if (cell_ptr == NULL) continue;

		file->Get_Field (lane_field, lane);
		file->Get_Field (offset_field, offset);

		lane--;
		cell = (int) (offset / cell_size + 0.5);

		if (lane >= cell_ptr->lanes || cell >= cell_ptr->cells) continue;

		file->Get_Field (occ_field, occupancy);
		if (occupancy == 0) occupancy = 1;

		data = &cell_ptr->data [lane * cell_ptr->cells + cell];

		*data += occupancy;

		cell_ptr->total++;
	}
	End_Progress ();
	record = Progress_Count ();

	if (last_period > 0) {
		if (method == MAXIMUM) {
			Maximum_Check ();
		}
		Write_Snapshot (last_period);
	}
	file->Close ();

	Print (2, "Number of Snapshot Records Read = ") << record;
	Print (1, "Number of Snapshot Records Saved = ") << nsaved;
}

