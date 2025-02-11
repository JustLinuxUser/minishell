COMMMAND = 
        <VAR_ASSIGN_LIST> <WORD_OR_REDIR_LIST>

WORD_OR_REDIR_LIST =
        <WORD> <WORD_OR_REDIR_LIST> |
        <REDIR> <WORD_OR_REDIR_LIST> |
        <EMPTY>

REDIR_LIST  =
        <REDIR> <REDIR_LIST> |
        <REDIR>

REDIR =
        '<' <WORD> |
        '>' <WORD> |
        '>>' <WORD> |
        <HEREDOC> 

HEREDOC =
        '<<' <HEREDOC_ARG> |
        '<<-' <HEREDOC_ARG>

VAR_ASSING_LIST =
        <VAR_ASSIGN> <VAR_ASSIGN_LIST> | <EMPTY>

VAR_ASSIGN = 
        <IDENT>'='<WORD> | <IDENT>'='

IDENT =
        <ALPHA> <ALPHA_NUM>*

WORD =
        <DQUOTED_STR> | <SQUOTED_STR> | <ALPHANUM>+ |
        <DQUOTED_STR> <WORD>

COMMAND_PIPELINE =
        <COMMAND> '|' <COMMAND_PIPELINE> |
        <COMMAND>

COMMAND_EXPR =
        <COMMAND_PIPELINE> '&&' <BRACED_EXPR> | 
        <COMMAND_PIPELINE> '||' <BRACED_EXPR>
        <COMMAND_PIPELINE>';'
        <BRACED_EXPR> ';'

BRACED_EXPR =
    '(' <COMMAND_EXPR> ')' <REDIR_LIST> | <COMMAND_EXPR>


BRACED_EXPR  <------------------┐ 
└-> COMMAND_EXPR                |  
     └-> COMMAND_PIPELINE | BRACED_EXPR
         └-> COMMAND | COMMAND_PIPELINE
              └-> WORD | ENV | REDIR | EQ

 EXPR <---------------------------┐ 
└-> BRACED_EXPR | COMMAND_EXPR    |  
     └-> COMMAND_PIPELINE | BRACED_EXPR
         └-> COMMAND | COMMAND_PIPELINE
              └-> WORD | ENV | REDIR | EQ


<complete_cmd> ::=  <and_or> <newline>
<and_or>       ::=  <pipeline> { ('&&' | '||') <pipeline> }        
<pipeline>     ::=  <command> { '|' <command> }
<command>      ::=  <simple_cmd> | '(' <and_or> ')'
<simple_cmd>   ::=  { <redirect> } <word> { ( <redirect> | <word> ) }
<redirect>     ::=  ( '<' | '>' | '<<' | '>>' ) <word>

