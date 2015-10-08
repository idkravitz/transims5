//*********************************************************
//	Traveler_Output.cpp - Output Interface Class
//*********************************************************

#include "Traveler_Output.hpp"

//---------------------------------------------------------
//	Traveler_Output constructor
//---------------------------------------------------------

Traveler_Output::Traveler_Output () : Static_Service ()
{
	Initialize ();
}

void Traveler_Output::operator()()
{
#ifdef MPI_EXE
	while (exe->io_barrier.Go ()) {
		MPI_Processing ();
		exe->io_barrier.Finish ();
	}
#else
 #ifdef BOOST_THREADS
	Traveler_Data data;

	while (traveler_queue.Get (data)) {
		Write_Traveler (data);
	}
 #endif
#endif
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Traveler_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;

	if (exe == 0) return;

	if (exe->Service_Level () < SIMULATOR_BASE) {
		exe->Error ("Traveler Output requires Simulator Base");
	}
	Control_Key traveler_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_TRAVELER_FILE, "NEW_TRAVELER_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRAVELER_FORMAT, "NEW_TRAVELER_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TRAVELER_ID_RANGE, "NEW_TRAVELER_ID_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", "> 1:0", NO_HELP },
		{ NEW_TRAVELER_TIME_FORMAT, "NEW_TRAVELER_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "DAY_TIME", TIME_FORMAT_RANGE, NO_HELP },
		{ NEW_TRAVELER_TIME_RANGE, "NEW_TRAVELER_TIME_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ NEW_TRAVELER_MODE_RANGE, "NEW_TRAVELER_MODE_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", MODE_RANGE, NO_HELP },
		{ NEW_TRAVELER_LINK_RANGE, "NEW_TRAVELER_LINK_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_TRAVELER_SUBAREA_RANGE, "NEW_TRAVELER_SUBAREA_RANGE", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ NEW_TRAVELER_COORDINATES, "NEW_TRAVELER_COORDINATES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", COORDINATE_RANGE, NO_HELP },
		END_CONTROL
	};
	exe->Key_List (traveler_keys);
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Traveler_Output::Read_Control (void)
{
	output_flag = false;
	if (exe == 0) return (output_flag);

	//---- read the output traveler data ----

	int i, num;

	num = exe->Highest_Control_Group (NEW_TRAVELER_FILE, 0);

	if (num != 0) {
		Output_Data out_data;
		Output_Itr output;
		String key;

		//---- process each file ----

		for (i=1; i <= num; i++) {

			if (!exe->Check_Control_Key (NEW_TRAVELER_FILE, i)) continue;

			output_array.push_back (out_data);
			output_flag = true;

			output = --(output_array.end ());

			output->coord_flag = false;

			exe->Break_Check (10);
			exe->Print (1);

			//---- get the file name ----

			key = exe->Project_Filename (exe->Get_Control_String (NEW_TRAVELER_FILE, i));

			output->file = new Traveler_File ();
			output->file->Filename (key);
			output->file->File_Type (exe->Current_Label ());

			//---- get the file format ----

			key = exe->Get_Control_String (NEW_TRAVELER_FORMAT, i);

			if (!key.empty ()) {
				output->file->Dbase_Format (key);
			}

			//---- traveler ID range ----

			key = exe->Get_Control_Text (NEW_TRAVELER_ID_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				if (key.find (":") != key.npos) {
					Strings ranges;
					String result, low_text, high_text;
					Str_Itr itr;
					Range_Data hhold, person;

					key.Parse (ranges);

					for (itr = ranges.begin (); itr != ranges.end (); itr++) {
						if (!itr->Range (low_text, high_text)) break;

						if (low_text.Split (result, ":")) {
							hhold.Low (result.Integer ());
							person.Low (low_text.Integer ());
						} else {
							hhold.Low (low_text.Integer ());
							person.Low (0);
						}
						if (high_text.Split (result, ":")) {
							hhold.High (result.Integer ());
							person.High (high_text.Integer ());
						} else {
							hhold.High (high_text.Integer ());
							person.High (MAX_INTEGER);
						}
						exe->hhold_range.push_back (hhold);
						exe->hhold_range.push_back (person);
					}
				} else {
					output->hhold_range.Add_Ranges (key);
				}
			}

			//---- get the time format ----

			key = exe->Get_Control_String (NEW_TRAVELER_TIME_FORMAT, i);
			if (!key.empty ()) {
				output->file->Time_Format (Time_Code (key));
			}

			//---- create the file ----

			if (exe->Master ()) {
				output->file->Create ();
			}

			//---- print the time format ----

			exe->Get_Control_Text (NEW_TRAVELER_TIME_FORMAT, i);

			//---- time ranges ----

			output->time_range.Add_Ranges (exe->Get_Control_Text (NEW_TRAVELER_TIME_RANGE, i));

			//---- mode range ----

			exe->Mode_Range_Key (NEW_TRAVELER_MODE_RANGE, output->mode, i);

			//---- get the link range ----

			key = exe->Get_Control_Text (NEW_TRAVELER_LINK_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->link_range.Add_Ranges (key);
			}

			//---- get the subarea range ----

			key = exe->Get_Control_Text (NEW_TRAVELER_SUBAREA_RANGE, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				output->subarea_range.Add_Ranges (key);
			}

			//---- get the coordinates ----

			key = exe->Get_Control_String (NEW_TRAVELER_COORDINATES, i);

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
		}
	}
	return (output_flag);

coord_error:
	exe->Error (String ("Output Traveler Coordinate Data #%d") % i);
	return (false);
}

//---------------------------------------------------------
//	In_Range
//---------------------------------------------------------

bool Traveler_Output::In_Range (Traveler_Data &data)
{
	if (!output_flag) return (false);

	Output_Itr output;
	
	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (data.Mode () < MAX_MODE && output->mode [data.Mode ()]) {
			if (output->time_range.In_Range (exe->step)) {
				if (output->hhold_range.empty ()) return (true);
				int index = output->hhold_range.In_Index (data.Household ());
				if (index >= 0) {
					if (output->person_range.empty ()) return (true);
					Range_Data *range_ptr = &output->person_range [index];
					if (data.Person () >= range_ptr->Low () && data.Person () <= range_ptr->High ()) {
						return (true);
					}
				}
			}
		}
	}
	return (false);
}

//---------------------------------------------------------
//	Output_Traveler
//---------------------------------------------------------

void Traveler_Output::Output_Traveler (Traveler_Data &traveler_data)
{
	if (!output_flag) return;

#ifdef MPI_EXE
	if (exe->Num_Threads () > 1) {
		boost::mutex::scoped_lock lock (data_mutex);
		data.Add_Data (&traveler_data, sizeof (traveler_data));
	} else {
		data.Add_Data (&traveler_data, sizeof (traveler_data));
	}
#else 
	if (exe->Num_Threads () > 1) {
 #ifdef BOOST_THREADS
		traveler_queue.Put (traveler_data);
 #endif
	} else {
		Write_Traveler (traveler_data);
	}
#endif
}

//---------------------------------------------------------
//	Write_Traveler
//---------------------------------------------------------

void Traveler_Output::Write_Traveler (Traveler_Data &data)
{
	if (!output_flag) return;

	Output_Itr output;
	Int_Itr veh_itr;
	
	for (output = output_array.begin (); output != output_array.end (); output++) {
		if (data.Mode () >= MAX_MODE || !output->mode [data.Mode ()]) continue;
		if (!output->time_range.In_Range (data.Time ())) continue;

		if (!output->hhold_range.empty ()) {
			int index = output->hhold_range.In_Index (data.Household ());
			if (index < 0) continue;
			if (!output->person_range.empty ()) {
				Range_Data *range_ptr = &output->person_range [index];
				if (data.Person () < range_ptr->Low () || data.Person () > range_ptr->High ()) continue;
			}
		}
		if (data.Dir_Index () >= 0) {
			Dir_Data *dir_ptr = &exe->dir_array [data.Dir_Index ()];
			Link_Data *link_ptr = &exe->link_array [dir_ptr->Link ()];

			if (!output->link_range.empty () && !output->link_range.In_Range (link_ptr->Link ())) continue;

			if (!output->subarea_range.empty ()) {
				Sim_Dir_Ptr sim_dir_ptr = &exe->sim_dir_array [data.Dir_Index ()];
				Int2_Key sub_key = exe->part_array [sim_dir_ptr->To_Part ()];

				if (!output->subarea_range.In_Range (sub_key.first)) continue;
			}
			if (output->coord_flag) {
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

				ax += (int) (bx * data.Offset () / length + 0.5);
				ay += (int) (by * data.Offset () / length + 0.5);

				if (output->coord_flag) {
					if (ax < output->x1 || ax > output->x2 ||
						ay < output->y1 || ay > output->y2) {
						continue;
					}
				}
			}
		}
		exe->Put_Traveler_Data (*output->file, data);
	}
}

//---------------------------------------------------------
//	End_Output
//---------------------------------------------------------

void Traveler_Output::End_Output (void)
{
#ifdef MPI_EXE
#else
 #ifdef BOOST_THREADS
	if (exe->Num_Threads () > 1) {
		traveler_queue.End_Queue ();
		traveler_queue.Exit_Queue ();
	}
 #endif
#endif
}

#ifdef MPI_EXE
//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Traveler_Output::MPI_Processing (void)
{
	if (!output_flag) return;
	
	int tag = (exe->Num_Threads () > 1) ? NEW_TRAVELER_FILE : 0;

	if (exe->Master ()) {
		int i, num, size;
		Traveler_Data traveler_data;

		size = sizeof (traveler_data);

		//---- save master problems ----

		num = (int) data.Size () / size;

		while (num-- > 0) {
			data.Get_Data (&traveler_data, size);
			Write_Traveler (traveler_data);
		}

		//---- process each slave ----

		for (i=1; i < exe->MPI_Size (); i++) {

			exe->Get_MPI_Buffer (data, tag);
			if (data.Size () == 0) continue;

			num = (int) data.Size () / size;

			while (num-- > 0) {
				data.Get_Data (&traveler_data, size);
				Write_Traveler (traveler_data);
			}
		}
		data.Size (0);

	} else {	//---- slave ----

		//---- send the data buffer ----

		exe->Send_MPI_Buffer (data, tag);
		data.Size (0);
	}
}
#endif
