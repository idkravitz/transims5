//*********************************************************
//	MPI_Write.cpp - MPI write setup
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	MPI_Write
//---------------------------------------------------------

void PlanCompare::MPI_Write (void)
{
#ifdef MPI_EXE
	if (Master ()) {

		mpi_buffer.Data (&num_select, sizeof (num_select));

		if (num_select > 0) {
			selected.Pack (mpi_buffer);
		}
		for (int rank=1; rank < MPI_Size (); rank++) {
			Send_MPI_Buffer (rank);
		}

	} else {	//---- slave ----

		num_select = 0;
		selected.clear ();

		Get_MPI_Buffer (0);

		if (mpi_buffer.Size () > 0) {
			mpi_buffer.Get_Data (&num_select, sizeof (num_select));

			if (num_select > 0) {
				selected.UnPack (mpi_buffer);
			}
		}
	}
#endif
}
