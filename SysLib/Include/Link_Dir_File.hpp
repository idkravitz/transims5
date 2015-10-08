//******************************************************** 
//	Link_Dir_File.hpp - Link Direction File Input/Output
//********************************************************

#ifndef LINK_DIR_FILE_HPP
#define LINK_DIR_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Link_Direction_File Class definition
//---------------------------------------------------------

class SYSLIB_API Link_Direction_File : public Db_Header, public Time_Periods
{
public:

	Link_Direction_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Link_Direction_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int Link_Dir (void)                     { return (Get_Integer (link_dir)); }
	int Link (void)                         { return (Get_Integer (link)); }
	int Dir (void)                          { return (Get_Integer (dir)); }
	double Data (int period)                { return (Get_Double (Index (period))); }
	double Data2 (int period)               { return (Get_Double (Index2 (period))); }
	double Diff (int period)                { return (Get_Double (Index3 (period))); }

	void Link_Dir (int value)               { Put_Field (link_dir, value); }
	void Link (int value)                   { Put_Field (link, value); }
	void Dir (int value)                    { Put_Field (dir, value); }
	void Data (int period, double value)    { Put_Field (Index (period), value); }
	void Data2 (int period, double value)   { Put_Field (Index2 (period), value); }
	void Diff (int period, double value)    { Put_Field (Index3 (period), value); }

	void Num_Decimals (int number)          { decimal = number; }

	Units_Type Data_Units (void)            { return (units); }
	void Data_Units (Units_Type type)       { units = type; }

	bool Direction_Index (void)             { return (index_flag); }
	void Direction_Index (bool flag)        { index_flag = flag; }

	bool Ignore_Periods (void)              { return (ignore_flag); }
	void Ignore_Periods (bool flag)         { ignore_flag = flag; }

	bool Join_Flag (void)                   { return (join_flag); }
	void Join_Flag (bool flag)              { join_flag = flag; }

	bool Difference_Flag (void)             { return (diff_flag); }
	void Difference_Flag (bool flag)        { diff_flag = flag; }

	virtual bool Create_Fields (void);
	bool Create_Fields (int low, int high, int increment = 0);

	void Zero_Fields (void);
	void Difference (void);

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	void Clear_Fields (void);

	int  Index (int period)     { return ((period >= 0 && period < (int) data.size ()) ? data [period] : -1); }
	int  Index2 (int period)    { return ((period >= 0 && period < (int) data2.size ()) ? data2 [period] : -1); }
	int  Index3 (int period)    { return ((period >= 0 && period < (int) diff.size ()) ? diff [period] : -1); }
	
	bool set_flag;
	Units_Type units;
	int link_dir, link, dir, decimal;
	bool ignore_flag, join_flag, diff_flag, index_flag;
	Integers data, data2, diff;
};

#endif
