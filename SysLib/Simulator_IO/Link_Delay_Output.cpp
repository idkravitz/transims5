//*********************************************************
//	Link_Delay_Output.cpp - Output Interface Class
//*********************************************************

#include "Link_Delay_Output.hpp"

//---------------------------------------------------------
//	Link_Delay_Output constructor
//---------------------------------------------------------

Link_Delay_Output::Link_Delay_Output (void) : Static_Service ()
{
	Initialize ();
}

void Link_Delay_Output::operator()()
{
	while (exe->io_barrier.Go ()) {
		Output_Check ();
		exe->io_barrier.Finish ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Link_Delay_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Link Delay Output requires Simulator Base");
	}
	Control_Key link_delay_keys [] = { //--- code, key, level, status, type, help ----
		{ NEW_LINK_DELAY_FILE, "NEW_LINK_DELAY_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_FORMAT, "NEW_LINK_DELAY_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_LINK_DELAY_TIME_FORMAT, "NEW_LINK_DELAY_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_INCREMENT, "NEW_LINK_DELAY_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_TIME_RANGE, "NEW_LINK_DELAY_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_LINK_RANGE, "NEW_LINK_DELAY_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_SUBAREA_RANGE, "NEW_LINK_DELAY_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_COORDINATES, "NEW_LINK_DELAY_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_VEH_TYPES, "NEW_LINK_DELAY_VEH_TYPES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_TURN_FLAG, "NEW_LINK_DELAY_TURN_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_FLOW_TYPE, "NEW_LINK_DELAY_FLOW_TYPE", LEVEL1, OPT_KEY, TEXT_KEY, "VEHICLES", FLOW_RANGE, NO_HELP },
		{ NEW_LINK_DELAY_LANE_FLOWS, "NEW_LINK_DELAY_LANE_FLOWS", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (link_delay_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Link_Delay_Output::Read_Control (void)
{
	output_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output link delay data ----

	int i, num;

	num = exe->Highest_Control_Group (NEW_LINK_DELAY_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- process each file ----

		for (i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_LINK_DELAY_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->data_flag = false;
			output->flow_flag = false;
			output->turn_flag = false;

			exe->Break_Check (10);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_LINK_DELAY_FILE, i));

			output->file = new Link_Delay_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the file format ----
			
			key = exe->Get_Control_String (NEW_LINK_DELAY_FORMAT, i);

			if (!key.empty ()) {
				output->file->Dbase_Format (key);
			}

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_LINK_DELAY_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- get the flow type ----

			key = exe->Get_Control_String (NEW_LINK_DELAY_FLOW_TYPE, i);
			if (!key.empty ()) {
				output->file->Flow_Units (Flow_Code (key));
			}

			//---- get the lane flow flag ----

			key = exe->Get_Control_String (NEW_LINK_DELAY_LANE_FLOWS, i);
			if (!key.empty ()) {
				output->flow_flag = exe->Set_Control_Flag (NEW_LINK_DELAY_LANE_FLOWS, i);
			}
			output->file->Lane_Use_Flows (output->flow_flag);

			//---- get the turn flag ----

			key = exe->Get_Control_String (NEW_LINK_DELAY_TURN_FLAG, i);
			if (!key.empty ()) {
				output->turn_flag = exe->Set_Control_Flag (NEW_LINK_DELAY_TURN_FLAG, i);
			}
			output->file->Turn_Flag (output->turn_flag);

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			}

			//---- print the time format ----

			exe->Get_Control_Text (NEW_LINK_DELAY_TIME_FORMAT, i);

			//---- time increment ----

			output->time_range.Increment (exe->Get_Control_Time (NEW_LINK_DELAY_INCREMENT, i));

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_LINK_DELAY_TIME_RANGE, i));

			//---- get the link range ----

			key = exe->Get_Control_Text (NEW_LINK_DELAY_LINK_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->link_range.Add_Ranges (key);
			}

			//---- get the subarea range ----

			key = exe->Get_Control_Text (NEW_LINK_DELAY_SUBAREA_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->subarea_range.Add_Ranges (key);
			}

			//---- get the coordinates ----

			key = exe->Get_Control_String (NEW_LINK_DELAY_COORDINATES, i);

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

			key = exe->Get_Control_Text (NEW_LINK_DELAY_VEH_TYPES, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->veh_types.Add_Ranges (key);
			}

			//---- print the turn flag ----

			exe->Get_Control_Flag (NEW_LINK_DELAY_TURN_FLAG, i);

			//---- print the flow type ----

			exe->Get_Control_Text (NEW_LINK_DELAY_FLOW_TYPE, i);

			//---- print the lane use flow flag ----

			exe->Get_Control_Flag (NEW_LINK_DELAY_LANE_FLOWS, i);

		}
	}
	return (output_flag);

