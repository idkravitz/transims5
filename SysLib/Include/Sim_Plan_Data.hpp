//*********************************************************
//	Sim_Plan_Data.hpp - travel plan data
//*********************************************************

#ifndef SIM_PLAN_DATA_HPP
#define SIM_PLAN_DATA_HPP

#include "Data_Pack.hpp"

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Sim_Leg_Data class definition
//---------------------------------------------------------

class Sim_Leg_Data
{
public:
	Sim_Leg_Data (void)               { Clear (); }

	int   Mode (void)                 { return (mode); }
	int   Type (void)                 { return (type); }
	int   Index (void)                { return (index); }
	int   Max_Speed (void)            { return (mode_data); }
	int   Stop_Number (void)          { return (mode_data); }
	Dtime Time (void)                 { return (time); }
	int   In_Lane_Low (void)          { return (in_lane_low); }
	int   In_Lane_High (void)         { return (in_lane_high); }
	int   In_Best_Low (void)          { return (in_best_low); }
	int   In_Best_High (void)         { return (in_best_high); }
	int   Out_Lane_Low (void)         { return (out_lane_low); }
	int   Out_Lane_High (void)        { return (out_lane_high); }
	int   Out_Best_Low (void)         { return (out_best_low); }
	int   Out_Best_High (void)        { return (out_best_high); }
	int   Connect (void)              { return (connect); }

	void  Mode (int value)            { mode = (char) value; }
	void  Type (int value)            { type = (char) value; }
	void  Index (int value)           { index = value; }
	void  Max_Speed (int value)       { mode_data = (short) value; }
	void  Stop_Number (int value)     { mode_data = (short) value; }
	void  Time (Dtime value)          { time = value; }
	void  In_Lane_Low (int value)     { in_lane_low = (char) value; }
	void  In_Lane_High (int value)    { in_lane_high = (char) value; }
	void  In_Best_Low (int value)     { in_best_low = (char) value; }
	void  In_Best_High (int value)    { in_best_high = (char) value; }
	void  Out_Lane_Low (int value)    { out_lane_low = (char) value; }
	void  Out_Lane_High (int value)   { out_lane_high = (char) value; }
	void  Out_Best_Low (int value)    { out_best_low = (char) value; }
	void  Out_Best_High (int value)   { out_best_high = (char) value; }
	void  Connect (int value)         { connect = value; }

	void  Clear (void)                { memset (this, '\0', sizeof (*this)); }

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)
	{
		return (data.Add_Data (this, sizeof (*this)));
	}
	bool UnPack (Data_Buffer &data)
	{
		return (data.Get_Data (this, sizeof (*this)));
	}
#endif

private:
	int   index;
	char  mode;
	char  type;
	short mode_data;		//---- max_speed or line stop number ----
	Dtime time;
	char  in_lane_low;
	char  in_lane_high;
	char  in_best_low;
	char  in_best_high;
	char  out_lane_low;
	char  out_lane_high;
	char  out_best_low;
	char  out_best_high;
	int   connect;			//---- connect_array index ---- 

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & index;
		ar & mode;
		ar & type;
		ar & mode_data;
		ar & time;
		ar & in_lane_low;
		ar & in_lane_high;
		ar & in_best_low;
		ar & in_best_high;
		ar & out_lane_low;
		ar & out_lane_high;
		ar & out_best_low;
		ar & out_best_high;
		ar & connect;
	}
#endif
};

typedef Deque <Sim_Leg_Data>             Sim_Leg_Queue;
typedef Sim_Leg_Queue::iterator          Sim_Leg_Itr;
typedef Sim_Leg_Queue::reverse_iterator  Sim_Leg_RItr;
typedef Sim_Leg_Data *                   Sim_Leg_Ptr;

//---------------------------------------------------------
//	Sim_Plan_Data class definition
//---------------------------------------------------------

class Sim_Plan_Data : public Sim_Leg_Queue
{
public:
	Sim_Plan_Data (void)            { Clear (); }

	int   Household (void)          { return (x.hhold); }
	int   Person (void)             { return (x.person); }
	int   Tour (void)               { return (x.tour); }
	int   Trip (void)               { return (x.trip); }
	Dtime Depart (void)             { return (x.depart); }
	Dtime Arrive (void)             { return (x.arrive); }
	Dtime Duration (void)           { return (x.duration); }
	Dtime Schedule (void)           { return (x.schedule); }
	Dtime Next_Event (void)         { return (x.next_event); }
	int   Origin (void)             { return (x.origin); }
	int   Destination (void)        { return (x.destination); }
	int   Purpose (void)            { return (x.purpose); }
	int   Mode (void)               { return (x.mode); }
	int   Constraint (void)         { return (x.constraint); }
	int   Priority (void)           { return (x.priority); }
	int   Vehicle (void)            { return (x.vehicle); }
	int   Type (void)               { return (x.type); }
	int   Passengers (void)         { return (x.passengers); }
	int   Partition (void)          { return (x.part); }

