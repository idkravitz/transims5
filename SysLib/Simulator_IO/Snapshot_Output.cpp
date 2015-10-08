//*********************************************************
//	Snapshot_Output.cpp - Output Interface Class
//*********************************************************

#include "Snapshot_Output.hpp"

//---------------------------------------------------------
//	Snapshot_Output constructor
//---------------------------------------------------------

Snapshot_Output::Snapshot_Output () : Static_Service ()
{
	Initialize ();
}

Snapshot_Output::~Snapshot_Output ()
{
	if (output_flag && exe->Master ()) {
		for (Output_Itr output = output_array.begin (); output != output_array.end (); output++) {
			if (output->compress && output->time_range.In_Range (exe->step) && output->time_range.At_Increment (exe->step)) {
				if (!output->file->Write_Index (exe->step, output->num_records)) {
					exe->Error ("Writing Snapshot Index File");
				}
			}
		}
	}
}

void Snapshot_Output::operator()()
{
	while (exe->io_barrier.Go ()) {
		Output_Check ();
		exe->io_barrier.Finish ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Snapshot_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Snapshot Output requires Simulator Base");
	}
	Control_Key snapshot_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_SNAPSHOT_FILE, "NEW_SNAPSHOT_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_FORMAT, "NEW_SNAPSHOT_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_SNAPSHOT_TIME_FORMAT, "NEW_SNAPSHOT_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_INCREMENT, "NEW_SNAPSHOT_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "900 seconds", SECOND_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_TIME_RANGE, "NEW_SNAPSHOT_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_LINK_RANGE, "NEW_SNAPSHOT_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_SUBAREA_RANGE, "NEW_SNAPSHOT_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_COORDINATES, "NEW_SNAPSHOT_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_MAX_SIZE, "NEW_SNAPSHOT_MAX_SIZE", LEVEL1, OPT_KEY, INT_KEY, "0", "0..4096", NO_HELP },
		{ NEW_SNAPSHOT_LOCATION_FLAG, "NEW_SNAPSHOT_LOCATION_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_CELL_FLAG, "NEW_SNAPSHOT_CELL_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_STATUS_FLAG, "NEW_SNAPSHOT_STATUS_FLAG", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_SNAPSHOT_COMPRESSION, "NEW_SNAPSHOT_COMPRESSION", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (snapshot_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Snapshot_Output::Read_Control (void)
{
	output_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output snapshot data ----

	int i, num;

	num = exe->Highest_Control_Group (NEW_SNAPSHOT_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- process each file ----

		for (i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_SNAPSHOT_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->coord_flag = false;
			output->size_flag = false;
			output->cell_flag = false;
			output->compress = false;
			output->max_size = 0;
			output->num_records = 0;

			exe->Break_Check (10);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_SNAPSHOT_FILE, i));

			output->file = new Snapshot_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_SNAPSHOT_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- check for compression ----

			output->compress = exe->Set_Control_Flag (NEW_SNAPSHOT_COMPRESSION, i);
			output->file->Compress_Flag (output->compress);

			if (output->compress) {
				output->file->Dbase_Format (BINARY);
			} else {

				//---- get the file format ----

				if (exe->Master ()) {
					key = exe->Get_Control_String (NEW_SNAPSHOT_FORMAT, i);

					if (!key.empty ()) {
						output->file->Dbase_Format (key);
					}
					if (output->file->Dbase_Format () != BINARY) {
						temp_file.Dbase_Format (BINARY);
						temp_file.Create_Fields ();
					}
				} else {
					output->file->Dbase_Format (BINARY);
				}

				//---- set the location flag ----

				output->file->Location_Flag (exe->Set_Control_Flag (NEW_SNAPSHOT_LOCATION_FLAG, i));

				//---- set the cell flag ----

				output->file->Cell_Flag (exe->Set_Control_Flag (NEW_SNAPSHOT_CELL_FLAG, i));

				//---- set the status flag ----

				output->file->Status_Flag (exe->Set_Control_Flag (NEW_SNAPSHOT_STATUS_FLAG, i));
			}

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			} else {
				output->file->Create_Fields ();
			}

			//---- print the time format ----

			exe->Get_Control_Text (NEW_SNAPSHOT_TIME_FORMAT, i);

			//---- time increment ----

			output->time_range.Increment (exe->Get_Control_Time (NEW_SNAPSHOT_INCREMENT, i));

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_SNAPSHOT_TIME_RANGE, i));

			//---- get the link range ----

			key = exe->Get_Control_Text (NEW_SNAPSHOT_LINK_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->link_range.Add_Ranges (key);
			}

			//---- get the subarea range ----

			key = exe->Get_Control_Text (NEW_SNAPSHOT_SUBAREA_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->subarea_range.Add_Ranges (key);
			}

			//---- get the coordinates ----

			key = exe->Get_Control_String (NEW_SNAPSHOT_COORDINATES, i);

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

			//---- get the max size ----

			output->max_size = exe->Get_Control_Integer (NEW_SNAPSHOT_MAX_SIZE, i);

			if (!output->compress) {
				output->max_size *= 1024 * 1024;

				//---- print the location flag ----

				exe->Get_Control_Flag (NEW_SNAPSHOT_LOCATION_FLAG, i);

				//---- print the cell flag ----

				output->cell_flag = exe->Get_Control_Flag (NEW_SNAPSHOT_CELL_FLAG, i);

				//---- print the status flag ----

				exe->Get_Control_Flag (NEW_SNAPSHOT_STATUS_FLAG, i);

			} else {
				output->max_size = (output->max_size * 1024 * 1024) / sizeof (Compressed_Snapshot);
				output->cell_flag = false;
			}

			//---- print the compress flag ----

			exe->Get_Control_Flag (NEW_SNAPSHOT_COMPRESSION, i);
		}
	}
	return (output_flag);

