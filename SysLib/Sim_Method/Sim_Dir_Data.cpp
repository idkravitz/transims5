//*********************************************************
//	Sim_Dir_Data.cpp - network link direction data
//*********************************************************

#include "Sim_Dir_Data.hpp"

//---------------------------------------------------------
//	Make_Data
//---------------------------------------------------------

void Sim_Dir_Data::Make_Data (void)
{
	if (Method () == MESOSCOPIC) {
		Sim_Lane_Data rec; 
		sim_lanes = new Sim_Lane_Array (); 
		sim_lanes->assign (Lanes (), rec);
	} else if (Method () == MACROSCOPIC) {
		sim_cap = new Sim_Cap_Data (); 
	}
}

//---------------------------------------------------------
//	Delete_Data
//---------------------------------------------------------

void Sim_Dir_Data::Delete_Data (void)
{
	if (Method () == MESOSCOPIC) {
		if (sim_lanes != 0) delete sim_lanes;
	} else if (Method () == MACROSCOPIC) {
		if (sim_cap != 0) delete sim_cap; 
	}
}

#ifdef MAKE_MPI

//---------------------------------------------------------
//	Pack
//---------------------------------------------------------

bool Sim_Dir_Data::Pack (Data_Buffer &data)
{
	if (data.Add_Data (&x, sizeof (x))) {
		if (Method () == MESOSCOPIC) {
			return (sim_lanes->Pack (data));
		} else if (Method () == MACROSCOPIC) {
			return (sim_cap->Pack (data)); 
		}
	}
	return (false);
}

//---------------------------------------------------------
//	UnPack
//---------------------------------------------------------

bool Sim_Dir_Data::UnPack (Data_Buffer &data)
{
	if (data.Get_Data (&x, sizeof (x))) {
		if (Method () == MESOSCOPIC) {
			if (sim_lanes == 0) {
				sim_lanes = new Sim_Lane_Array (); 
			} else {
				sim_lanes->clear ();
			}
			return (sim_lanes->UnPack (data));
		} else if (Method () == MACROSCOPIC) {
			if (sim_cap == 0) sim_cap = new Sim_Cap_Data ();
			return (sim_cap->UnPack (data));
		}
	}
	return (false);
}
#endif

//---------------------------------------------------------
//	destructor
//---------------------------------------------------------

Sim_Dir_Array::~Sim_Dir_Array (void)
{
	//Sim_Dir_Itr itr;
	//for (itr = begin (); itr != end (); itr++) {
	//	itr->Delete_Data ();
	//}
}
