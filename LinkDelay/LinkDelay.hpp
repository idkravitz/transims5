//*********************************************************
//	LinkDelay.hpp - manipulate the link delay file
//*********************************************************

#ifndef LINKDELAY_HPP
#define LINKDELAY_HPP

#include "Data_Service.hpp"
#include "Smooth_Data.hpp"

//---------------------------------------------------------
//	LinkDelayTrips - execution class definition
//---------------------------------------------------------

class LinkDelay : public Data_Service
{
public:

	LinkDelay (void);
	virtual ~LinkDelay (void);
	
	virtual void Execute (void);

protected:
	enum LinkDelay_Keys { 
		MERGE_LINK_DELAY_FILE = 1, MERGE_LINK_DELAY_FORMAT, PROCESSING_METHOD, MERGE_WEIGHTING_FACTOR, MAX_TRAVEL_TIME_RATIO, SET_MIN_TRAVEL_TIME
	};
	virtual void Program_Control (void);

private:
	Link_Delay_File merge_file;

	int method;
	double factor, time_ratio;	
	bool smooth_flag, merge_flag, ratio_flag, turn_flag, min_time_flag;

	Smooth_Data smooth_data;
	
	void Merge_Delay (void);

	Flow_Time_Period_Array merge_link_array;
	Flow_Time_Period_Array merge_turn_array;

	void Smooth_Delay (void);
	void Set_Min_Time (void);
};
#endif
