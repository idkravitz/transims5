//******************************************************** 
//	Trip_File.hpp - Trip File Input/Output
//********************************************************

#ifndef TRIP_FILE_HPP
#define TRIP_FILE_HPP

#include "Db_Header.hpp"
#include "Trip_Data.hpp"
#include "Trip_Index.hpp"

//---------------------------------------------------------
//	Trip_File Class definition
//---------------------------------------------------------

class SYSLIB_API Trip_File : public Db_Header
{
public:
	Trip_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Trip_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	
	//---- read/write functions ----

	bool Read_Trip (Trip_Data &trip);
	bool Write_Trip (Trip_Data &trip);

	void Get_Data (Trip_Data &trip);
	void Put_Data (Trip_Data &trip);

	void Get_Index (Trip_Index &index);
	void Get_Index (Time_Index &index);

	int    Household (void)           { return (Get_Integer (hhold)); }
	int    Person (void)              { return (Get_Integer (person)); }
	int    Tour (void)                { return (Get_Integer (tour)); }
	int    Trip (void)                { return (Get_Integer (trip)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	Dtime  Duration (void)            { return (Get_Time (duration)); }
	int    Origin (void)              { return (Get_Integer (origin)); }
	int    Destination (void)         { return (Get_Integer (destination)); }
	int    Purpose (void)             { return (Get_Integer (purpose)); }
	int    Mode (void)                { return (Get_Integer (mode)); }
	int    Constraint (void)          { return (Get_Integer (constraint)); }
	int    Priority (void)            { return (Get_Integer (priority)); }
	int    Vehicle (void)             { return (Get_Integer (vehicle)); }
	int    Passengers (void)          { return (Get_Integer (passengers)); }
	int    Type (void)                { return (Get_Integer (type)); }
	int    Partition (void)           { return (Get_Integer (partition)); }

	void   Household (int value)      { Put_Field (hhold, value); }
	void   Person (int value)         { Put_Field (person, value); }
	void   Tour (int value)           { Put_Field (tour, value); }
	void   Trip (int value)           { Put_Field (trip, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Duration (Dtime value)     { Put_Field (duration, value); }
	void   Origin (int value)         { Put_Field (origin, value); }
	void   Destination (int value)    { Put_Field (destination, value); }
	void   Purpose (int value)        { Put_Field (purpose, value); }
	void   Mode (int value)           { Put_Field (mode, value); }
	void   Constraint (int value)     { Put_Field (constraint, value); }
	void   Priority (int value)       { Put_Field (priority, value); }
	void   Vehicle (int value)        { Put_Field (vehicle, value); }
	void   Passengers (int value)     { Put_Field (passengers, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Partition (int value)      { Put_Field (partition, value); }
	
	bool   Partition_Flag (void)      { return (partition >= 0); }
	void   Option_Flag (bool flag)    { opt_flag = flag; }

	//---- size functions ----

	void Print_Summary (void);

	int  Num_Records (void)           { return (num_record); }
	int  Num_Households (void)        { return (num_hhold); }
	int  Num_Persons (void)           { return (num_person); }
	int  Num_Tours (void)             { return (num_tour); }
	int  Num_Trips (void)             { return (num_trip); }

	void Add_Records (int num)        { num_record += num; }
	void Add_Households (int num)     { num_hhold += num; }
	void Add_Persons (int num)        { num_person += num; }
	void Add_Tours (int num)          { num_tour += num; }
	void Add_Trips (int num)          { num_trip += num; }

	void Add_Trip (int hhold, int person, int tour);

	void Add_Counters (Trip_File *file) { Max_File (file->Num_Files ()); Add_Records (file->Num_Records ()), 
		Add_Households (file->Num_Households ()); Add_Persons (file->Num_Persons ()); 
		Add_Tours (file->Num_Tours ()); Add_Trips (file->Num_Trips ()); }

	void Reset_Counters (int files = 0, int rec = 0, int hhold = 0, int person = 0, int tour = 0, int trip = 0) { 
		Num_Files (files); num_record = rec; num_hhold = hhold; num_person = person; num_tour = tour; num_trip = trip; 
		last_hhold = last_person = last_tour = 0; 
	}

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

	void Setup (void);

private:
	int num_record, num_hhold, num_person, num_tour, num_trip;
	int last_hhold, last_person, last_tour;

	int hhold, person, tour, trip, start, end, duration, origin, destination, purpose, mode;
	int constraint, priority, vehicle, passengers, type, partition;
	bool opt_flag;
};

#endif
