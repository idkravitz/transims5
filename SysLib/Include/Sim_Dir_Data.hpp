//*********************************************************
//	Sim_Dir_Data.hpp - network link direction data
//*********************************************************

#ifndef SIM_DIR_DATA_HPP
#define SIM_DIR_DATA_HPP

#include "Sim_Vehicle.hpp"
#include "Sim_Cap_Data.hpp"
#include "Sim_Lane_Data.hpp"
#include "Data_Pack.hpp"

#include <vector>
using namespace std;

#ifdef BOOST_THREADS
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#endif

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

//---------------------------------------------------------
//	Sim_Dir_Data class definition
//---------------------------------------------------------
 
class Sim_Dir_Data
{
public:
	Sim_Dir_Data (void)             { Clear (); }

	int   Speed (void)              { return (x.speed); }
	int   Dir (void)                { return (x.dir); }
	int   Type (void)               { return (x.type); }
	int   Lanes (void)              { return (x.lanes); }
	int   Cells (void)              { return (x.cells); }
	int   In_Cell (void)            { return (x.in_cell); }
	int   Out_Cell (void)           { return (x.out_cell); }
	int   Cell_Out (void)           { return (x.cells - x.out_cell - 1); }
	int   Method (void)             { return (x.method); }
	bool  Transfer (void)           { return (x.transfer > 0); }
	bool  Boundary (void)           { return (x.from_part != x.to_part); }
	bool  Turn (void)               { return (x.turn > 0); }
	int   From_Part (void)          { return (x.from_part); }
	int   To_Part (void)            { return (x.to_part); }

	void  Speed (int value)         { x.speed = (short) value; }
	void  Dir (int value)           { x.dir = (char) value; }
	void  Type (int value)          { x.type = (char) value; }
	void  Lanes (int value)         { x.lanes = (char) value; }
	void  Cells (int value)         { x.cells = (short) value; }
	void  In_Cell (int value)       { x.in_cell = (char) value; }
	void  Out_Cell (int value)      { x.out_cell = (char) value; }
	void  Method (int value)        { x.method = (char) value; }
	void  Transfer (int value)      { x.transfer = (char) value; }
	void  Turn (bool flag)          { x.turn = (flag) ? 1 : 0; }
	void  From_Part (int value)     { x.from_part = (short) value; }
	void  To_Part (int value)       { x.to_part = (short) value; }

	void  Make_Data (void);
	void  Delete_Data (void);

	bool  Active (void)             { return ((x.method == MESOSCOPIC && sim_lanes) || (x.method == MACROSCOPIC && sim_cap)); }

	int   Partition (int cell)      { return ((cell >= Split_Cell ()) ? x.to_part : x.from_part); }
	int   Split_Cell (void)         { return (5 * x.cells / 8); }

	//---- macroscopic data ----

	Sim_Cap_Ptr Sim_Cap (void)      { return (sim_cap); }

	void  Reset_Use (int min_lane, int max_lane)   { if (x.method == MACROSCOPIC && sim_cap) sim_cap->Reset_Use (min_lane, max_lane); }

	//---- mescoscopic data ----

	Sim_Lane_Ptr Lane (int lane)    { return (&sim_lanes->at (lane)); }

	int   Cell (int lane, int cell)            { return ((x.method == MESOSCOPIC && sim_lanes) ? (*sim_lanes) [lane] [cell] : -1); }
	void  Cell (int lane, int cell, int value) { if (x.method == MESOSCOPIC && sim_lanes) (*sim_lanes) [lane] [cell] = value; }

	int   Cell (Cell_Data &rec)                { return (Cell (rec.Lane (), rec.Cell ())); }
	void  Cell (Cell_Data &rec, int value)     { Cell (rec.Lane (), rec.Cell (), value); }

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data);
	bool UnPack (Data_Buffer &data);
#endif

	void Clear (void)
	{
		memset (&x, '\0', sizeof (x)); sim_lanes = 0;
	}

private:
	struct {
		short  speed;
		char   dir;
		char   lanes;
		short  cells;
		char   in_cell;
		char   out_cell;
		char   method;
		char   transfer;
		char   turn;
		char   type;
		short  from_part;
		short  to_part;
	} x;

	union {
		Sim_Lane_Array  *sim_lanes;
		Sim_Cap_Data    *sim_cap;
	};

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & x.speed;
		ar & x.dir;
		ar & x.lanes;
		ar & x.cells;
		ar & x.in_cell;
		ar & x.out_cell;
		ar & x.method;
		ar & x.transfer;
		ar & x.turn;
		ar & x.type;
		ar & x.from_part;
		ar & x.to_part;

		if (x.method == MACROSCOPIC) {
			ar & *sim_cap;
		} else if (x.method == MESOSCOPIC) {
			ar & *sim_lanes;
		}
	}
#endif
};

typedef Sim_Dir_Data *           Sim_Dir_Ptr;

//---------------------------------------------------------
//	Sim_Dir_Array class definition
//---------------------------------------------------------

class Sim_Dir_Array : public vector <Sim_Dir_Data>
{
public:
	Sim_Dir_Array (void)  { }
	~Sim_Dir_Array (void);

	int   Cell (int index, int lane, int cell)              { return (at (index).Cell (lane, cell)); }
	void  Cell (int index, int lane, int cell, int value)   { at (index).Cell (lane, cell, value); }

	int   Cell (Cell_Data &rec)            { return (Cell (rec.Index (), rec.Lane (), rec.Cell())); }
	void  Cell (Cell_Data &rec, int value) { Cell (rec.Index (), rec.Lane (), rec.Cell (), value); }
};

typedef Sim_Dir_Array::iterator  Sim_Dir_Itr;

#endif