coord_error:
	exe->Error (String ("Output Link Delay Coordinate Data #%d") % i);
	return (true);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Link_Delay_Output::Output_Check (void)
{
	if (!output_flag || exe->step <= exe->Model_Start_Time ()) return (false);

	Output_Itr output;

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (output->time_range.In_Range (exe->step)) {
			if (!output->data_flag) {

				//---- initialize the link data ----

				Flow_Time_Data data_rec;

				output->link_delay.assign (exe->dir_array.size (), data_rec);

				if (output->turn_flag) {
					output->turn_delay.assign (exe->connect_array.size (), data_rec);
				}
				output->data_flag = true;

			} else {
				
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

void Link_Delay_Output::Summarize (Travel_Step &step)
{
	if (!output_flag || step.Vehicle () < 0 || step.Traveler () < 0) return;

	int dir_index, turn_index, cell, cells;
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
						Flow_Time_Data *flow_time_ptr = &output->link_delay [dir_index];

						flow_time_ptr->Add_Flow (vmt);
						flow_time_ptr->Add_Time (vht);

						if (output->turn_flag && turn_index >= 0) {
							flow_time_ptr = &output->turn_delay [turn_index];

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
//	Write_Summary
//---------------------------------------------------------

void Link_Delay_Output::Write_Summary (Dtime step, Output_Data *output)
{
	int num, index, count;
	int i, link [20], increment;
	Dtime time [20];
	double flow [20], speed, ttime, length, rate;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Flow_Time_Data *flow_time_ptr;
	Flow_Time_Itr flow_time_itr;
	Connect_Data *connect_ptr;

	increment = output->time_range.Increment ();
	if (increment < 1) increment = 1;

	//---- output the current increment ----

	for (num=0, flow_time_itr = output->link_delay.begin (); flow_time_itr != output->link_delay.end (); flow_time_itr++, num++) {
		if (flow_time_itr->Flow () == 0) continue;
			
		dir_ptr = &exe->dir_array [num];
		link_ptr = &exe->link_array [dir_ptr->Link ()];

		//---- calculate the performance ----

		if (flow_time_itr->Time () > 0) {
			speed = flow_time_itr->Flow () / flow_time_itr->Time ();
			if (speed < 0.5) speed = 0.5;
		} else {
			speed = 0.5;
		}
		length = UnRound (link_ptr->Length ());
		if (length < 0.1) length = 0.1;

		ttime = length / speed;
		if (ttime < 0.1) ttime = 0.1;

		rate = UnRound (flow_time_itr->Flow () / length);
		if (rate < 0.1) rate = 0.1;

		//---- save the link record ----

		output->file->Link (link_ptr->Link ());
		output->file->Dir (dir_ptr->Dir ());
		output->file->Start (step - increment);
		output->file->End (step);
		output->file->Flow (rate);
		output->file->Time (Dtime (ttime, SECONDS));

		//---- gather the turn delays ----

		count = 0;

		if (output->turn_flag) {
			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &exe->connect_array [index];

				flow_time_ptr = &output->turn_delay [index];
				if (flow_time_ptr->Flow () == 0) continue;

				dir_ptr = &exe->dir_array [connect_ptr->To_Index ()];
				link_ptr = &exe->link_array [dir_ptr->Link ()];

				//---- calculate the performance ----

				if (flow_time_ptr->Time () > 0) {
					speed = flow_time_ptr->Flow () / flow_time_ptr->Time ();
					if (speed < 0.5) speed = 0.5;
				} else {
					speed = 0.5;
				}
				ttime = length / speed;
				if (ttime < 0.1) ttime = 0.1;

				rate = UnRound (flow_time_ptr->Flow () / length);
				if (rate < 0.1) rate = 0.1;

				link [count] = link_ptr->Link ();
				flow [count] = rate;
				time [count] = Dtime (ttime, SECONDS);
				count++;

				//--- clear for the next time slice ----

				flow_time_ptr->Clear ();
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

		//--- clear for the next time slice ----

		flow_time_itr->Clear ();
	}
	return;

write_error:
	exe->Error (String ("Writing %s") % output->file->File_Type ());
}

#ifdef MPI_EXE

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Link_Delay_Output::MPI_Processing (Output_Itr output)
{
	int i, num, index, rank, size, tag;

	Flow_Time_Data *data_ptr, data_rec;
	Flow_Time_Itr data_itr;
	
	tag = (exe->Num_Threads () > 1) ? NEW_LINK_DELAY_FILE : 0;
	size = (int) sizeof (Flow_Time_Data);

	if (exe->Master ()) {
		for (i=1; i < exe->MPI_Size (); i++) {

			//---- retrieve the data buffer ----

			rank = exe->Get_MPI_Buffer (data, tag);

			num = (int) data.Size () / (sizeof (int) + size);

			while (num-- > 0) {
				data.Get_Data (&index, sizeof (int));
				data.Get_Data (&data_rec, size);

				data_ptr = &output->link_delay [index];

				data_ptr->Add_Flow (data_rec.Flow ());
				data_ptr->Add_Time (data_rec.Time ());
			}
			if (output->turn_flag) {
				exe->Get_MPI_Buffer (data, tag, rank);

				num = (int) data.Size () / (sizeof (int) + size);

				while (num-- > 0) {
					data.Get_Data (&index, sizeof (int));
					data.Get_Data (&data_rec, size);

					data_ptr = &output->turn_delay [index];

					data_ptr->Add_Flow (data_rec.Flow ());
					data_ptr->Add_Time (data_rec.Time ());
				}
			}
		}

	} else {	//---- slave ----

		data.Size (0);
			
		for (index=0, data_itr = output->link_delay.begin (); data_itr != output->link_delay.end (); data_itr++, index++) {
			if (data_itr->Time () > 0) {
				data.Add_Data (&index, sizeof (int));
				data.Add_Data (&(*data_itr), size);
				data_itr->Clear ();
			}
		}
		exe->Send_MPI_Buffer (data, tag);

		if (output->turn_flag) {
			data.Size (0);

			for (index=0, data_itr = output->turn_delay.begin (); data_itr != output->turn_delay.end (); data_itr++, index++) {
				if (data_itr->Time () > 0) {
					data.Add_Data (&index, sizeof (int));
					data.Add_Data (&(*data_itr), size);
					data_itr->Clear ();
				}
			}
			exe->Send_MPI_Buffer (data, tag);
		}
	}
}
#endif
