//*********************************************************
//	Performance_Output.cpp - Output Interface Class
//*********************************************************

#include "Performance_Output.hpp"

//---------------------------------------------------------
//	Performance_Output constructor
//---------------------------------------------------------

Performance_Output::Performance_Output (void) : Static_Service ()
{
	Initialize ();
}

void Performance_Output::operator()()
{
	while (exe->io_barrier.Go ()) {
		Output_Check ();
		exe->io_barrier.Finish ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Performance_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Performance Output requires Simulator Base");
	}
	Control_Key performance_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_PERFORMANCE_FILE, "NEW_PERFORMANCE_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_FORMAT, "NEW_PERFORMANCE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_PERFORMANCE_TIME_FORMAT, "NEW_PERFORMANCE_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_INCREMENT, "NEW_PERFORMANCE_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_TIME_RANGE, "NEW_PERFORMANCE_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_LINK_RANGE, "NEW_PERFORMANCE_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_SUBAREA_RANGE, "NEW_PERFORMANCE_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_COORDINATES, "NEW_PERFORMANCE_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_VEH_TYPES, "NEW_PERFORMANCE_VEH_TYPES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_TURN_FLAG, "NEW_PERFORMANCE_TURN_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_FLOW_TYPE, "NEW_PERFORMANCE_FLOW_TYPE", LEVEL1, OPT_KEY, TEXT_KEY, "VEHICLES", FLOW_RANGE, NO_HELP },
		{ NEW_PERFORMANCE_LANE_FLOWS, "NEW_PERFORMANCE_LANE_FLOWS", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (performance_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Performance_Output::Read_Control (void)
{
	output_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output link delay data ----

	int i, num;

	num = exe->Highest_Control_Group (NEW_PERFORMANCE_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- process each file ----

		for (i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_PERFORMANCE_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->data_flag = false;
			output->flow_flag = false;
			output->turn_flag = false;

			exe->Break_Check (10);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_PERFORMANCE_FILE, i));

			output->file = new Performance_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the file format ----
			
			key = exe->Get_Control_String (NEW_PERFORMANCE_FORMAT, i);

			if (!key.empty ()) {
				output->file->Dbase_Format (key);
			}

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_PERFORMANCE_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- get the flow type ----

			key = exe->Get_Control_String (NEW_PERFORMANCE_FLOW_TYPE, i);
			if (!key.empty ()) {
				output->file->Flow_Units (Flow_Code (key));
			}

			//---- get the lane use flow flag ----

			key = exe->Get_Control_String (NEW_PERFORMANCE_LANE_FLOWS, i);
			if (!key.empty ()) {
				output->flow_flag = exe->Set_Control_Flag (NEW_PERFORMANCE_LANE_FLOWS, i);
			}
			output->file->Lane_Use_Flows (output->flow_flag);

			//---- get the turn flag ----

			key = exe->Get_Control_String (NEW_PERFORMANCE_TURN_FLAG, i);
			if (!key.empty ()) {
				output->turn_flag = exe->Set_Control_Flag (NEW_PERFORMANCE_TURN_FLAG, i);
			}
			output->file->Turn_Flag (output->turn_flag);

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			}

			//---- print the time format ----

			exe->Get_Control_Text (NEW_PERFORMANCE_TIME_FORMAT, i);

			//---- time increment ----

			output->time_range.Increment (exe->Get_Control_Time (NEW_PERFORMANCE_INCREMENT, i));

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_PERFORMANCE_TIME_RANGE, i));

			//---- get the link range ----

			key = exe->Get_Control_Text (NEW_PERFORMANCE_LINK_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->link_range.Add_Ranges (key);
			}

			//---- get the subarea range ----

			key = exe->Get_Control_Text (NEW_PERFORMANCE_SUBAREA_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->subarea_range.Add_Ranges (key);
			}

			//---- get the coordinates ----

			key = exe->Get_Control_String (NEW_PERFORMANCE_COORDINATES, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				String token;

				key.Split (token);
				output->x1 = Round (token.Double ());

				if (!key.Split (token)) goto coord_error;
				output->y1 = Round (token.Double ());

				if (!key.Split (token)) goto coord_error;
				output->x2 = Round (token.Double ());

				if (!key.Split (token)) goto coord_error;
				output->y2 = Round (token.Double ());

				if (output->x2 < output->x1) {
					int x = output->x2;
					output->x2 = output->x1;
					output->x1 = x;
				}
				if (output->y2 < output->y1) {
					int y = output->y2;
					output->y2 = output->y1;
					output->y1 = y;
				}
				output->coord_flag = true;
			} else {
				output->coord_flag = false;
			}

			//---- get the veh types ----

			key = exe->Get_Control_Text (NEW_PERFORMANCE_VEH_TYPES, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->veh_types.Add_Ranges (key);
			}

			//---- print the turn flag ----

			exe->Get_Control_Flag (NEW_PERFORMANCE_TURN_FLAG, i);

			//---- print the flow type ----

			exe->Get_Control_Text (NEW_PERFORMANCE_FLOW_TYPE, i);
		}
	}
	return (output_flag);

