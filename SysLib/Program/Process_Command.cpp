//********************************************************* 
//	Process_Command.cpp - Process a Token Command
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Process_Command
//---------------------------------------------------------

bool User_Program::Process_Command (int type)
{
	static int math_level [] = {
		0, 0, 1, 1, 2, 3, 4
	};

	static int rel_level [] = {
		2, 2, 2, 2, 2, 2, 1, 0, 0
	};
	
	int i, j, jump;
	
	while (level > 1) {	
		cmd_ptr = &token_level [level];

		if (type == MATH) {
	
			if (cmd_ptr->type != MATH) return (true);
		
			if (math_level [cmd_ptr->token - PLUS] <
				math_level [cmd.token - PLUS]) return (true);
				
		} else if (type == RELATION) {
		
			if (cmd_ptr->type != RELATION) return (true);
			
			if (rel_level [cmd_ptr->token - EQ] < rel_level [cmd.token - EQ]) return (true);
				
		} else if (type == LIMIT && cmd_ptr->token == OPEN_PAR) {
			
			if (cmd.token == CLOSE_PAR) {
				cmd_ptr = &token_level [--level];
				
				if (cmd_ptr->type == FUNCTION || cmd_ptr->type == CONVERT ||
					cmd_ptr->type == DATETIME || cmd_ptr->type == IN_OUT ||	
					cmd_ptr->type == TABLE || cmd_ptr->type == RETURN) {

					cmd_ptr = &token_level [level--];

					command.push_back (*cmd_ptr);
				}
			}
			return (true);
		}
		
		if (cmd_ptr->token == EQUALS) {

			command.push_back (*cmd_ptr);

			cmd_ptr = &token_level [--level];

			command.push_back (*cmd_ptr);
			
			level--;
			if (type == LIMIT) return (true);
			
		} else if (cmd_ptr->type == LOGICAL) {
		
			if (type != LOGICAL) return (true);
			level--;

			if (cmd_ptr->token == WHILE) {
				if (cmd.token != LOOP) {
					return (Syntax_Error ("Loop"));
				}
				while_jump [loop] = (int) command.size ();
				num_break [loop] = 0;

				command.push_back (*cmd_ptr);

			} else if (cmd_ptr->token == LOOP) {
				if (cmd.token == ENDLOOP) {

					//---- add the loop jump ----

					cmd_ptr->value = loop_jump [loop];
					jump = (int) command.size ();

					command.push_back (*cmd_ptr);

					//---- add the break jumps ----

					for (i=0; i < num_break [loop]; i++) {
						j = break_jump [loop] [i];
						if (j > 0) {
							cmd_ptr = &command [j];
							cmd_ptr->value = jump;
						}
					}

					//---- add the while jump ----

					j = while_jump [loop];
					if (j > 0) {
						cmd_ptr = &command [j];
						cmd_ptr->value = jump;
					}
				} else {
					return (Syntax_Error ("Loop"));
				}
			} else if (cmd_ptr->token == IF) {
				if (cmd.token != THEN) {
					return (Syntax_Error ("Logical"));
				}
				if_jump [nest] = (int) command.size ();

				command.push_back (*cmd_ptr);

			} else if (cmd_ptr->token == THEN) {
			
				if (cmd.token == ELSE || cmd.token == ENDIF) {
					if (cmd.token == ELSE) {
						then_jump [nest] = jump = (int) command.size ();

						command.push_back (*cmd_ptr);
					} else {
						jump = (int) command.size () - 1;
					}
					j = if_jump [nest];
					if (j > 0) {
						cmd_ptr = &command [j];
						cmd_ptr->value = jump;
					}
				} else {
					return (Syntax_Error ("Logical"));
				}
			} else if (cmd_ptr->token == ELSE) {
				
				if (cmd.token != ENDIF) {
					return (Syntax_Error ("Logical"));
				}
				jump = (int) command.size () - 1;

				j = then_jump [nest];
				if (j > 0) {
					cmd_ptr = &command [j];
					cmd_ptr->value = jump;
				}				
			}
			return (true);
			
		} else {
			command.push_back (*cmd_ptr);
			level--;	
		}
	}
	return (true);
}
