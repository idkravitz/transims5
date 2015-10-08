//*********************************************************
//	Sim_Lane_Data.hpp - mesoscopic lane grid data
//*********************************************************

#ifndef SIM_LANE_DATA_HPP
#define SIM_LANE_DATA_HPP

#include "TypeDefs.hpp"
#include "Data_Pack.hpp"

//---------------------------------------------------------
//	Sim_Lane_Data class definition
//---------------------------------------------------------

class Sim_Lane_Data : public Packed_Integers
{
public:
	Sim_Lane_Data (void)            { Clear (); }

	int   Min_Veh_Type (void)       { return (x.min_veh); }
	int   Max_Veh_Type (void)       { return (x.max_veh); }
	int   Min_Traveler (void)       { return (x.min_trav); }
	int   Max_Traveler (void)       { return (x.max_trav); }
	int   Use (void)                { return (x.use); }
	int   Type (void)               { return (x.type); }
	int   Thru_Lane (void)          { return (x.lane); }
	int   Thru_Link (void)          { return (x.link); }
	int   First_Use (void)          { return (x.index); }

	void  Min_Veh_Type (int value)  { x.min_veh = (char) value; }
	void  Max_Veh_Type (int value)  { x.max_veh = (char) value; }
	void  Min_Traveler (int value)  { x.min_trav = (char) value; }
	void  Max_Traveler (int value)  { x.max_trav = (char) value; }
	void  Use (int value)           { x.use = (short) value; }
	void  Type (int value)          { x.type = (char) value; }
	void  Thru_Lane (int value)     { x.lane = (char) value; }
	void  Thru_Link (int value)     { x.link = value; }
	void  First_Use (int value)     { x.index = value; }

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&x, sizeof (x))) {
			return (Packed_Integers::Pack (data));
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&x, sizeof (x))) {
			return (Packed_Integers::UnPack (data));
		}
		return (false);
	}
#endif

	void Clear (void)
	{
		memset (&x, '\0', sizeof (x)); x.link = x.index = -1; clear ();
	}
private:
	struct {
		char   min_veh;
		char   max_veh;
		char   min_trav;
		char   max_trav;
		short  use;
		char   type;
		char   lane;
		int    link;
		int    index;
	} x;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & boost::serialization::base_object <Packed_Integers> (*this);
		ar & x.min_veh;
		ar & x.max_veh;
		ar & x.min_trav;
		ar & x.max_trav;
		ar & x.use;
		ar & x.type;
		ar & x.lane;
		ar & x.link;
		ar & x.index;
	}
#endif
};

typedef Vector <Sim_Lane_Data>    Sim_Lane_Array;
typedef Sim_Lane_Array::iterator  Sim_Lane_Itr;
typedef Sim_Lane_Data *           Sim_Lane_Ptr;

#endif
