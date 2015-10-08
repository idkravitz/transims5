//*********************************************************
//	Draw_Timings.cpp - draw signal timing plan records
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Draw_Timing_Plans
//---------------------------------------------------------

void ArcNet::Draw_Timing_Plans (void)
{
	Show_Message (String ("Writing %s -- Record") % arcview_timing.File_Type ());
	Set_Progress ();

	int signal_fld, signal, timing_fld, phase_fld, timing, phasing, phase;

	Signal_Data *signal_ptr;
	Int_Map_Itr map_itr;
	Signal_Time_Itr time_itr;
	Phasing_Itr phasing_itr;
	Movement_Itr move_itr;

	signal_fld = timing_db.Optional_Field (SIGNAL_FIELD_NAMES);
	timing_fld = timing_db.Required_Field ("TIMING", "TIMING_PLAN", "PLAN", "TIMING_ID");
	phase_fld = timing_db.Required_Field ("PHASE");

	timing_db.Rewind ();

	while (timing_db.Read_Record ()) {
		Show_Progress ();

		arcview_timing.Copy_Fields (timing_db);
		arcview_timing.parts.clear ();
		arcview_timing.clear ();

		signal = timing_db.Get_Integer (signal_fld);
		timing = timing_db.Get_Integer (timing_fld);
		phase = timing_db.Get_Integer (phase_fld);

		map_itr = signal_map.find (signal);
		if (map_itr == signal_map.end ()) continue;

		signal_ptr = &signal_array [map_itr->second];

		for (time_itr = signal_ptr->begin (); time_itr != signal_ptr->end (); time_itr++) {
			if (time_itr->Timing () == timing) break;
		}
		if (time_itr == signal_ptr->end ()) continue;

		//---- draw the phasing connections ----

		phasing = time_itr->Phasing ();

		for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
			if (phasing_itr->Phasing () == phasing && phasing_itr->Phase () == phase) {
				for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
					double side_in, side_out;
					Points_Itr pt_itr;

					Connect_Data *connect_ptr = &connect_array [move_itr->Connection ()];

					int dir_in = connect_ptr->Dir_Index ();
					int dir_out = connect_ptr->To_Index ();					

					if (lanes_flag) {
						int center_in, center_out;
						double width;

						Lane_Map_Array lane_map;
						Lane_Map_Itr map_itr;

						width = lane_width / 2.0;
						center_in = center_out = 1;

						if (center_flag) {
							Link_Data *link_ptr;
							Dir_Data *dir_ptr;

							//---- gather inbound data ----

							dir_ptr = &dir_array [dir_in];
							link_ptr = &link_array [dir_ptr->Link ()];

							if (link_ptr->BA_Dir () < 0 || link_ptr->AB_Dir () < 0) {
								center_in = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
							}

							//---- gather outbound data ----

							dir_ptr = &dir_array [dir_out];
							link_ptr = &link_array [dir_ptr->Link ()];

							if (link_ptr->BA_Dir () < 0 || link_ptr->AB_Dir () < 0) {
								center_out = dir_ptr->Left () + dir_ptr->Lanes () + dir_ptr->Right () + 1;
							}
						}
						
						//---- get the lane map ----

						Lane_Map (connect_ptr, lane_map);

						//---- draw each connection ----

						for (map_itr = lane_map.begin (); map_itr != lane_map.end (); map_itr++) {
							side_in = (2 + 2 * map_itr->In_Lane () - center_in) * width;
							side_out = (2 + 2 * map_itr->Out_Lane () - center_out) * width;

							Turn_Shape (dir_in, dir_out, points, turn_shape_setback, curve_flag, side_in, side_out);

							arcview_timing.parts.push_back ((int) arcview_timing.size ());

							for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
								arcview_timing.push_back (*pt_itr);
							}
						}
					} else {
						side_in = side_out = link_offset;

						Turn_Shape (dir_in, dir_out, points, turn_shape_setback, curve_flag, side_in, side_out);

						arcview_timing.parts.push_back ((int) arcview_timing.size ());

						for (pt_itr = points.begin (); pt_itr != points.end (); pt_itr++) {
							arcview_timing.push_back (*pt_itr);
						}
					}
				}
			}
		}
		if (arcview_timing.size () == 0) {
			Warning (String ("No Movements for Signal %d Phasing Plan %d Phase %d") % signal_ptr->Signal () % phasing % phase);
			continue;
		}
		if (!arcview_timing.Write_Record ()) {
			Error (String ("Writing %s") % arcview_timing.File_Type ());
		}
	}
	End_Progress ();
	arcview_timing.Close ();
}

