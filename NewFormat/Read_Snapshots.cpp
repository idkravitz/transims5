//*********************************************************
//	Read_Snapshots.cpp - read the snapshot file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Read_Snapshots
//---------------------------------------------------------

void NewFormat::Read_Snapshots (void)
{
	int link, dir, index, lane, offset;
	unsigned record;
	Dtime time, end_time, last_time;
	bool compress_in, compress_out;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Range_Array record_range;
	Dtimes times;

	//---- process the snapshot file ----

	Show_Message (String ("Reading %s -- Record") % snap_file.File_Type ());
	Set_Progress ();

	compress_in = snap_file.Compress_Flag ();
	compress_out = new_snap_file.Compress_Flag ();

	//---- read the time index ----

	if (compress_in) {
		last_time = 0;
		index = -1;

		while (snap_file.Read_Index (time, record)) {
			if (index >= 0) {
				times.push_back (last_time);
				record_range.Add_Range (index, record - 1);
			}
			last_time = time;
			index = record;
		}
	}
	//---- read each snapshot record ----

	last_time = -1;
	record = 0;

	while (snap_file.Read ()) {
		Show_Progress ();

		//---- check/convert the link number and direction ----
		
		link = snap_file.Link ();
		dir = snap_file.Dir ();

		link_ptr = Set_Link_Direction (snap_file, link, dir);

		if (link_ptr == 0) continue;

		if (dir) {
			index = link_ptr->BA_Dir ();
		} else {
			index = link_ptr->AB_Dir ();
		}
		if (index < 0) {
			Error (String ("Snapshot %d Link Direction %s was Not Found") % Progress_Count () % ((dir) ? "BA" : "AB"));
		}

		dir_ptr = &dir_array [index];

		if (compress_in) {
			index = record_range.In_Index (record);
			if (index >= 0) {
				end_time = times [index];
			} else {
				end_time = -1;
			}
			record++;
		} else {
			end_time = snap_file.Time ();
		}
		time = end_time - 1;

		if (last_time != end_time) {
			last_time = end_time;

			if (compress_out) {
				if (!new_snap_file.Write_Index (end_time)) {
					exe->Error ("Writing Snapshot Index File");
				}
			}
		}

		if (snap_file.Version () <= 40) {
			new_snap_file.Household (snap_file.Household () / scale);
			new_snap_file.Vehicle (snap_file.Household () % scale);
		} else {
			new_snap_file.Household (snap_file.Household ());
			new_snap_file.Vehicle (snap_file.Vehicle ());
		}
		new_snap_file.Cell (snap_file.Cell ());
		new_snap_file.Time (end_time);
		new_snap_file.Link (snap_file.Link ());
		new_snap_file.Dir (snap_file.Dir ());
		new_snap_file.Speed (snap_file.Speed ());
		new_snap_file.Passengers (snap_file.Passengers ());
		new_snap_file.Type (snap_file.Type ());
		new_snap_file.Wait (snap_file.Wait ());
		new_snap_file.Time_Diff (snap_file.Time_Diff ());
		new_snap_file.User (snap_file.User ());
		new_snap_file.X (snap_file.X ());
		new_snap_file.Y (snap_file.Y ());
		new_snap_file.Z (snap_file.Z ());
		new_snap_file.Bearing (snap_file.Bearing ());

		//---- save the cell ----

		lane = snap_file.Lane ();
		offset = Round (snap_file.Offset ());

		if (!compress_in) {
			if (snap_file.Version () > 40) {
				lane = Convert_Lane_ID (dir_ptr, lane);
			} else {
				lane--;
			}
			if (offset > link_ptr->Length ()) {
				offset = link_ptr->Length ();
			}
		}
		if (compress_out) {
			new_snap_file.Lane (lane);
			new_snap_file.Offset (Resolve (offset));

		} else {
			lane = Make_Lane_ID (dir_ptr, lane);

			new_snap_file.Lane (lane);
			new_snap_file.Offset (UnRound (offset));
		}
		if (!new_snap_file.Write ()) {
			exe->Error (String ("Writing %s") % new_snap_file.File_Type ());
		}

	}
	End_Progress ();
	snap_file.Close ();

	Print (2, String ("Number of %s Records = %d") % snap_file.File_Type () % Progress_Count ());
}
