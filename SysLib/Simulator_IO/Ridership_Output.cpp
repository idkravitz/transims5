//*********************************************************
//	Ridership_Output.cpp - Output Interface Class
//*********************************************************

#include "Ridership_Output.hpp"

//---------------------------------------------------------
//	Ridership_Output constructor
//---------------------------------------------------------

Ridership_Output::Ridership_Output (void) : Static_Service ()
{
	Initialize ();
}

void Ridership_Output::operator()()
{
	while (exe->io_barrier.Go ()) {
		Output_Check ();
		exe->io_barrier.Finish ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Ridership_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Ridership Output requires Simulator Base");
	}
	output_flag = false;

	Control_Key ridership_keys [] = { //--- code, key, level, status, type, help ----
		{ NEW_RIDERSHIP_FILE, "NEW_RIDERSHIP_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_RIDERSHIP_FORMAT, "NEW_RIDERSHIP_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_RIDERSHIP_TIME_FORMAT, "NEW_RIDERSHIP_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_RIDERSHIP_TIME_RANGE, "NEW_RIDERSHIP_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_RIDERSHIP_ROUTE_RANGE, "NEW_RIDERSHIP_ROUTE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_RIDERSHIP_ALL_STOPS, "NEW_RIDERSHIP_ALL_STOPS", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (ridership_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Ridership_Output::Read_Control (void)
{
	output_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output ridership data ----
	
	int num = exe->Highest_Control_Group (NEW_RIDERSHIP_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- check the transit file status ----

		if (!exe->System_File_Flag (TRANSIT_STOP) || !exe->System_File_Flag (TRANSIT_ROUTE) ||
			!exe->System_File_Flag (TRANSIT_SCHEDULE)) {
			exe->Print (1);
			exe->Warning ("Ridership Output Requires Transit Network Data");
			return (false);
		}

		//---- process each file ----

		for (int i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_RIDERSHIP_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->all_stops = false;

			exe->Break_Check (7);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_RIDERSHIP_FILE, i));

			output->file = new Ridership_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the file format ----
			
			key = exe->Get_Control_String (NEW_RIDERSHIP_FORMAT, i);

			if (!key.empty ()) {
				output->file->Dbase_Format (key);
			}

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_RIDERSHIP_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			}

			//---- print the time format ----

			exe->Get_Control_Text (NEW_RIDERSHIP_TIME_FORMAT, i);

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_RIDERSHIP_TIME_RANGE, i));

			//---- get the route range ----

			key = exe->Get_Control_Text (NEW_RIDERSHIP_ROUTE_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->route_range.Add_Ranges (key);
			}

			//---- get all stops flag ----

			output->all_stops = exe->Get_Control_Flag (NEW_RIDERSHIP_ALL_STOPS, i);
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Output_Check
//---------------------------------------------------------

bool Ridership_Output::Output_Check (void)
{
	if (!output_flag) return (false);;
	return (true);
}

//---------------------------------------------------------
//	Output
//---------------------------------------------------------

void Ridership_Output::Output (void)
{
	if (!output_flag) return;

	//---- validate the offset data ----

	int run, nrun, board, alight, load;
	Dtime time, schedule;
	double capacity;
	bool route_flag;

	Output_Itr output;
	Line_Run *run_ptr;
	Line_Stop_Itr stop_itr;
	Line_Itr line_itr;

	Stop_Data *stop_ptr;
	Veh_Type_Data *veh_type_ptr;

	for (output = output_array.begin (); output != output_array.end (); output++) {
		route_flag = !output->route_range.empty ();

		for (line_itr = exe->line_array.begin (); line_itr != exe->line_array.end (); line_itr++) {

			//---- check the route selection criteria -----

			if (route_flag) {
				if (!output->route_range.In_Range (line_itr->Route ())) continue;
			}
			output->file->Mode (line_itr->Mode ());
			output->file->Route (line_itr->Route ());

			//---- get the vehicle capacity ----

			veh_type_ptr = &exe->veh_type_array [line_itr->Type ()];

			capacity = veh_type_ptr->Capacity ();
			if (capacity > 1.0) {
				capacity = 1.0 / capacity;
			}

			//---- process each run stop ----

			stop_itr = line_itr->begin (); 

			nrun = (int) stop_itr->size ();

			for (run=0; run < nrun; run++) {
				load = 0;

				for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {

					run_ptr = &stop_itr->at (run);

					//---- check for riders ----
					
					board = run_ptr->Board ();
					alight = run_ptr->Alight ();

					load += board - alight;

					if (!output->all_stops && load == 0 && board == 0 && alight == 0) continue;

					//---- check the time range criteria ----

					time = run_ptr->Time ();
					schedule = run_ptr->Schedule ();

					if (time == 0 && schedule != 0) continue;

					if (!output->time_range.In_Range (time)) continue;

					//---- output the data record ----

					stop_ptr = &exe->stop_array [stop_itr->Stop ()];

					output->file->Run (run);
					output->file->Stop (stop_ptr->Stop ());
					output->file->Schedule (schedule);
					output->file->Time (time);
					output->file->Board (board);
					output->file->Alight (alight);
					output->file->Load (load);
					output->file->Factor (load * capacity);

					if (!output->file->Write ()) {
						exe->Error ("Writing Ridership Output File");
					}
				}
			}
		}
	}
}

