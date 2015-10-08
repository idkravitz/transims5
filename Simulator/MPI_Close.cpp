//*********************************************************
//	MPI_Close.cpp - coordinated MPI exit
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	MPI_Close
//---------------------------------------------------------

void Simulator::MPI_Close (void)
{
#ifdef MPI_EXE
	if (Master ()) {

		for (int i=1; i < MPI_Size (); i++) {
			Sim_Statistics rank_stats;
			int problem_array [MAX_PROBLEM];

			int rank = Show_MPI_Message ();

			Get_MPI_Buffer (rank);

			mpi_buffer.Get_Data (&rank_stats, sizeof (Sim_Statistics));

			Add_Statistics (rank_stats);

			mpi_buffer.Get_Data (problem_array, sizeof (problem_array));

			Add_Problems (problem_array);
		}

	} else {	//---- slave ----

		Send_MPI_Message (String ("Slave %d Maximum Vehicles = %d at %s") % 
			MPI_Rank () % max_vehicles % max_time.Time_String ());

		mpi_buffer.Size (0);
		mpi_buffer.Add_Data (&Get_Statistics (), sizeof (Sim_Statistics));
		mpi_buffer.Add_Data (Get_Problems (), MAX_PROBLEM * sizeof (int));
		
		Send_MPI_Buffer ();

		Exit_Stat (DONE);
	}
#endif
}
