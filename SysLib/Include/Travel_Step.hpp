//*********************************************************
//	Travel_Step.hpp - traveler activity during a time step
//*********************************************************

#ifndef TRAVEL_STEP_HPP
#define TRAVEL_STEP_HPP

#include "Sim_Traveler.hpp"
#include "Sim_Vehicle.hpp"
#include "Sim_Dir_Data.hpp"
#include "Veh_Type_Data.hpp"
#include "TypeDefs.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Travel_Step class definition
//---------------------------------------------------------
 
class Travel_Step : public Cell_Array
{
public:
	Travel_Step (void)                        { Clear (); }

	int   Traveler (void)                     { return (traveler); }
	int   Vehicle (void)                      { return (vehicle); }
	int   Dir_Index (void)                    { return (dir_index); }
	int   Parking (void)                      { return (parking); }
	int   Stop (void)                         { return (stop); }
	int   Veh_Speed (void)                    { return (veh_speed); }
	int   Problem (void)                      { return (problem); }
	int   Status (void)                       { return (status); }
	int   Next_Vehicle (void)                 { return (next_veh); }
	int   Partition (void)                    { return (partition); }
	int   Position (void)                     { return (position); }
	int   Delay (void)                        { return (delay); }

	void  Traveler (int value)                { traveler = value; }
	void  Vehicle (int value)                 { vehicle = value; }
	void  Dir_Index (int value)               { dir_index = value; }
	void  Parking (int value)                 { parking = value; }
	void  Stop (int value)                    { stop = value; }
	void  Veh_Speed (int value)               { veh_speed = value; }
	void  Problem (int value)                 { problem = value; }
	void  Status (int value)                  { status = value; }
	void  Next_Vehicle (int value)            { next_veh = value; }
	void  Partition (int value)               { partition = value; }
	void  Position (int value)                { position = value; }
	void  Delay (int value)                   { delay = value; }

	void  Next_Position (int pce = 1)         { position += pce; }
	void  Exit_Link (int pce = 1)             { position -= pce; }

	Sim_Traveler_Ptr  sim_traveler_ptr;
	Sim_Veh_Ptr       sim_veh_ptr;
	Sim_Dir_Ptr       sim_dir_ptr;
	Veh_Type_Data     *veh_type_ptr;

	void  Clear_Output (void)
	{
		dir_index = parking = stop = next_veh = -1; veh_speed = 0; problem = status = delay = 0; sim_dir_ptr = 0; clear ();
	}
	void  Clear (void)
	{
		Clear_Output (); traveler = vehicle = -1; partition = position = 0; sim_traveler_ptr = 0; sim_veh_ptr = 0; veh_type_ptr = 0;
	}
private:
	int   traveler;
	int   vehicle;
	int   dir_index;
	int   parking;
	int   stop;
	int   veh_speed;
	int   problem;
	int   status;
	int   next_veh;
	int   partition;
	int   position;
	int   delay;
};

#endif
