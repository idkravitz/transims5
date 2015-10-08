//*********************************************************
//	User_Program.hpp - Processes User Programs
//*********************************************************

#ifndef USER_PROGRAM_HPP
#define USER_PROGRAM_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Static_Service.hpp"
#include "Db_Base.hpp"
#include "Random.hpp"
#include "TypeDefs.hpp"

//---- space allocation parameters ----

#define	MAX_LEVEL    100
#define	MAX_NEST     100
#define MAX_LOOP     10
#define MAX_BREAK    10
#define	MAX_TABLE    10

//---------------------------------------------------------
//	User_Program Class definition
//---------------------------------------------------------

class SYSLIB_API User_Program : public Static_Service
{
public:
	User_Program (void);
	User_Program (Db_Base_Array &files);
	~User_Program (void)                            { Clear (); }

	bool Initialize (Db_Base_Array &files, int random_seed = 0);
	void Replicate (User_Program *ptr);

	bool Compile (string &text, bool list_flag = false);
	bool Compile (Db_File &script, bool list_flag);
	int  Execute (int record = -1);
	void Print_Commands (bool header = true);
	int  Random_Seed (int seed = 0)                 { return (random.Seed (seed)); }

	bool Compiled (void)                            { return (command.size () > 0); }

	void Read_Only (int num, bool flag);

	void Clear (void);

private:

	//---- code words ----

	enum Token_Type {LIMIT = 1, LOGICAL, RELATION, MATH, FUNCTION, CONVERT, DATETIME, IN_OUT, 
		TABLE, RETURN, DECLARE, DATA, INTEGER, FLOAT, STRING, LVALUE, FVALUE, SVALUE, 
		LVARIABLE, FVARIABLE, SVARIABLE, RECORD};

	enum Data_Type {INT_DATA, FLOAT_DATA, STRING_DATA, STRUCT_DATA};

	enum Tokens { EQUALS = 1, OPEN_PAR, CLOSE_PAR, COMMA, END_CMD, RANGE_DOT,   //---- limit ----
		IF, THEN, ELSE, ENDIF, WHILE, LOOP, BREAK, ENDLOOP,                     //---- logical ----
		EQ, NEQ, LT, LE, GT, GE, NOT, AND, OR,                                  //---- relational ----
		PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, POWER, NEGATIVE,                 //---- math ----
		FMIN, FMAX, ABS, SQRT, EXP, LOG, LOG10, RANDOM,                         //---- function ----
		FINT, FFLOAT, ROUND, ATOI, ATOF, ITOA, FTOA, SUBSTR, TRIM,              //---- convert ----
		TTOI, TTOF, ITOT, FTOT,
		DOW, HOUR, MONTH, YEAR, DATE, DATE_TIME, DATE_STR, TIME_STR,            //---- datetime ----
		PRINT, LIST, FORMAT, FREAD, FWRITE, INPUT, OUTPUT };                    //---- in_out ----

	enum Table_Tokens { ITAB, RTAB, STAB, END_TAB };

	enum Declare_Tokens { IVAR, RVAR, SVAR, END_DEF };

	//--- File_Data Array ---

	struct File_Data {
		Db_Base *file;
		int file_type;
		int file_stat;
		int read_only;
		int out_flag;

		File_Data () { file = 0; file_type = file_stat = read_only = out_flag = 0; }
	};

	typedef vector <File_Data>    File_Array;
	typedef File_Array::iterator  File_Itr;

	//---- Stack_Data Array ----

	struct Stack_Data {
		int    type;
		union {
			int      lvalue;
			double   fvalue;
			String * str_ptr;
		};
		Stack_Data () { type = lvalue = 0; fvalue = 0; str_ptr = 0; }
	};

	typedef vector <Stack_Data>    Stack_Array;
	typedef Stack_Array::iterator  Stack_Itr;

	//---- Command_Data Array ----

	struct Command_Data {
		int  type;
		int  token;
		int  value;

		Command_Data () { type = token = value = 0; }
	};

	typedef vector <Command_Data>    Command_Array;
	typedef Command_Array::iterator  Command_Itr;

	//---- Token_Data Array ----

	struct Token_Data {
		String name;            //---- text string ----
		int	   type;            //---- token type ----
		int	   code;            //---- token code ----
	};

	typedef vector <Token_Data>    Token_Array;
	typedef Token_Array::iterator  Token_Itr;

	//---- data ----

	int num_files;
	File_Array file_data;

	Token_Data * Keyword (string &token);
	Token_Data * Symbol (string &token);
	Token_Data * Special (string &token);
	Token_Data * Variable (string &token);

	string Token_Name (int type, int code);
	string Variable_Name (int type, int code);

	const char * Data_Type (int type)          { return ((type >= 0 && type <= STRUCT_DATA) ? data_type [type] : 0); }

	Token_Data *token_ptr;
	Token_Array variables;

	Command_Data cmd, *cmd_ptr, token_level [MAX_LEVEL];
	Stack_Data *s, *s1;

	Stack_Array stack;
	Command_Array command;
	Integers lvalue, lvariable;
	Doubles fvalue, fvariable;
	Strings svalue, svariable;
	Int_Map svalue_index;

	Random random;

	int record, line_num, level, assign_set, last_value, first_read, if_level;
	int nest, if_jump [MAX_NEST], then_jump [MAX_NEST], stack_index;
	int loop, while_jump [MAX_LOOP], loop_jump [MAX_LOOP];
	int num_break [MAX_LOOP], break_jump [MAX_LOOP] [MAX_BREAK];

	Integers table [3] [MAX_TABLE];

	bool table_flag, declare_flag, ascii_write, open_par;
	int table_type, declare_type;
	int sindex;

	bool Assign (void);
	bool Relation (void);
	bool Math (void);
	bool Function (void);
	bool Convert (void);
	bool Date_Time (void);
	bool Input_Output (void);
	bool Table (void);
	bool Read_Value (void);

	bool Initial_Table (String &line);
	bool Initial_Declare (String &line);

	bool Get_Token (String &line);
	bool Process_Token (void);
	bool Process_Command (int type);

	bool Syntax_Error (const char *text) {
		exe->Error (String ("%s Syntax Problem on Line Number %d") % text % line_num);
		return (false);
	}
	static Token_Data keywords [];
	static Token_Data symbols [];
	static Token_Data specials [];
	static const char *data_type [];	
};
#endif
