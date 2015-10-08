//*********************************************************
//	Flow_Time_Service.cpp - flow time service keys and data
//*********************************************************

#include "Flow_Time_Service.hpp"

//---------------------------------------------------------
//	Flow_Time_Service constructor
//---------------------------------------------------------

Flow_Time_Service::Flow_Time_Service (void)
{
	flow_updates = turn_updates = time_updates = false;
	first_update = true;
	update_rate = 0;
	flow_factor = 1.0;
}

//---------------------------------------------------------
//	Flow_Time_Service_Keys
//---------------------------------------------------------

void Flow_Time_Service::Flow_Time_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ UPDATE_FLOW_RATES, "UPDATE_FLOW_RATES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_TURNING_MOVEMENTS, "UPDATE_TURNING_MOVEMENTS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ CLEAR_INPUT_FLOW_RATES, "CLEAR_INPUT_FLOW_RATES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_TRAVEL_TIMES, "UPDATE_TRAVEL_TIMES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LINK_DELAY_UPDATE_RATE, "LINK_DELAY_UPDATE_RATE", LEVEL0, OPT_KEY, INT_KEY, "0", "-1..5000", NO_HELP },
		{ LINK_DELAY_FLOW_FACTOR, "LINK_DELAY_FLOW_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "1..100000", NO_HELP },
		{ EQUATION_PARAMETERS, "EQUATION_PARAMETERS", LEVEL1, OPT_KEY, LIST_KEY, "BPR, 0.15, 4.0, 0.75" , EQUATION_RANGE, NO_HELP},
		END_CONTROL
	};

	if (keys == 0) {
		dat->Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					dat->Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				dat->Error (String ("Flow-Time Service Key %d was Not Found") % keys [i]);
			}
		}
	}
}

//---------------------------------------------------------
//	Read_Flow_Time_Keys
//---------------------------------------------------------

void Flow_Time_Service::Read_Flow_Time_Keys (void)
{
	String key;
	
	dat->Print (2, "Flow-Time Service Controls:");

	//---- update flow rates ----

	if (dat->Control_Key_Status (UPDATE_FLOW_RATES)) {
		Flow_Updates (dat->Get_Control_Flag (UPDATE_FLOW_RATES));
	}

	if (Flow_Updates ()) {

		//---- update turning movements ----

		if (dat->Control_Key_Status (UPDATE_TURNING_MOVEMENTS)) {
			if (dat->System_File_Flag (CONNECTION)) {
				Turn_Updates (dat->Get_Control_Flag (UPDATE_TURNING_MOVEMENTS));
			}
			if (dat->System_File_Flag (NEW_LINK_DELAY)) {
				Link_Delay_File *file = (Link_Delay_File *) dat->System_File_Handle (NEW_LINK_DELAY);

				file->Turn_Flag (Turn_Updates ());
				file->Clear_Fields ();
				file->Create_Fields ();
				file->Write_Header ();
			}
			if (dat->System_File_Flag (NEW_PERFORMANCE)) {
				Performance_File *file = (Performance_File *) dat->System_File_Handle (NEW_PERFORMANCE);

				file->Turn_Flag (Turn_Updates ());
				file->Clear_Fields ();
				file->Create_Fields ();
				file->Write_Header ();
			}
		}

		//---- clear input link flows ----

		if (dat->Control_Key_Status (CLEAR_INPUT_FLOW_RATES) && dat->System_File_Flag (LINK_DELAY)) {
			dat->Clear_Flow_Flag (dat->Get_Control_Flag (CLEAR_INPUT_FLOW_RATES));
		}
	}

	//---- update travel times ----

	if (dat->Control_Key_Status (UPDATE_TRAVEL_TIMES)) {
		dat->Print (1);
		Time_Updates (dat->Get_Control_Flag (UPDATE_TRAVEL_TIMES));
	}

	if (Time_Updates ()) {

		//---- link delay updates and volume factor ----

		if (dat->Control_Key_Status (LINK_DELAY_UPDATE_RATE)) {
			update_rate = dat->Get_Control_Integer (LINK_DELAY_UPDATE_RATE);

			if (update_rate == 0) {
				update_rate = -1;
				Flow_Updates (true);
			}
		}
		
		//---- link delay flow factor ----

		if (dat->Control_Key_Status (LINK_DELAY_FLOW_FACTOR)) {
			flow_factor = dat->Get_Control_Double (LINK_DELAY_FLOW_FACTOR);
		}

		if (dat->Control_Key_Status (EQUATION_PARAMETERS)) {
			dat->Print (1);

			//---- equation parameters ----

			for (int i=1; i <= EXTERNAL; i++) {
				key = dat->Get_Control_String (EQUATION_PARAMETERS, i);

				equation.Add_Equation (i, key);
			}
		}
	}
}

