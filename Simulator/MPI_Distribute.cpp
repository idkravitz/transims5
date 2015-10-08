//*********************************************************
//	MPI_Distribute.cpp - distribute travelers to MPI machines
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	MPI_Distribute
//---------------------------------------------------------

bool Simulator::MPI_Distribute (bool status)
{
#ifdef MPI_EXE
	int i, num, rec, traveler, part, lvalue [3];
	Sim_Traveler_Ptr sim_traveler_ptr = 0;

	if (Master ()) {
		int rank;
		Int2_Itr itr;
		Integers *list;
		Int_Itr t_itr;

		lvalue [0] = status;
		lvalue [2] = (int) sim_traveler_array.size ();

		for (rank=0, itr = mpi_range.begin (); itr != mpi_range.end (); itr++, rank++) {
			if (rank == 0) continue;

			//---- count the part lists assigned to each machine ----

			mpi_buffer.Size (0);
			rec = 0;

			for (part = itr->first; part <= itr->second; part++) {
				rec += (int) mpi_parts [part].size ();
				list = &mpi_parts [part];

				for (t_itr = list->begin (); t_itr != list->end (); t_itr++) {
					traveler = *t_itr;
					sim_traveler_ptr = sim_traveler_array [traveler];

					mpi_buffer.Add_Data (&traveler, sizeof (traveler));
					mpi_buffer.Add_Data (&part, sizeof (part));

					sim_traveler_ptr->Pack (mpi_buffer);
				}
				list->clear ();
			}
			lvalue [1] = rec;

			//---- distribute the travel plans ----

			Send_MPI_Array (lvalue, 3, rank);

			if (rec == 0) continue;

			Send_MPI_Buffer (rank);
		}

	} else {	//---- slave ----

		Get_MPI_Array (lvalue, 3, 0);

		status = (lvalue [0] != 0);

		rec = lvalue [1];

		if (rec == 0) return (status);

		//---- expand the traveler array ----
			
		num = lvalue [2] - (int) sim_traveler_array.size ();

		if (num > 0) {
			sim_traveler_array.insert (sim_traveler_array.end (), num, sim_traveler_ptr);
		}

		//---- retrieve the traveler data ----

		Get_MPI_Buffer (0);

		//---- unpack the traveler data ----

		for (i=0; i < rec; i++) {
			mpi_buffer.Get_Data (&traveler, sizeof (traveler));
			
			mpi_buffer.Get_Data (&part, sizeof (part));

			part -= First_Partition ();

			sim_traveler_ptr = sim_traveler_array [traveler];
			if (sim_traveler_ptr != 0) delete sim_traveler_ptr;
						
			sim_traveler_ptr = new Sim_Traveler_Data ();
			sim_traveler_ptr->UnPack (mpi_buffer);
			
			sim_traveler_array [traveler] = sim_traveler_ptr;
			work_step.Put (traveler, part);
		}
	}
#endif
	return (status);
}
