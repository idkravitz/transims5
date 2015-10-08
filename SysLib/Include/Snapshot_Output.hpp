//*********************************************************
//	Snapshot_Output.hpp - Output Interface Class
//*********************************************************

#ifndef SNAPSHOT_OUTPUT_HPP
#define SNAPSHOT_OUTPUT_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Simulator_Service.hpp"
#include "Data_Range.hpp"
#include "Time_Periods.hpp"
#include "Snapshot_File.hpp"
#include "Data_Buffer.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Snapshot_Output Class definition
//---------------------------------------------------------

class SYSLIB_API Snapshot_Output : public Static_Service
{
public:
	Snapshot_Output (void);
	~Snapshot_Output (void);

	void operator()();

	void Initialize (Simulator_Service *exe = 0);
	
	bool Read_Control (void);

	bool Output_Check (void);
	
	bool Output_Flag (void)            { return (output_flag); }

private:
	enum Snaptshot_Keys { 
		NEW_SNAPSHOT_FILE = SNAPSHOT_OUTPUT_OFFSET, NEW_SNAPSHOT_FORMAT, NEW_SNAPSHOT_TIME_FORMAT,
		NEW_SNAPSHOT_INCREMENT, NEW_SNAPSHOT_TIME_RANGE, NEW_SNAPSHOT_LINK_RANGE, NEW_SNAPSHOT_SUBAREA_RANGE,
		NEW_SNAPSHOT_COORDINATES, NEW_SNAPSHOT_MAX_SIZE, NEW_SNAPSHOT_LOCATION_FLAG, NEW_SNAPSHOT_CELL_FLAG,
		NEW_SNAPSHOT_STATUS_FLAG, NEW_SNAPSHOT_COMPRESSION
	};
	typedef struct {
		Snapshot_File *file;
		Time_Periods time_range;
		Data_Range link_range;
		Data_Range subarea_range;
		int x1, y1, x2, y2;		//---- rounded ----
		bool coord_flag;
		bool cell_flag;
		bool size_flag;
		bool compress;
		unsigned max_size;
		unsigned num_records;
	} Output_Data;

	typedef vector <Output_Data>    Output_Array;
	typedef Output_Array::iterator  Output_Itr;

	bool output_flag;

	Output_Array output_array;

	Simulator_Service *exe;

	Data_Buffer data;
	Snapshot_File temp_file;
};
#endif
