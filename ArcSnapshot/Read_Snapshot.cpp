//*********************************************************
//	Read_Snapshot.cpp - reads the snapshot file
//*********************************************************

#include "ArcSnapshot.hpp"
#include "TypeDefs.hpp"
#include "Range_Data.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Snapshot
//---------------------------------------------------------

void ArcSnapshot::Read_Snapshot (void)
{
	int i, period, index, type, pass;
	unsigned record;
	Dtime time, end_time, last_time;
	double size;
	bool compress_flag;
	Range_Array record_range;
	Dtimes times;

	Int_Map_Itr map_itr;
	Db_Header *file;

	if (occupancy_flag) {
		file = &occupancy_file;
		compress_flag = false;
	} else {
		compress_flag = snapshot_file.Compress_Flag ();

		if (compress_flag) {
			last_time = 0;
			index = -1;

			while (snapshot_file.Read_Index (time, record)) {
				if (index >= 0) {
					times.push_back (last_time);
					record_range.Add_Range (index, record - 1);
				}
				last_time = time;
				index = record;
			}
		}
		file = &snapshot_file;
	}

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	Print (1);

	//---- read each snapshot record ----

	last_time = -1;
	period = type = record = 0;

	while (file->Read ()) {
		Show_Progress ();

		//---- get the time data ----

		if (occupancy_flag) {

			time = occupancy_file.Start ();
			end_time = occupancy_file.End ();
			time = (time + end_time) / 2;

		} else {
			
			//---- check the vehicle type ----

			type = snapshot_file.Type ();

			if (select_vehicles && !vehicle_range.In_Range (type)) continue;

			if (compress_flag) {
				index = record_range.In_Index (record);
				if (index >= 0) {
					end_time = times [index];
				} else {
					end_time = -1;
				}
				record++;
			} else {
				end_time = snapshot_file.Time ();
			}
			time = end_time - 1;
		}
		if (time > max_time) break;
		if (!sum_periods.In_Range (time) || !sum_periods.At_Increment (end_time)) continue;

		if (sum_flag && end_time != last_time) {
			if (last_time >= 0 && period) {
				Write_Summary (period);
			}
			period = sum_periods.Period (end_time);

			for (i=0; i < max_type; i++) {
				if (!vehicles [i].empty ()) {
					vehicles [i].assign (num_links, 0);
					persons [i].assign (num_links, 0);
				}
			}
		}

		//---- open a new file ----

		if (snapshot_flag && end_time != last_time) {
			if (last_time >= 0) {
				arcview_snapshot.Close ();
			}

			//---- create a new shape file ----

			arcview_snapshot.Open (String ("%s_%s.shp") % shapename % end_time.Time_Label (pad_flag));
			arcview_snapshot.Write_Header ();
		}
		last_time = end_time;
		pass = snapshot_file.Passengers () + 1;

		if (sum_flag) {
			map_itr = link_map.find (snapshot_file.Link ());
			if (map_itr == link_map.end ()) continue;

			index = map_itr->second;

			if (index > 0) {
				vehicles [0] [index]++;
				persons [0] [index] += pass;

				if (type < max_type) {
					vehicles [type] [index]++;
					persons [type] [index] += pass;
				}
			}
		}
		if (circle_flag) {
			size = sqrt (pass / PI);
			arcview_snapshot.Put_Field (circle_field, size);
		}
		if (square_flag) {
			size = sqrt ((double) pass);
			arcview_snapshot.Put_Field (square_field, size);
		}
		if (compress_flag) {
			dummy_snapshot.Household (snapshot_file.Household ());
			dummy_snapshot.Time (end_time);
			dummy_snapshot.Link (snapshot_file.Link ());
			dummy_snapshot.Dir (snapshot_file.Dir ());
			dummy_snapshot.Lane (snapshot_file.Lane ());
			dummy_snapshot.Offset (snapshot_file.Offset ());
			dummy_snapshot.Speed (snapshot_file.Speed ());
			dummy_snapshot.Passengers (snapshot_file.Passengers ());
			dummy_snapshot.Type (snapshot_file.Type ());
			dummy_snapshot.Wait (snapshot_file.Wait ());
			dummy_snapshot.Time_Diff (snapshot_file.Time_Diff ());
			dummy_snapshot.User (snapshot_file.User ());

			arcview_snapshot.Copy_Fields (dummy_snapshot);
		} else {
			arcview_snapshot.Copy_Fields ((*file));
		}
		Draw_Vehicle ();
	}
	End_Progress ();

	file->Close ();

	if (snapshot_flag && last_time >= 0) {
		arcview_snapshot.Close ();
	}
	Print (2, "Number of Snapshot Records Read = ") << Progress_Count ();
	Print (1, "Number of Snapshot Records Saved = ") << nsaved;

	if (sum_flag) {
		Print (2, "Number of Link Summary Records Saved = ") << num_sum;
	}
}

