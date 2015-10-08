//*********************************************************
//	Turn_Vol_Output.cpp - Output Interface Class
//*********************************************************

#include "Turn_Vol_Output.hpp"

//---------------------------------------------------------
//	Turn_Vol_Output constructor
//---------------------------------------------------------

Turn_Vol_Output::Turn_Vol_Output (void) : Static_Service ()
{
	Initialize ();
}

void Turn_Vol_Output::operator()()
{
	while (exe->io_barrier.Go ()) {
		Output_Check ();
		exe->io_barrier.Finish ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Turn_Vol_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Turn Vol Output requires Simulator Base");
	}
	Control_Key turn_vol_keys [] = { //--- code, key, level, status, type, help ----
		{ NEW_TURN_VOLUME_FILE, "NEW_TURN_VOLUME_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_FORMAT, "NEW_TURN_VOLUME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TURN_VOLUME_FILTER, "NEW_TURN_VOLUME_FILTER", LEVEL1, OPT_KEY, INT_KEY, "0", ">= 0", NO_HELP },
		{ NEW_TURN_VOLUME_TIME_FORMAT, "NEW_TURN_VOLUME_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_INCREMENT, "NEW_TURN_VOLUME_INCREMENT", LEVEL1, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_TIME_RANGE, "NEW_TURN_VOLUME_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_NODE_RANGE, "NEW_TURN_VOLUME_NODE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_TURN_VOLUME_SUBAREA_RANGE, "NEW_TURN_VOLUME_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (turn_vol_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Turn_Vol_Output::Read_Control (void)
{
	output_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output turn volume data ----

	int num = exe->Highest_Control_Group (NEW_TURN_VOLUME_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- process each file ----

		for (int i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_TURN_VOLUME_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->data_flag = false;

			exe->Break_Check (10);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_TURN_VOLUME_FILE, i));

			output->file = new Turn_Vol_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the file format ----
			
			key = exe->Get_Control_String (NEW_TURN_VOLUME_FORMAT, i);

			if (!key.empty ()) {
				output->file->Dbase_Format (key);
			}

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_TURN_VOLUME_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			}

			//---- get the filter ----

			output->filter = exe->Get_Control_Integer (NEW_TURN_VOLUME_FILTER, i);

			//---- print the time format ----

			exe->Get_Control_Text (NEW_TURN_VOLUME_TIME_FORMAT, i);

			//---- time increment ----

			output->time_range.Increment (exe->Get_Control_Time (NEW_TURN_VOLUME_INCREMENT, i));

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_TURN_VOLUME_TIME_RANGE, i));

			//---- get the node range ----

			key = exe->Get_Control_Text (NEW_TURN_VOLUME_NODE_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->node_range.Add_Ranges (key);
			}

			//---- get the subarea range ----

			key = exe->Get_Control_Text (NEW_TURN_VOLUME_SUBAREA_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->subarea_range.Add_Ranges (key);
			}
		}
	}
	return (output_flag);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Turn_Vol_Output::Output_Check (void)
{
	if (!output_flag || exe->step <= exe->Model_Start_Time ()) return (false);

	Output_Itr output;

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (output->time_range.In_Range (exe->step)) {
			if (!output->data_flag) {

				//---- initialize the data ----

				output->data_flag = true;
			} else {

				//---- check the output time increment ----

				if (output->time_range.At_Increment (exe->step)) {
#ifdef MPI_EXE
					MPI_Processing (output);
#endif
					if (exe->Master ()) {
						Write_Turn (exe->step, &(*output));
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

void Turn_Vol_Output::Summarize (Travel_Step &step)
{
	if (!output_flag || step.size () < 1) return;
					
	int dir_index = step.Dir_Index ();
	int to_index = (--step.end ())->Index ();

	if (dir_index == to_index || dir_index < 0 || to_index < 0) return;

	int node = 0;

	Node_Data *node_ptr = 0;
	Output_Itr output;
	Sort_Key key;
	Turn_Map_Stat map_stat;

	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (!output->time_range.In_Range (exe->step)) continue;

		if (node == 0) {
			Dir_Data *dir_ptr = &dat->dir_array [dir_index];
			Link_Data *link_ptr = &dat->link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 1) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
			node_ptr = &dat->node_array [node];
			node = node_ptr->Node ();
		}
		if (!output->node_range.empty ()) {
			if (!output->node_range.In_Range (node)) continue;
		}
		if (!output->subarea_range.empty ()) {
			if (!output->subarea_range.In_Range (node_ptr->Subarea ())) continue;
		}
		key.node = node;
		key.dir_index = dir_index;
		key.to_index = to_index;

		map_stat = output->turn_map.insert (Turn_Map_Data (key, 1));

		if (!map_stat.second) {
			map_stat.first->second++;
		}
	}
}

//---------------------------------------------------------
//	Write_Turn
//---------------------------------------------------------

void Turn_Vol_Output::Write_Turn (int step, Output_Data *output)
{
	int node, dir_index, to_index, last_dir, last_to, link, to_link;	

	Turn_Map_Itr map_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	last_dir = last_to = link = to_link = -1;

	for (map_itr = output->turn_map.begin (); map_itr != output->turn_map.end (); map_itr++) {
		if (map_itr->second >= output->filter) {
			node = map_itr->first.node;
			dir_index = map_itr->first.dir_index;
			to_index = map_itr->first.to_index;

			if (dir_index != last_dir) {
				dir_ptr = &dat->dir_array [dir_index];
				link_ptr = &dat->link_array [dir_ptr->Link ()];
				link = link_ptr->Link ();
				last_dir = dir_index;
			}
			if (to_index != last_to) {
				dir_ptr = &dat->dir_array [to_index];
				link_ptr = &dat->link_array [dir_ptr->Link ()];
				to_link = link_ptr->Link ();
				last_to = to_index;
			}
			output->file->Node (node);
			output->file->Link (link);
			output->file->To_Link (to_link);

			output->file->Start (step - output->time_range.Increment ());
			output->file->End (step);

			output->file->Volume (map_itr->second);

			if (!output->file->Write ()) {
				exe->Error ("Writing Turn Volume File");
			}
		}
		map_itr->second = 0;
	}
}

bool operator < (Turn_Vol_Output::Sort_Key left, Turn_Vol_Output::Sort_Key right)
{
	if (left.node < right.node) return (true);
	if (left.node == right.node) {
		if (left.dir_index < right.dir_index) return (true);
		if (left.dir_index == right.dir_index) {
			if (left.to_index < right.to_index) return (true);
		}
	}
	return (false); 
}

#ifdef MPI_EXE

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Turn_Vol_Output::MPI_Processing (Output_Itr output)
{
	int i, num, lvalue [4], tag;

	Sort_Key key;
	Turn_Map_Stat map_stat;
	Turn_Map_Itr map_itr;
	
	tag = (exe->Num_Threads () > 1) ? NEW_TURN_VOLUME_FILE : 0;

	if (exe->Master ()) {
		for (i=1; i < exe->MPI_Size (); i++) {

			//---- retrieve the data buffer ----

			exe->Get_MPI_Buffer (data, tag);

			num = (int) data.Size ();
			if (num == 0) continue;

			num = num / sizeof (lvalue);

			while (num-- > 0) {
				data.Get_Data (lvalue, sizeof (lvalue));

				key.node = lvalue [0];
				key.dir_index = lvalue [1];
				key.to_index = lvalue [2];

				map_stat = output->turn_map.insert (Turn_Map_Data (key, lvalue [3]));

				if (!map_stat.second) {
					map_stat.first->second += lvalue [3];
				}
			}
		}

	} else {	//---- slave ----

		data.Size (0);

		for (map_itr = output->turn_map.begin (); map_itr != output->turn_map.end (); map_itr++) {
			if (map_itr->second >= output->filter) {
				lvalue [0] = map_itr->first.node;
				lvalue [1] = map_itr->first.dir_index;
				lvalue [2] = map_itr->first.to_index;
				lvalue [3] = map_itr->second;

				data.Add_Data (lvalue, sizeof (lvalue));
			}
			map_itr->second = 0;
		}

		//---- send the data buffer ----

		exe->Send_MPI_Buffer (data, tag);
	}
}
#endif
