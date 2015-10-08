//*********************************************************
//	Read_Veh_Types.cpp - Read the Vehicle Type File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Veh_Types
//---------------------------------------------------------

void Data_Service::Read_Veh_Types (void)
{
	Veh_Type_File *file = (Veh_Type_File *) System_File_Handle (VEHICLE_TYPE);
	
	int num;
	double length;
	Veh_Type_Data veh_type_rec;
	Int_Map_Stat map_stat;
	Veh_Type_Itr veh_type_itr;

	//---- store the vehicle type data ----

	Show_Message (String ("Reading %s -- Record") % file->File_Type ());
	Set_Progress ();
	
	Initialize_Veh_Types (*file);

	while (file->Read ()) {
		Show_Progress ();

		veh_type_rec.Clear ();

		if (Get_Veh_Type_Data (*file, veh_type_rec)) {
			map_stat = veh_type_map.insert (Int_Map_Data (veh_type_rec.Type (), (int) veh_type_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Vehicle Type Number = ") << veh_type_rec.Type ();
				continue;
			} else {
				veh_type_array.push_back (veh_type_rec);
			}
		}
	}
	End_Progress ();
	file->Close ();
	
	Print (2, String ("Number of %s Records = %d") % file->File_Type () % Progress_Count ());

	num = (int) veh_type_array.size ();

	if (num && num != Progress_Count ()) {
		Print (1, String ("Number of %d Data Records = %d") % file->File_ID () % num);
	}
	if (num > 0) System_Data_True (VEHICLE_TYPE);

	//---- set the PCE factor ----

	num = Use_Code ("CAR");
	length = 0.0;

	for (veh_type_itr = veh_type_array.begin (); veh_type_itr != veh_type_array.end (); veh_type_itr++) {
		if ((veh_type_itr->Use () & num) != 0) {
			length = veh_type_itr->Length ();
			break;
		}
	}
	if (length <= 0.0) return;

	for (veh_type_itr = veh_type_array.begin (); veh_type_itr != veh_type_array.end (); veh_type_itr++) {
		veh_type_itr->PCE (veh_type_itr->Length () / length);
	}
}

//---------------------------------------------------------
//	Initialize_Veh_Types
//---------------------------------------------------------

void Data_Service::Initialize_Veh_Types (Veh_Type_File &file)
{
	int percent = System_Data_Reserve (VEHICLE_TYPE);

	if (veh_type_array.capacity () == 0 && percent > 0) {
		int num = file.Num_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		}
		if (num > 1) {
			veh_type_array.reserve (num);
			if (num > (int) veh_type_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Veh_Type_Data
//---------------------------------------------------------

bool Data_Service::Get_Veh_Type_Data (Veh_Type_File &file, Veh_Type_Data &veh_type_rec)
{
	int i, j, k, type;
	double dvalue, fac1, fac2;

	//---- check the vehicle type ----

	type = file.Type ();
	if (type <= 0) return (false);

	//---- set the record key ----

	veh_type_rec.Type (type);

	if (file.SubType_Flag () && file.Version () <= 40) {
		veh_type_rec.Type (VehType40_Map (type, file.SubType ()));
	}

	//---- vehicle length ----

	veh_type_rec.Length (file.Length ());

	if (veh_type_rec.Length () < Round (4.0)) {
		dvalue = UnRound (veh_type_rec.Length ());

		Warning (String ("Vehicle Type %d Length %.1lf is Out of Range") % type % dvalue);
		return (false);
	}

	//---- maximum speed ----

	veh_type_rec.Max_Speed (file.Max_Speed ());

	if (veh_type_rec.Max_Speed () < Round (5.0)) {
		dvalue = UnRound (veh_type_rec.Max_Speed ());

		Warning (String ("Vehicle Type %d Maximum Speed %.1lf is Out of Range") % type % dvalue);
		return (false);
	}

	//---- maximum acceleration ----

	veh_type_rec.Max_Accel (file.Max_Accel ());

	if (veh_type_rec.Max_Accel () < Round (0.5)) {
		dvalue = UnRound (veh_type_rec.Max_Accel ());

		Warning (String ("Maximum Acceleration %.1lf for Vehicle Type %d-%d is Out of Range") % type % dvalue); 
		return (false);
	}

	//---- maximum deceleration ----

	veh_type_rec.Max_Decel (file.Max_Decel ());

	if (veh_type_rec.Max_Decel () < Round (0.5)) {
		veh_type_rec.Max_Decel (veh_type_rec.Max_Accel ());
	}

	//---- operating cost ----

	veh_type_rec.Op_Cost (file.Op_Cost ());

	//---- vehicle use code ----

	if (!file.Use_Flag () && file.Version () <= 40) {
		veh_type_rec.Use (Veh_Type_Map (type));
	} else {
		veh_type_rec.Use (file.Use ());
	}
	if (veh_type_rec.Use () == CAR) veh_type_rec.Use (SOV);

	//---- vehicle capacity ----

	veh_type_rec.Capacity (file.Capacity ());
	
	if (veh_type_rec.Capacity () < 1) {
		Warning (String ("Vehicle Type %d Capacity %d is Out of Range") % type % veh_type_rec.Capacity ()); 
		return (false);
	}

	//---- loading/unloading rate (seconds per passenger * 100) ----

	veh_type_rec.Loading (file.Loading ());
	veh_type_rec.Unloading (file.Unloading ());

	if (veh_type_rec.Loading () <= 0) {
		veh_type_rec.Loading (3.0);
	}
	if (veh_type_rec.Unloading () <= 0) {
		veh_type_rec.Unloading (2.0);
	}

	//---- loading method ----

	veh_type_rec.Method (file.Method ());

	//---- dwell time ----

	veh_type_rec.Min_Dwell (file.Min_Dwell ());
	veh_type_rec.Max_Dwell (file.Max_Dwell ());

	//---- grade factors ----

	veh_type_rec.Grade_Flag (false);
	fac1 = 1.0;

	for (i=k=0; i < 10; i++) {
		fac2 = file.Grade (i);
		if (fac2 == 0.0) fac2 = 1.0;

		dvalue = (fac2 - fac1) / 10.0;

		for (j=0; j < 10; j++) {
			fac1 += dvalue;
			if (fac1 <= 0.0) fac1 = 0.01;
			veh_type_rec.Grade (k++, fac1);
			if (fac1 != 1.0) veh_type_rec.Grade_Flag (true);
		}
	}
	return (true);
}
