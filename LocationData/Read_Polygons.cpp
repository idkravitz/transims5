//*********************************************************
//	Read_Polygons.cpp - read the boundary polygon files
//*********************************************************

#include "LocationData.hpp"

//---------------------------------------------------------
//	Read_Polygons
//---------------------------------------------------------

void LocationData::Read_Polygons (void)
{
	int index;

	Polygon_Itr poly_itr;
	Points_Map_Stat map_stat;

	//---- read the arcview boundary file ----

	for (poly_itr = polygons.begin (); poly_itr != polygons.end (); poly_itr++) {
		Show_Message (String ("Reading %s -- Record") % poly_itr->file->File_Type ());
		Set_Progress ();

		//---- read each record ----

		index = 0;

		while (poly_itr->file->Read_Record ()) {
			Show_Progress ();

			index++;
			map_stat = poly_itr->polygon.insert (Points_Map_Data (index, *poly_itr->file));

			//----- add to the database ----

			poly_itr->data_db->Copy_Fields (*poly_itr->file);
			poly_itr->data_db->Put_Field (1, index);

			poly_itr->data_db->Write_Record (index);
		}
		End_Progress ();

		Print (2, String ("Number of %s Records = %d") % poly_itr->file->File_Type () % Progress_Count ());
		
		poly_itr->file->Close ();
	}
}
