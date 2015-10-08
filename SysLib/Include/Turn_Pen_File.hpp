//******************************************************** 
//	Turn_Pen_File.hpp - Turn Penalty File Input/Output
//********************************************************

#ifndef TURN_PEN_FILE_HPP
#define TURN_PEN_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Turn_Pen_File Class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Pen_File : public Db_Header
{
public:
	Turn_Pen_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Turn_Pen_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    To_Link (void)             { return (Get_Integer (to_link)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	int    Use (void)                 { return (Get_Integer (use)); }
	int    Min_Veh_Type (void)        { return (Get_Integer (min_type)); }
	int    Max_Veh_Type (void)        { return (Get_Integer (max_type)); }
	int    Penalty (void)             { return (Get_Integer (penalty)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   To_Link (int value)        { Put_Field (to_link, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Use (int value)            { Put_Field (use, value); }
	void   Min_Veh_Type (int value)   { Put_Field (min_type, value); }
	void   Max_Veh_Type (int value)   { Put_Field (max_type, value); }
	void   Penalty (int value)        { Put_Field (penalty, value); }

	bool   Node_Based (void)          { return (link < 0 && to_link < 0 && in_node >= 0 && out_node >= 0); }

	int    In_Node (void)             { return (Get_Integer (in_node)); }
	int    Out_Node (void)            { return (Get_Integer (out_node)); }

	void   In_Node (int value)        { Put_Field (in_node, value); }
	void   Out_Node (int value)       { Put_Field (out_node, value); }
	
	bool   Use_Flag (void)            { return (use >= 0); }
	bool   Veh_Type_Flag (void)       { return (min_type >= 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, to_link, start, end, use, min_type, max_type, penalty, in_node, out_node;
};

#endif
