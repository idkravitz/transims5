//*********************************************************
//	Get_Problem_Data.cpp - Read the Problem File
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Problem_Data
//---------------------------------------------------------

bool ArcPlan::Get_Problem_Data (Problem_File &file, Problem_Data &problem_rec, int partition)
{
	int dir_index, center;
	double side, offset;
	bool dir_flag;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Location_Data *loc_ptr;
	Trip_Index trip_index;

	if (Data_Service::Get_Problem_Data (file, problem_rec, partition)) {
		if (select_problems && !problem_range.In_Range (problem_rec.Problem ())) return (false);
		if (select_households && !hhold_range.In_Range (problem_rec.Household ())) return (false);
		if (problem_rec.Mode () < MAX_MODE && !select_mode [problem_rec.Mode ()]) return (false);
		if (select_purposes && !purpose_range.In_Range (problem_rec.Purpose ())) return (false);
		if (select_start_times && !start_range.In_Range (problem_rec.Start ())) return (false);
		if (select_end_times && !end_range.In_Range (problem_rec.End ())) return (false);
		if (select_origins && !org_range.In_Range (file.Origin ())) return (false);
		if (select_destinations && !des_range.In_Range (file.Destination ())) return (false);

		if (select_org_zones) {
			loc_ptr = &location_array [problem_rec.Origin ()];
			if (!org_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
		if (select_des_zones) {
			loc_ptr = &location_array [problem_rec.Destination ()];
			if (!des_zone_range.In_Range (loc_ptr->Zone ())) return (false);
		}
		if (select_travelers && !traveler_range.In_Range (problem_rec.Type ())) return (false);

		//---- check the selection records ----

		if (System_File_Flag (SELECTION)) {
			Select_Map_Itr sel_itr;

			sel_itr = select_map.Best (problem_rec.Household (), problem_rec.Person (), problem_rec.Tour (), problem_rec.Trip ());
			if (sel_itr == select_map.end ()) return (false);
		}

		//---- draw the problem record ----

		if (problem_out) {
			dir_index = problem_rec.Dir_Index ();
			if (dir_index < 0) return (false);

			dir_ptr = &dir_array [dir_index];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (select_facilities && !select_facility [link_ptr->Type ()]) return (false);

			arcview_problem.Copy_Fields (file);

			offset = UnRound (problem_rec.Offset ());

			if (lanes_flag && problem_rec.Lane () >= 0) {
				if (dir_ptr->Dir () == 1) {
					dir_flag = (link_ptr->AB_Dir () >= 0);
				} else {
					dir_flag = (link_ptr->BA_Dir () >= 0);
				}
				if (center_flag && !dir_flag) {
					center = dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right () + 1;
				} else {
					center = 1;
				}
				side = (2 + 2 * problem_rec.Lane () - center) * lane_width / 2.0;
			} else {
				if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
					side = link_offset;
				} else {
					side = 0.0;
				}
			}
			Link_Shape (link_ptr, dir_ptr->Dir (), arcview_problem, offset, 0, side);

			if (!arcview_problem.Write_Record ()) {
				Error (String ("Writing %s") % arcview_problem.File_Type ());
			}
			num_problem++;
		}

		//---- save the problem index ----

		if (plan_flag) {
			trip_index.Set (problem_rec.Household (), problem_rec.Person (), problem_rec.Tour (), problem_rec.Trip ());

			problem_map.insert (Trip_Map_Data (trip_index, problem_rec.Problem ()));
		}
	}
	return (false);
}
