//*********************************************************
//	Turn_Vol_Output.hpp - Output Interface Class
//*********************************************************

#ifndef TURN_VOL_OUTPUT_HPP
#define TURN_VOL_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Turn_Vol_File.hpp"
#include "Turn_Vol_Data.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Travel_Step.hpp"
#include "Data_Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Turn_Vol_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Vol_Output : public Static_Service
{
public:
	Turn_Vol_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool Output_Check (void);
	
	bool Output_Flag (void)            { return (output_flag); }

	void Summarize (Travel_Step &step);

	typedef struct {
		int node;
		int dir_index;
		int to_index;
	} Sort_Key;

private:
	enum Turn_Vol_Keys { 
		NEW_TURN_VOLUME_FILE = TURN_VOL_OUTPUT_OFFSET, NEW_TURN_VOLUME_FORMAT, 
		NEW_TURN_VOLUME_FILTER, NEW_TURN_VOLUME_TIME_FORMAT, NEW_TURN_VOLUME_INCREMENT, 
		NEW_TURN_VOLUME_TIME_RANGE, NEW_TURN_VOLUME_NODE_RANGE, NEW_TURN_VOLUME_SUBAREA_RANGE
	};

	//---- data index ----
	
	typedef map <Sort_Key, int>       Turn_Map;
	typedef pair <Sort_Key, int>      Turn_Map_Data;
	typedef Turn_Map::iterator        Turn_Map_Itr;
	typedef pair <Turn_Map_Itr, bool> Turn_Map_Stat;

	typedef struct {
		Turn_Vol_File *file;
		int  filter;
		bool data_flag;
		Time_Periods time_range;
		Data_Range node_range;
		Data_Range subarea_range;
		Turn_Map turn_map;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	Output_Array output_array;

	bool output_flag;

	void Write_Turn (int step, Output_Data *output);

	Simulator_Service *exe;

#ifdef MPI_EXE
	Data_Buffer data;

	void MPI_Processing (Output_Itr output);
#endif
};
	
bool operator < (Turn_Vol_Output::Sort_Key left, Turn_Vol_Output::Sort_Key right);

#endif
