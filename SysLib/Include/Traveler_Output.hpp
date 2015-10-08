//*********************************************************
//	Traveler_Output.hpp - Output Interface Class
//*********************************************************

#ifndef TRAVELER_OUTPUT_HPP
#define TRAVELER_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Bounded_Queue.hpp"
#include "Traveler_Data.hpp"
#include "Traveler_File.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"

#ifdef BOOST_THREADS
#include "Bounded_Queue.hpp"

typedef Bounded_Queue <Traveler_Data> Traveler_Queue;
#endif

//---------------------------------------------------------
//	Traveler_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Traveler_Output : public Static_Service
{
public:
	Traveler_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool In_Range (Traveler_Data &data);

	void Output_Traveler (Traveler_Data &data);
	
	bool Output_Flag (void)            { return (output_flag); }

	void End_Output (void);

private:
	enum Event_Keys { 
		NEW_TRAVELER_FILE = TRAVELER_OUTPUT_OFFSET, NEW_TRAVELER_FORMAT, NEW_TRAVELER_ID_RANGE, 
		NEW_TRAVELER_TIME_FORMAT, NEW_TRAVELER_TIME_RANGE, NEW_TRAVELER_MODE_RANGE, 
		NEW_TRAVELER_LINK_RANGE, NEW_TRAVELER_SUBAREA_RANGE, NEW_TRAVELER_COORDINATES 
	};
	typedef struct {
		Traveler_File *file;
		Data_Range hhold_range;
		Data_Range person_range;
		Time_Periods time_range;
		Data_Range link_range;
		Data_Range subarea_range;
		bool mode [MAX_MODE];
		int x1, y1, x2, y2;		//---- rounded ----
		bool coord_flag;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	bool output_flag;

	Output_Array output_array;

	void Write_Traveler (Traveler_Data &data);

	Simulator_Service *exe;

#ifdef MPI_EXE
public:
	void MPI_Processing (void);
private:
	Data_Buffer data;
	boost::mutex  data_mutex;
#else
 #ifdef BOOST_THREADS
	Traveler_Queue traveler_queue;
 #endif
#endif
};
#endif
