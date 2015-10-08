//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void ArcPlan::Execute (void)
{
	int nfile;

	//---- read the network ----

	Data_Service::Execute ();

	if (problem_flag) {
		arcview_problem.Close ();
	}

	//---- allocate bandwidth memory ----

	if (width_flag) {
		width_data.assign (dir_array.size (), 0);
	}

	//---- allocate contour memory ----

	if (time_flag || distance_flag) {
		link_flag.assign (dir_array.size (), 0);
	}

	//---- allocate transit memory ----

	if (rider_flag || on_off_flag) {
		board_data.assign (stop_pt.size (), 0);
		alight_data.assign (stop_pt.size (), 0);
	}

	//---- allocate parking memory ----

	if (parking_flag) {
		parking_out.assign (parking_pt.size (), 0);
		parking_in.assign (parking_pt.size (), 0);
	}

	//---- read the plan file ----

	if (plan_flag) {
		for (nfile=0; ; nfile++) {
			if (!plan_file->Open (nfile)) break;

			//---- process the plan files ----

			Read_Plan ();
		}

		if (nfile == 0) {
			File_Error ("No Plan Files were Found", plan_file->Filename ());
		}
	}
	if (path_flag) {
		arcview_plan.Close ();
	}
	if (time_flag) {
		arcview_time.Close ();
	}
	if (distance_flag) {
		arcview_distance.Close ();
	}
	if (access_flag) {
		arcview_access.Close ();
	}

	//---- write bandwidth shapes ----

	if (width_flag) {
		Write_Bandwidth ();
	}

	//---- write ridership shapes ----

	if (rider_flag) {
		//Write_Riders ();
	}

	//---- write transit stops file ----

	if (on_off_flag) {
		if (demand_flag) Write_Stops ();
		if (group_flag) Write_Group ();
	}

	//---- write parking lots file ----

	if (parking_flag) {
		Write_Parking ();
	}

	//---- write summary statistics ----

	if (plan_flag) {
		plan_file->Print_Summary ();
	}
	Write (1);
	if (path_flag) Write (1, "Number of Arc Plan Shape Records = ") << num_out;
	if (problem_flag) Write (1, "Number of Arc Problem Shape Records = ") << num_problem;
	if (width_flag) Write (1, "Number of Arc Bandwidth Shape Records = ") << num_width;
	if (time_flag) Write (1, "Number of Arc Time Contour Shape Records = ") << num_time;
	if (distance_flag) Write (1, "Number of Arc Distance Contour Shape Records = ") << num_distance;
	if (access_flag) Write (1, "Number of Arc Accessibility Shape Records = ") << num_access;
	if (rider_flag) Write (1, "Number of Arc Ridership Shape Records = ") << num_rider;
	if (demand_flag) Write (1, "Number of Arc Stop Demand Shape Records = ") << num_stop;
	if (group_flag) Write (1, "Number of Arc Stop Group Shape Records = ") << num_group;
	if (parking_flag) Write (1, "Number of Arc Parking Demand Shape Records = ") << num_parking;

	Exit_Stat (DONE);
}

