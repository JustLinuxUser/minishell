%%


r_simple_list 	: r_pipeline {('&&' | '||' | ';') (\n*) r_pipeline} {';'}


redirection		: '>' WORD
			 	| '<' WORD
				| '<<' WORD
				| '<<-' WORD
				;

rpipeline		: r_command {'|' (\n*) pipeline}
				;

r_command		: rsimple_command
	   			| subshell ({redirection} +)
	   			;

rsimple_command	: simple_command_element +
			 	;

subshell		: '(' rcompound_list ')'
		  		;

simple_command_element: {(WORD | ASSIGNMENT_WORD | redirection)}+;


/* A list allows leading or trailing newlines and
   newlines as operators (equivalent to semicolons).
   It must end with a newline or semicolon.
   Lists are used within commands such as if, for, while.  */

rcompound_list 	: r_pipeline {('&&' | '||' | ';' | '\n') (\n*) r_pipeline} {;}(\n*)
%%
