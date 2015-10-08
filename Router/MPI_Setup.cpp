//*********************************************************
//	MPI_Setup.cpp - setup MPI processing ranges
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	MPI_Setup
//---------------------------------------------------------

void Router::MPI_Setup (void)
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
		key = Get_Control_String (System_File_ID (PLAN));

		if (!key.empty ()) {
			temp_file.Filename (dir + key);
			num_parts = temp_file.Num_Parts ();
			if (num_parts < 2) return;
		} else {
			key = Get_Control_String (System_File_ID (TRIP));

			if (key.empty ()) {
				Error ("A Trip or Plan File is Required");
				return;
			}
			temp_file.Filename (dir + key);
			num_parts = temp_file.Num_Parts ();
			if (num_parts < 2) {
				key = Get_Control_String (System_File_ID (SELECTION));
				
				temp_file.Filename (dir + key);
				num_parts = temp_file.Num_Parts ();

				if (num_parts < 2) {
					Selection_File file;
					file.First_Open (false);
					if (!file.Open (dir + key)) {
						Error (String ("Opening %s") % file.File_Type ());
					}
					num_parts = 0;
					Show_Message (1, "Scanning the ") << file.File_Type ();

					while (file.Read ()) {
						rank = file.Partition ();
						if (rank >= num_parts) num_parts = rank + 1;
					}
					Show_Message (1);
				}
				if (num_parts < 2) return;
			}
		}
		Write (1, "Number of MPI Processors = ") << MPI_Size ();
		Write (1, "Number of Plan Partitions = ") << num_parts;
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
