//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "IntControl.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void IntControl::Program_Control (void)
{
	int i, num, code, max, min_cap, max_cap;
	bool act_flag, cap_flag;

	String key;
	Strings split_text;
	Str_Itr str_itr;
	String_List text_list;
	Str_Itr list_itr;
	Signal_Group group_rec;
	Group_Map_Itr group_itr;
	Group_Map_Stat group_stat;

	//---- split method (LANES/CAPACITY/VOLUME/UPDATE) ----

	delete_flag = Check_Control_Key (DELETE_NODE_CONTROL_FILE);
	signal_flag = Check_Control_Key (System_File_ID (NEW_SIGNAL)) || Check_Control_Key (System_File_ID (NEW_TIMING_PLAN));

	if (signal_flag) {

		//---- scan for update and volume splits ----

		num = Highest_Control_Group (SIGNAL_TYPE_CODE, 0), 
		i = Highest_Control_Group (MINIMUM_PHASE_TIME, 0);
		if (i > num) num = i;

		if (num == 0) {
			Warning ("Signal Timing Control Parameters were Not Provided");
		}

		//---- process each file ----

		for (i=1; i <= num; i++) {

			if (!Check_Control_Key (SIGNAL_TYPE_CODE, i) && 
				!Check_Control_Key (MINIMUM_PHASE_TIME, i)) continue;

			key = Get_Control_String (SIGNAL_SPLIT_METHOD, i);
			key.Parse (split_text);

			for (str_itr = split_text.begin (); str_itr != split_text.end (); str_itr++) {
				code = Signal_Split_Code (*str_itr);
				if (code == UPDATE_SPLITS) update_flag = true;
				if (code == VOLUME_SPLITS) volume_flag = true;
			}
		}
	}

	if (update_flag && !delete_flag) {
		System_File_False (NEW_SIGNAL);
		System_File_False (NEW_PHASING_PLAN);
		System_File_False (NEW_DETECTOR);
	}

	//---- create the network files ----

	Data_Service::Program_Control ();

	sign_flag = System_File_Flag (NEW_SIGN);	
	signal_flag = System_File_Flag (NEW_SIGNAL) || System_File_Flag (NEW_TIMING_PLAN);

	if (!sign_flag && !signal_flag) {
		Error ("No Sign or Signal Output is Requested");
	}

	//---- open the delete node control file ----

	key = Get_Control_String (DELETE_NODE_CONTROL_FILE);

	if (!key.empty ()) {
		Print (1);
		delete_file.File_Type ("Delete Node Control File");

		if (!delete_file.Open (Project_Filename (key))) {
			File_Error ("Opening Delete Node Control File", key);
		}
		delete_flag = true;
	}

	//---- process sign keys ----

	if (sign_flag) {

		//---- open the input sign file ----

		key = Get_Control_String (INPUT_SIGN_FILE);

		if (!key.empty ()) {
			Print (1);
			sign_file.File_Type ("Input Sign File");

			if (!sign_file.Open (Project_Filename (key))) {
				File_Error ("Opening Input Sign File", key);
			}
			input_sign_flag = true;
		}

		//---- sign warning messages ----

		warning_flag = Get_Control_Flag (PRINT_SIGN_WARNINGS);

		if (!input_sign_flag && !System_File_Flag (SIGN) && !delete_flag) {
			Write (1);
			Warning ("Input Signs were Not Provided");
		}
	}

	//---- process signal keys ----

	if (!signal_flag) return;

	//---- open the input signal file ----

	key = Get_Control_String (INPUT_SIGNAL_FILE);

	if (!key.empty ()) {
		Print (1);
		signal_file.File_Type ("Input Signal File");

		if (!signal_file.Open (Project_Filename (key))) {
			File_Error ("Opening Input Signal File", key);
		}
		input_signal_flag = true;
	}

	//---- check the signal processing keys ----

	if (!input_signal_flag && !System_File_Flag (SIGNAL)) {
		Write (1);
		Warning ("Input Signals were Not Provided");
		Write (1, "\t\tThe Output Signal Files will be Empty");
		signal_flag = false;
		return;
	}
	if (update_flag) {
		if (!System_File_Flag (SIGNAL) || !System_File_Flag (TIMING_PLAN) ||
			!System_File_Flag (PHASING_PLAN)) {
			Error ("Signal, Timing, and Phasing files are Required for Timing Updates");
		}
		if (!System_File_Flag (NEW_TIMING_PLAN)) {
			Error ("A New Timing Plan is Required for Timing Updates");
		}
		regen_flag = !input_signal_flag;
	} else {
		if (!System_File_Flag (NEW_SIGNAL)) {
			Error ("A New Signal File is Required to Create Signals");
		}
		if (input_signal_flag) {
			if (!System_File_Flag (SIGNAL)) {
				if (delete_flag) {
					Error ("Signal File is Required for Signal Deletes");
				}
				if (System_File_Flag (TIMING_PLAN) || System_File_Flag (PHASING_PLAN) || 
					System_File_Flag (DETECTOR)) {
					Error ("Signal File is Required for Signal Edits");
				}
			} else if (!System_File_Flag (TIMING_PLAN) || !System_File_Flag (PHASING_PLAN)) {
				Error ("Timing and Phasing files are Required for Signal Changes");
			}
		} else if (System_File_Flag (TIMING_PLAN) || System_File_Flag (PHASING_PLAN)) {
			if (!delete_flag) {
				Error ("Conflict between Timing/Phasing files and No Input Signals");
			}
		} else if (delete_flag) {
			Error ("Timing and Phasing files are Required for Signal Deletes");
		} else {
			regen_flag = true;
		}
		if (!System_File_Flag (NEW_TIMING_PLAN) ||	!System_File_Flag (NEW_PHASING_PLAN)) {
			Error ("New Timing and Phasing Plan files are Required for Signal Processing");
		}
		if (!System_File_Flag (NEW_DETECTOR)) {
			Write (1);
			Warning ("If Actuated Signals are generated, a New Detector file is needed");
		}
	}
	detector_flag = System_File_Flag (NEW_DETECTOR);

	//---- sign merge messages ----

	merge_flag = Get_Control_Flag (PRINT_MERGE_WARNINGS);

	//---- get the number of signal groups ----

	num = Highest_Control_Group (SIGNAL_TYPE_CODE, 0), 
	i = Highest_Control_Group (MINIMUM_PHASE_TIME, 0);
	if (i > num) num = i;

	if (num == 0) {
		Warning ("Signal Timing Control Parameters were Not Provided");
	}

	//---- process each file ----

	for (i=1; i <= num; i++) {

		if (!Check_Control_Key (SIGNAL_TYPE_CODE, i) && 
			!Check_Control_Key (MINIMUM_PHASE_TIME, i)) continue;

		group_stat = group_map.insert (Group_Map_Data (i, group_rec));
		if (!group_stat.second) continue;

		group_itr = group_stat.first;
		act_flag = cap_flag = false;

		//---- get the signal type code ----

		Print (1);
		Get_Control_List (SIGNAL_TYPE_CODE, text_list, i);

		for (list_itr = text_list.begin (); list_itr != text_list.end (); list_itr++) {
			code = Signal_Code (*list_itr);
			if (code == ACTUATED) act_flag = true;
			group_itr->second.signal_type.push_back (code);
		}

		//---- get the number of rings ----

		Get_Control_List (NUMBER_OF_RINGS, group_itr->second.num_rings, i);

		//---- get the signal time breaks ----

		key = Get_Control_Text (SIGNAL_TIME_BREAKS, i);
		if (!key.empty ()) {
			group_itr->second.time_breaks.Add_Breaks (key);
		}

		//---- get the cycle length ----

		Get_Control_List (SIGNAL_CYCLE_LENGTH, group_itr->second.cycle_len, i);

		//---- get the minimum phase time ----

		Get_Control_List (MINIMUM_PHASE_TIME, group_itr->second.min_phase, i);

		//---- get the yellow phase time ----

		Get_Control_List (YELLOW_PHASE_TIME, group_itr->second.yellow, i);

		//---- get the red clear phase time ----

		Get_Control_List (RED_CLEAR_PHASE_TIME, group_itr->second.red_clear, i);

		//---- split method (LANES/CAPACITY/VOLUME/UPDATE) ----

		Get_Control_List (SIGNAL_SPLIT_METHOD, text_list, i);

		for (list_itr = text_list.begin (); list_itr != text_list.end (); list_itr++) {
			code = Signal_Split_Code (*list_itr);
			if (code == CAPACITY_SPLITS) cap_flag = true;
			group_itr->second.method.push_back (code);
		}

		//---- capacity range ----

		if (cap_flag) {
			Get_Control_List (MINIMUM_LANE_CAPACITY, group_itr->second.min_cap, i);
			Get_Control_List (MAXIMUM_LANE_CAPACITY, group_itr->second.max_cap, i);

			max = (int) MAX (group_itr->second.min_cap.size (), group_itr->second.max_cap.size ());

			for (code=0; code < max; code++) {
				min_cap = group_itr->second.min_cap.Best (code);
				max_cap = group_itr->second.max_cap.Best (code);

				if (min_cap > max_cap || min_cap < 0 || max_cap < 0) {
					Error (String ("Minimum/Maximum Capacity is Illegal (%d, %d)") % min_cap % max_cap);
				}
			}
		}

		//---- get the pocket lane factor ----

		Get_Control_List (POCKET_LANE_FACTOR, group_itr->second.pocket_factor, i);

		//---- get the shared lane factor ----

		Get_Control_List (SHARED_LANE_FACTOR, group_itr->second.shared_factor, i);

		//---- get the turn movement factor ----

		Get_Control_List (TURN_MOVEMENT_FACTOR, group_itr->second.turn_factor, i);

		//---- get the permitted left factor ----

		Get_Control_List (PERMITTED_LEFT_FACTOR, group_itr->second.permit_factor, i);

		//---- get the general factor ----

		Get_Control_List (GENERAL_GREEN_FACTOR, group_itr->second.general_factor, i);

		if (act_flag) {

			//---- get the extend green factor ----

			Get_Control_List (EXTENDED_GREEN_FACTOR, group_itr->second.extend_factor, i);

			//---- get the maximum green factor ----

			Get_Control_List (MAXIMUM_GREEN_FACTOR, group_itr->second.max_factor, i);

			//---- get the detector length ----

			group_itr->second.detector = Round (Get_Control_Double (SIGNAL_DETECTOR_LENGTH, i));
		}
	}

	//---- turn volume file ----

	key = Get_Control_String (TURN_VOLUME_FILE);

	if (!key.empty ()) {
		Print (1);
		turn_file.Open (Project_Filename (key));

		turn_flag = true;
	}

	//---- link delay file ----

	if (System_File_Flag (LINK_DELAY)) {
		if (turn_flag) {
			Error ("Use Link Delay or Turn Movements, but not both");
		}
		Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
		if (!file->Turn_Flag ()) {
			Error ("The Link Delay File does not include Turning Movements");
		}
		delay_flag = true;
	}
	if (turn_flag || delay_flag) {
		if (!volume_flag && !update_flag) {
			Warning ("Link Delay or Turn Volumes are used by Volume/Update Methods");
		}
	} else if (volume_flag || update_flag) {
		Error ("Volume or Update Methods require Link Delay or Turn Movements");
	}
	update_report = Report_Flag (TIMING_UPDATE);
} 