coord_error:
	exe->Error (String ("Output Link Delay Coordinate Data #%d") % i);
	return (false);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Performance_Output::Output_Check (void)
{
	if (!output_flag || exe->step <= exe->Model_Start_Time ()) return (false);;

	Output_Itr output;

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (output->time_range.In_Range (exe->step)) {
			if (!output->data_flag) {

				//---- initialize the link data ----

				Link_Perf_Data data_rec;
				Flow_Time_Data turn_rec;

				output->link_perf.assign (exe->dir_array.size (), data_rec);

				if (output->turn_flag) {
					output->turn_perf.assign (exe->connect_array.size (), turn_rec);
				}
				output->data_flag = true;

			} else {
				Link_Perf_Itr data_itr;

				//---- update the density data ----

				for (data_itr = output->link_perf.begin (); data_itr != output->link_perf.end (); data_itr++) {
					if (data_itr->Occupancy () > 0) {
						if (data_itr->Max_Density () < data_itr->Occupancy ()) {
							data_itr->Max_Density (data_itr->Occupancy ());
						}
						data_itr->Add_Density (data_itr->Occupancy ());
						data_itr->Occupancy (0);

						if (data_itr->Max_Queue () < data_itr->Stop_Count ()) {
							data_itr->Max_Queue (data_itr->Stop_Count ());
						}
						data_itr->Add_Queue (data_itr->Stop_Count ());
						data_itr->Stop_Count (0);
					}
				}

				//---- check the output time increment ----

				if (output->time_range.At_Increment (exe->step)) {
#ifdef MPI_EXE
					MPI_Processing (output);
#endif
					if (exe->Master ()) {
						Write_Summary (exe->step, &(*output));
					}
				}
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Summarize
//---------------------------------------------------------

void Performance_Output::Summarize (Travel_Step &step)
{
	if (!output_flag || step.Vehicle () < 0 || step.Traveler () < 0) return;

	int dir_index, turn_index, cell, cells, occupancy;
	double weight, vmt;
	Dtime vht;
	bool skip;

	Output_Itr output;
	Cell_Itr cell_itr;
	Sim_Leg_Itr leg_itr;

	//---- sum the data ----

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (!output->time_range.In_Range (exe->step)) continue;

		//---- get the vehicle type data ----

		weight = 1.0;

		if (output->file->Flow_Units () != VEHICLES || !output->veh_types.empty ()) {
			if (!output->veh_types.empty ()) {
				if (!output->veh_types.In_Range (step.veh_type_ptr->Type ())) continue;
			}
			if (output->file->Flow_Units () == PERSONS) {
				weight = step.sim_veh_ptr->Passengers ();
			} else if (output->file->Flow_Units () == PCE) {
				if (step.veh_type_ptr > 0) {
					weight = step.veh_type_ptr->PCE ();
				} else {
					weight = (int) step.sim_veh_ptr->size ();
				}
			}
		}

		//---- get the connection ID ----

		turn_index = -1;
						
		if (output->turn_flag) {
			Sim_Plan_Ptr plan_ptr = step.sim_traveler_ptr->plan_ptr;
			if (plan_ptr != 0) {
				leg_itr = plan_ptr->begin ();
				if (leg_itr != plan_ptr->end ()) {
					turn_index = leg_itr->Connect ();
				}
			}
		}

		//---- process each movement cell ----
	
		cells = (int) step.size ();
		vmt = 0;
		dir_index = -1;

		for (cell=0, cell_itr = step.begin (); cell <= cells; cell_itr++, cell++) {
			if (cell == cells || cell_itr->Index () != dir_index) {
				vht = DTOI (exe->param.step_size * weight);

				if (cells == 0) {
					dir_index = step.Dir_Index ();
				} else {
					vht = DTOI (vmt * vht / cells);
					if (vht < 1) vht = 1;
				}
				if (dir_index >= 0) {
					vmt *= exe->param.cell_size;
					skip = false;

					if (!output->link_range.empty () || output->coord_flag) {
						Dir_Data *dir_ptr = &exe->dir_array [dir_index];
						Link_Data *link_ptr = &exe->link_array [dir_ptr->Link ()];

						if (!output->link_range.empty ()) {
							if (!output->link_range.In_Range (link_ptr->Link ())) skip = true;
						}
						if (!skip && output->coord_flag) {
							Node_Data *node_ptr;

							node_ptr = &exe->node_array [link_ptr->Anode ()];
							if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
								node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {

								node_ptr = &exe->node_array [link_ptr->Bnode ()];

								if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
									node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {
									skip = true;
								}
							}
						}
					}
					if (!skip && !output->subarea_range.empty ()) {
						Sim_Dir_Ptr sim_dir_ptr = &exe->sim_dir_array [dir_index];
						Int2_Key sub_key = exe->part_array [sim_dir_ptr->Partition (cell)];

						if (!output->subarea_range.In_Range (sub_key.first)) skip = true;
					}
					if (!skip) {
						Link_Perf_Data *link_perf_ptr = &output->link_perf [dir_index];

						occupancy = Round (weight);

						link_perf_ptr->Add_Flow (vmt);
						link_perf_ptr->Add_Time (vht);
						link_perf_ptr->Add_Occupant (occupancy);

						if (step.Veh_Speed () == 0) {
							link_perf_ptr->Add_Stop (occupancy);
						}
						if (output->turn_flag && turn_index >= 0) {
							Flow_Time_Data *flow_time_ptr = &output->turn_perf [turn_index];

							flow_time_ptr->Add_Flow (vmt);
							flow_time_ptr->Add_Time (vht);
						}
					}
				}
				if (cell == cells) break;
				dir_index = cell_itr->Index ();
				vmt = 1.0;

				if (output->turn_flag && turn_index >= 0) {
					turn_index = leg_itr->Connect ();
					leg_itr++;
				}
			} else {
				vmt += 1.0;
			}
		}
	}
}

//---------------------------------------------------------
//	Cycle_Failure
//---------------------------------------------------------

void Performance_Output::Cycle_Failure (Dtime step, int dir_index, int vehicles, int persons, int veh_type)
{
	if (!output_flag || dir_index < 0 || vehicles < 0) return;

	Link_Perf_Data *data_ptr;
	Output_Itr output;

	//---- get the link data ----

	bool link_flag;
	Dir_Data *dir_ptr = 0;
	Link_Data *link_ptr = 0;

	//---- sum the data ----

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (!output->veh_types.empty () && veh_type > 0) {
			if (!output->veh_types.In_Range (veh_type)) continue;
		}
		if (!output->time_range.In_Range (step)) continue;

		link_flag = !output->link_range.empty ();

		if ((link_flag || output->coord_flag) && link_ptr == 0) {
			dir_ptr = &exe->dir_array [dir_index];
			link_ptr = &exe->link_array [dir_ptr->Link ()];
		}
		if (link_flag && !output->link_range.In_Range (link_ptr->Link ())) continue;

		if (output->coord_flag) {
			Node_Data *node_ptr;

			if (dir_ptr->Dir () == 0) {
				node_ptr = &exe->node_array [link_ptr->Bnode ()];
			} else {
				node_ptr = &exe->node_array [link_ptr->Anode ()];
			}
			if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
				node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) continue;
		}
		if (!output->subarea_range.empty ()) {
			Sim_Dir_Ptr sim_dir_ptr = &exe->sim_dir_array [dir_index];
			Int2_Key sub_key = exe->part_array [sim_dir_ptr->To_Part ()];

			if (!output->subarea_range.In_Range (sub_key.first)) continue;
		}

		//---- save the failure ----

		data_ptr = &output->link_perf [dir_index];

		if (output->file->Flow_Units () == PERSONS) {
			data_ptr->Add_Failure (persons);
		} else {
			data_ptr->Add_Failure (vehicles);
		}
	}
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Performance_Output::Write_Summary (Dtime step, Output_Data *output)
{
	int num, index, count;
	int i, link [20], increment;
	Dtime time [20];
	double flow [20], speed, ttime, length, rate, density, factor;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Flow_Time_Data *turn_perf_ptr;
	Link_Perf_Itr link_itr;
	Connect_Data *connect_ptr;

	increment = output->time_range.Increment ();
	if (increment < 1) increment = 1;

	//---- output the current increment ----

	for (num=0, link_itr = output->link_perf.begin (); link_itr != output->link_perf.end (); link_itr++, num++) {
		if (link_itr->Flow () == 0) continue;
			
		dir_ptr = &exe->dir_array [num];
		link_ptr = &exe->link_array [dir_ptr->Link ()];

		//---- calculate the performance ----

		if (link_itr->Time () > 0) {
			speed = link_itr->Flow () / link_itr->Time ();
			if (speed < 0.5) speed = 0.5;
		} else {
			speed = 0.5;
		}
		length = UnRound (link_ptr->Length ());
		if (length < 0.1) length = 0.1;

		ttime = length / speed;
		if (ttime < 0.1) ttime = 0.1;

		rate = UnRound (link_itr->Flow () / length);
		if (rate < 0.1) rate = 0.1;

		//---- save the link record ----

		output->file->Link (link_ptr->Link ());
		output->file->Dir (dir_ptr->Dir ());
		output->file->Start (step - increment);
		output->file->End (step);
		output->file->Flow (rate);
		output->file->Time (Dtime (ttime, SECONDS));
		output->file->Speed (speed);

		density = (double) link_itr->Density () / increment;
		if (density < 0.01) density = 0.01;

		factor = dir_ptr->Lanes ();
		if (factor < 1) factor = 1;

		if (exe->Metric_Flag ()) {
			factor = 1000.0 / (length * factor);
		} else {
			factor = MILETOFEET / (length * factor);
		}
		density *= factor;
		if (density > 0.0 && density < 0.01) density = 0.01;
		output->file->Density (density);

		density = UnRound (link_itr->Max_Density ()) * factor;
		if (density > 0.0 && density < 0.01) density = 0.01;
		output->file->Max_Density (density);

		output->file->Queue (UnRound (link_itr->Queue ()) / increment);
		output->file->Max_Queue (Resolve (link_itr->Max_Queue ()));
		output->file->Cycle_Failure (link_itr->Failure ());

		factor = dir_ptr->Time0 ().Seconds ();
		if (factor < 0.1) factor = 0.1;

		output->file->Delay (Dtime ((ttime - factor), SECONDS));
		output->file->Time_Ratio (100 * ttime / factor);

		//---- clear for the next time slice ----

		link_itr->Clear ();

		//---- gather the turn delays ----

		count = 0;

		if (output->turn_flag) {
			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &exe->connect_array [index];

				turn_perf_ptr = &output->turn_perf [index];
				if (turn_perf_ptr->Flow () == 0) continue;

				dir_ptr = &exe->dir_array [connect_ptr->To_Index ()];
				link_ptr = &exe->link_array [dir_ptr->Link ()];

				//---- calculate the performance ----

				if (turn_perf_ptr->Time () > 0) {
					speed = turn_perf_ptr->Flow () / turn_perf_ptr->Time ();
					if (speed < 0.5) speed = 0.5;
				} else {
					speed = 0.5;
				}
				ttime = length / speed;
				if (ttime < 0.1) ttime = 0.1;

				rate = UnRound (turn_perf_ptr->Flow () / length);
				if (rate < 0.1) rate = 0.1;

				link [count] = link_ptr->Link ();
				flow [count] = rate;
				time [count] = Dtime (ttime, SECONDS);
				count++;

				//--- clear for the next time slice ----

				turn_perf_ptr->Clear ();
			}
		}
		output->file->Num_Nest (count);

		if (!output->file->Write (false)) goto write_error;

		//---- save the turn delays ----

		for (i=0; i < count; i++) {
			output->file->Out_Link (link [i]);
			output->file->Out_Flow (flow [i]);
			output->file->Out_Time (time [i]);

			if (!output->file->Write (true)) goto write_error;
		}

		////---- calculate the performance ----


	}
	return;

write_error:
	exe->Error (String ("Writing %s") % output->file->File_Type ());
}

