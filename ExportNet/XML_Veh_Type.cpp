//*********************************************************
//	XML_Veh_Type.cpp - convert the vehicle type data
//*********************************************************

#include "ExportNet.hpp"

//---------------------------------------------------------
//	Write_Link
//---------------------------------------------------------

void ExportNet::XML_Vehicle_Types (void)
{
	Use_Type type;

	fstream &fh = xml_file.File ();

	Veh_Type_Itr type_itr;

	//---- write the vehicle types ----

	Set_Progress ();

	fh << "\n<VEHTYPES>" << endl;

	for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
		Show_Progress ();

		type = (Use_Type) type_itr->Use ();

		fh << "<VEHTYPE NO=\"" << type_itr->Type () << "\" NAME=\"" << Veh_Use_Code (type) << "\" VEHCATEGORY=\"";

		if (type == BUS) {
			fh << "Bus";
		} else if (type == RAIL) {
			fh << "Tram";
		} else if (type == TRUCK || type == LIGHTTRUCK || type == HEAVYTRUCK) {
			fh << "HGV";
		} else {
			fh << "Car";
		}
		fh << "\" />" << endl;
	}
	fh << "</VEHTYPES>" << endl;

	//---- write the vehicle classes ----

	fh << "<VEHCLASSES>" << endl;

	//---- auto vehicles ----
			
	fh << "<VEHCLASS ID=\"C\">" << endl;

	for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
		Show_Progress ();
		type = (Use_Type) type_itr->Use ();

		if (type == CAR || type == SOV || type == HOV2 || type == HOV3 || 
			type == HOV4 || type == TAXI) {

			fh << "<VEHTYPEID NO=\"" << type_itr->Type () << "\" />" << endl;
		}
	}
	fh << "</VEHCLASS>" << endl;

	//---- transit vehicles ----

	fh << "<VEHCLASS ID=\"PtDefault\">" << endl;

	for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
		Show_Progress ();
		type = (Use_Type) type_itr->Use ();

		if (type == BUS || type == RAIL) {
			fh << "<VEHTYPEID NO=\"" << type_itr->Type () << "\" />" << endl;
		}
	}
	fh << "</VEHCLASS>" << endl;

	//---- truck vehicles ----

	fh << "<VEHCLASS ID=\"T\">" << endl;

	for (type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++) {
		Show_Progress ();
		type = (Use_Type) type_itr->Use ();

		if (type == TRUCK || type == LIGHTTRUCK || type == HEAVYTRUCK) {
			fh << "<VEHTYPEID NO=\"" << type_itr->Type () << "\" />" << endl;
		}
	}
	fh << "</VEHCLASS>" << endl;
	fh << "</VEHCLASSES>" << endl;
}
