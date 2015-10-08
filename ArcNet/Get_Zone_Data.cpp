//*********************************************************
//	Get_Zone_Data.cpp - additional zone processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool ArcNet::Get_Zone_Data (Zone_File &file, Zone_Data &zone_rec)
{
	zone_rec.Zone (0);

	if (arcview_zone.Is_Open ()) {
		arcview_zone.Copy_Fields (file);

		XYZ_Point *point = arcview_zone.Get_Points ();

		point->x = file.X ();
		point->y = file.Y ();
		point->z = file.Z ();

		if (!arcview_zone.Write_Record ()) {
			Error (String ("Writing %s") % arcview_zone.File_Type ());
		}
	}
	return (false);
}
