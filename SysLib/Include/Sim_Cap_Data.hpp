//*********************************************************
//	Sim_Cap_Data.hpp - macroscopic link capacity data
//*********************************************************

#ifndef SIM_CAP_DATA_HPP
#define SIM_CAP_DATA_HPP

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Cap_Data class definition
//---------------------------------------------------------

class Sim_Cap_Data
{
public:
	Sim_Cap_Data (void)             { Clear (); }

	int   First_Veh (void)          { return (first_veh); }
	int   Last_Veh (void)           { return (last_veh); }
	int   Max_Capacity (void)       { return (max_cap); }
	int   Low_Min_Lane (void)       { return (low_min); }
	int   Low_Max_Lane (void)       { return (low_max); }
	int   Low_Capacity (void)       { return (low_cap); }
	int   Low_Volume (void)         { return (low_vol); }
	int   High_Use (void)           { return (high_use); }
	int   High_Min_Lane (void)      { return (high_min); }
	int   High_Max_Lane (void)      { return (high_max); }
	int   High_Capacity (void)      { return (high_cap); }
	int   High_Volume (void)        { return (high_vol); }

	void  First_Veh (int veh)       { first_veh = veh; }
	void  Last_Veh (int veh)        { last_veh = veh; }
	void  Max_Capacity (int pce)    { max_cap = (short) pce; }
	void  Low_Min_Lane (int lane)   { low_min = (char) lane; }
	void  Low_Max_Lane (int lane)   { low_max = (char) lane; }
	void  Low_Capacity (int pce)    { low_cap = (short) pce; }
	void  Low_Volume (int pce)      { low_vol = (short) pce; }
	void  High_Use (int use)        { high_use = (short) use; }
	void  High_Min_Lane (int lane)  { high_min = (char) lane; }
	void  High_Max_Lane (int lane)  { high_max = (char) lane; }
	void  High_Capacity (int pce)   { high_cap = (short) pce; }
	void  High_Volume (int pce)     { high_vol = (short) pce; }

	void  Add_High (int vol = 1)    { high_vol = (short) (high_vol + vol); if (high_vol < 0) { high_vol = 0; if (low_vol > 0) low_vol--; }}
	void  Add_Low (int vol = 1)     { low_vol = (short) (low_vol + vol); if (low_vol < 0) { low_vol = 0; if (high_vol > 0) high_vol--; }}

	void  Reset_Use (int min_lane, int max_lane)
	{
		high_cap = max_cap; high_use = ANY_USE_CODE; high_min = (char) min_lane; high_max = (char) max_lane; low_cap = 0; low_min = low_max = -1; 
	}
	void  Clear (void)
	{
		first_veh = last_veh = -1; max_cap = low_cap = low_vol = high_cap = high_vol = 0;
		low_min = low_max = high_min = high_max = -1; high_use = ANY_USE_CODE;
	}
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
	int   first_veh;
	int   last_veh;
	short max_cap;
	char  low_min;
	char  low_max;
	short low_cap;
	short low_vol;
	short high_use;
	char  high_min;
	char  high_max;
	short high_cap;
	short high_vol;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		ar & first_veh;
		ar & last_veh;
		ar & max_cap;
		ar & low_min;
		ar & low_max;
		ar & low_cap;
		ar & low_vol;
		ar & high_use;
		ar & high_min;
		ar & high_max;
		ar & high_cap;
		ar & high_vol;
	}
#endif
};

typedef vector <Sim_Cap_Data>    Sim_Cap_Array;
typedef Sim_Cap_Array::iterator  Sim_Cap_Itr;
typedef Sim_Cap_Data *           Sim_Cap_Ptr;

#endif
