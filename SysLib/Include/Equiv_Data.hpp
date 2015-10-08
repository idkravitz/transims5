//*********************************************************
//	Equiv_Data.hpp - base class for Equivalence Files
//*********************************************************

#ifndef EQUIV_DATA_HPP
#define EQUIV_DATA_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Time_Periods.hpp"
#include "Db_File.hpp"
#include "String.hpp"

//---------------------------------------------------------
//	Equiv_Data Class definition
//---------------------------------------------------------

class SYSLIB_API Equiv_Data : public Db_File
{
public:
	Equiv_Data (string type = "");

	void    Type (string _type)         { type = _type; }
	string& Type (void)                 { return (type); }

	virtual bool Read (bool report_flag);
	
	string Group_Label (int group);
	Int_Set * Group_List (int group);
	Time_Periods * Group_Period (int group);

	int  Num_Groups (void)              { return ((int) group_map.size ()); };
	int  Max_Group (void);

protected:
	void   Time_Flag (void)             { time_flag = true; }

	typedef struct {
		String label;
		Int_Set list;
		Time_Periods period;
	} Equiv_Group;

	typedef map <int, Equiv_Group>      Group_Map;
	typedef Group_Map::value_type       Group_Map_Data;
	typedef Group_Map::iterator         Group_Map_Itr;
	typedef pair <Group_Map_Itr, bool>  Group_Map_Stat;

	Group_Map group_map;

private:
	bool time_flag;
	String type;
};

//---------------------------------------------------------
//	Link_Equiv
//---------------------------------------------------------

class SYSLIB_API Link_Equiv : public Equiv_Data
{
public:
	Link_Equiv (void) : Equiv_Data ("Link") {}
};

//---------------------------------------------------------
//	Line_Equiv
//---------------------------------------------------------

class SYSLIB_API Line_Equiv : public Equiv_Data
{
public:
	Line_Equiv (void) : Equiv_Data ("Line") {}
};

//---------------------------------------------------------
//	Stop_Equiv
//---------------------------------------------------------

class SYSLIB_API Stop_Equiv : public Equiv_Data
{
public:
	Stop_Equiv (void) : Equiv_Data ("Stop") {}
};

//---------------------------------------------------------
//	Zone_Equiv
//---------------------------------------------------------

class SYSLIB_API Zone_Equiv : public Equiv_Data
{
public:
	Zone_Equiv (void) : Equiv_Data ("Zone") 
	{
		num_warning = num_missing = 0;
	}
	virtual bool Read (bool report_flag);

	int  Zone_Group (int zone);

	int  Num_Warning (void)             { return (num_warning); };
	int  Num_Missing (void)             { return (num_missing); };

private:
	int num_warning, num_missing;

	Int_Map zone_map;
};

//---------------------------------------------------------
//	Time_Equiv
//---------------------------------------------------------

class SYSLIB_API Time_Equiv : public Equiv_Data
{
public:
	Time_Equiv (void) : Equiv_Data ("Time") { Time_Flag (); }

	int Period (int time);
};

#endif
