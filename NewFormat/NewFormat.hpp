//*********************************************************
//	NewFormat.hpp - convert data files to version 4.2 format
//*********************************************************

#ifndef NEWFORMAT_HPP
#define NEWFORMAT_HPP

#include "Data_Service.hpp"
#include "Toll_File.hpp"
#include "Activity_File.hpp"
#include "Person_File.hpp"
#include "Old_Plan_File.hpp"
#include "Data_Queue.hpp"
#include "Snapshot_File.hpp"

//---------------------------------------------------------
//	NewFormat - execution class definition
//---------------------------------------------------------

class NewFormat : public Data_Service
{
public:
	NewFormat (void);
	virtual ~NewFormat (void);

	virtual void Execute (void);

protected:
	enum NewFormat_Keys { 
		COPY_EXISTING_FIELDS = 1, FLATTEN_OUTPUT_FLAG, TOLL_FILE, ACTIVITY_FILE, PERSON_FILE, 
		SNAPSHOT_FILE, NEW_SNAPSHOT_FILE, NEW_SNAPSHOT_FORMAT, NEW_SNAPSHOT_COMPRESSION, 
		VERSION4_TIME_FORMAT, 
		VERSION4_PLAN_FILE, VERSION4_PLAN_FORMAT,
		NODE_LIST_PATHS, TRAVELER_SCALING_FACTOR,
		VERSION4_ROUTE_HEADER, VERSION4_ROUTE_NODES
	};
	virtual void Program_Control (void);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);
	virtual bool Get_Skim_Data (Skim_File &file, Skim_Record &data, int partition = 0);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);

private:
	bool toll_flag, activity_flag, person_flag, plan_flag, route_flag, snap_flag, copy_flag;

	int num_new_trip, max_trip_part, num_new_skim, max_skim_part, num_new_act, route_periods, scale;
	Units_Type time_units;

	Activity_File activity_file;
	Toll_File toll_file;
	Person_File person_file;
	Old_Plan_File old_plan;

	Location_File *new_loc_file;
	Zone_File *new_zone_file;

	Snapshot_File snap_file, new_snap_file;

	Db_Header route_header, route_nodes;

	void Read_Tolls (void);
	void Read_Activity (void);
	void Read_Persons (void);
	void Read_Snapshots (void);
	void Check_Phasing_Plans (void);
	void Read_Route_Nodes (void);

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (NewFormat *_exe);
		void operator()();

	private:
		NewFormat *exe;
		bool thread_flag;

		Old_Plan_File   old_plan;
		Plan_File       new_plan;
	
		void Travel_Plans (int part);
	};
	Plan_Processing **plan_processing;
};
#endif
