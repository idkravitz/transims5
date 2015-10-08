//*********************************************************
//	Write_Snapshot.cpp - write snapshot records
//*********************************************************

#include "ArcSnapshot.hpp"

//---------------------------------------------------------
//	Write_Snapshot
//---------------------------------------------------------

void ArcSnapshot::Write_Snapshot (int period)
{
	int dir, cell, lane, occupancy;
	Dtime low, high;
	String buffer;

	Cell_Data_Itr cell_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	if (snapshot_flag) {
		Show_Message (String ("Writing %s -- Record") % arcview_snapshot.File_Type ());

		//---- open the output file ----

		buffer = String ("%s_%s.shp") % shapename % sum_periods.Range_Label (period, pad_flag);

		if (!arcview_snapshot.Open (buffer)) {
			File_Error ("Opening ArcView Snapshot File", arcview_snapshot.Shape_Filename ());
		}
		arcview_snapshot.Write_Header ();

	} else {
		Show_Message (String ("Writing %s -- Record") % new_snapshot_file.File_Type ());
	}
	Set_Progress ();

	sum_periods.Period_Range (period, low, high);

	arcview_snapshot.Put_Field (start_field, low);
	arcview_snapshot.Put_Field (end_field, high);

	for (dir=0, cell_itr = cell_array.begin (); cell_itr != cell_array.end (); cell_itr++, dir++) {
		if (method == MAXIMUM) {
			if (cell_itr->max_total == 0) continue;
		} else {
			if (cell_itr->total == 0) continue;
		}
		dir_ptr = &dir_array [dir];
		link_ptr = &link_array [dir_ptr->Link ()];

		arcview_snapshot.Put_Field (link_field, link_ptr->Link ());
		arcview_snapshot.Put_Field (dir_field, dir_ptr->Dir ());

		for (lane=0; lane < cell_itr->lanes; lane++) {
			for (cell=0; cell < cell_itr->cells; cell++) {
				if (method == MAXIMUM) {
					occupancy = cell_itr->maximum [lane * cell_itr->cells + cell];
				} else {
					occupancy = cell_itr->data [lane * cell_itr->cells + cell];
				}
				if (occupancy > 0) {
					arcview_snapshot.Put_Field (lane_field, lane);		//******??******//
					arcview_snapshot.Put_Field (offset_field, cell * cell_size + cell_size);
					arcview_snapshot.Put_Field (occ_field, occupancy);

					Show_Progress ();
					Draw_Vehicle ();
				}
			}
		}
		cell_itr->data.assign ((cell_itr->lanes * cell_itr->cells), 0);
		cell_itr->total = 0;

		if (method == MAXIMUM) {
			cell_itr->maximum.assign ((cell_itr->lanes * cell_itr->cells), 0);
			cell_itr->max_total = 0;
		}
	}
	End_Progress ();

	if (snapshot_flag) {
		arcview_snapshot.Close ();
	}
}
