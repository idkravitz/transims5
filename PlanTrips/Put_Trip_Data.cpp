//*********************************************************
//	Put_Trip_Data.cpp - write a new trip file
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Put_Trip_Data
//---------------------------------------------------------

int PlanTrips::Put_Trip_Data (Trip_File &file, Trip_Data &data)
{
	file.Household (data.Household ());
	file.Person (data.Person ());
	file.Tour (MAX (data.Tour (), 1));
	file.Trip (data.Trip ());
	file.Start (data.Start ());
	file.End (data.End ());
	file.Duration (data.Duration ());
	file.Origin (data.Origin ());
	file.Destination (data.Destination ());

	file.Purpose (data.Purpose ());
	file.Mode (data.Mode ());
	file.Constraint (data.Constraint ());
	file.Priority (data.Priority ());

	file.Vehicle (data.Vehicle ());
	file.Passengers (data.Passengers ());
	file.Type (data.Type ());
	file.Partition (data.Partition ());

	if (!file.Write ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	file.Add_Trip (data.Household (), data.Person (), data.Tour ());
	return (1);
}
