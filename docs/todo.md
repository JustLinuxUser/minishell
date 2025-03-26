# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    todo.md                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/03/26 19:18:24 by anddokhn          #+#    #+#              #
#    Updated: 2025/03/26 19:22:57 by anddokhn         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


- 3.2.5.3 Grouping Commands, since the list is executed in a subshell, variable assignments do not remain in effect after the subshell completes. 
- name=[value], value is optional
- name=[value] can appear as a param to export builtin


[*] Backslash can negate astrisk/tilde expansion
    - Done by converting \c to 'c'
    [x] Backslashed chars inside normal words should be treated as squoted thing
    [x] Backslash chars inside dquotes should be treated as squoted chars

[x] Env var expands when it's that commnad's turn, not in a pass before
[x] Tilde expansion, only done in unquoted string
[x] Variable expancion, depends on the quoting



[x] ambigous redirect, when an env_var doesn't exist
[x] When executing a scope, the inner redirection wins, e.g (./test.sh < test.sh) <test2 will recieve the input from test.sh

[x] Execution of commands
[ ] Builtins

[x] Special envvars
    [x] $$ -> /proc/self/stat, ft_split()[0]
    [x] $?
        [x] Set on syntax error
    [ ] PWD in new processes

[ ] Temp env set

[?] Ctrl-D handling with non empty lines

[ ] Using $IFS for word separation
    [ ] ft_split with an array as the second opt
[-] Managing syntax errors properly
    [x] Setting proper status
    [ ] Proper message formating

[x] Managing HEREDOCs
    [x] <<-
    [x] quoted
    [x] unquoted
    [?] errors while writing
    [x] Expand separator, without envvars -
    [x] \char, should have it's own type, not just sqword, or mb normal word
    [x] Check for separator, after splitting by newlines, because readline can read multiple lines at once :)

[x] Buffered input
[x] Multiple line pasting, managing with custom readline that wraps the normal readline

[x] a=b bash, env var assignments before command
[ ] Ambiguous redirect better error messages
[ ] All open / write calls should be error protected
[ ] Glob expansion

[ ] Signals

[x] Close fds properly ;(
    [x] cat | ls


[x] Make sure that all the vec / deque data types can be initialized  with = {0}
    [x] In minishell
    [x] in libft

[x] Expansion of the special env vars
