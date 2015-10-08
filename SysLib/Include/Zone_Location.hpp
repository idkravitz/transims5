//*********************************************************
//	Zone_Location.hpp - zone location map class
//*********************************************************

#ifndef ZONE_LOCATION_HPP
#define ZONE_LOCATION_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Db_File.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Zone_Location_Map Class definition
//---------------------------------------------------------

class SYSLIB_API Zone_Location_Map : public Db_File, public Ints_Map
{
public:
	Zone_Location_Map (void);

	bool Read (void);
	bool Write (void);

	Integers * Locations (int zone);

	int  Num_Zones (void)               { return ((int) size ()); };
};

#endif
