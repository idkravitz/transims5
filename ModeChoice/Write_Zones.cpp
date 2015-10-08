//*********************************************************
//	Write_Zones.cpp - write the zone mode shares
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Write_Zones
//---------------------------------------------------------

void ModeChoice::Write_Zones (void)
{
	int zone, fld;
	double value;

	Show_Message ("Writing Zone Data");
	Set_Progress ();

	//---- process each zone ----

	for (zone=0; zone < zones; zone++) {
		Show_Progress ();

		if (prod_flag) {
			prod_file.Put_Field (0, zone+1);

			for (fld=0; fld <= num_modes; ) {
				value = prod_share [fld] [zone];
				prod_file.Put_Field (++fld, value);
			}
			prod_file.Write ();
		}
		if (attr_flag) {
			attr_file.Put_Field (0, zone+1);

			for (fld=0; fld <= num_modes; ) {
				value = attr_share [fld] [zone];
				attr_file.Put_Field (++fld, value);
			}
			attr_file.Write ();
		}
	}
	End_Progress ();
	if (prod_flag) {
		prod_file.Close ();
	}
	if (attr_flag) {
		attr_file.Close ();
	}
}
