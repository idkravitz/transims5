//*********************************************************
//	Event_Output.hpp - Output Interface Class
//*********************************************************

#ifndef EVENT_OUTPUT_HPP
#define EVENT_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Bounded_Queue.hpp"
#include "Event_Data.hpp"
#include "Event_File.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"

#ifdef BOOST_THREADS
#include "Bounded_Queue.hpp"

typedef Bounded_Queue <Event_Data> Event_Queue;
#endif

//---------------------------------------------------------
//	Event_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Event_Output : public Static_Service
{
public:
	Event_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool In_Range (Event_Type type, int mode = DRIVE_MODE, int subarea = 0);

	void Output_Event (Event_Data &data);
	
	bool Output_Flag (void)            { return (output_flag); }

	void End_Output (void);

private:
	enum Event_Keys { 
		NEW_EVENT_FILE = EVENT_OUTPUT_OFFSET, NEW_EVENT_FORMAT, NEW_EVENT_FILTER, 
		NEW_EVENT_TIME_FORMAT, NEW_EVENT_TIME_RANGE, NEW_EVENT_TYPE_RANGE, NEW_EVENT_MODE_RANGE, 
		NEW_EVENT_LINK_RANGE, NEW_EVENT_SUBAREA_RANGE, NEW_EVENT_COORDINATES 
	};
	typedef struct {
		Event_File *file;
		Time_Periods time_range;
		Data_Range link_range;
		Data_Range subarea_range;
		bool type [MAX_EVENT];
		bool mode [MAX_MODE];
		int filter;
		int x1, y1, x2, y2;		//---- rounded ----
		bool coord_flag;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	bool output_flag;

	Output_Array output_array;

	void Write_Event (Event_Data &data);

	Simulator_Service *exe;

#ifdef MPI_EXE
public:
	void MPI_Processing (void);
private:
	Data_Buffer data;
	boost::mutex  data_mutex;
#else
 #ifdef BOOST_THREADS
	Event_Queue event_queue;
 #endif
#endif
};
#endif