//---------------------------------------------------------
//	Build_Flow_Time_Arrays
//---------------------------------------------------------

void Flow_Time_Service::Build_Flow_Time_Arrays (Flow_Time_Period_Array &link_delay, Flow_Time_Period_Array &turn_delay)
{
	if (link_delay.size () != 0) return;

	link_delay.Initialize (&dat->time_periods);

	link_delay.Set_Time0 ();

	int records = (int) dat->connect_array.size ();

	if (Turn_Updates () && records > 0) {
		turn_delay.Initialize (&dat->time_periods, records);
	}
}

//---------------------------------------------------------
//	Build_Turn_Arrays
//---------------------------------------------------------

void Flow_Time_Service::Build_Turn_Arrays (Flow_Time_Period_Array &turn_delay)
{
	if (turn_delay.size () != 0 || !Turn_Updates ()) return;

	turn_delay.Initialize (&dat->time_periods, (int) dat->connect_array.size ());
}

//---------------------------------------------------------
//	Update_Travel_Times
//---------------------------------------------------------

void Flow_Time_Service::Update_Travel_Times (int mpi_size)
{
	int type, lanes, lanes0, lanes1, cap, tod_cap, len, index, rec, flow_index;
	Dtime time0, tod1, tod, period;
	double flow, flow_fac;

	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Lane_Use_Period *period_ptr;

	Flow_Time_Data  *flow_ptr;
	Flow_Time_Itr flow_itr;
	Flow_Time_Period_Itr  per_itr;

	period = dat->time_periods.Increment ();
	if (period < 1) period = 1;

	tod1 = dat->Model_Start_Time () + period / 2;
	flow_fac = flow_factor * Dtime (60, MINUTES) / period;
	if (mpi_size > 1) flow_fac *= mpi_size;

	for (index=0, dir_itr = dat->dir_array.begin (); dir_itr != dat->dir_array.end (); dir_itr++, index++) {
		link_ptr = &dat->link_array [dir_itr->Link ()];

		len = link_ptr->Length ();
		type = link_ptr->Type ();

		time0 = dir_itr->Time0 ();
		cap = dir_itr->Capacity ();
		lanes = dir_itr->Lanes ();
		if (lanes < 1) lanes = 1;
		tod = tod1;

		for (per_itr = dat->link_delay_array.begin (); per_itr != dat->link_delay_array.end (); per_itr++, tod += period) {
			flow_ptr = &per_itr->at (index);

			flow = flow_ptr->Flow () * flow_fac;

			flow_index = dir_itr->Flow_Index ();

			if (!dat->Lane_Use_Flows () && flow_index >= 0) {
				flow += per_itr->at (flow_index).Flow () * flow_fac;
				flow_index = -1;
			}
			if (flow == 0.0) continue;

			//---- check for time of day restrictions ----

			lanes1 = 0;
			rec = dir_itr->First_Lane_Use ();

			if (rec >= 0) {
				lanes0 = lanes;

				for (;; rec++) {
					period_ptr = &dat->use_period_array [rec];

					if (period_ptr->Start () <= tod && tod < period_ptr->End ()) {
						lanes0 = period_ptr->Lanes0 ();
						lanes1 = period_ptr->Lanes1 ();
						break;
					}
					if (period_ptr->Periods () == 0) break;
				}
				tod_cap = (cap * lanes0 + lanes0 / 2) / lanes;
			} else {
				tod_cap = cap;
			}
			flow_ptr->Time (equation.Apply_Equation (type, time0, flow, tod_cap, len));

			if (flow_index >= 0 && lanes1 > 0) {
				flow_ptr = &per_itr->at (flow_index);

				flow = flow_ptr->Flow () * flow_fac;
				if (flow == 0.0) continue;

				tod_cap = (cap * lanes1 + lanes1 / 2) / lanes;

				flow_ptr->Time (equation.Apply_Equation (type, time0, flow, tod_cap, len));
			}
		}
	}
	if (first_update) {
		first_update = false;

		if (turn_updates) {
			for (per_itr = dat->turn_delay_array.begin (); per_itr != dat->turn_delay_array.end (); per_itr++) {
				for (flow_itr = per_itr->begin (); flow_itr != per_itr->end (); flow_itr++) {
					flow_itr->Time (0);
				}
			}
		}
	}
}

