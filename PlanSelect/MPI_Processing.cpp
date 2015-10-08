//*********************************************************
//	MPI_Processing.cpp - MPI data transfer
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	MPI_Processing
//---------------------------------------------------------

void PlanSelect::MPI_Processing (void)
{
#ifdef MPI_EXE
	int i, lvalue [7], rank;

	if (Master ()) {
		Write (1);

		for (i=1; i < MPI_Size (); i++) {

			rank = Show_MPI_Message ();

			//---- retrieve the data buffer ----

			Get_MPI_Buffer (rank);
			
			if (mpi_buffer.Size () == 0) continue;

			//---- retrieve the plan statistics

			mpi_buffer.Get_Data (lvalue, sizeof (lvalue));

			plan_file->Add_Files (lvalue [0]);
			plan_file->Add_Records (lvalue [1]);
			plan_file->Add_Households (lvalue [2]);
			plan_file->Add_Persons (lvalue [3]);
			plan_file->Add_Tours (lvalue [4]);
			plan_file->Add_Trips (lvalue [5]);
			num_trips += lvalue [6];

			//---- selection map data ----

			select_map.UnPack (mpi_buffer);
		}
		Show_Message (1);

		plan_file->Close ();

	} else {	//---- slave ----

		if (Partition_Range ()) {
			Send_MPI_Message (String ("Slave %d Read %ss %d..%d") % MPI_Rank () % plan_file->File_Type () % First_Partition () % Last_Partition ());
		} else {
			Send_MPI_Message (String ("Slave %d Read %s %d") % MPI_Rank () % plan_file->File_Type () % First_Partition ());
		}

		//---- send plan statistics ----

		lvalue [0] = plan_file->Num_Files ();
		lvalue [1] = plan_file->Num_Records ();
		lvalue [2] = plan_file->Num_Households ();
		lvalue [3] = plan_file->Num_Persons ();
		lvalue [4] = plan_file->Num_Tours ();
		lvalue [5] = plan_file->Num_Trips ();
		lvalue [6] = num_trips;
		plan_file->Close ();

		mpi_buffer.Data (lvalue, sizeof (lvalue));

		//---- selection map data ----

		select_map.Pack (mpi_buffer);

		//---- send the data buffer ----

		Send_MPI_Buffer ();

		if (!new_plan_flag) Exit_Stat (DONE);
	}
#endif
}
