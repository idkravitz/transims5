//*********************************************************
//	Occupancy_Output.cpp - Output Interface Class
//*********************************************************

#include "Occupancy_Output.hpp"

//---------------------------------------------------------
//	Occupancy_Output constructor
//---------------------------------------------------------

Occupancy_Output::Occupancy_Output () : Static_Service ()
{
	Initialize ();
}

void Occupancy_Output::operator()()
{
	while (exe->io_barrier.Go ()) {
		Output_Check ();
		exe->io_barrier.Finish ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Occupancy_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = total_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Occupancy Output requires Simulator Base");
	}
	Control_Key occupancy_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_OCCUPANCY_FILE, "NEW_OCCUPANCY_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_OCCUPANCY_FORMAT, "NEW_OCCUPANCY_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_OCCUPANCY_TIME_FORMAT, "NEW_OCCUPANCY_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_OCCUPANCY_INCREMENT, "NEW_OCCUPANCY_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "900 seconds", "1..3600 seconds", NO_HELP },
		{ NEW_OCCUPANCY_TIME_RANGE, "NEW_OCCUPANCY_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_OCCUPANCY_LINK_RANGE, "NEW_OCCUPANCY_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_OCCUPANCY_SUBAREA_RANGE, "NEW_OCCUPANCY_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_OCCUPANCY_COORDINATES, "NEW_OCCUPANCY_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
		{ NEW_OCCUPANCY_MAX_FLAG, "NEW_OCCUPANCY_MAX_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (occupancy_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Occupancy_Output::Read_Control (void)
{
	output_flag = total_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output occupancy data ----

	int i, num;

	num = exe->Highest_Control_Group (NEW_OCCUPANCY_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- process each file ----

		for (i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_OCCUPANCY_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->data_flag = false;
			output->coord_flag = false;
			output->max_flag = false;

			exe->Break_Check (10);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_OCCUPANCY_FILE, i));

			output->file = new Occupancy_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the file format ----
			
			key = exe->Get_Control_String (NEW_OCCUPANCY_FORMAT, i);

			if (!key.empty ()) {
				output->file->Dbase_Format (key);
			}

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_OCCUPANCY_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			}

			//---- print the time format ----

			exe->Get_Control_Text (NEW_OCCUPANCY_TIME_FORMAT, i);

			//---- time increment ----

			output->time_range.Increment (exe->Get_Control_Time (NEW_OCCUPANCY_INCREMENT, i));

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_OCCUPANCY_TIME_RANGE, i));

			//---- get the link range ----

			key = exe->Get_Control_Text (NEW_OCCUPANCY_LINK_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->link_range.Add_Ranges (key);
			}

			//---- get the subarea range ----

			key = exe->Get_Control_Text (NEW_OCCUPANCY_SUBAREA_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->subarea_range.Add_Ranges (key);
			}

			//---- get the coordinates ----

			key = exe->Get_Control_String (NEW_OCCUPANCY_COORDINATES, i);

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

			//---- print the max flag ----

			output->max_flag = exe->Get_Control_Flag (NEW_OCCUPANCY_MAX_FLAG, i);
			if (!output->max_flag) total_flag = true;
		}
	}
	return (output_flag);

