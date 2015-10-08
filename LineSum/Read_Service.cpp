//*********************************************************
//	Read_Service.cpp - read the service file
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Read_Service
//---------------------------------------------------------

void LineSum::Read_Service (void)
{
	String line;
	int peak, offpeak, service;

	//---- read the line service file ----

	Show_Message (String ("Reading %s -- Record") % service_file.File_Type ());
	Set_Progress ();

	//---- read each record ----

	while (service_file.Read ()) {
		Show_Progress ();

		line = service_file.Get_String (line_fld);
		line.To_Upper ();

		peak = service_file.Get_Integer (peak_fld);
		offpeak = service_file.Get_Integer (offpeak_fld);
		service = (peak << 16) + offpeak;

		service_map.insert (Str_ID_Data (line, service));
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % service_file.File_Type () % service_map.size ());
	
	service_file.Close ();
}