	void  Household (int value)     { x.hhold = value; }
	void  Person (int value)        { x.person = (short) value; }
	void  Tour (int value)          { x.tour = (char) value; }
	void  Trip (int value)          { x.trip = (char) value; }
	void  Depart (Dtime value)      { x.depart = value; }
	void  Arrive (Dtime value)      { x.arrive = value; }
	void  Duration (Dtime value)    { x.duration = value; }
	void  Schedule (Dtime value)    { x.schedule = value; }
	void  Next_Event (Dtime value)  { x.next_event = value; }
	void  Origin (int value)        { x.origin= value; }
	void  Destination (int value)   { x.destination = value; }
	void  Purpose (int value)       { x.purpose = (char) value; }
	void  Mode (int value)          { x.mode = (char) value; }
	void  Constraint (int value)    { x.constraint = (char) value; }
	void  Priority (int value)      { x.priority = (char) value; }
	void  Vehicle (int value)       { x.vehicle = value; }
	void  Type (int value)          { x.type = (char) value; }
	void  Passengers (int value)    { x.passengers = (char) value; }
	void  Partition (int value)     { x.part = (short) value; }
	
	bool  In_Partition (int part)   { return (part == x.part); }

	bool  Active (void)             { return (!empty ()); }
	bool  Check_Ahead (void)        { return (size () > 1); }
	bool  Next_Leg (void)           { if (Active ()) { x.schedule += front ().Time (); pop_front (); } return (Active ()); }

	void  Clear (void)
	{
		memset (&x, '\0', sizeof (x)); x.vehicle = x.origin = x.destination = -1; x.next_event = -1; clear ();
	}
#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&x, sizeof (x))) {
			return (Sim_Leg_Queue::Pack (data));
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&x, sizeof (x))) {
			return (Sim_Leg_Queue::UnPack (data));
		}
		return (false);
	}
#endif

private:
	struct {
		int   hhold;
		short person;
		char  tour;
		char  trip;
		Dtime depart;
		Dtime arrive;
		Dtime duration;
		Dtime schedule;
		Dtime next_event;
		int   origin;
		int   destination;
		char  purpose;
		char  mode;
		char  constraint;
		char  priority; 
		int   vehicle;			//---- vehicle/line index ----
		char  passengers;
		char  type;
		short part;
	} x;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object <Sim_Leg_Queue> (*this);
		ar & x.hhold;
		ar & x.person;
		ar & x.tour;
		ar & x.trip;
		ar & x.depart;
		ar & x.arrive;
		ar & x.duration;
		ar & x.schedule;
		ar & x.next_event;
		ar & x.origin;
		ar & x.destination;
		ar & x.purpose;
		ar & x.mode;
		ar & x.constraint;
		ar & x.priority;
		ar & x.vehicle;
		ar & x.type;
		ar & x.passengers;
		ar & x.part;
	}
#endif
};

typedef Sim_Plan_Data *           Sim_Plan_Ptr;

//---------------------------------------------------------
//	Sim_Plan_Array class definition
//---------------------------------------------------------

class Sim_Plan_Array : public vector <Sim_Plan_Ptr>
{
public:
	Sim_Plan_Array (void)  { }
	~Sim_Plan_Array (void) {
		vector <Sim_Plan_Ptr>::iterator  itr;
		for (itr = begin (); itr != end (); itr++) {
			if (*itr != 0) {
				delete *itr;
				*itr = 0;
			}
		}
		clear ();
	}
};

typedef Sim_Plan_Array::iterator    Sim_Plan_Itr;

//---------------------------------------------------------
//	Plan_Index class definition
//---------------------------------------------------------

class SYSLIB_API Plan_Index : public Int2_Key
{
public:
	Plan_Index (void)                    { Clear (); }

	int   Traveler (void)                { return (first); }
	int   Sequence (void)                { return (second); }

	void  Traveler (int value)           { first = value; }
	void  Sequence (int value)           { second = value; }
	void  Sequence (int tour, int trip)  { second = (tour << 16) + trip; }
	
	void  Clear (void)                   { first = second = 0; }
};

typedef map <Plan_Index, Sim_Plan_Ptr>    Sim_Plan_Map;
typedef pair <Plan_Index, Sim_Plan_Ptr>   Sim_Plan_Map_Data;
typedef Sim_Plan_Map::iterator            Sim_Plan_Map_Itr;
typedef pair <Sim_Plan_Map_Itr, bool>     Sim_Plan_Map_Stat;

typedef map <int, Sim_Plan_Ptr>           Route_PlanSim_Route_Map;
typedef pair <Plan_Index, Sim_Plan_Ptr>   Sim_Plan_Map_Data;
typedef Sim_Plan_Map::iterator            Sim_Plan_Map_Itr;
typedef pair <Sim_Plan_Map_Itr, bool>     Sim_Plan_Map_Stat;

#endif