coord_error:
	exe->Error (String ("Output Occupancy Coordinate Data #%d") % i);
	return (false);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Occupancy_Output::Output_Check (void)
{
	if (!output_flag || exe->step <= exe->Model_Start_Time ()) return (false);

	Output_Itr output;

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (output->time_range.In_Range (exe->step)) {
			if (!output->data_flag) {

				//---- initialize the link data ----

				int num_rec = (int) exe->dir_array.size ();
				Cell_Occ occ_rec;
				
				occ_rec.lanes = 0;
				occ_rec.cells = 0;
				occ_rec.total = 0;
				occ_rec.occ.clear ();

				output->occ_array.assign (num_rec, occ_rec);
				output->data_flag = true;
			
				if (output->max_flag) {
					int index;

					if (!output->link_range.empty () || output->coord_flag) {

						Dir_Itr dir_itr;
						Link_Data *link_ptr;
						Cell_Occ *occ_ptr;
						Sim_Dir_Data *sim_dir_ptr;

						for (index=0, dir_itr = exe->dir_array.begin (); dir_itr != exe->dir_array.end (); dir_itr++, index++) {
							link_ptr = &exe->link_array [dir_itr->Link ()];

							if (!output->link_range.empty ()) {
								if (!output->link_range.In_Range (link_ptr->Link ())) continue;
							}
							if (output->coord_flag) {
								Node_Data *node_ptr;

								node_ptr = &exe->node_array [link_ptr->Anode ()];
								if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
									node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {

									node_ptr = &exe->node_array [link_ptr->Bnode ()];

									if (node_ptr->X () < output->x1 || node_ptr->X () > output->x2 ||
										node_ptr->Y () < output->y1 || node_ptr->Y () > output->y2) {
										continue;
									}
								}
							}

							//---- allocate memory ----

							occ_ptr = &output->occ_array [index];
							sim_dir_ptr = &exe->sim_dir_array [index];

							occ_ptr->lanes = sim_dir_ptr->Lanes ();
							occ_ptr->cells = sim_dir_ptr->Cells ();

							occ_ptr->occ.assign ((occ_ptr->lanes * occ_ptr->cells), 0);
						}
					} else {
						Cell_Occ *occ_ptr;
						Sim_Dir_Itr sim_dir_itr;
							
						//---- allocate memory ----

						for (index=0, sim_dir_itr = exe->sim_dir_array.begin (); sim_dir_itr != exe->sim_dir_array.end (); sim_dir_itr++, index++) {
							occ_ptr = &output->occ_array [index];

							occ_ptr->lanes = sim_dir_itr->Lanes ();
							occ_ptr->cells = sim_dir_itr->Cells ();

							occ_ptr->occ.assign ((occ_ptr->lanes * occ_ptr->cells), 0);
						}
					}
				}
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

			//---- set the maximum values ----

			if (output->max_flag) {
				int index, lanes, lane, cell, total, vehicle;
				Cell_Occ_Itr occ_itr;
				Sim_Dir_Ptr sim_dir_ptr;
				Sim_Lane_Ptr sim_lane_ptr;
				Sim_Cap_Ptr sim_cap_ptr;
				Sim_Veh_Ptr sim_veh_ptr;
				Int_Itr int_itr;
				Cell_Itr cell_itr;

				for (index=0, occ_itr = output->occ_array.begin (); occ_itr != output->occ_array.end (); occ_itr++, index++) {
					if (occ_itr->lanes == 0) continue;

					sim_dir_ptr = &exe->sim_dir_array [index];
					if (!sim_dir_ptr->Active ()) continue;
					
					if (sim_dir_ptr->Method () == MACROSCOPIC) {
						sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

						if (occ_itr->total != 0) {
							total = 0;

							for (vehicle = sim_cap_ptr->First_Veh (); vehicle >= 0; vehicle = sim_veh_ptr->Follower ()) {
								sim_veh_ptr = exe->sim_veh_array [vehicle];
								if (sim_veh_ptr != 0) {
									total += (int) sim_veh_ptr->size ();
								}
							}
							if (total <= occ_itr->total) continue;
							occ_itr->occ.assign ((occ_itr->lanes * occ_itr->cells), 0);
						}
						total = 0;

						for (vehicle = sim_cap_ptr->First_Veh (); vehicle >= 0; vehicle = sim_veh_ptr->Follower ()) {
							sim_veh_ptr = exe->sim_veh_array [vehicle];
							if (sim_veh_ptr == 0) continue;

							for (cell_itr = sim_veh_ptr->begin (); cell_itr != sim_veh_ptr->end (); cell_itr++) {
								occ_itr->occ [cell_itr->Lane () * occ_itr->cells + cell_itr->Cell ()]++;
								total++;
							}
						}
						occ_itr->total = total;

					} else {	//---- mesoscopic link ----

						lanes = sim_dir_ptr->Lanes ();

						if (occ_itr->total != 0) {
							total = 0;

							for (lane=0; lane < lanes; lane++) {
								sim_lane_ptr = sim_dir_ptr->Lane (lane);

								for (cell=0, int_itr = sim_lane_ptr->begin (); int_itr != sim_lane_ptr->end (); int_itr++, cell++) {
									if (*int_itr >= 0) total++;
								}
							}
							if (total <= occ_itr->total) continue;
							occ_itr->occ.assign ((occ_itr->lanes * occ_itr->cells), 0);
						}
						total = 0;

						for (lane=0; lane < lanes; lane++) {
							sim_lane_ptr = sim_dir_ptr->Lane (lane);

							for (cell=0, int_itr = sim_lane_ptr->begin (); int_itr != sim_lane_ptr->end (); int_itr++, cell++) {
								if (*int_itr >= 0) {
									occ_itr->occ [lane * occ_itr->cells + cell]++;
									total++;
								}
							}
						}
						occ_itr->total = total;
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

void Occupancy_Output::Summarize (Travel_Step &step)
{
	if (!output_flag || !total_flag || step.Vehicle () < 0) return;
	
	int index, last_index, lane, cell, cells, occupancy;
	bool skip;

	Output_Itr output;
	Cell_Array *array_ptr;
	Cell_Itr cell_itr;
	Cell_Occ *occ_ptr;

	//---- sum the data ----

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (!output->time_range.In_Range (exe->step)) continue;
	
		occupancy = exe->param.step_size;
		cells = (int) step.size ();

		//---- stopped or one-cell vehicles ----

		if (cells == 0 || step.sim_veh_ptr->size () == 1) {
			last_index = -1;
			occ_ptr = 0;
			skip = false;

			if (cells == 0) {
				array_ptr = step.sim_veh_ptr;
			} else {
				array_ptr = &step;
				occupancy = DTOI ((double) occupancy / cells);
				if (occupancy < 1) occupancy = 1;
			}

			//---- process each cell position ----

			for (cell_itr = array_ptr->begin (); cell_itr != array_ptr->end (); cell_itr++) {
				cell_itr->Location (&index, &lane, &cell);
				if (index < 0 || lane < 0 || cell < 0) continue;

				if (index != last_index) {
					last_index = index;
					skip = false;

					//---- check the link selection criteria ----

					if (!output->link_range.empty () || output->coord_flag) {
						Dir_Data *dir_ptr = &exe->dir_array [index];
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
					if (!skip) {
						occ_ptr = &output->occ_array [index];

						//---- allocate memory ----

						if (occ_ptr->lanes == 0) {
							Sim_Dir_Data *sim_dir_ptr = &exe->sim_dir_array [index];

							occ_ptr->lanes = sim_dir_ptr->Lanes ();
							occ_ptr->cells = sim_dir_ptr->Cells ();

							occ_ptr->occ.assign ((occ_ptr->lanes * occ_ptr->cells), 0);
						}
					}
				}
				if (!skip) {

					//---- increment the occupancy counters ----

					occ_ptr->occ [lane * occ_ptr->cells + cell] += occupancy;
					occ_ptr->total += occupancy;
				}
			}
		} else {

			//---- multi-cell vehicle movement ----

			occupancy = DTOI ((double) occupancy / cells);
			if (occupancy < 1) occupancy = 1;
			occ_ptr = 0;
			skip = false;
			last_index = -1;

			for (cell_itr = step.begin (); cell_itr != step.end (); cell_itr++) {
				cell_itr->Location (&index, &lane, &cell);
				if (index < 0 || lane < 0 || cell < 0) continue;

				if (index != last_index) {
					skip = false;
					last_index = index;

					if (!output->link_range.empty () || output->coord_flag) {
						Dir_Data *dir_ptr = &exe->dir_array [index];
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
					if (!skip) {
						occ_ptr = &output->occ_array [index];

						//---- allocate memory ----

						if (occ_ptr->lanes == 0) {
							Sim_Dir_Data *sim_dir_ptr = &exe->sim_dir_array [index];

							occ_ptr->lanes = sim_dir_ptr->Lanes ();
							occ_ptr->cells = sim_dir_ptr->Cells ();

							occ_ptr->occ.assign ((occ_ptr->lanes * occ_ptr->cells), 0);
						}
					}
				}
				if (!skip) {

					//---- increment the occupancy counters ----

					occ_ptr->occ [lane * occ_ptr->cells + cell] += occupancy;
					occ_ptr->total += occupancy;
				}
			}
		}
	}
}

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void Occupancy_Output::Write_Summary (Dtime step, Output_Data *output)
{
	int index, offset, lane, cell, lane_off, occupancy;

	Cell_Occ_Itr occ_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
		
	output->file->Start (step - output->time_range.Increment ());
	output->file->End (step);

	for (index=0, occ_itr = output->occ_array.begin (); occ_itr != output->occ_array.end (); occ_itr++, index++) {
		if (occ_itr->lanes == 0 || occ_itr->total == 0) continue;

		dir_ptr = &exe->dir_array [index];
		link_ptr = &exe->link_array [dir_ptr->Link ()];

		if (output->file->LinkDir_Type () == LINK_SIGN) {
			output->file->Link (-link_ptr->Link ());
		} else {
			output->file->Link (link_ptr->Link ());

			if (output->file->LinkDir_Type () == LINK_DIR) {
				output->file->Dir (dir_ptr->Dir ());
			}
		}
		for (lane=0; lane < occ_itr->lanes; lane++) {
			output->file->Lane (exe->Make_Lane_ID (dir_ptr, lane));

			lane_off = lane * occ_itr->cells;

			for (cell=0; cell < occ_itr->cells; cell++) {
				occupancy = occ_itr->occ [lane_off + cell];
				if (!output->max_flag) occupancy = exe->Resolve (occupancy);

				if (occupancy > 0) {
					offset = (cell + 1) * exe->param.cell_size;
					if (offset > link_ptr->Length ()) offset = link_ptr->Length ();

					output->file->Offset (UnRound (offset));
					output->file->Occupancy (occupancy);

					if (!output->file->Write ()) {;
						exe->Error ("Writing Occupancy Output File");
					}
				}
			}
		}
		occ_itr->occ.assign ((occ_itr->lanes * occ_itr->cells), 0);
		occ_itr->total = 0;
	}
}


#ifdef MPI_EXE

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Occupancy_Output::MPI_Processing (Output_Itr output)
{
	int i, num, index, size, tag, lane_off, occupancy, *int_ptr;
	short lane, cell;

	Cell_Occ *occ_ptr;
	Cell_Occ_Itr occ_itr;
	
	tag = (exe->Num_Threads () > 1) ? NEW_OCCUPANCY_FILE : 0;
	size = (int) (2 * sizeof (int) + 2 * sizeof (short));

	if (exe->Master ()) {
		for (i=1; i < exe->MPI_Size (); i++) {

			//---- retrieve the data buffer ----

			exe->Get_MPI_Buffer (data, tag);

			num = (int) data.Size () / size;

			while (num-- > 0) {
				data.Get_Data (&index, sizeof (int));
				data.Get_Data (&lane, sizeof (short));
				data.Get_Data (&cell, sizeof (short));
				data.Get_Data (&occupancy, sizeof (int));

				occ_ptr = &output->occ_array [index];

				//---- allocate memory ----

				if (occ_ptr->lanes == 0) {
					Sim_Dir_Data *sim_dir_ptr = &exe->sim_dir_array [index];

					occ_ptr->lanes = sim_dir_ptr->Lanes ();
					occ_ptr->cells = sim_dir_ptr->Cells ();

					occ_ptr->occ.assign ((occ_ptr->lanes * occ_ptr->cells), 0);
				}				
				lane_off = lane * occ_ptr->cells + cell;
				int_ptr = &occ_ptr->occ [lane_off];

				*int_ptr += occupancy;
				occ_ptr->total += occupancy;
			}
		}

	} else {	//---- slave ----

		data.Size (0);

		for (index=0, occ_itr = output->occ_array.begin (); occ_itr != output->occ_array.end (); occ_itr++, index++) {
			if (occ_itr->lanes == 0 || occ_itr->total == 0) continue;

			for (lane=0; lane < occ_itr->lanes; lane++) {
				lane_off = lane * occ_itr->cells;

				for (cell=0; cell < occ_itr->cells; cell++) {
					occupancy = occ_itr->occ [lane_off + cell];

					if (occupancy > 0) {
						data.Add_Data (&index, sizeof (int));
						data.Add_Data (&lane, sizeof (short));
						data.Add_Data (&cell, sizeof (short));
						data.Add_Data (&occupancy, sizeof (int));
					}
				}
			}
			occ_itr->occ.assign ((occ_itr->lanes * occ_itr->cells), 0);
			occ_itr->total = 0;
		}
		exe->Send_MPI_Buffer (data, tag);
	}
}
#endif