#ifdef MPI_EXE

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Performance_Output::MPI_Processing (Output_Itr output)
{
	int i, num, index, rank, link_size, turn_size, tag;

	Flow_Time_Data *turn_ptr, turn_rec;
	Flow_Time_Itr turn_itr;
	Link_Perf_Data *link_ptr, link_rec;
	Link_Perf_Itr link_itr;
	
	tag = (exe->Num_Threads () > 1) ? NEW_PERFORMANCE_FILE : 0;
	link_size = (int) sizeof (Link_Perf_Data);
	turn_size = (int) sizeof (Flow_Time_Data);

	if (exe->Master ()) {
		for (i=1; i < exe->MPI_Size (); i++) {

			//---- retrieve the data buffer ----

			rank = exe->Get_MPI_Buffer (data, tag);

			num = (int) data.Size () / (sizeof (int) + link_size);

			while (num-- > 0) {
				data.Get_Data (&index, sizeof (int));
				data.Get_Data (&link_rec, link_size);

				link_ptr = &output->link_perf [index];

				link_ptr->Add_Flow (link_rec.Flow ());
				link_ptr->Add_Time (link_rec.Time ());
				link_ptr->Add_Density (link_rec.Density ());
				link_ptr->Add_Max_Density (link_rec.Max_Density ());
				link_ptr->Add_Queue (link_rec.Queue ());
				link_ptr->Add_Max_Queue (link_rec.Max_Queue ());
				link_ptr->Add_Failure (link_rec.Failure ());
				link_ptr->Add_Occupant (link_rec.Occupancy ());
				link_ptr->Add_Stop (link_rec.Stop_Count ());
				link_ptr->Add_Ratio (link_rec.Ratio_Count ());
				link_ptr->Add_Ratio_VMT (link_rec.Ratio_VMT ());
				link_ptr->Add_Ratio_VHT (link_rec.Ratio_VHT ());
			}
			if (output->turn_flag) {
				exe->Get_MPI_Buffer (data, tag, rank);

				num = (int) data.Size () / (sizeof (int) + turn_size);

				while (num-- > 0) {
					data.Get_Data (&index, sizeof (int));
					data.Get_Data (&turn_rec, turn_size);

					turn_ptr = &output->turn_perf [index];

					turn_ptr->Add_Flow (turn_rec.Flow ());
					turn_ptr->Add_Time (turn_rec.Time ());
				}
			}
		}

	} else {	//---- slave ----

		data.Size (0);
			
		for (index=0, link_itr = output->link_perf.begin (); link_itr != output->link_perf.end (); link_itr++, index++) {
			if (link_itr->Time () > 0) {
				data.Add_Data (&index, sizeof (int));
				data.Add_Data (&(*link_itr), link_size);
				link_itr->Clear ();
			}
		}
		exe->Send_MPI_Buffer (data, tag);

		if (output->turn_flag) {
			data.Size (0);

			for (index=0, turn_itr = output->turn_perf.begin (); turn_itr != output->turn_perf.end (); turn_itr++, index++) {
				if (turn_itr->Time () > 0) {
					data.Add_Data (&index, sizeof (int));
					data.Add_Data (&(*turn_itr), turn_size);
					turn_itr->Clear ();
				}
			}
			exe->Send_MPI_Buffer (data, tag);
		}
	}
}
#endif

