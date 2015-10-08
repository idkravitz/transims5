//*********************************************************
//	MPI_Setup.cpp - setup MPI processing ranges
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	MPI_Setup
//---------------------------------------------------------

void Simulator::MPI_Setup (void)
{
#ifdef MPI_EXE
	if (MPI_Size () == 1) return;

	int rank, num_parts, part;
	Integers count;
	Int_Itr itr;
	Int_RItr ritr;
	Int2_Key mpi_key;
	Integers int_rec;

	//---- assign partitions to MPI machines ----

	num_parts = Num_Parts ();
	if (num_parts < 2) {
		if (Master ()) Error ("Multiple Subareas or Partitions are Required for MPI Processing");
		Exit_Stat (FATAL);
	}

	mpi_parts.assign (num_parts, int_rec);
	part_rank.assign (num_parts, 0);

	if (Master ()) {
		Write (1, "Number of MPI Processors = ") << MPI_Size ();
		Write (1);
	}
	count.assign (MPI_Size (), 0);

	for (itr = count.begin (); itr != count.end (); itr++) {
		(*itr)++;
		if (--num_parts == 0) break;
	}
	while (num_parts > 0) {
		for (ritr = count.rbegin (); ritr != count.rend (); ritr++) {
			(*ritr)++;
			if (--num_parts == 0) break;
		}
	}
	num_parts = 0;

	for (rank=0, itr = count.begin (); itr != count.end (); itr++, rank++) {
		mpi_key.first = num_parts;
		num_parts += *itr;
		mpi_key.second = num_parts - 1;

		mpi_range.push_back (mpi_key);

		if (rank == MPI_Rank ()) {
			First_Partition (mpi_key.first);
			Last_Partition (mpi_key.second);
		}
		for (part=mpi_key.first; part <= mpi_key.second; part++) {
			part_rank [part] = rank;
		}
	}
#endif
}
