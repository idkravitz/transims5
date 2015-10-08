//*********************************************************
//	Sim_Traveler.hpp - simulator traveler data
//*********************************************************

#ifndef SIM_TRAVELER_HPP
#define SIM_TRAVELER_HPP

#include "Sim_Plan_Data.hpp"
#include "Random.hpp"
#include "Data_Pack.hpp"

#ifdef BOOST_THREADS
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#endif

#ifdef BOOST_MPI
#include <boost/serialization/base_object.hpp>
#endif

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Sim_Traveler_Data class definition
//---------------------------------------------------------

class Sim_Traveler_Data
{
public:
	Sim_Traveler_Data (void)        { Clear (); }
	~Sim_Traveler_Data (void)       { Delete (); }

	int  Type (void)                { return (type); }
	int  Problem (void)             { return (problem); }
	int  Status (void)              { return (status); }

	void Type (int value)           { type = (short) value; }
	void Problem (int value)        { problem = (char) value; }
	void Status (int value)         { status = (char) value; }

	//---- pointer methods ----

	Sim_Plan_Ptr                    plan_ptr;

	bool Active (void)              { return (plan_ptr != 0); }
	Sim_Plan_Ptr Create (void)      { return (plan_ptr = new Sim_Plan_Data ()); }
	void Delete (void)              { if (plan_ptr) { delete plan_ptr; plan_ptr = 0; } }

	Random random;

	void Clear (void)               { type = 0; problem = status = 0; plan_ptr = 0; }

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&type, sizeof (type))) {
			if (data.Add_Data (&problem, sizeof (problem))) {
				if (data.Add_Data (&status, sizeof (status))) {
					if (data.Add_Data (&random, sizeof (random))) {
						bool ptr_flag = (plan_ptr != 0);
						if (data.Add_Data (&ptr_flag, sizeof (ptr_flag))) {
							if (ptr_flag) {
								return (plan_ptr->Pack (data));
							} else {
								return (true);
							}
						}
					}
				}
			}
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&type, sizeof (type))) {
			if (data.Get_Data (&problem, sizeof (problem))) {
				if (data.Get_Data (&status, sizeof (status))) {
					if (data.Get_Data (&random, sizeof (random))) {
						bool ptr_flag;
						if (data.Get_Data (&ptr_flag, sizeof (ptr_flag))) {
							if (ptr_flag) {
								plan_ptr = new Sim_Plan_Data ();
								return (plan_ptr->UnPack (data));
							} else {
								plan_ptr = 0;
								return (true);
							}
						}
					}
				}
			}
		}
		return (false);
	}
#endif

private:
	short type;
	char  problem;
	char  status;

#ifdef BOOST_MPI
	friend class boost::serialization::access;

	template <class Archive>
	void serialize (Archive &ar, const unsigned int version)
	{
		bool ptr_flag = (plan_ptr != 0);
		ar & type;
		ar & problem;
		ar & status;
		ar & random.Seed ();
		ar & ptr_flag;
		if (ptr_flag) {
			ar & *plan_ptr;
		}
	}
#endif
};

typedef Sim_Traveler_Data * Sim_Traveler_Ptr;

//---------------------------------------------------------
//	Sim_Traveler_Array class definition
//---------------------------------------------------------

class Sim_Traveler_Array : public vector <Sim_Traveler_Ptr>
{
public:
	Sim_Traveler_Array (void)  { }
	~Sim_Traveler_Array (void) {
		vector <Sim_Traveler_Ptr>::iterator  itr;
		for (itr = begin (); itr != end (); itr++) {
			if (*itr != 0) {
				delete *itr;
				*itr = 0;
			}
		}
		clear ();
	}
};

typedef Sim_Traveler_Array::iterator    Sim_Traveler_Itr;

#endif
