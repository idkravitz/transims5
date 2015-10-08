//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TransitNet::Execute (void)
{
	int old_parking, old_access, old_stop, old_lines, nlines, old_route, old_schedule, old_driver;

	//---- read the network ----

	Data_Service::Execute ();

	//---- prepare the network data ----

	Data_Setup ();

	old_parking = nparking = (int) parking_array.size ();
	old_access = naccess = (int) access_array.size ();
	old_stop = nstop = (int) stop_array.size ();
	old_lines = (int) line_array.size ();
	old_route = line_array.Route_Records ();
	old_schedule = line_array.Schedule_Records ();
	old_driver = line_array.Driver_Records ();
	new_stop = max_stop;
	new_access = max_access;

	//---- create the route data ----

	Build_Routes ();

	//---- write the transit files ----

	Write_Stops ();
	Write_Lines ();
	Write_Schedules ();
	Write_Drivers ();

	//---- read the park and ride file ----

	if (parkride_flag) {
		Read_ParkRide ();
		Write_Parking_Lots ();
	}

	//---- add access links to rail stations ----

	if (access_flag) {
		Station_Access ();
		Write_Access_Links ();
	}

	//---- write summary statistics ----

	Break_Check (12);
	Write (1);
	if (parkride_flag) {
		Write (1, String ("Number of Parking Records = %d (%d new)") %
				nparking % (nparking - old_parking));
	}
	if (access_flag) {
		naccess = (int) access_array.size ();
		Write (1, "Number of Access Records = ") << naccess;
		if (old_access > 0) Write (0, String (" (%d new)") % (naccess - old_access));
	}
	nstop = (int) stop_array.size ();
	Write (1, "Number of Transit Stops = ") << nstop;
	if (old_stop > 0) Write (0, String (" (%d new)") % (nstop - old_stop));

	nlines = (int) line_array.size ();
	Write (1, "Number of Transit Routes = ") << nlines;
	if (old_lines > 0) {
		nlines -= old_lines;
		if (line_edit) {
			Write (0, String (" (%d new %d changed)") % nlines % line_edit);
		} else {
			Write (0, String (" (%d new)") % nlines);
		}
	}
	nroute = line_array.Route_Records ();
	Write (1, "Number of Transit Route Records = ") << nroute;
	if (old_route > 0) {
		nroute -= old_route;
		if (route_edit) {
			Write (0, String (" (%d new %d changed)") % nroute % route_edit);
		} else {
			Write (0, String (" (%d new)") % nroute);
		}
	}
	nschedule = line_array.Schedule_Records ();
	Write (1, "Number of Transit Schedule Records = ") << nschedule;
	if (old_schedule > 0) {
		nschedule -= old_schedule;
		if (schedule_edit > nschedule) {
			Write (0, String (" (%d new %d changed)") % nschedule % (schedule_edit - nschedule));
		} else {
			Write (0, String (" (%d new)") % nschedule);
		}
	}
	ndriver = line_array.Driver_Records ();
	Write (1, "Number of Transit Driver Records = ") << ndriver;
	if (old_driver > 0) {
		ndriver -= old_driver;
		if (driver_edit > ndriver) {
			Write (0, String (" (%d new %d changed)") % ndriver % (driver_edit - ndriver));
		} else {
			Write (0, String (" (%d new)") % ndriver);
		}
	}
	if (end_warnings) {
		Write (2, "Number of Route End Node Warnings = ") << end_warnings;
	}
	if (parking_warnings) {
		Write (2, "Number of Parking Lot Warning Messages = ") << parking_warnings;
	}
	Exit_Stat (DONE);
}


