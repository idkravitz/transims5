//********************************************************* 
//	Old_Plan_File.hpp - Version 3 Plan File Input/Output
//*********************************************************

#ifndef OLD_PLAN_FILE_HPP
#define OLD_PLAN_FILE_HPP

#include "Db_File.hpp"
#include "String.hpp"

#include "stdlib.h"

typedef struct {
	int   key1;
	int   key2;
	int   start_id;
	int   end_id;
	int   duration;
	int   stop_time;
	int   cost;
	int   gcf;
	char  trip;
	char  leg;	
	char  start_type;
	char  end_type;
	char  driver_flag;
	char  mode;
	short tokens;
	int   data [1];
} Old_Plan_Data;

//---------------------------------------------------------
//	Old_Plan_File Class definition
//---------------------------------------------------------

class SYSLIB_API Old_Plan_File : public Db_File
{
public:
	enum Memory_Type {PRIVATE_MEMORY, ALLOCATE_MEMORY};
	enum Sort_Type {TRAVELER_SORT, TIME_SORT};
	enum Plan_Mode {AUTO_MODE, TRANSIT_MODE, WALK_MODE, BIKE_MODE, ACTIVITY_MODE, OTHER_MODE,
					MAGIC_MODE, CARPOOL_MODE, DRIVER_MODE};

	Old_Plan_File (Access_Type access = READ, Memory_Type memory = PRIVATE_MEMORY, Sort_Type sort = TRAVELER_SORT, int traveler_scale = 100);
	Old_Plan_File (string filename, Access_Type access = READ, Memory_Type memory = PRIVATE_MEMORY, Sort_Type sort = TRAVELER_SORT, int traveler_scale = 100);
	~Old_Plan_File ();
	
	bool Open (int ext_number);
	bool Open (string filename = "");

	//---- read/write functions ----

	bool Read (long offset = -1);
	bool Write (Old_Plan_Data *plan = 0);

	//---- size functions ----

	int  Num_Records (void)		{ return (num_record); };
	int  Num_Plans (void)		{ return (num_plan); };
	int  Num_Travelers (void)	{ return (num_traveler); };
	int  Num_Trips (void)		{ return (num_trip); };

	void Add_Counters (Old_Plan_File *file) { num_record += file->num_record, 
		num_plan += file->num_plan; num_traveler += file->num_traveler; num_trip += file->num_trip; }

	void Reset_Counters (int rec = 0, int plan = 0, int trv = 0, int trip = 0)	
					{ num_record = rec; num_plan = plan; num_traveler = trv; num_trip = trip; };

	//---- output data fields ----

	int Traveler (void)		{ return ((time_sort) ? plan->key2 : plan->key1); };
	int Household (void)	{ return (Traveler () / scale); }
	int Person (void)		{ return (Traveler () % scale); }
	int Trip (void)			{ return ((int) plan->trip); };
	int Leg (void)			{ return ((int) plan->leg); };
	int Time (void)			{ return ((time_sort) ? plan->key1 : plan->key2); };
	int Start_ID (void)		{ return (plan->start_id); };
	int Start_Type (void)	{ return ((int) plan->start_type); };
	int End_ID (void)		{ return (plan->end_id); };
	int End_Type (void)		{ return ((int) plan->end_type); };
	int Duration (void)		{ return (plan->duration); };
	int Stop_Time (void)	{ return (plan->stop_time); };
	int Cost (void)			{ return (plan->cost); };
	int GCF (void)			{ return (plan->gcf); };
	int Driver_Flag (void)	{ return ((int) plan->driver_flag); };
	int Mode (void)			{ return ((int) plan->mode); };
	int Tokens (void)		{ return ((int) plan->tokens); };

	int Vehicle (void);
	int Route (void);
	int Mode_ID (void);
	int *Path (int *num_path);
	int *Data (int *max_token);

	//---- input data fields ----

	void Traveler (int data)		{ if (time_sort) plan->key2 = data; else plan->key1 = data; };
	void Household (int data)		{ Traveler (data * scale + Person ()); }
	void Person (int data)			{ Traveler (Household () * scale + data); }
	void Trip (int data)			{ plan->trip = (char) data; };
	void Leg (int data)				{ plan->leg = (char) data; };
	void Time (int data)			{ if (time_sort) plan->key1 = data; else plan->key2 = data; };
	void Start_ID (int data)		{ plan->start_id = data; };
	void Start_Type (int data)		{ plan->start_type = (char) data; };
	void End_ID (int data)			{ plan->end_id = data; };
	void End_Type (int data)		{ plan->end_type = (char) data; };
	void Duration (int data)		{ plan->duration = data; };
	void Stop_Time (int data)		{ plan->stop_time = data; };
	void Cost (int data)			{ plan->cost = data; };
	void GCF (int data)				{ plan->gcf = data; };
	void Driver_Flag (int data)		{ plan->driver_flag = (char) data; };
	void Mode (int data)			{ plan->mode = (char) data; };
	void Tokens (int data)			{ plan->tokens = (short) data; };

	void Vehicle (int data);
	void Route (int data);
	void Mode_ID (int data);
	void Num_Path (int data);

	Old_Plan_Data * Plan (void)			{ return (plan); };
	bool Plan (Old_Plan_Data *plan);

	void Delete_Plan (Old_Plan_Data *plan = 0);

	bool Check_Size (int size);

	//--- type functions ----

	Memory_Type Plan_Memory (void)			{ return ((allocate_memory) ? ALLOCATE_MEMORY : PRIVATE_MEMORY); };
	Sort_Type   Plan_Sort (void)			{ return ((time_sort) ? TIME_SORT : TRAVELER_SORT); };

	void Plan_Memory (Memory_Type memory)	{ allocate_memory = (memory == ALLOCATE_MEMORY); };
	void Plan_Sort (Sort_Type sort)			{ time_sort = (sort == TIME_SORT); };

	int  Traveler_Scale (void)              { return (scale); }
	void Traveler_Scale (int value)         { scale = value; };

	bool Node_Based_Flag (void)             { return (node_based); }
	void Node_Based_Flag (bool flag)        { node_based = flag; }

private:
	bool Setup_Record (void);
	void Setup (Memory_Type memory, Sort_Type sort, int traveler_scale = 100);

	bool Read_Plan_Def (void);
	bool Write_Plan_Def (void);

	bool allocate_memory, time_sort, node_based;
	int num_record, num_plan, num_traveler, num_trip, max_data, scale;

	Old_Plan_Data *plan;
};

#endif
