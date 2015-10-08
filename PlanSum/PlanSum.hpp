//*********************************************************
//	PlanSum.hpp - travel plan processing utility
//*********************************************************

#ifndef PLANSUM_HPP
#define PLANSUM_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Flow_Time_Service.hpp"
#include "Data_Queue.hpp"
#include "Db_File.hpp"
#include "Trip_Sum_Data.hpp"
#include "Link_Data_File.hpp"
#include "Transfer_Data.hpp"

//---------------------------------------------------------
//	PlanSum - execution class definition
//---------------------------------------------------------

class PlanSum : public Data_Service, public Select_Service, public Flow_Time_Service
{
public:

	PlanSum (void);
	virtual ~PlanSum (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum PlanSum_Keys { NEW_TRIP_TIME_FILE = 1, NEW_LINK_VOLUME_FILE, 
	};
	virtual void Program_Control (void);

private:
	enum PlanSum_Reports { TOP_100 = 1, VC_RATIO, LINK_GROUP, LINK_EQUIV, ZONE_EQUIV, STOP_EQUIV, 
		SUM_RIDERS, SUM_STOPS, SUM_TRANSFERS, XFER_DETAILS, STOP_GROUP, SUM_PASSENGERS, RIDER_GROUP, 
		TRIP_TIME, SUM_TRAVEL };

	int num_process, max_rail;
	bool select_flag, new_delay_flag, turn_flag, skim_flag, trip_flag, zone_flag, time_flag, travel_flag;
	bool volume_flag, passenger_flag, transfer_flag, rider_flag, xfer_flag, xfer_detail;
	double cap_factor, minimum_vc;
	String xfer_label;
	
	Plan_File *plan_file;
	Db_File time_file;
	Link_Data_File volume_file;

	void MPI_Setup (void);
	void MPI_Processing (void);
	void Write_Times (void);
	void Write_Volumes (void);

	typedef Data_Queue <int> Partition_Queue;

	Partition_Queue partition_queue;
	
	typedef vector <Ints_Array>   Board_Array;
	typedef Board_Array::iterator Board_Itr;

	Ints_Array total_on_array;
	Board_Array walk_on_array;
	Board_Array drive_on_array;

	//---------------------------------------------------------
	//	Plan_Processing - process plan partitions
	//---------------------------------------------------------

	class Plan_Processing
	{
	public:
		Plan_Processing (PlanSum *_exe);
		void operator()();

	private:
		PlanSum *exe;
		bool thread_flag, turn_flag;
		Doubles start_time, mid_time, end_time;

		Plan_File               plan_file;
		Flow_Time_Period_Array  link_delay_array;
		Flow_Time_Period_Array  turn_delay_array;
		Trip_Sum_Data           trip_sum_data;
		Trip_Sum_Data           pass_sum_data;
		Transfer_Array          transfer_array;
		Ints_Array              total_on_array;
		Board_Array             walk_on_array;
		Board_Array             drive_on_array;

		void Read_Plans (int part);
	};
	Plan_Processing **plan_processing;

	//---- report data ----

	Doubles start_time, mid_time, end_time;

	Trip_Sum_Data trip_sum_data, pass_sum_data;
	Transfer_Array transfer_array;

	typedef struct {
		union {
			int lines;
			int links;
		};
		union {
			int runs;
			int capacity;
		};
		union {
			int riders;
			int volume;
		};
	} Group_Data;

	typedef vector <Group_Data> Group_Array;
	typedef Group_Array::iterator Group_Itr;

	//---- report methods ----
	
	void Top_100_Report (void);
	void Top_100_Header (void);

	void VC_Ratio (double min_vc);
	void VC_Ratio_Header (void);

	void Link_Group (double min_vc);
	void Link_Group_Header (void);

	void Trip_Time (void);
	void Trip_Time_Header (void);

	void Transit_Report (void);
	void Transit_Header (void);

	void Stop_Report (void);
	void Stop_Header (void);

	void Stop_Group_Report (void);
	void Stop_Group_Header (void);

	void Transfer_Report (int num, int mode = 0);
	void Transfer_Header (void);

	void Rider_Group (void);
	void Rider_Header (void);
};
#endif
