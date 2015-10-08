//*********************************************************
//	Read_Persons.cpp - read the person file
//*********************************************************

#include "NewFormat.hpp"
#include "Household_Data.hpp"

//---------------------------------------------------------
//	Read_Persons
//---------------------------------------------------------

void NewFormat::Read_Persons (void)
{
	int hhold;

	Person_Data person_rec;
	Int_Map_Itr map_itr;
	Household_Data *hhold_ptr;

	//---- process the person file ----

	Show_Message (String ("Reading %s -- Record") % person_file.File_Type ());
	Set_Progress ();

	while (person_file.Read ()) {
		Show_Progress ();

		hhold = person_file.Household ();

		map_itr = hhold_map.find (hhold);
		if (map_itr == hhold_map.end ()) {
			Error (String ("Person %d Household %d was Not Found") % Progress_Count () % hhold);
		}
		hhold_ptr = &hhold_array [map_itr->second];

		person_rec.Person (person_file.Person ());
		person_rec.Age (person_file.Age ());
		person_rec.Relate (person_file.Relate ());
		person_rec.Gender (person_file.Gender ());
		person_rec.Work (person_file.Work ());
		person_rec.Drive (person_file.Drive ());

		hhold_ptr->push_back (person_rec);
	}
	End_Progress ();
	person_file.Close ();

	Print (2, String ("Number of %s Records = %d") % person_file.File_Type () % Progress_Count ());
}
