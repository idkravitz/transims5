//*********************************************************
//	Get_Problem_Data.cpp - Read the Problem File
//*********************************************************

#include "ProblemSelect.hpp"

//---------------------------------------------------------
//	Initialize_Problems
//---------------------------------------------------------

void ProblemSelect::Initialize_Problems (Problem_File &file)
{
	Required_File_Check (file, LOCATION);
}

//---------------------------------------------------------
//	Get_Problem_Data
//---------------------------------------------------------

bool ProblemSelect::Get_Problem_Data (Problem_File &file, Problem_Data &problem_rec, int partition)
{
	Location_Data *loc_ptr;

	if (Data_Service::Get_Problem_Data (file, problem_rec, partition)) {
		max_problem++;

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

			if (sel_itr->second.Type () > 0) {
				sel_itr->second.Type (-sel_itr->second.Type ());
			} else if (sel_itr->second.Type () == 0) {
				sel_itr->second.Type (-1);
			}
		} else {
			Selection_Data selection_rec;
			Trip_Index trip_index;
			Select_Data select_data;
			Select_Map_Stat map_stat;

			selection_rec.Household (problem_rec.Household ());
			selection_rec.Person (problem_rec.Person ());
			selection_rec.Tour (problem_rec.Tour ());
			selection_rec.Trip (problem_rec.Trip ());

			selection_rec.Get_Trip_Index (trip_index);

			select_data.Type (problem_rec.Type ());
			select_data.Partition (problem_rec.Partition ());

			//---- process the record ----

			map_stat = select_map.insert (Select_Map_Data (trip_index, select_data));

			if (map_stat.second) {
				if (select_data.Partition () > select_map.Max_Partition ()) {
					select_map.Max_Partition (select_data.Partition ());
				}
				if (selection_rec.Household () > select_map.Max_Household ()) {
					select_map.Max_Household (selection_rec.Household ());
				}
			}
		}
		num_problem++;
	}
	return (false);
}
