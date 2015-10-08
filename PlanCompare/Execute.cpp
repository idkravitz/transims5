//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanCompare::Execute (void)
{
#ifdef BOOST_THREADS		
	boost::thread_group threads;
#endif

	//---- read the network data ----

	Data_Service::Execute ();

	//---- set the processing queue ----

	int part, num;

	num = plan_file->Num_Parts ();

	for (part=0; part < num; part++) {
		if (part > 0 && !compare_file.Find_File (part)) {
			Error (String ("%s %d was Not Found") % compare_file.File_Type () % part);
		}
		partition_queue.Put (part);
		if (new_plan_flag) output_queue.Put (part);
	}
	plan_file->Close ();
	plan_file->Reset_Counters ();

	compare_file.Close ();
	compare_file.Reset_Counters ();

	partition_queue.End_of_Queue ();
	if (new_plan_flag) output_queue.End_of_Queue ();

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
		Show_Message (1);
#endif
	} else {
		(**plan_processing) ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- trip match file ----

	if (match_flag) {
		Write_Match ();
	}

	//---- select plans ----

	if (Master ()) {

		//---- print processing summary ----

		plan_file->Print_Summary ();

		compare_file.Print_Summary ();

		//---- select the trip records ----

		Select_Plans ();
	}

	//---- write plans or selection records ----

	if (new_plan_flag || select_parts) {

		//---- distribute MPI selection data ----

		MPI_Write ();

		//---- write the new plan file ----

		if (new_plan_flag) {
			if (num_select > 0 || merge_flag) {

				//---- write the selected plans ---

				plan_output = new Plan_Output * [Num_Threads ()];

				for (int i=0; i < Num_Threads (); i++) {
					delete plan_processing [i];

					plan_output [i] = new Plan_Output (this);
#ifdef BOOST_THREADS
					if (Num_Threads () > 1) {
						threads.create_thread (boost::ref (*(plan_output [i])));
					}
#endif
				}
				delete plan_processing;
				plan_processing = 0;

				if (Num_Threads () > 1) {
#ifdef BOOST_THREADS
					threads.join_all ();
#endif
				} else {
					(**plan_output) ();
				}
			
			} else {

				//---- create empty files ----

				while (output_queue.Get (part)) {
					new_plan_file->Open (part);
					new_plan_file->Close ();
				}
			}
		}

		//---- MPI Close ----

		MPI_Close ();

		//---- print output summary ----

		if (new_plan_flag) new_plan_file->Print_Summary ();
	}

	//---- write the new selection file ----

	if (System_File_Flag (NEW_SELECTION) && (!select_parts || MPI_Size () < 1)) {
		select_map.swap (selected);
		Write_Selections ();
	}

	//---- write the distribution file ----

	if (time_diff.Output_Flag ()) {
		time_diff.Write_Distribution ();
	}
	if (cost_diff.Output_Flag ()) {
		cost_diff.Write_Distribution ();
	}

	//---- write the trip gap files ----

	if (time_gap.Output_Flag ()) {
		time_gap.Write_Trip_Gap_File ();
	}
	if (cost_gap.Output_Flag ()) {
		cost_gap.Write_Trip_Gap_File ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOTAL_TIME:		//---- total time distribution ----
				time_diff.Distribution_Report (TOTAL_TIME, true);
				break;
			case PERIOD_TIME:		//---- period time distributions ----
				time_diff.Distribution_Report (PERIOD_TIME, false);
				break;
			case TOT_TIME_SUM:		//---- total time summary ----
				time_diff.Total_Summary ();
				break;
			case PER_TIME_SUM:		//---- period time summaries ----
				time_diff.Period_Summary (PER_TIME_SUM);
				break;
			case TOTAL_COST:		//---- total cost distribution ----
				cost_diff.Distribution_Report (TOTAL_COST, true);
				break;
			case PERIOD_COST:		//---- period cost distributions ----
				cost_diff.Distribution_Report (PERIOD_COST, false);
				break;
			case TOT_COST_SUM:		//---- total cost summary ----
				cost_diff.Total_Summary ();
				break;
			case PER_COST_SUM:		//---- period cost summaries ----
				cost_diff.Period_Summary (PER_COST_SUM);
				break;
			case TRP_TIME_GAP:		//---- trip time gap ----
				time_gap.Trip_Gap_Report (TRP_TIME_GAP);
				break;
			case TRP_COST_GAP:		//---- trip cost gap ----
				cost_gap.Trip_Gap_Report (TRP_COST_GAP);
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanCompare::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOTAL_TIME:		//---- total time distribution ----
			time_diff.Distribution_Header (true);
			break;
		case PERIOD_TIME:		//---- period time distributions ----
			time_diff.Distribution_Header (false);
			break;
		case TOT_TIME_SUM:		//---- total time summary ----
			break;
		case PER_TIME_SUM:		//---- period time summary ----
			time_diff.Period_Header ();
			break;
		case TOTAL_COST:		//---- total cost distribution ----
			cost_diff.Distribution_Header (true);
			break;
		case PERIOD_COST:		//---- period cost distributions ----
			cost_diff.Distribution_Header (false);
			break;
		case TOT_COST_SUM:		//---- total cost summary ----
			break;
		case PER_COST_SUM:		//---- period cost summary ----
			cost_diff.Period_Header ();
			break;
		case TRP_TIME_GAP:		//---- trip time gap ----
			time_gap.Trip_Gap_Header ();
			break;
		case TRP_COST_GAP:		//---- trip cost gap ----
			cost_gap.Trip_Gap_Header ();
		default:
			break;
	}
}
