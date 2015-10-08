//*********************************************************
//	Sim_Vehicle.hpp - simulator vehicle data
//*********************************************************

#ifndef SIM_VEHICLE_HPP
#define SIM_VEHICLE_HPP

#include "Dtime.hpp"
#include "Data_Pack.hpp"

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

//---------------------------------------------------------
//	Cell_Data class definition
//---------------------------------------------------------

class Cell_Data
{
public:
	Cell_Data (void)                { Clear (); }
	Cell_Data (int i, int l, int c) { Index (i); Lane (l); Cell (c); }

	int   Index (void)              { return (index); }
	int   Lane (void)               { return (lane); }
	int   Cell (void)               { return (cell); }

	void  Index (int value)         { index = value; }
	void  Lane (int value)          { lane = (short) value; }
	void  Cell (int value)          { cell = (short) value; }

	void  Lane_Change (int change)  { lane = (short) (lane + change); }
	
	void  Location (int *_index, int *_lane, int *_cell) 
									{ *_index = index; *_lane = lane; *_cell = cell; }

	Cell_Data Location (int index, int lane, int cell) 
									{ Index (index); Lane (lane); Cell (cell); return (*this); }

	void  Clear (void)              { index = -1; lane = -1; cell = 0; }

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
	short lane;
	short cell;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & index;
		ar & lane;
		ar & cell;
	}
#endif
};

typedef Vector <Cell_Data>    Cell_Array;
typedef Cell_Array::iterator  Cell_Itr;
typedef Cell_Data *           Cell_Ptr;

//---------------------------------------------------------
//	Sim_Veh_Data class definition
//---------------------------------------------------------

class Sim_Veh_Data : public Cell_Array
{
public:
	Sim_Veh_Data (int cells = 1)    { Clear (); Num_Cells (cells); }

	int   Driver (void)             { return (x.driver); }
	int   Leader (void)             { return (x.leader); }
	int   Follower (void)           { return (x.follower); }
	int   Speed (void)              { return (x.speed); }
	int   Type (void)               { return (x.type); }
	int   Passengers (void)         { return (x.passengers); }
	int   Wait (void)               { return (x.wait); }
	int   Priority (void)           { return (x.priority); }
	int   Change (void)             { return (x.change); }
   	Dtime Next_Event (void)         { return (x.next_event); }
	int   Num_Cells (void)          { return ((int) size ()); }

	void  Driver (int value)        { x.driver = value; }
	void  Leader (int value)        { x.leader = value; }
	void  Follower (int value)      { x.follower = value; }
	void  Speed (int value)         { x.speed = (short) value; }
	void  Type (int value)          { x.type = (unsigned char) value; }
	void  Passengers (int value)    { x.passengers = (unsigned char) value; }
	void  Wait (int value)          { x.wait = (unsigned short) value; }
	void  Priority (int value)      { x.priority = (char) value; }
	void  Change (int value)        { x.change = (char) value; }
	void  Next_Event (Dtime value)  { x.next_event = value; }

	void  Num_Cells (int length)    { Cell_Data cell; assign (length, cell); }

	void  Add_Wait (int value)      { x.wait = (unsigned short) (x.wait + value); }

	bool  Front (int index, int lane, int cell) 
	                                { int i, l, c; at (0).Location (&i, &l, &c); 
	                                  return (i == index && l == lane && c == cell); }
	void  Set_Front (int index, int lane, int cell)
	                                { Cell_Itr itr = begin (); itr->Index (index); 
	                                  itr->Lane (lane), itr->Cell (cell); }

	bool  Restricted (void)         { return (x.change == 1); }  
	void  Restricted (bool flag)    { x.change = (flag) ? 1 : 0; }

	void Clear (void)
	{
		memset (&x, '\0', sizeof (x)); x.driver = x.leader = x.follower = -1; clear ();
	}
#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&x, sizeof (x))) {
			return (Cell_Array::Pack (data, true));
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&x, sizeof (x))) {
			return (Cell_Array::UnPack (data, true));
		}
		return (false);
	}
#endif

private:
	struct {
		int            driver;
		int            leader;
		int            follower;
		short          speed;
		unsigned char  type;
		unsigned char  passengers;
		unsigned short wait;
		char           priority;
		char           change;
		Dtime          next_event;
	} x;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object <Cell_Array> (*this);
		ar & x.driver;
		ar & x.leader;
		ar & x.follower;
		ar & x.speed;
		ar & x.type;
		ar & x.passengers;
		ar & x.wait;
		ar & x.priority;
		ar & x.change;
		ar & x.next_event;
	}
#endif
};

typedef Sim_Veh_Data * Sim_Veh_Ptr;

//---------------------------------------------------------
//	Sim_Veh_Array class definition
//---------------------------------------------------------

class Sim_Veh_Array : public vector <Sim_Veh_Ptr>
{
public:
	Sim_Veh_Array (void)  { }
	~Sim_Veh_Array (void) {
		vector <Sim_Veh_Ptr>::iterator  itr;
		for (itr = begin (); itr != end (); itr++) {
			if (*itr != 0) {
				delete *itr;
				*itr = 0;
			}
		}
		clear ();
	}
};
typedef Sim_Veh_Array::iterator  Sim_Veh_Itr;

#endif
