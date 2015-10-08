//*********************************************************
//	ConvertTrips.hpp - Trip Table Conversion Utility
//*********************************************************

#ifndef CONVERTTRIPS_HPP
#define CONVERTTRIPS_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Time_Periods.hpp"
#include "Matrix_Data.hpp"
#include "Factor_Data.hpp"
#include "Factor_File.hpp"
#include "Diurnal_Data.hpp"
#include "User_Program.hpp"
#include "Dtime.hpp"
#include "TypeDefs.hpp"

#include "File_Group.hpp"
#include "Convert_Location.hpp"
#include "Tour_Map.hpp"
#include "Tour_File.hpp"

//---------------------------------------------------------
//	ConvertTrips - execution class definition
//---------------------------------------------------------

class ConvertTrips : public Data_Service, public Select_Service
{
public:
	ConvertTrips (void);
	virtual ~ConvertTrips (void);

	virtual void Execute (void);

protected:
	enum ConvertTrips_Keys { 
		FIRST_HOUSEHOLD_NUMBER = 1, ADDITIONAL_TRAVEL_TIME, TOUR_DATA_FILE, TOUR_DATA_FORMAT, 
		TOUR_HOUSEHOLD_FIELD, TOUR_PERSON_FIELD, TOUR_NUMBER_FIELD, TOUR_PURPOSE_FIELD, 
		TOUR_MODE_FIELD, TOUR_ORIGIN_FIELD, TOUR_DESTINATION_FIELD, TOUR_STOP_OUT_FIELD, 
		TOUR_STOP_IN_FIELD, TOUR_START_FIELD, TOUR_RETURN_FIELD, TOUR_GROUP_FIELD, TOUR_GROUP_SCRIPT,
		TRIP_TABLE_FILE, TRIP_TABLE_FORMAT, TRIP_SCALING_FACTOR, TIME_DISTRIBUTION_FILE,	
		TIME_DISTRIBUTION_FORMAT, TIME_DISTRIBUTION_SCRIPT, TIME_DISTRIBUTION_FIELD, 
		TIME_DISTRIBUTION_TYPE, TIME_PERIOD_RANGE, TIME_SCHEDULE_CONSTRAINT, ORIGIN_WEIGHT_FIELD, 
		DESTINATION_WEIGHT_FIELD, DISTANCE_WEIGHT_FLAG, STOP_WEIGHT_FIELD, TRAVELER_TYPE_SCRIPT, 
		TRAVELER_TYPE_CODE, TRIP_PURPOSE_CODE, TRAVEL_MODE_CODE, RETURN_TRIP_FLAG, ACTIVITY_DURATION, 
		TRIP_STOP_DURATION, TRIP_PRIORITY_CODE, VEHICLE_TYPE_CODE, VEHICLE_PASSENGERS, 
		AVERAGE_TRAVEL_SPEED, TRIP_FACTOR_FILE, TRIP_FACTOR_FORMAT, NEW_TIME_DISTRIBUTION_FILE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Access_Data (Access_File &file, Access_Data &data);
	virtual bool Get_Household_Data (Household_File &file, Household_Data &data, int partition = 0);
	virtual bool Get_Vehicle_Data (Vehicle_File &file, Vehicle_Data &data, int partition = 0);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);

private:
	enum ConvertTrips_Reports { GROUP_SCRIPT = 1, GROUP_STACK, DIURNAL_SCRIPT, DIURNAL_STACK, 
		TIME_EQUIV, ZONE_EQUIV, TRAVELER_SCRIPT, TRAVELER_STACK };

	bool time_flag, equiv_flag, skim_flag, zone_loc_flag, tour_flag, script_flag, diurnal_flag, problem_flag;
	int num_zone, tot_tours, tot_trips, tot_errors, group_number;
	int hhold_id, new_trips, new_hholds, new_persons, new_vehicles;
	int trip_copy, hhold_copy, veh_copy, max_hh_in, max_veh_in;
	int group_fld, org_fld, des_fld, start_fld, mode_fld, purpose_fld, duration_fld, constraint_fld, priority_fld, veh_type_fld, passengers_fld, type_fld;
	Dtime additional_time, midnight, max_start, minute, two_minutes, five_minutes, ten_minutes, one_hour;
	double tot_add, tot_del;

	Trip_File *trip_file;
	Vehicle_File *vehicle_file;
	Household_File *household_file;
	Problem_File *problem_file;

	Random random_org, random_des, random_tod, random_stop;
	Time_Periods tour_periods;
	Integers zone_loc;
	Int_Map field_map;

	Db_File diurnal_file;

	Integers group_index;
	File_Group_Array file_group;
	Convert_Loc_Array convert_array;

	Factor_Period default_factor;

	Dtime *skim_ptr;
	Skim_File *skim_file;

	Db_Base traveler_file;

	Tour_File tour_file;
	Db_File group_script;	
	User_Program user_group;
	Db_Base script_base;

	//---- methods ----

	void Compile_Scripts (void);
	void Zone_Location (void);
	void Parking_Check (void);
	void Tour_Processing (void);
	void Read_Trip_Table (File_Group_Itr group);
	int  Set_Trips (File_Group_Itr group, int org, int des, int trips, int num, int t1, int t2, bool return_flag);
	bool Set_Time (File_Group *group, Dtime &start, Dtime &end, Dtime ttim, int num);
	bool Locate_OD (File_Group *group, int &org, int &des, int &stop, int &park, double &dist1, double &dist2);
	void Adjust_Trips (void);
	void Diurnal_Results (void);
};
#endif
