//*********************************************************
//	Simulator_Service.cpp - simulation service
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Simulator_Service constructor
//---------------------------------------------------------

Simulator_Service::Simulator_Service (void) : Router_Service (), Select_Service ()
{
	Service_Level (SIMULATOR_SERVICE);
	max_subarea = num_vehicles = 0;
	num_subareas = num_parts = num_sims = 1;
	first_part = last_part = -1;
	veh_status = 0;
	
#ifdef DEBUG_KEYS
	debug_flag = debug_time_flag = false;
#endif
	memset (&stats, '\0', sizeof (stats));
}

//---------------------------------------------------------
//	Simulator_Service destructor
//---------------------------------------------------------

Simulator_Service::~Simulator_Service (void)
{
	//if (veh_status != 0) {
	//	delete veh_status;
	//	veh_status = 0;
	//}
}

//---------------------------------------------------------
//	Simulator_Service_Keys
//---------------------------------------------------------

void Simulator_Service::Simulator_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SIMULATION_START_TIME, "SIMULATION_START_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0:00", "", NO_HELP },
		{ SIMULATION_END_TIME, "SIMULATION_END_TIME", LEVEL0, OPT_KEY, TIME_KEY, "24:00", "", NO_HELP },
		{ TIME_STEPS_PER_SECOND, "TIME_STEPS_PER_SECOND", LEVEL0, OPT_KEY, INT_KEY, "1", "1..10", NO_HELP },
		{ CELL_SIZE, "CELL_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 meters", "0, 3..10 meters", NO_HELP },
		{ PLAN_FOLLOWING_DISTANCE, "PLAN_FOLLOWING_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "526 meters", "0..2000 meters", NO_HELP },
		{ LOOK_AHEAD_DISTANCE, "LOOK_AHEAD_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "260 meters", "0..2000 meters", NO_HELP },
		{ LOOK_AHEAD_LANE_FACTOR, "LOOK_AHEAD_LANE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "4.0", "1.0..25.0", NO_HELP },
		{ LOOK_AHEAD_TIME_FACTOR, "LOOK_AHEAD_TIME_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..5.0", NO_HELP },
		{ LOOK_AHEAD_VEHICLE_FACTOR, "LOOK_AHEAD_VEHICLE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAXIMUM_SWAPPING_SPEED, "MAXIMUM_SWAPPING_SPEED", LEVEL0, OPT_KEY, FLOAT_KEY, "40 mps", "-1..40 mps", NO_HELP },
		{ MAXIMUM_SPEED_DIFFERENCE, "MAXIMUM_SPEED_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10 mps", "0..10 mps", NO_HELP },
		{ ENFORCE_PARKING_LANES, "ENFORCE_PARKING_LANES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ FIX_VEHICLE_LOCATIONS, "FIX_VEHICLE_LOCATIONS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ DRIVER_REACTION_TIME, "DRIVER_REACTION_TIME", LEVEL1, OPT_KEY, LIST_KEY, "1.0 seconds", "0.0..5.0 seconds", NO_HELP },
		{ PERMISSION_PROBABILITY, "PERMISSION_PROBABILITY", LEVEL1, OPT_KEY, LIST_KEY, "50 percent", "0..100 percent", NO_HELP },
		{ SLOW_DOWN_PROBABILITY, "SLOW_DOWN_PROBABILITY", LEVEL1, OPT_KEY, LIST_KEY, "0 percent", "0..100 percent", NO_HELP },
		{ SLOW_DOWN_PERCENTAGE, "SLOW_DOWN_PERCENTAGE", LEVEL1, OPT_KEY, LIST_KEY, "0 percent", "0..50 percent", NO_HELP },		
		{ MAX_COMFORTABLE_SPEED, "MAX_COMFORTABLE_SPEED", LEVEL1, OPT_KEY, LIST_KEY, "37.5 mps", "0, 15..40 mps", NO_HELP },
		{ TRAVELER_TYPE_FACTORS, "TRAVELER_TYPE_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..2.0", NO_HELP },
		{ PRIORITY_LOADING_TIME, "PRIORITY_LOADING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "60 seconds", "0..6500 seconds", NO_HELP },
		{ MAXIMUM_LOADING_TIME, "MAXIMUM_LOADING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "120 seconds", "0..6500 seconds", NO_HELP },
		{ PRIORITY_WAITING_TIME, "PRIORITY_WAITING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "60 seconds", "0..6500 seconds", NO_HELP },
		{ MAXIMUM_WAITING_TIME, "MAXIMUM_WAITING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "120 seconds", "0..6500 seconds", NO_HELP },
		{ MAX_DEPARTURE_TIME_VARIANCE, "MAX_DEPARTURE_TIME_VARIANCE", LEVEL0, OPT_KEY, TIME_KEY, "60 minutes", "0..180 minutes", NO_HELP },
		{ MAX_ARRIVAL_TIME_VARIANCE, "MAX_ARRIVAL_TIME_VARIANCE", LEVEL0, OPT_KEY, TIME_KEY, "60 minutes", "0..180 minutes", NO_HELP },
		{ RELOAD_CAPACITY_PROBLEMS, "RELOAD_CAPACITY_PROBLEMS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },		
		{ COUNT_PROBLEM_WARNINGS, "COUNT_PROBLEM_WARNINGS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ PRINT_PROBLEM_MESSAGES, "PRINT_PROBLEM_MESSAGES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UNSIMULATED_SUBAREAS, "UNSIMULATED_SUBAREAS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ MACROSCOPIC_SUBAREAS, "MACROSCOPIC_SUBAREAS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ MESOSCOPIC_SUBAREAS, "MESOSCOPIC_SUBAREAS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ MICROSCOPIC_SUBAREAS, "MICROSCOPIC_SUBAREAS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ TURN_POCKET_FACTOR, "TURN_POCKET_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.8", "0..1.0", NO_HELP },
		{ MERGE_POCKET_FACTOR, "MERGE_POCKET_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.6", "0..1.0", NO_HELP },
		{ OTHER_POCKET_FACTOR, "OTHER_POCKET_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.5", "0..1.0", NO_HELP },
		END_CONTROL
	};
	if (keys == 0) {
		Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				Error (String ("Simulator Service Key %d was Not Found") % keys [i]);
			}
		}
	}
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_TRAVELER_TYPES, SELECT_VEHICLE_TYPES, SELECT_PROBLEM_TYPES, 0
	};
	Select_Service_Keys (select_service_keys);
}

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator_Service::Program_Control (void)
{
	int i;
	String key;
	Double_List list;
	Dbl_Itr dbl_itr;

	Router_Service::Program_Control ();

	Read_Select_Keys ();

	memset (&param, '\0', sizeof (param));

	if (System_File_Flag (NEW_PROBLEM)) {
		param.problem_flag = true;
		Problem_File *problem_file = (Problem_File *) System_File_Handle (NEW_PROBLEM);
		problem_file->Simulator_Data ();
	}
	param.control_flag = (System_File_Flag (SIGNAL) && System_File_Flag (TIMING_PLAN) && System_File_Flag (PHASING_PLAN));
	
	if (!param.control_flag && (System_File_Flag (SIGNAL) || System_File_Flag (TIMING_PLAN) || 
		System_File_Flag (PHASING_PLAN) || System_File_Flag (DETECTOR))) {
			Warning ("Signal, Timing Plan, and Phasing Plan files are Required for Signal Processing");
	}
	if (!param.control_flag) {
		Warning ("Signal Processing is Inactive");
	}
	param.transit_flag = (System_File_Flag (TRANSIT_STOP) && System_File_Flag (TRANSIT_ROUTE) && 
					System_File_Flag (TRANSIT_SCHEDULE) && System_File_Flag (TRANSIT_DRIVER));

	if (!param.transit_flag && (System_File_Flag (TRANSIT_STOP) || System_File_Flag (TRANSIT_ROUTE) || 
		System_File_Flag (TRANSIT_SCHEDULE) || System_File_Flag (TRANSIT_DRIVER) || 
		System_File_Flag (TRANSIT_FARE))) {
			Warning ("Transit Stop, Route, Schedule and Driver files are Required for Transit Processing");
	}
	if (!param.transit_flag && System_File_Flag (TRANSIT_FARE)) {
		Warning ("Transit Processing is Inactive");
	}
	Print (2, String ("%s Control Keys:") % Program ());

	//---- simulation start and end time ----

	param.start_time_step = Get_Control_Time (SIMULATION_START_TIME);
	param.end_time_step = Get_Control_Time (SIMULATION_END_TIME);

	if (param.start_time_step == 0) param.start_time_step = Model_Start_Time ();
	if (param.end_time_step == 0 || !Check_Control_Key (SIMULATION_END_TIME)) {
		param.end_time_step = Model_End_Time ();
	}

	//---- time steps per second ----

	Print (1);
	param.steps_per_second = Get_Control_Integer (TIME_STEPS_PER_SECOND);

	param.step_size = Dtime (1, SECONDS);
	if (param.steps_per_second > 1) {
		param.step_size = param.step_size / param.steps_per_second;
		if (param.step_size < 1) param.step_size = 1;
	}

	//---- get the cell size ----

	param.cell_size = Round (Get_Control_Double (CELL_SIZE));

	//---- plan following distance ----

	Print (1);
	param.plan_follow = Get_Control_Integer (PLAN_FOLLOWING_DISTANCE);

	//---- look ahead distance ----

	param.look_ahead = Get_Control_Integer (LOOK_AHEAD_DISTANCE);

	param.look_ahead_flag = (param.look_ahead > 0);

	if (param.look_ahead_flag) {

		//---- look ahead lane factor ----

		param.lane_factor = Get_Control_Double (LOOK_AHEAD_LANE_FACTOR);

		//---- look ahead time factor ----

		param.time_factor = Get_Control_Double (LOOK_AHEAD_TIME_FACTOR);

		//---- look ahead vehicle factor ----

		param.veh_factor = Get_Control_Double (LOOK_AHEAD_VEHICLE_FACTOR);
	}

	//---- maximum swapping speed ----

	param.max_swap_speed = Round (Get_Control_Double (MAXIMUM_SWAPPING_SPEED));

	//---- maximum speed difference ----

	param.max_swap_diff = Round (Get_Control_Double (MAXIMUM_SPEED_DIFFERENCE));

	//---- enforce parking lanes -----

	param.parking_lanes = Get_Control_Flag (ENFORCE_PARKING_LANES);

	//---- fix vehicle locations -----

	param.fix_locations = Get_Control_Flag (FIX_VEHICLE_LOCATIONS);

	//---- driver reaction time ----

	param.reaction_flag = Get_Control_List_Groups (DRIVER_REACTION_TIME, list);
	
	for (i=0; i <= EXTERNAL; i++) {
		param.reaction_time [i] = list.Best (i);
	}

	//---- permission probability ----

	Get_Control_List_Groups (PERMISSION_PROBABILITY, list);

	param.permit_none = param.permit_all = true;

	for (i=0; i <= EXTERNAL; i++) {
		param.permission_prob [i] = list.Best (i) / 100.0;
		if (param.permission_prob [i] != 0.0) param.permit_none = false;
		if (param.permission_prob [i] != 1.0) param.permit_all = false;
	}

	//---- slow down probability ----

	param.slow_down_flag = Get_Control_List_Groups (SLOW_DOWN_PROBABILITY, list);

	for (i=0; i <= EXTERNAL; i++) {
		param.slow_down_prob [i] = list.Best (i) / 100.0;
	}

	//---- slow down percentage ----

	if (param.slow_down_flag) {
		Get_Control_List_Groups (SLOW_DOWN_PERCENTAGE, list);

		for (i=0; i <= EXTERNAL; i++) {
			param.slow_down_percent [i] = list.Best (i) / 100.0;
		}
	}

	//---- max comfortable speed ----

	Get_Control_List_Groups (MAX_COMFORTABLE_SPEED, param.comfort_speed, true);

	for (dbl_itr = param.comfort_speed.begin (); dbl_itr != param.comfort_speed.end (); dbl_itr++) {
		if (*dbl_itr != 0.0) {
			param.comfort_flag = true;
			break;
		}
	}

	//---- traveler type factors ----

	Get_Control_List_Groups (TRAVELER_TYPE_FACTORS, param.traveler_fac);

	param.traveler_flag = false;

	for (i=0; i < (int) param.traveler_fac.size (); i++) {
		if (param.traveler_fac [i] != 1.0) {
			param.traveler_flag = true;
		}
	}

	//---- priority loading time ----
	
	Print (1);
	param.min_load_time = Get_Control_Time (PRIORITY_LOADING_TIME);

	//---- maximum loading time ----

	param.max_load_time = Get_Control_Time (MAXIMUM_LOADING_TIME);

	if (param.min_load_time > param.max_load_time) {
		Error ("Priority Loading Time > Maximum Loading Time");
	}
	param.priority_load = (param.min_load_time + param.max_load_time) / 2;

	//---- priority waiting time ----

	param.min_wait_time = Get_Control_Time (PRIORITY_WAITING_TIME);

	//---- maximum waiting time ----

	param.max_wait_time = Get_Control_Time (MAXIMUM_WAITING_TIME);

	if (param.min_wait_time > param.max_wait_time) {
		Error ("Priority Waiting Time > Maximum Waiting Time");
	}
	param.priority_wait = (param.min_wait_time + param.max_wait_time) / 2;

	//---- max departure time variance ----

	Print (1);
	param.max_start_variance = Get_Control_Time (MAX_DEPARTURE_TIME_VARIANCE);

	//---- max arrival time variance ----

	param.max_end_variance = Get_Control_Time (MAX_ARRIVAL_TIME_VARIANCE);

	//---- problem related flags ----

	if (Check_Control_Key (RELOAD_CAPACITY_PROBLEMS) ||
		Check_Control_Key (COUNT_PROBLEM_WARNINGS) || 
		Check_Control_Key (PRINT_PROBLEM_MESSAGES)) {
		Print (1);
	}

	//---- reload capacity problems ----

	param.reload_problems = Get_Control_Flag (RELOAD_CAPACITY_PROBLEMS);
	
	//---- count problem warnings ----

	param.count_warnings = Get_Control_Flag (COUNT_PROBLEM_WARNINGS);

	if (select_problems && param.count_warnings) {
		for (i=1; i < MAX_PROBLEM; i++) {
			if (i != WAIT_PROBLEM && i != ARRIVAL_PROBLEM && i != DEPARTURE_PROBLEM) {
				if (problem_range.In_Range (i)) {
					param.count_warnings = false;
					break;
				}
			}
		}
	}

	//---- print problem messages ----

	param.print_problems = Get_Control_Flag (PRINT_PROBLEM_MESSAGES);

	//---- simulation methods ----

	Print (1);

	if (Check_Control_Key (UNSIMULATED_SUBAREAS) || 
		Check_Control_Key (MACROSCOPIC_SUBAREAS) || 
		Check_Control_Key (MICROSCOPIC_SUBAREAS)) {

		//---- unsimulated subareas ----

		key = Get_Control_Text (UNSIMULATED_SUBAREAS);

		if (!key.empty ()) {
			no_range.Add_Ranges (key);
		}

		//---- macroscopic subareas ----

		key = Get_Control_Text (MACROSCOPIC_SUBAREAS);

		if (!key.empty ()) {
			macro_range.Add_Ranges (key);
		}

		//---- mesoscopic subareas ----

		key = Get_Control_Text (MESOSCOPIC_SUBAREAS);

		if (!key.empty () && Check_Control_Key (MESOSCOPIC_SUBAREAS)) {
			meso_range.Add_Ranges (key);
		}

		//---- microscopic subareas ----

		key = Get_Control_Text (MICROSCOPIC_SUBAREAS);

		if (!key.empty ()) {
			micro_range.Add_Ranges (key);
		}

	} else {

		//---- default subareas ----

		key = exe->Get_Control_Text (MESOSCOPIC_SUBAREAS);

		if (!key.empty ()) {
			meso_range.Add_Ranges (key);
		}
	}

	//---- macroscopic capacity factors ----

	if (macro_range.size () > 0) {
		if (Check_Control_Key (TURN_POCKET_FACTOR) || 
			Check_Control_Key (MERGE_POCKET_FACTOR) || 
			Check_Control_Key (OTHER_POCKET_FACTOR)) {
			Print (1);
		}
		param.turn_factor = Get_Control_Double (TURN_POCKET_FACTOR);
		param.merge_factor = Get_Control_Double (MERGE_POCKET_FACTOR);
		param.other_factor = Get_Control_Double (OTHER_POCKET_FACTOR);
	}
}

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Simulator_Service::Execute (void)
{
	int i;

	Int2_Map_Itr part_itr;
	Int2_Key part_key;
	Node_Itr node_itr;

	Router_Service::Execute ();

	if (!Control_Flag ()) {
		Error ("Simulator_Service::Program_Control has Not Been Called");
	}
	Num_Subareas (max_subarea + 1);
	Num_Parts ((int) part_map.size ());
	Num_Simulators (Num_Parts ());

	First_Part (0);
	Last_Part (Num_Parts () - 1);

	param.subarea_method.assign (Num_Subareas (), NO_SIMULATION);

	for (i=0; i < Num_Subareas (); i++) {
		if (no_range.In_Range (i)) {
			param.subarea_method [i] = NO_SIMULATION;
		}
		if (macro_range.In_Range (i)) {
			param.subarea_method [i] = MACROSCOPIC;
		}
		if (meso_range.In_Range (i)) {
			param.subarea_method [i] = MESOSCOPIC;
		}
		if (micro_range.In_Range (i)) {
Write (1, "Microscopic Processing is Not Implemented");
			param.subarea_method [i] = MICROSCOPIC;
		}
	}
	Print (1);
	Write (1, "Number of Subareas = ") << Num_Subareas ();
	if (Num_Parts () != Num_Subareas ()) {
		Write (1, "Number of Partitions = ") << Num_Parts ();
	}

	//---- convert the partition numbers ----

	for (i=0, part_itr = part_map.begin (); part_itr != part_map.end (); part_itr++, i++) {
		part_itr->second = i;
		part_key = part_itr->first;
		part_array.push_back (part_key);
	}
	for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
		part_itr = part_map.find (Int2_Key (node_itr->Subarea (), node_itr->Partition ()));
		node_itr->Partition (part_itr->second);
	}
	Show_Message (1);
}

//---------------------------------------------------------
//	Global_Data
//---------------------------------------------------------

void Simulator_Service::Global_Data (void)
{
	int i, half_cell, use_code, cells, c0, c1, dir, index, in_off, out_off, bnode, lane, min_lane, max_lane;
	int part_a, part_b, met_b, partition_a, partition_b, method_a, method_b, transfer;
	
	Veh_Type_Itr type_itr;
	Veh_Type_Data type_rec;
	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Node_Itr node_itr;
	Node_Data *node_ptr;
	Location_Itr loc_itr;
	Parking_Itr park_itr;
	Sim_Park_Data park_rec, *sim_park_ptr;
	Sim_Dir_Data sim_dir_rec, *sim_dir_ptr;
	Connect_Data *connect_ptr;
	Sim_Connection sim_con_rec;
	Sim_Signal_Data sim_signal_rec;
	Pocket_Data *pocket_ptr;
	Int2_Key mpi_key;
	Vehicle_Data vehicle_rec;

	//---- set the vehicle cell size ----

	use_code = Use_Code ("CAR");

	if (param.cell_size == 0) {
		for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
			if ((type_itr->Use () & use_code) != 0) {
				param.cell_size = type_itr->Length ();
				break;
			}
		}
		if (param.cell_size == 0) {
			Error ("Car Length is Zero");
		}
		Print (2, "Vehicle Cell Size = ") << UnRound (param.cell_size) << ((Metric_Flag ()) ? " meters" : " feet");
	}
	half_cell = param.cell_size >> 1;

	//---- set the vehicle type attributes ----

	for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
		type_rec = *type_itr;
		type_rec.Length (MAX (((type_itr->Length () + half_cell) / param.cell_size), 1));
		type_rec.Max_Speed (MAX (((type_itr->Max_Speed () + half_cell) / param.cell_size), 1));
		type_rec.Max_Accel (MAX (((type_itr->Max_Accel () + half_cell) / param.cell_size), 1));
		type_rec.Max_Decel (MAX (((type_itr->Max_Decel () + half_cell) / param.cell_size), 1));
		sim_type_array.push_back (type_rec);
	}

	//---- create transit vehicles -----

	if (param.transit_flag) {
		Transit_Vehicles ();
	}

	//---- convert the distance/speed parameters to cells ----

	param.plan_follow = MAX (((Round (param.plan_follow) + half_cell) / param.cell_size), 1);
	param.look_ahead = MAX (((Round (param.look_ahead) + half_cell) / param.cell_size), 1);
	if (param.max_swap_speed > 0) {
		param.max_swap_speed = MAX (((Round (param.max_swap_speed) + half_cell) / param.cell_size), 1);
	}
	if (param.max_swap_diff > 0) {
		param.max_swap_diff = MAX (((Round (param.max_swap_diff) + half_cell) / param.cell_size), 1);
	}
	param.lane_change_levels = 10;
	param.change_priority = param.lane_change_levels / 3;
	param.connect_lane_weight = 4;
	param.lane_use_weight = 4;

	//---- allocate simulation network memory ----
	
	veh_status = new char [vehicle_array.size ()];

	sim_dir_array.assign (dir_array.size (), sim_dir_rec);
	sim_connection.assign (connect_array.size (), sim_con_rec);
	sim_signal_array.assign (signal_array.size (), sim_signal_rec);
	sim_veh_array.assign (vehicle_array.size (), 0);
	sim_park_array.assign (parking_array.size (), park_rec);

	if (Num_Parts () > 1) {
		location_part.assign (location_array.size (), 0);
	}

	//---- initialize sim_dir_data ----

	use_code = Use_Code ("CAR/TRUCK/BUS/RAIL");
	transfer = 0;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		if ((link_itr->Use () & use_code) == 0) continue;

		node_ptr = &node_array [link_itr->Anode ()];
		partition_a = node_ptr->Partition ();
		method_a = param.subarea_method.Best (node_ptr->Subarea ());

		node_ptr = &node_array [link_itr->Bnode ()];
		partition_b = node_ptr->Partition ();
		method_b = param.subarea_method.Best (node_ptr->Subarea ());

#ifdef MPI_EXE
		int rank_a = part_rank [partition_a];
		int rank_b = part_rank [partition_b];

		transfer = (rank_a != rank_b) ? 1 : 0;
#endif

		//---- process each direction ----

		for (dir=0; dir < 2; dir++) {

			if (dir) {
				index = link_itr->BA_Dir ();
				bnode = link_itr->Anode ();
				in_off = link_itr->Boffset ();
				out_off = link_itr->Aoffset ();
				part_a = partition_b;
				part_b = partition_a;
				met_b = method_a;
			} else {
				index = link_itr->AB_Dir ();
				bnode = link_itr->Bnode ();
				in_off = link_itr->Aoffset ();
				out_off = link_itr->Boffset ();
				part_a = partition_a;
				part_b = partition_b;
				met_b = method_b;
			}
			if (index < 0) continue;

			dir_ptr = &dir_array [index];
			sim_dir_ptr = &sim_dir_array [index];

			sim_dir_ptr->Speed (MAX (((dir_ptr->Speed () + half_cell) / param.cell_size), 1));
			sim_dir_ptr->Dir (dir);
			sim_dir_ptr->Type (link_itr->Type ());
			sim_dir_ptr->Turn (0);
			sim_dir_ptr->Cells (MAX (((link_itr->Length () + half_cell) / param.cell_size), 1));
			if (sim_dir_ptr->Cells () > 1) {
				sim_dir_ptr->In_Cell (MAX (((in_off + half_cell) / param.cell_size), 1));
				sim_dir_ptr->Out_Cell (MAX (((out_off + half_cell) / param.cell_size), 1));
			} else {
				sim_dir_ptr->In_Cell (0);
				sim_dir_ptr->Out_Cell (0);
			}
			sim_dir_ptr->Lanes (dir_ptr->Lanes () + dir_ptr->Left () + dir_ptr->Right ());
			sim_dir_ptr->Method (met_b);
			sim_dir_ptr->Transfer (transfer);
			sim_dir_ptr->From_Part (part_a);
			sim_dir_ptr->To_Part (part_b);

#ifdef MPI_EXE
			if (transfer) {
				if (dir) {
					mpi_key.first = rank_b;
					mpi_key.second = rank_a;
				} else {
					mpi_key.first = rank_a;
					mpi_key.second = rank_b;
				}
				mpi_exchange.insert (mpi_key);

				if (dir) {
					mpi_key.first = rank_a;
					mpi_key.second = rank_b;
				} else {
					mpi_key.first = rank_b;
					mpi_key.second = rank_a;
				}
				I2_Ints_Map_Itr i2_itr = mpi_boundary.find (mpi_key);

				if (i2_itr == mpi_boundary.end ()) {
					Integers ints;
					ints.push_back (index);
					mpi_boundary.insert (I2_Ints_Map_Data (mpi_key, ints));
				} else {
					i2_itr->second.push_back (index);
				}
			}
#endif
			//---- initialize the traffic controls ----

			node_ptr = &node_array [bnode];

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];

				if (node_ptr->Control () < 0) {
					if (dir_ptr->Sign () == STOP_SIGN || dir_ptr->Sign () == ALL_STOP) {
						connect_ptr->Control (STOP_GREEN);
					} else if (dir_ptr->Sign () == YIELD_SIGN) {
						connect_ptr->Control (PERMITTED_GREEN);
					} else {
						connect_ptr->Control (UNCONTROLLED);
					}
				} else {
					connect_ptr->Control (RED_LIGHT);
				}
			}
		}
	}

	//---- assign locations to partitions ----

	if (Num_Parts () > 1) {
		for (i=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, i++) {
			link_ptr = &link_array [loc_itr->Link ()];

			dir = (loc_itr->Offset () < link_ptr->Length () / 2) ? 1 : 0;

			if (loc_itr->Dir () == dir) {
				node_ptr = &node_array [link_ptr->Bnode ()];
			} else {
				node_ptr = &node_array [link_ptr->Anode ()];
			}
			location_part [i] = node_ptr->Partition ();
		}
	}

	//---- parking cell and partition number ----

	for (i=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, i++) {
		link_ptr = &link_array [park_itr->Link ()];

		sim_park_ptr = &sim_park_array [i];
		cells = MAX (((link_ptr->Length () + half_cell) / param.cell_size), 1);

		c0 = (park_itr->Offset () + half_cell) / param.cell_size;
		if (c0 >= cells) c0 = cells - 1;

		c1 = (link_ptr->Length () - park_itr->Offset () + half_cell) / param.cell_size;
		if (c1 >= cells) c1 = cells - 1;

		if (park_itr->Dir () == 0) {
			sim_park_ptr->Cell_AB (c0);
			sim_park_ptr->Cell_BA (c1);
		} else {
			sim_park_ptr->Cell_BA (c0);
			sim_park_ptr->Cell_AB (c1);
		}
		sim_park_ptr->Dir (park_itr->Dir ());
		sim_park_ptr->Type (park_itr->Type ());

		//---- set the access lanes for each direction ----

		for (dir=0; dir < 2; dir++) {
			if (dir) {
				index = link_ptr->BA_Dir ();
				c0 = sim_park_ptr->Cell_BA ();
			} else {
				index = link_ptr->AB_Dir ();
				c0 = sim_park_ptr->Cell_AB ();
			}
			if (index < 0) continue;

			dir_ptr = &dir_array [index];
			sim_dir_ptr = &sim_dir_array [index];

			min_lane = dir_ptr->Left ();
			max_lane = dir_ptr->Lanes () + dir_ptr->Left () - 1;

			for (index = dir_ptr->First_Pocket (); index >= 0; index = pocket_ptr->Next_Index ()) {
				pocket_ptr = &pocket_array [index];
				c1 = (pocket_ptr->Offset () + half_cell) / param.cell_size;

				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == RIGHT_TURN) {
					if (c0 < c1) continue;
				} else if (pocket_ptr->Type () == LEFT_MERGE || pocket_ptr->Type () == RIGHT_MERGE) {
					if (c0 > c1) continue;
				} else {
					if (c0 < c1) continue;
					c1 += (pocket_ptr->Length () + half_cell) / param.cell_size;
					if (c0 > c1) continue;
				}
				if (pocket_ptr->Type () == LEFT_TURN || pocket_ptr->Type () == LEFT_MERGE) {
					lane = dir_ptr->Left () - pocket_ptr->Lanes ();
					if (lane < min_lane) min_lane = lane;
				} else {
					lane = dir_ptr->Left () + dir_ptr->Lanes () + pocket_ptr->Lanes () - 1;
					if (lane > max_lane) max_lane = lane;
				}
			}
			if (sim_park_ptr->Type () != BOUNDARY) {
				if (sim_park_ptr->Dir () == dir) {
					min_lane = MAX (max_lane - 1, min_lane);
				} else {
					max_lane = MIN (min_lane + 1, max_lane);
				}
			}
			if (dir) {
				sim_park_ptr->Min_Lane_BA (min_lane);
				sim_park_ptr->Max_Lane_BA (max_lane);
				sim_park_ptr->Part_BA (sim_dir_ptr->Partition (c0));
			} else {
				sim_park_ptr->Min_Lane_AB (min_lane);
				sim_park_ptr->Max_Lane_AB (max_lane);
				sim_park_ptr->Part_AB (sim_dir_ptr->Partition (c0));
			}
		}
	}

	//---- transit plans ----

	if (param.transit_flag) {
		Transit_Plans ();
	}
}

