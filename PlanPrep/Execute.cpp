//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanPrep::Execute (void)
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
		if (part > 0 && merge_flag && !merge_file.Find_File (part)) {
			Error (String ("%s %d was Not Found") % merge_file.File_Type () % part);
		}
		partition_queue.Put (part);
	}
	plan_file->Close ();
	plan_file->Reset_Counters ();

	merge_file.Close ();
	merge_file.Reset_Counters ();

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

	//---- combine plans ----

	if (combine_flag) {
		Combine_Plans ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- print processing summary ----

	plan_file->Print_Summary ();

	if (merge_flag) {
		merge_file.Print_Summary ();
	}
	if (System_File_Flag (NEW_PLAN)) {
		new_plan_file->Print_Summary ();
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case FIRST_REPORT:		//---- First Report ----
			//First_Header ();
			break;
		case SECOND_REPORT:		//---- Second Report ----
			Print (1, "Second Report Header");
			Print (1);
			break;
		default:
			break;
	}
}
