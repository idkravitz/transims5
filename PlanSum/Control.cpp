//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanSum::Program_Control (void)
{
	String key;
	bool link_flag, report_flag;

	link_flag = report_flag = false;

	if (!Set_Control_Flag (UPDATE_FLOW_RATES) && !Set_Control_Flag (UPDATE_TRAVEL_TIMES)) {
		System_File_False (NEW_LINK_DELAY);
	}

	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- set equivalence flags ---
	
	if (Report_Flag (LINK_GROUP) || Report_Flag (RIDER_GROUP)) {
		Link_Equiv_Flag (true);
		link_flag = true;
	}
	if (skim_flag || trip_flag) {
		Zone_Equiv_Flag (true);
		zone_flag = true;
	}
	Stop_Equiv_Flag (Report_Flag (STOP_GROUP));

	//---- create the network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();
	Read_Flow_Time_Keys ();

	plan_file = (Plan_File *) System_File_Handle (PLAN);
	if (!plan_file->Part_Flag ()) Num_Threads (1);

	select_flag = System_File_Flag (SELECTION);
	new_delay_flag = System_File_Flag (NEW_LINK_DELAY);

	if (System_File_Flag (LINK_DELAY)) {
		link_flag = true;
		Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);
		turn_flag = file->Turn_Flag ();
	}
	if (new_delay_flag) {
		link_flag = true;
		Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (NEW_LINK_DELAY);
		turn_flag = file->Turn_Flag ();
	} else {
		turn_flag = System_File_Flag (CONNECTION);
	}
	Print (2, String ("%s Control Keys:") % Program ());

	//---- new trip time file ----

	key = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (!key.empty ()) {
		time_file.File_Type ("New Trip Time File");
		time_file.File_ID ("Time");

		if (Master ()) {
			time_file.Create (Project_Filename (key));
		}
		time_flag = true;
	}

	//---- new link volume file ----

	key = Get_Control_String (NEW_LINK_VOLUME_FILE);

	if (!key.empty ()) {
		volume_file.File_Type ("New Link Volume File");
		volume_file.File_ID ("Volume");

		if (Master ()) {
			volume_file.Create (Project_Filename (key));

			volume_file.Num_Decimals (1);
			volume_file.Data_Units (Performance_Units_Map (FLOW_DATA));
			volume_file.Copy_Periods (sum_periods);

			volume_file.Create_Fields ();
			volume_file.Write_Header ();
		}
		volume_flag = new_delay_flag = link_flag = true;
	}

	//---- read report types ----

	List_Reports ();

	if (Report_Flag (TOP_100) || Report_Flag (VC_RATIO) || Report_Flag (LINK_GROUP)) {
		new_delay_flag = link_flag = report_flag = true;
		cap_factor = (double) sum_periods.Range_Length () / (Dtime (1, HOURS) * sum_periods.Num_Periods ());
	}
	if (link_flag && (!System_File_Flag (LINK) || !System_File_Flag (NODE))) {
		Error ("Link and Node Files are Required for Link-Based Output");
	}
	if (System_File_Flag (LANE_USE) && !System_File_Flag (LINK)) {
		Error ("A Link File is Required for Lane-Use Processing");
	}
	travel_flag = Report_Flag (SUM_TRAVEL);

	//---- ridership summary ----

	passenger_flag = Report_Flag (SUM_PASSENGERS);
	transfer_flag = (Report_Flag (SUM_STOPS) || Report_Flag (STOP_GROUP));
	rider_flag = (System_File_Flag (NEW_RIDERSHIP) || Report_Flag (SUM_RIDERS) || Report_Flag (RIDER_GROUP));
	xfer_flag = Report_Flag (SUM_TRANSFERS);
	xfer_detail = Report_Flag (XFER_DETAILS);

	if (passenger_flag || transfer_flag || rider_flag || xfer_flag || xfer_detail) {
		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE)) {

			Error ("Transit Network Files are Required for Ridership Output");
		}
		if ((passenger_flag || rider_flag) && !System_File_Flag (TRANSIT_DRIVER)) {
			Error ("A Transit Driver File is needed for Passengers Summaries");
		}
	} else {
		System_File_False (TRANSIT_STOP);
		System_File_False (TRANSIT_ROUTE);
		System_File_False (TRANSIT_SCHEDULE);
		System_File_False (TRANSIT_DRIVER);
	}

	//---- process support data ----

	if (Link_Equiv_Flag ()) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}
	if (Zone_Equiv_Flag ()) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
	if (Stop_Equiv_Flag ()) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}

	//---- allocate work space ----

	if (time_flag || Report_Flag (TRIP_TIME)) {
		time_flag = true;

		int periods = sum_periods.Num_Periods ();
		
		start_time.assign (periods, 0);
		mid_time.assign (periods, 0);
		end_time.assign (periods, 0);
	}

	//---- transfer arrays ----

	if (xfer_flag || xfer_detail) {
		Integers rail;

		rail.assign (10, 0);
		total_on_array.assign (10, rail);

		if (xfer_detail) {
			int num = sum_periods.Num_Periods ();
			if (num < 1) num = 1;

			walk_on_array.assign (num, total_on_array);
			drive_on_array.assign (num, total_on_array);
		}
	}

	//---- initialize the trip summary data ----

	if (travel_flag) {
		trip_sum_data.Copy_Periods (sum_periods);
	}

	//---- initialize the passenger summary data ----

	if (passenger_flag) {
		pass_sum_data.Copy_Periods (sum_periods);
	}
}