//---------------------------------------------------------
//	Set_Parameters
//---------------------------------------------------------

void Simulator_Service::Set_Parameters (Sim_Parameters &p)
{
	memcpy (&p, &param, sizeof (param));
}

//---------------------------------------------------------
//	Add_Statistics
//---------------------------------------------------------

void Simulator_Service::Add_Statistics (Sim_Statistics &s)
{
	stats.tot_hours += s.tot_hours;
	stats.num_trips += s.num_trips;
	stats.num_vehicles += s.num_vehicles;
	stats.num_start += s.num_start;
	stats.num_end += s.num_end;
	stats.num_veh_trips += s.num_veh_trips;
	stats.num_veh_start += s.num_veh_start;
	stats.num_veh_end += s.num_veh_end;
	stats.num_veh_lost += s.num_veh_lost;
	stats.num_change += s.num_change;
	stats.num_swap += s.num_swap;
	stats.num_look_ahead += s.num_look_ahead;
	stats.num_slow_down += s.num_slow_down;
	stats.num_runs += s.num_runs;
	stats.num_run_start += s.num_run_start;
	stats.num_run_end += s.num_run_end;
	stats.num_run_lost += s.num_run_lost;
	stats.num_transit += s.num_transit;
	stats.num_board += s.num_board;
	stats.num_alight += s.num_alight;
}

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool Simulator_Service::Get_Node_Data (Node_File &file, Node_Data &node_rec)
{
	if (Data_Service::Get_Node_Data (file, node_rec)) {
		if (node_rec.Subarea () > max_subarea) max_subarea = node_rec.Subarea ();
		part_map.insert (Int2_Map_Data (Int2_Key (node_rec.Subarea (), node_rec.Partition ()), 0));
		return (true);
	}
	return (false);
}

