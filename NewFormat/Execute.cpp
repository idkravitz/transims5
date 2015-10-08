//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void NewFormat::Execute (void)
{
	System_Read_False (HOUSEHOLD);

	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);
	
	System_Read_False (SKIM);
	System_Data_Reserve (SKIM, 0);

	Data_Service::Execute ();

	if (toll_flag) Read_Tolls ();

	//---- consolidate phasing plans ----

	if (System_File_Flag (NEW_SIGNAL) && System_File_Flag (NEW_PHASING_PLAN)) {
		Check_Phasing_Plans ();
	}

	//---- write the network data files ----

	if (System_File_Flag (NEW_NODE)) Write_Nodes ();
	if (System_File_Flag (NEW_ZONE) && !copy_flag) Write_Zones ();	
	if (System_File_Flag (NEW_LINK)) Write_Links ();
	if (System_File_Flag (NEW_SHAPE)) Write_Shapes ();
	if (System_File_Flag (NEW_POCKET)) Write_Pockets ();
	if (System_File_Flag (NEW_LANE_USE)) Write_Lane_Uses ();
	if (System_File_Flag (NEW_CONNECTION)) Write_Connections ();
	if (System_File_Flag (NEW_TURN_PENALTY)) Write_Turn_Pens ();
	if (System_File_Flag (NEW_PARKING)) Write_Parking_Lots ();
	if (System_File_Flag (NEW_LOCATION) && !copy_flag) Write_Locations ();
	if (System_File_Flag (NEW_ACCESS_LINK)) Write_Access_Links ();
	if (System_File_Flag (NEW_SIGN)) Write_Signs ();
	if (System_File_Flag (NEW_SIGNAL)) Write_Signals ();
	if (System_File_Flag (NEW_TIMING_PLAN)) Write_Timing_Plans ();
	if (System_File_Flag (NEW_PHASING_PLAN)) Write_Phasing_Plans ();
	if (System_File_Flag (NEW_DETECTOR)) Write_Detectors ();
	if (System_File_Flag (NEW_TRANSIT_STOP)) Write_Stops ();
	if (System_File_Flag (NEW_TRANSIT_ROUTE)) Write_Lines ();
	if (System_File_Flag (NEW_TRANSIT_SCHEDULE)) Write_Schedules ();
	if (System_File_Flag (NEW_TRANSIT_DRIVER)) Write_Drivers ();
	if (System_File_Flag (NEW_TRANSIT_FARE)) Write_Fares ();

	//---- process person file ----

	if (System_File_Flag (HOUSEHOLD)) {
		Read_Households ();
		if (person_flag) Read_Persons ();

		if (System_File_Flag (NEW_HOUSEHOLD)) Write_Households ();

		Household_Array ().swap (hhold_array);
	}

	//---- write the data files ----

	if (System_File_Flag (NEW_SELECTION)) Write_Selections ();
	if (System_File_Flag (NEW_LINK_DELAY)) Write_Link_Delays ();
	if (System_File_Flag (NEW_PERFORMANCE)) Write_Performance ();
	if (System_File_Flag (NEW_RIDERSHIP)) Write_Ridership ();
	if (System_File_Flag (NEW_VEHICLE_TYPE)) Write_Veh_Types ();
	if (System_File_Flag (NEW_VEHICLE)) Write_Vehicles ();

	//---- read the snapshot file ----

	if (snap_flag) {
		Read_Snapshots ();
	}

	//---- convert activity file ----

	if (activity_flag && System_File_Flag (NEW_TRIP)) {
		Read_Activity ();

		Trip_File *file = (Trip_File *) System_File_Handle (NEW_TRIP);
		
		Print (1, String ("%s Records = %d") % file->File_Type () % num_new_act);
	}

	//---- convert trip file ----

	if (System_File_Flag (TRIP) && System_File_Flag (NEW_TRIP)) {
		Read_Trips ();

		Trip_File *file = (Trip_File *) System_File_Handle (NEW_TRIP);
		
		Print (1, String ("%s Records = %d") % file->File_Type () % num_new_trip);
		if (max_trip_part > 1) Print (0, String (" (%d files)") % max_trip_part);
	}

	if (System_File_Flag (NEW_PROBLEM)) Write_Problems ();

	//---- convert plan file ----

	if (plan_flag) {
#ifdef BOOST_THREADS		
		boost::thread_group threads;
#endif
		//---- set the processing queue ----

		int part, num;
		num = old_plan.Num_Parts ();

		for (part=0; part < num; part++) {
			partition_queue.Put (part);
		}
		old_plan.Close ();
		old_plan.Reset_Counters ();

		partition_queue.End_of_Queue ();

		//---- processing threads ---

		plan_processing = new Plan_Processing * [Num_Threads ()];

		for (int i=0; i < Num_Threads (); i++) {
			plan_processing [i] = new Plan_Processing (this);

	#ifdef BOOST_THREADS
			if (Num_Threads () > 1) {
				threads.create_thread (boost::ref (*(plan_processing [i])));
			}
	#endif
		}

		if (Num_Threads () > 1) {
	#ifdef BOOST_THREADS
			threads.join_all ();
	#endif
		} else {
			(**plan_processing) ();
		}

		//---- report the results ----

		Break_Check (7);
		Write (2, old_plan.File_Type ());
		Write (1, "Number of Input Files = ") << num;
		Write (1, "Number of Input Plans = ") << old_plan.Num_Plans ();
		Write (1, "Number of Input Records = ") << old_plan.Num_Records ();
		Write (1, "Number of Input Travelers = ") << old_plan.Num_Travelers ();
		Write (1, "Number of Input Trips = ") << old_plan.Num_Trips ();
	} else {
		if (System_File_Flag (NEW_PLAN)) Write_Plans ();
	}
	if (System_File_Flag (NEW_PLAN)) {
		Plan_File *file = (Plan_File *) System_File_Base (NEW_PLAN);
		file->Print_Summary ();
	}

	//---- convert skim file ----

	if (System_File_Flag (SKIM) && System_File_Flag (NEW_SKIM)) {
		Read_Skims ();

		Skim_File *file = (Skim_File *) System_File_Handle (NEW_SKIM);
		
		Print (1, String ("%s Records = %d") % file->File_Type () % num_new_skim);
		if (max_skim_part > 1) Print (0, String (" (%d files)") % max_skim_part);
	}

	//---- convert the route nodes ----

	if (route_flag) {
		Read_Route_Nodes ();
		Write_Route_Nodes ();
	}
	Exit_Stat (DONE);
}
