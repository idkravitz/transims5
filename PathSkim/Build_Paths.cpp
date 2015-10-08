//*********************************************************
//	Build_Paths.cpp - process the path building commands
//*********************************************************

#include "PathSkim.hpp"

#include "Plan_Processor.hpp"	

//---------------------------------------------------------
//	Build_Paths
//---------------------------------------------------------

void PathSkim::Build_Paths (void)
{
	int hhold, mode, period, periods, out_period, last_period;
	Dtime low, high;

	Location_Itr loc_itr;
	Plan_Data *plan_ptr;
	Plan_Ptr_Array *plan_ptr_array;
	Int_Itr org_itr, des_itr;
	Plan_Processor plan_processor;
	Veh_Type_Data *veh_type_ptr;

	//---- initialize the data ----

	Show_Message ("Building Specified Paths -- Record");
	Set_Progress ();

	hhold = 0;
	if (mode_flag) {
		mode = new_mode;
	} else {
		mode = DRIVE_MODE;
	}
	periods = route_periods.Num_Periods ();
	last_period = -1;
	
	if ((int) veh_type_array.size () <= veh_type) {
		Veh_Type_Data veh_type_data;
		veh_type_array.assign (veh_type, veh_type_data);
	}
	veh_type_ptr = &veh_type_array [veh_type];
	veh_type_ptr->Use (use_type);

	//---- initialize the path processor ----

	plan_processor.Initialize (this, Num_Threads ());

	plan_processor.Start_Processing ();

	//---- loop through each time increment ----

	for (period = 0; period < periods; period++) {
		if (!route_periods.Period_Range (period, low, high)) continue;

		if (skim_flag) {
			out_period = skim_file->Period ((forward_flag) ? low : (high - 1));
			if (out_period < 0) continue;
			if (Partition_Index (out_period) < 0) continue;

			//---- save the last time period ----

			if (out_period != last_period) {
				if (last_period >= 0) {
					End_Progress ();
					plan_processor.Stop_Processing ();

					Output_Skims (last_period);

					Show_Message ("Building Specified Paths -- Record");
					Set_Progress ();
					plan_processor.Start_Processing ();
				}
				last_period = out_period;
			}
		}

		//---- loop through each origin location ----

		for (org_itr = org_loc.begin (); org_itr != org_loc.end (); org_itr++) {

			//---- loop through each destination location ----

			for (des_itr = des_loc.begin (); des_itr != des_loc.end (); des_itr++) {
				if (*org_itr == *des_itr) continue;
				Show_Progress ();
				nprocess++;
				hhold++;
				
				//---- construct the plan data ----

				plan_ptr_array = new Plan_Ptr_Array ();
				plan_ptr = new Plan_Data ();

				plan_ptr->Method (BUILD_PATH);

				plan_ptr->Household (hhold);
				plan_ptr->Person (1);
				plan_ptr->Trip (1);
				plan_ptr->Purpose (1);
				plan_ptr->Origin (*org_itr);
				plan_ptr->Destination (*des_itr);

				plan_ptr->Mode (mode);
				plan_ptr->Vehicle (veh_type);
				plan_ptr->Constraint (constraint);
				plan_ptr->Type (traveler_type);

				if (forward_flag) {
					plan_ptr->Start (low);
					plan_ptr->End (Model_End_Time ());
				} else {
					plan_ptr->Start (Model_Start_Time ());
					plan_ptr->End (high);
				}

				//---- build the path ----

				plan_ptr_array->push_back (plan_ptr);
		
				plan_processor.Plan_Build (plan_ptr_array);
			}
		}
	}
	End_Progress ();
	plan_processor.Stop_Processing ();

	if (skim_flag && last_period >= 0) {
		Output_Skims (last_period);
	}

	//---- write the path summary ----

	Show_Message (1, "Number of Paths Written = ") << plan_file->Num_Trips ();
	plan_file->Print_Summary ();
}