//---------------------------------------------------------
//	Transit_Vehicles
//---------------------------------------------------------

void Simulator_Service::Transit_Vehicles (void)
{
	int i, j, k, k1, last_veh, route;
	String transit_veh;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Vehicle_Map_Itr vehicle_itr;
	Vehicle_Map_Stat map_stat;
	Vehicle_Index veh_index;
	Vehicle_Data vehicle_rec;
	Int_Map_Itr map_itr;

	//---- initialize the vehicle ID ----

	if (vehicle_map.size () > 0) {
		vehicle_itr = --vehicle_map.end ();
		last_veh = vehicle_itr->second;
	} else {
		last_veh = 0;
	}
	transit_id = line_array.Vehicle_ID (last_veh);

	transit_veh = (String ("%d") % transit_id);

	i = (int) transit_veh.size ();

	j = (int) (String ("%d") % line_array.Max_Runs ()).size ();
	j = i-j;

	for (k=j; k < i; k++) {
		transit_veh [k] = '#';
	}
	if (line_map.size () > 0) {
		map_itr = --line_map.end ();
		route = map_itr->first;
	} else {
		route = 0;
	}
	k1 = (int) (String ("%d") % route).size ();

	for (k=j-k1; k < j; k++) {
		transit_veh [k] = 'x';
	}
	Print (2, "Transit Vehicle ID = ") << transit_veh;
	Print (1, "Number of Transit Vehicles = ") << line_array.Num_Runs ();

	//---- create a vehicle for each transit run ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {

		stop_itr = line_itr->begin ();

		//---- set vehicle data ----

		vehicle_rec.Type (line_itr->Type ());
		vehicle_rec.Parking (stop_itr->Stop ());
		vehicle_rec.Vehicle (0);
		veh_index.Vehicle (0);

		//---- create the transit vehicles ----

		for (i=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, i++) {

			vehicle_rec.Household (line_array.Vehicle_ID (line_itr->Route (), i));
			veh_index.Household (vehicle_rec.Household ());


			//---- process the record ----

			map_stat = vehicle_map.insert (Vehicle_Map_Data (veh_index, (int) vehicle_array.size ()));

			if (!map_stat.second) {
				Warning (String ("Duplicate Transit Vehicle = %d-%d") % line_itr->Route () % (i + 1));
			} else {
				vehicle_array.push_back (vehicle_rec);
			}
		}
		if (i == 0) {
			Warning (String ("Transit Route %d has No Scheduled Service") % line_itr->Route ());
		}
	}
}

