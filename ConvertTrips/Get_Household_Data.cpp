//*********************************************************
//	Get_Household_Data.cpp - copy the household file
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Get_Household_Data
//---------------------------------------------------------

bool ConvertTrips::Get_Household_Data (Household_File &file, Household_Data &data, int partition)
{
	int hhold;
	Person_Index person_index;

	hhold = file.Household ();
	if (hhold > max_hh_in) max_hh_in = hhold;

	hhold_map.insert (Int_Map_Data (hhold, 1));

	if (file.Nested ()) {
		person_index.Household (hhold);
		person_index.Person (file.Person ());

		person_map.insert (Person_Map_Data (person_index, 1));
	}
	if (select_households && !hhold_range.In_Range (hhold)) return (false);
	
	household_file->Copy_Fields (file);

	if (!household_file->Write (file.Nested ())) {
		Error ("Writing Household File");
	}
	hhold_copy++;

	data.Household (hhold);
	hhold = partition;

	//---- don't save the record ----

	return (false);
}
