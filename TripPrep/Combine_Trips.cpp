//*********************************************************
//	Combine_Trips.cpp - combine temporary trips to output trips
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Combine_Trips
//---------------------------------------------------------

void TripPrep::Combine_Trips (bool mpi_flag)
{
	int num_temp;

	//---- initialize the file handles ----

	new_trip_file->File_Type ("New Trip File");
	new_trip_file->Clear_Fields ();
	new_trip_file->Reset_Counters ();

	if (MPI_Size () > 1 && !mpi_flag) {
		new_trip_file->Dbase_Format (BINARY);
	} else {
		new_trip_file->Dbase_Format ((Format_Type) new_format);
		if (mpi_flag) {
			pathname.erase (pathname.size () - 2);
		}
	}
	new_trip_file->Part_Flag (false);
	new_trip_file->Pathname (pathname);
	new_trip_file->First_Open (false);

	new_trip_file->Open (0);

	Trip_File temp_file;

	temp_file.Part_Flag (true);
	if (mpi_flag) {
		temp_file.Pathname (pathname);
	} else {
		temp_file.Pathname (pathname + ".temp");
	}
	temp_file.First_Open (false);

	num_temp = temp_file.Num_Parts ();

	if (num_temp == 0) {
		Error ("No Trip Files to Combine");
	}

	//---- process the trip files ----

	if (Trip_Sort () == TIME_SORT) {
		Time_Combine (&temp_file, num_temp);
	} else {
		Trip_Combine (&temp_file, num_temp);
	}
	new_trip_file->Close ();
}