//---------------------------------------------------------
//	Transit_Plans
//---------------------------------------------------------

void Simulator_Service::Transit_Plans (void)
{
	int i, j, index, offset, half_cell, last_cell, next_cell;
	Dtime time, dwell;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Driver_Itr driver_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Stop_Data *stop_ptr;
	Stop_Itr st_itr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Plan_Ptr plan_ptr;
	Sim_Leg_Data leg_rec;
	Sim_Leg_RItr leg_itr, last_itr;
	Vehicle_Map_Itr veh_map_itr;
	Vehicle_Index veh_index;
	Veh_Type_Data *veh_type_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Connect_Data *connect_ptr;
	Sim_Stop_Data sim_stop_rec, *sim_stop_ptr;

	//---- convert stop offsets to cells ----

	half_cell = param.cell_size >> 1;

	sim_stop_array.assign (stop_array.size (), sim_stop_rec);

	for (i=0, st_itr = stop_array.begin (); st_itr != stop_array.end (); st_itr++, i++) {
		link_ptr = &link_array [st_itr->Link ()];
		offset = st_itr->Offset ();

		if (st_itr->Dir () == 1) {
			index = link_ptr->BA_Dir ();
		} else {
			index = link_ptr->AB_Dir ();
		}
		if (index == 0) {
			Warning (String ("Link %d is Not Available in the %s Direction") % link_ptr->Link () % ((st_itr->Dir () == 0) ? "AB" : "BA"));
			continue;
		}
		dir_ptr = &dir_array [index];
		sim_dir_ptr = &sim_dir_array [index];

		sim_stop_ptr = &sim_stop_array [i];
	
		offset = MIN (MAX (((offset + half_cell) / param.cell_size), sim_dir_ptr->In_Cell ()), sim_dir_ptr->Cell_Out ());

		sim_stop_ptr->Cell (offset);
		sim_stop_ptr->Partition (sim_dir_ptr->Partition (offset));

		sim_stop_ptr->Min_Lane (dir_ptr->Lanes () + dir_ptr->Left () - 1);
		sim_stop_ptr->Max_Lane (sim_stop_ptr->Min_Lane () + dir_ptr->Right ());
	}

	//---- process each transit route ----
	
	transit_plans.assign (line_array.size (), 0);

	leg_rec.Mode (DRIVE_MODE);

	for (i=0, line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++, i++) {
		if (line_itr->size () == 0 || line_itr->begin ()->size () == 0) continue;

		//---- create the transit plan ----

		transit_plans [i] = plan_ptr = new Sim_Plan_Data ();

		plan_ptr->Mode (TRANSIT_MODE);
		plan_ptr->Household (line_array.Vehicle_ID (line_itr->Route (), 0));
		plan_ptr->Person (0);

		veh_index.Household (plan_ptr->Household ());
		veh_index.Vehicle (0);

		veh_map_itr = vehicle_map.find (veh_index);

		if (veh_map_itr == vehicle_map.end ()) {
			Warning (String ("Transit Route %d Vehicle was Not Found") % line_itr->Route ());
			continue;
		}
		plan_ptr->Vehicle (veh_map_itr->second);

		veh_type_ptr = &sim_type_array [line_itr->Type ()];
		
		dwell = (veh_type_ptr->Min_Dwell () + veh_type_ptr->Max_Dwell ()) / 2;
		if (dwell < Dtime (2, SECONDS)) dwell.Seconds (2);

		time = 0;
		last_cell = 0;
		driver_itr = line_itr->driver_array.begin ();

		for (j=0, stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++, j++) {
			stop_ptr = &stop_array [stop_itr->Stop ()];

			sim_stop_ptr = &sim_stop_array [stop_itr->Stop ()];
			next_cell = sim_stop_ptr->Cell ();
			if (j == 0) {
				plan_ptr->Partition (sim_stop_ptr->Partition ());
			}
			link_ptr = &link_array [stop_ptr->Link ()];

			if (stop_ptr->Dir () == 0) {
				index = link_ptr->AB_Dir ();
			} else {
				index = link_ptr->BA_Dir ();
			}
			for (; driver_itr != line_itr->driver_array.end (); driver_itr++) {
				sim_dir_ptr = &sim_dir_array [*driver_itr];
				dir_ptr = &dir_array [*driver_itr];

				if (j != 0) {
					leg_rec.Mode (DRIVE_MODE);
					leg_rec.Type (DIR_ID);
					leg_rec.Index (*driver_itr);
					leg_rec.Max_Speed (sim_dir_ptr->Speed ());
					if (*driver_itr == index) {
						offset = next_cell - last_cell;
					} else {
						offset = sim_dir_ptr->Cells () - last_cell;
					}
					leg_rec.Time (dir_ptr->Time0 () * offset / sim_dir_ptr->Cells () + 1);
					time += leg_rec.Time ();
					plan_ptr->push_back (leg_rec);
				}
				if (*driver_itr == index) break;
				last_cell = 0;
			}
			last_cell = next_cell;

			leg_rec.Mode (OTHER_MODE);
			leg_rec.Type (STOP_ID);
			leg_rec.Index (stop_itr->Stop ());
			leg_rec.Stop_Number (j);
			if (j == 0) {
				leg_rec.Time (0);
			} else {
				leg_rec.Time (dwell);
			}
			time += leg_rec.Time ();
			plan_ptr->push_back (leg_rec);
		}
		plan_ptr->Duration (time);

		//---- set the lane connections ----

		last_itr = plan_ptr->rend ();

		for (leg_itr = plan_ptr->rbegin (); leg_itr != plan_ptr->rend (); leg_itr++) {
			if (leg_itr->Type () == STOP_ID) {
				sim_stop_ptr = &sim_stop_array [leg_itr->Index ()];

				leg_itr->In_Lane_Low (sim_stop_ptr->Min_Lane ());
				leg_itr->In_Lane_High (sim_stop_ptr->Max_Lane ());
				leg_itr->In_Best_Low (sim_stop_ptr->Min_Lane ());
				leg_itr->In_Best_High (sim_stop_ptr->Max_Lane ());

				leg_itr->Out_Lane_Low (sim_stop_ptr->Min_Lane ());
				leg_itr->Out_Lane_High (sim_stop_ptr->Max_Lane ());
				leg_itr->Out_Best_Low (sim_stop_ptr->Min_Lane ());
				leg_itr->Out_Best_High (sim_stop_ptr->Max_Lane ());

				leg_itr->Connect (-1);

				if (last_itr != plan_ptr->rend ()) {
					last_itr->In_Lane_Low (sim_stop_ptr->Min_Lane ());
					last_itr->In_Lane_High (sim_stop_ptr->Max_Lane ());
					last_itr->In_Best_Low (sim_stop_ptr->Min_Lane ());
					last_itr->In_Best_High (sim_stop_ptr->Max_Lane ());
				}
			} else if (last_itr != plan_ptr->rend ()) {
				if (last_itr->Type () == STOP_ID) {
					leg_itr->Out_Lane_Low (last_itr->In_Lane_Low ());
					leg_itr->Out_Lane_High (last_itr->In_Lane_High ());
					leg_itr->Out_Best_Low (last_itr->In_Best_Low ());
					leg_itr->Out_Best_High (last_itr->In_Best_High ());

					leg_itr->Max_Speed (1);
					leg_itr->Connect (-1);
				} else {

					//---- get the connection to the next link ----

					map2_itr = connect_map.find (Int2_Key (leg_itr->Index (), last_itr->Index ()));

					if (map2_itr == connect_map.end ()) {
						if (param.print_problems) {
							dir_ptr = &dir_array [last_itr->Index ()];
							link_ptr = &link_array [dir_ptr->Link ()];
							index = link_ptr->Link ();

							dir_ptr = &dir_array [leg_itr->Index ()];
							link_ptr = &link_array [dir_ptr->Link ()];

							Warning (String ("Transit Route %d Connection was Not Found between Links %d and %d") % 
								line_itr->Route () % link_ptr->Link () % index);
						}
						continue;
					}
					connect_ptr = &connect_array [map2_itr->second];

					leg_itr->Out_Lane_Low (connect_ptr->Low_Lane ());
					leg_itr->Out_Lane_High (connect_ptr->High_Lane ());
					leg_itr->Out_Best_Low (connect_ptr->Low_Lane ());
					leg_itr->Out_Best_High (connect_ptr->High_Lane ());

					last_itr->In_Lane_Low (connect_ptr->To_Low_Lane ());
					last_itr->In_Lane_High (connect_ptr->To_High_Lane ());
					last_itr->In_Best_Low (connect_ptr->To_Low_Lane ());
					last_itr->In_Best_High (connect_ptr->To_High_Lane ());

					if (connect_ptr->Speed () > 0) {
						leg_itr->Max_Speed (connect_ptr->Speed ());
					}
					leg_itr->Connect (map2_itr->second);
				}
			}
			last_itr = leg_itr;
		}
	}
}
