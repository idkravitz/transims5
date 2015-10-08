//*********************************************************
//	EventSum.hpp - Summarize Event File Data
//*********************************************************

#ifndef EVENTSUM_HPP
#define EVENTSUM_HPP

#include "APIDefs.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Difference_Data.hpp"
#include "Trip_Gap_Data.hpp"
#include "Travel_Time_File.hpp"
#include "Time_Sum_File.hpp"
#include "Travel_Time_Data.hpp"
#include "Time_Sum_Data.hpp"

//---------------------------------------------------------
//	EventSum - execution class definition
//---------------------------------------------------------

class EventSum : public Data_Service, public Select_Service
{
public:
	EventSum (void);
	virtual ~EventSum (void);

	virtual void Execute (void);

protected:

	enum EventSum_Keys { 
		TRAVEL_TIME_FILE = 1, TRAVEL_TIME_FORMAT, NEW_TRAVEL_TIME_FILE, NEW_TRAVEL_TIME_FORMAT,
		NEW_TIME_SUMMARY_FILE, NEW_TIME_SUMMARY_FORMAT, MINIMUM_SCHEDULE_VARIANCE, NEW_LINK_EVENT_FILE, NEW_LINK_EVENT_FORMAT,
		NEW_TIME_DISTRIBUTION_FILE, NEW_TRIP_TIME_GAP_FILE
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum EventSum_Reports { TOTAL_TIME = 1, PERIOD_TIME, TOT_TIME_SUM, PER_TIME_SUM, 
		 TRP_TIME_GAP
	};
	bool link_event_flag, trip_flag, travel_flag, group_flag, time_in_flag;
	bool time_file_flag, compare_flag, trip_file_flag, diff_file_flag, new_select_flag;
	bool distrib_flag, time_flag, select_flag, update_flag;
	bool time_sum_flag, time_diff_flag, time_gap_flag;
	int increment, num_inc, min_time, max_time, nfile;
	double percent_diff, percent, max_percent;
	Dtime min_variance;

	Travel_Time_File travel_time_file, time_in_file;
	Time_Sum_File time_sum_file;
	Link_Time_Sum_File link_event_file;

	Difference_Data time_diff;
	Trip_Gap_Data time_gap;

	Trip_Map travel_time_map;
	Travel_Time_Array travel_time_array;
	Time_Sum_Array time_sum_array;
	Link_Time_Map link_time_map;

	Select_Map selected;

	//---- methods ----

	void Read_Times (void);
	void Read_Trips (void);
	void Read_Events (void);

	void Select_Travelers (void);

	void Write_Times (void);
	void Write_Time_Sum (void);
	void Write_Links (void);

	void Update_Plans (void);
};

#endif
