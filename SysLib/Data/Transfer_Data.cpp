//*********************************************************
//	Transfer_Data.cpp - transit stop transfer data
//*********************************************************

#include "Transfer_Data.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Sum_Data
//---------------------------------------------------------

void Transfer_Data::Sum_Data (Transfer_Data &data)
{
	walk_board += data.Walk_Board ();
	bus_board += data.Bus_Board ();
	rail_board += data.Rail_Board ();
	drive_board += data.Drive_Board ();
	walk_alight += data.Walk_Alight ();
	bus_alight += data.Bus_Alight ();
	rail_alight += data.Rail_Alight ();
	drive_alight += data.Drive_Alight ();
}

//---------------------------------------------------------
//	Replicate
//---------------------------------------------------------

void Transfer_Array::Replicate (Transfer_Array &xfer_array)
{
	if (xfer_array.size () > 0) {
		Transfer_Itr itr;
		Transfer_Data transfer_data;

		assign ((int) xfer_array.size (), transfer_data);
	}
}

//---------------------------------------------------------
//	Merge_Data
//---------------------------------------------------------

void Transfer_Array::Merge_Data (Transfer_Array &xfer_array)
{
	if (xfer_array.size () > 0) {
		int i;
		Transfer_Itr itr;

		for (i=0, itr = begin (); itr != end (); itr++, i++) {
			itr->Sum_Data (xfer_array [i]);
		}
	}
}
