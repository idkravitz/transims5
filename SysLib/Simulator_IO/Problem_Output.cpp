//*********************************************************
//	Problem_Output.cpp - Output Interface Class
//*********************************************************

#include "Problem_Output.hpp"

//---------------------------------------------------------
//	Problem_Output constructor
//---------------------------------------------------------

Problem_Output::Problem_Output () : Static_Service ()
{
	Initialize ();
}

void Problem_Output::operator()()
{
#ifdef MPI_EXE
	while (exe->io_barrier.Go ()) {
		MPI_Processing ();
		exe->io_barrier.Finish ();
	}
#else
 #ifdef BOOST_THREADS
	Problem_Data problem;

	while (problem_queue.Get (problem)) {
		Write_Problem (problem);
	}
 #endif
#endif
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Problem_Output::Initialize (Simulator_Service *_exe)
{
	exe = _exe;
	output_flag = false;
	file = 0;
}

//---------------------------------------------------------
//	Read_Control
//---------------------------------------------------------

bool Problem_Output::Read_Control (void)
{
	if (exe == 0) return (false);

	output_flag = exe->System_File_Flag (NEW_PROBLEM);

	if (output_flag) {
		file = (Problem_File *) exe->System_File_Handle (NEW_PROBLEM);
	}
	return (output_flag);
}

//---------------------------------------------------------
//	Output_Problem
//---------------------------------------------------------

void Problem_Output::Output_Problem (Problem_Data &problem)
{
	if (!output_flag) return;

#ifdef MPI_EXE
	if (exe->Num_Threads () > 1) {
		boost::mutex::scoped_lock lock (data_mutex);
		data.Add_Data (&problem, sizeof (problem));
	} else {
		data.Add_Data (&problem, sizeof (problem));
	}
#else 
	if (exe->Num_Threads () > 1) {
 #ifdef BOOST_THREADS
		problem_queue.Put (problem);
 #endif
	} else {
		Write_Problem (problem);
	}
#endif
}

//---------------------------------------------------------
//	Write_Problem
//---------------------------------------------------------

void Problem_Output::Write_Problem (Problem_Data &problem)
{
	if (output_flag) {
		exe->Put_Problem_Data (*file, problem);
	}
}

//---------------------------------------------------------
//	End_Output
//---------------------------------------------------------

void Problem_Output::End_Output (void)
{
#ifdef MPI_EXE
	if (exe->Slave ()) {
		file->Close ();
		string message = file->Filename ();
		remove (message.c_str ());
		message += ".def";
		remove (message.c_str ());
	}
#else
 #ifdef BOOST_THREADS
	if (exe->Num_Threads () > 1) {
		problem_queue.End_Queue ();
		problem_queue.Exit_Queue ();
	}
 #endif
#endif
}

#ifdef MPI_EXE
//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void Problem_Output::MPI_Processing (void)
{
	if (!output_flag) return;

	int tag = (exe->Num_Threads () > 1) ? NEW_PROBLEM : 0;

	if (exe->Master ()) {
		int i, num, size;
		Problem_Data problem;

		size = sizeof (problem);

		//---- save master problems ----

		num = (int) data.Size () / size;

		while (num-- > 0) {
			data.Get_Data (&problem, size);

			exe->Put_Problem_Data (*file, problem);
		}

		//---- process each slave ----

		for (i=1; i < exe->MPI_Size (); i++) {

			exe->Get_MPI_Buffer (data, tag);
			if (data.Size () == 0) continue;

			num = (int) data.Size () / size;

			while (num-- > 0) {
				data.Get_Data (&problem, size);

				exe->Put_Problem_Data (*file, problem);
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
