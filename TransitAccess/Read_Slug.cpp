//*********************************************************
//	Read_Slug.cpp - read the slug zone file
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	Read_Slug
//---------------------------------------------------------

void TransitAccess::Read_Slug (void)
{
	int zone, nslug;
	Zone_Map_Itr map_itr;

	Show_Message (String ("Reading %s -- Record") % slug_file.File_Type ());
	Set_Progress ();

	nslug = 0;

	while (slug_file.Read ()) {
		Show_Progress ();

		zone = slug_file.Get_Integer (slug_fld);

		map_itr = zone_map.find (zone);

		if (map_itr != zone_map.end ()) {
			map_itr->second.slug = 1;
			nslug++;
		}
	}
	End_Progress ();
	slug_file.Close ();

	Print (2, "Number of Slug Zone Records = ") << Progress_Count ();

	if (nslug != Progress_Count ()) {
		Print (1, "Number of Slug Zones Kept = ") << nslug;
	}
}
