
- 3.2.5.3 Grouping Commands, since the list is executed in a subshell, variable assignments do not remain in effect after the subshell completes. 
- name=[value], value is optional
- name=[value] can appear as a param to export builtin

[ ] Astrisk expands when it's that command's turn, not in a pass before

[*] Backslash can negate astrisk/tilde expansion
    - Done by converting \c to 'c'
    [x] Backslashed chars inside normal words should be treated as squoted thing
    [x] Backslash chars inside dquotes should be treated as squoted chars

[x] Env var expands when it's that commnad's turn, not in a pass before
[x] Tilde expansion, only done in unquoted string

[?] Using $IFS for word separation

[x] Variable expancion, depends on the quoting

[ ] ambigous redirect, when an env_var doesn't exist
[ ] When executing a scope, the inner redirection wins, e.g (./test.sh < test.sh) <test2 will recieve the input from test.sh

[ ] Execution of commands
[ ] Builtins

[ ] Make sure that all the vec / deque data types can be initialized  with = {0}
