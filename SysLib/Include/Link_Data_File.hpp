//******************************************************** 
//	Link_Data_File.hpp - Link Data File Input/Output
//********************************************************

#ifndef LINK_DATA_FILE_HPP
#define LINK_DATA_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Link_Data_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Data_File : public Db_Header, public Time_Periods
{
public:

	Link_Data_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Data_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                          { return (Get_Integer (link)); }
	int    Anode (void)                         { return (Get_Integer (anode)); }
	int    Bnode (void)                         { return (Get_Integer (bnode)); }
	double Data_AB (int period)                 { return (Get_Double (AB (period))); }
	double Data_BA (int period)                 { return (Get_Double (BA (period))); }

	void   Link (int value)                     { Put_Field (link, value); }
	void   Anode (int value)                    { Put_Field (anode, value); }
	void   Bnode (int value)                    { Put_Field (bnode, value); }
	void   Data_AB (int period, double value)   { Put_Field (AB (period), value); }
	void   Data_BA (int period, double value)   { Put_Field (BA (period), value); }

	void   Num_Decimals (int number)            { decimal = number; }
	
	Units_Type Data_Units (void)                { return (units); }
	void Data_Units (Units_Type type)           { units = type; }

	virtual bool Create_Fields (void);
	bool Create_Fields (int low, int high, int increment = 0);
	
	bool Set_Flag (void)                        { return (set_flag); }
	void Set_Flag (bool flag)                   { set_flag = flag; }

	void Zero_Fields (void);

	int  AB (int period)                        { return ((period >= 0 && period < (int) ab.size ()) ? ab [period] : -1); }
	int  BA (int period)                        { return ((period >= 0 && period < (int) ba.size ()) ? ba [period] : -1); }

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	void Clear_Fields (void);

	bool set_flag;
	Units_Type units;
	int link, anode, bnode, decimal;
	Integers ab, ba;
};

#endif
