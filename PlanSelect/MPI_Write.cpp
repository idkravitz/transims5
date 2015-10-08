//*********************************************************
//	MPI_Write.cpp - MPI write setup
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	MPI_Write
//---------------------------------------------------------

void PlanSelect::MPI_Write (void)
{
#ifdef MPI_EXE
	if (Master ()) {

		mpi_buffer.Data (&num_select, sizeof (num_select));

		if (num_select > 0) {
			select_map.Pack (mpi_buffer);
		}
		for (int rank=1; rank < MPI_Size (); rank++) {
			Send_MPI_Buffer (rank);
		}

	} else {	//---- slave ----

		num_select = 0;
		select_map.clear ();

		Get_MPI_Buffer (0);

		if (mpi_buffer.Size () > 0) {
			mpi_buffer.Get_Data (&num_select, sizeof (num_select));

			if (num_select > 0) {
				select_map.UnPack (mpi_buffer);
			}
		}
	}
#endif
}
