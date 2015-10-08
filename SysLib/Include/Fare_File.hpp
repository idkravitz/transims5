//******************************************************** 
//	Fare_File.hpp - Fare File Input/Output
//********************************************************

#ifndef FARE_FILE_HPP
#define FARE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Fare_File Class definition
//---------------------------------------------------------

class SYSLIB_API Fare_File : public Db_Header
{
public:
	Fare_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Fare_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	String From_Zone (void)         { return (Get_String (from_zone)); }
	int    iFrom_Zone (void)        { return (Get_Integer (from_zone)); }
	String To_Zone (void)           { return (Get_String (to_zone)); }
	int    iTo_Zone (void)          { return (Get_Integer (to_zone)); }
	String From_Mode (void)         { return (Get_String (from_mode)); }
	int    iFrom_Mode (void)        { return (Get_Integer (from_mode)); }
	String To_Mode (void)           { return (Get_String (to_mode)); }
	int    iTo_Mode (void)          { return (Get_Integer (to_mode)); }
	String Period (void)            { return (Get_String (period)); }
	int    iPeriod (void)           { return (Get_Integer (period)); }
	String Class (void)             { return (Get_String (type)); }
	int    iClass (void)            { return (Get_Integer (type)); }
	int    Fare (void)              { return (Get_Integer (fare)); }

	void From_Zone (char * value)   { Put_Field (from_zone, value); }
	void From_Zone (string value)   { Put_Field (from_zone, value); }
	void From_Zone (int value)      { Put_Field (from_zone, value); }
	void To_Zone (char * value)     { Put_Field (to_zone, value); }
	void To_Zone (string value)     { Put_Field (to_zone, value); }
	void To_Zone (int value)        { Put_Field (to_zone, value); }
	void From_Mode (char * value)   { Put_Field (from_mode, value); }
	void From_Mode (string value)   { Put_Field (from_mode, value); }
	void From_Mode (int value)      { Put_Field (from_mode, value); }
	void To_Mode (char * value)     { Put_Field (to_mode, value); }
	void To_Mode (string value)     { Put_Field (to_mode, value); }
	void To_Mode (int value)        { Put_Field (to_mode, value); }
	void Period (char * value)      { Put_Field (period, value); }
	void Period (string value)      { Put_Field (period, value); }
	void Period (int value)         { Put_Field (period, value); }
	void Class (char * value)       { Put_Field (type, value); }
	void Class (string value)       { Put_Field (type, value); }
	void Class (int value)          { Put_Field (type, value); }
	void Fare (int value)           { Put_Field (fare, value); }
	
	int  Period (Dtime time)        { return (time_periods.Period (time)); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int from_zone, to_zone, from_mode, to_mode, period, type, fare;

	Time_Periods time_periods;
};

#endif
