//*********************************************************
//	MPI_Setup.cpp - setup MPI processing ranges
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	MPI_Setup
//---------------------------------------------------------

void TripPrep::MPI_Setup (void)
{
#ifdef MPI_EXE
	if (MPI_Size () == 1) return;

	int range [2];

	if (Master ()) {
		int num_parts, rank;
		Db_File temp_file;
		String key, dir;
		Integers count;
		Int_Itr itr;
		Int_RItr ritr;

		dir = Get_Control_String (PROJECT_DIRECTORY);
		if (!dir.empty ()) {
			Fix_Directory (dir);
		}
		key = Get_Control_String (System_File_ID (TRIP));
	
		temp_file.Filename (dir + key);
		num_parts = temp_file.Num_Parts ();
		if (num_parts < 2) return;

		Write (1, "Number of MPI Processors = ") << MPI_Size ();
		Write (1, "Number of Trip Partitions = ") << num_parts;
		Write (1);

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
			range [0] = num_parts;
			num_parts += *itr;
			range [1] = num_parts - 1;

			if (rank == 0) {
				First_Partition (range [0]);
				Last_Partition (range [1]);
			} else {
				Send_MPI_Array (range, 2, rank);
			}
		}

	} else {	//---- slave ----

		Get_MPI_Array (range, 2, 0);

		First_Partition (range [0]);
		Last_Partition (range [1]);
	}
#endif
}