coord_error:
	exe->Error (String ("Output Snapshot Coordinate Data #%d") % i);
	return (false);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Snapshot_Output::Output_Check (void)
{
	if (!output_flag || exe->step <= exe->Model_Start_Time ()) return (false);

	bool first = true;
	Output_Itr output;
	Int_Itr veh_itr;

#ifdef MPI_EXE	
	int tag = (exe->Num_Threads () > 1) ? NEW_SNAPSHOT_FILE : 0;
#endif

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (output->size_flag) continue;
		if (output->time_range.In_Range (exe->step) && output->time_range.At_Increment (exe->step)) {

			int dir_index, offset, speed, lane, cell;

			Vehicle_Data *vehicle_ptr;
			Sim_Veh_Ptr sim_veh_ptr;
			Cell_Itr cell_itr;
			Dir_Data *dir_ptr;
			Link_Data *link_ptr;

			if (first) {
				first = false;
				exe->Active_Vehicles ();
			}
			if (exe->Master ()) {
				if (output->compress) {
					if (!output->file->Write_Index (exe->step, output->num_records)) {
						exe->Error ("Writing Snapshot Index File");
					}
				}
			} else {
				data.Size (0);
			}
			for (veh_itr = exe->active_vehicles.begin (); veh_itr != exe->active_vehicles.end (); veh_itr++) {
				sim_veh_ptr = exe->sim_veh_array [*veh_itr];
				if (sim_veh_ptr == 0) continue;

				cell_itr = sim_veh_ptr->begin ();
				if (cell_itr == sim_veh_ptr->end ()) continue;

				dir_index = cell_itr->Index ();
				if (dir_index < 0 || cell_itr->Lane () < 0) continue;

				dir_ptr = &exe->dir_array [dir_index];
				link_ptr = &exe->link_array [dir_ptr->Link ()];

				if (!output->link_range.empty () && !output->link_range.In_Range (link_ptr->Link ())) continue;

				if (!output->subarea_range.empty ()) {
					Sim_Dir_Ptr sim_dir_ptr = &exe->sim_dir_array [dir_index];
					Int2_Key sub_key = exe->part_array [sim_dir_ptr->Partition (cell_itr->Cell ())];

					if (!output->subarea_range.In_Range (sub_key.first)) continue;
				}
				vehicle_ptr = &exe->vehicle_array [*veh_itr];

				output->file->Household (vehicle_ptr->Household ());
				output->file->Vehicle (vehicle_ptr->Vehicle ());
				output->file->Time (exe->step);

				speed = sim_veh_ptr->Speed () * exe->param.cell_size;

				output->file->Speed (UnRound (speed));

				Veh_Type_Data *type_ptr = &exe->sim_type_array [sim_veh_ptr->Type ()];

				output->file->Type (type_ptr->Type ());
				output->file->Passengers (sim_veh_ptr->Passengers ());

				if (output->file->Status_Flag ()) {
					output->file->Wait (Resolve (sim_veh_ptr->Wait ()));
					output->file->Time_Diff (0);
					output->file->User (sim_veh_ptr->Priority ());

					if (sim_veh_ptr->Driver () >= 0) {
						MAIN_LOCK
						Sim_Traveler_Ptr sim_traveler_ptr = exe->sim_traveler_array [sim_veh_ptr->Driver ()];
						if (sim_traveler_ptr != 0) {
							Dtime schedule = sim_traveler_ptr->plan_ptr->Schedule ();
							Sim_Leg_Itr leg_itr = sim_traveler_ptr->plan_ptr->begin ();

							offset = (cell_itr->Cell () + 1) * exe->param.cell_size;
							if (offset > link_ptr->Length ()) offset = link_ptr->Length ();

							schedule += leg_itr->Time () * offset / link_ptr->Length ();
							schedule = exe->step - schedule;

							output->file->Time_Diff (DTOI (schedule.Seconds ()));
						}
						END_LOCK
					}
				}

				for (cell=0;; cell++) {
					if (output->file->LinkDir_Type () == LINK_SIGN) {
						output->file->Link (-link_ptr->Link ());
					} else {
						output->file->Link (link_ptr->Link ());

						if (output->file->LinkDir_Type () == LINK_DIR) {
							output->file->Dir (dir_ptr->Dir ());
						}
					}
					offset = (cell_itr->Cell () + 1) * exe->param.cell_size;
					if (offset > link_ptr->Length ()) offset = link_ptr->Length ();

					if (output->coord_flag || output->file->Location_Flag () || 
						output->file->LinkDir_Type () == LINK_NODE) {

						int ax, ay, bx, by;
						Node_Data *node_ptr;

						if (dir_ptr->Dir () == 0) {
							node_ptr = &exe->node_array [link_ptr->Bnode ()];
							bx = node_ptr->X ();
							by = node_ptr->Y ();
							node_ptr = &exe->node_array [link_ptr->Anode ()];
						} else {
							node_ptr = &exe->node_array [link_ptr->Anode ()];
							bx = node_ptr->X ();
							by = node_ptr->Y ();
							node_ptr = &exe->node_array [link_ptr->Bnode ()];
						}
						bx -= ax = node_ptr->X ();
						by -= ay = node_ptr->Y ();

						double length = link_ptr->Length ();

						ax += (int) (bx * offset / length + 0.5);
						ay += (int) (by * offset / length + 0.5);

						if (output->coord_flag) {
							if (ax < output->x1 || ax > output->x2 ||
								ay < output->y1 || ay > output->y2) {
								
								if (!output->cell_flag) break;
								continue;
							}
						}
						if (output->file->Location_Flag ()) {
							bx = exe->compass.Direction ((double) bx, (double) by);
							output->file->X (UnRound (ax));
							output->file->Y (UnRound (ay));
							output->file->Z (0.0);
							output->file->Bearing ((double) bx);
						}
						if (output->file->LinkDir_Type () == LINK_NODE) {
							output->file->Dir (node_ptr->Node ());
						}
					}

					//---- save the cell ----

					if (output->compress) {
						output->file->Lane (cell_itr->Lane ());
						output->file->Offset (Resolve (offset));
					} else {
						lane = exe->Make_Lane_ID (dir_ptr, cell_itr->Lane ());

						output->file->Cell (cell);
						output->file->Lane (lane);
						output->file->Offset (UnRound (offset));
					}
					if (exe->Master ()) {
						if (!output->file->Write ()) {
							exe->Error ("Writing Snapshot Output File");
						}
					} else {
						data.Add_Data (output->file->Record ());
					}
					output->num_records++;

					//---- next cell ----

					if (!output->cell_flag || ++cell_itr == sim_veh_ptr->end ()) break;

					dir_index = cell_itr->Index ();
					if (dir_index < 0 || cell_itr->Lane () < 0) break;

					dir_ptr = &exe->dir_array [dir_index];
					link_ptr = &exe->link_array [dir_ptr->Link ()];
				}

				//---- check the file size ----

				if (output->max_size > 0) {
					if (output->compress) {
						if (output->num_records >= output->max_size) {
							output->size_flag = true;
						}
						break;
					} else {
						if ((unsigned) output->file->Db_File::Offset () >= output->max_size) {
							output->size_flag = true;
						}
						break;
					}
				}
			}
#ifdef MPI_EXE
			if (exe->Master ()) {
				int i, rec_size;
				bool temp_flag;
				char *data_ptr;
				Buffer *rec_buffer;

				if (output->file->Dbase_Format () == BINARY) {
					temp_flag = false;
					rec_size = output->file->Record_Size ();
					rec_buffer = &output->file->Record ();
				} else {
					temp_flag = true;
					rec_size = temp_file.Record_Size ();
					rec_buffer = &temp_file.Record ();
				}
				data_ptr = rec_buffer->Data ();

				//---- retrieve data from each slave ----

				for (i=1; i < exe->MPI_Size (); i++) {
					exe->Get_MPI_Buffer (data, tag);

					int num = (int) data.Size ();
					if (num == 0) continue;

					num = num / rec_size;

					while (num-- > 0) {
						data.Get_Data (data_ptr, rec_size);

						if (temp_flag) {
							output->file->Copy_Fields (temp_file);
						}
						if (!output->file->Write ()) {
							exe->Error ("Writing Snapshot Output File");
						}
						output->num_records++;
					}
				}
			} else {
				exe->Send_MPI_Buffer (data, tag);
				data.Size (0);
			}			
#endif
		} else if (exe->Master () && output->compress && output->time_range.In_Range (exe->step-1) && output->time_range.At_Increment (exe->step)) {
			if (!output->file->Write_Index (exe->step, output->num_records)) {
				exe->Error ("Writing Snapshot Index File");
			}
		}
	}
	return (true);
}

