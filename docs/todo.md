- 3.2.5.3 Grouping Commands, since the list is executed in a subshell, variable assignments do not remain in effect after the subshell completes. 

- name=[value], value is optional
    - [x] Before command
    - [x] by itself, changing env
- [x] name=[value] can appear as a param to export builtin


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
    [x] echo
    [x] exit
        [x] properly free all memory
    [ ] export
    [-] cd
        [ ] cd -

[x] Special envvars
    [x] $$ -> /proc/self/stat, ft_split()[0]
    [x] $?
        [x] Set on syntax error
    [ ] $RANDOM
    [ ] _=, the path of the cmd
    [x] PWD in new processes (father)

[?] Ctrl-D handling with non empty lines
    - nope

[?] Using $IFS for word separation
    [ ] ft_split with an array as the second opt
[-] Managing syntax errors properly
    [x] Setting proper status
    [ ] Proper message formating

[x] Managing HEREDOCs
    [x] <<-
    [x] quoted
    [x] unquoted
    [ ] errors while writing
    [x] Expand separator, without envvars -
        [ ] Proper error handling
    [x] \char, should have it's own type, not just sqword, or mb normal word
    [x] Check for separator, after splitting by newlines, because readline can read multiple lines at once :)
    [x] `\$HOME` doesn't expand
    [ ] Herestrings
        - No word splitting or pathname extension
        - Nahhh

[x] Buffered input
[x] Multiple line pasting, managing with custom readline that wraps the normal readline

[x] a=b bash, env var assignments before command
[x] Ambiguous redirect better error messages
[ ] All open / write calls should be error protected

[ ] return proper exit status when executing, and getting a directory

- [x] Glob expansion
    - [x] sort ascii
    - [x] echo test, becomes echo test test


- [ ] History
    - [x] normal history management + multi-line
    - [x] save line to history, even if it contains syntax error
    - [x] history file
        - [x] history file parsing.
        - [x] for \, I would have to escape \ with \\, and `<CR>` with `\<CR>` like zsh, kinda
        - [ ] Don't save if the same as last cmd
    - [ ] clear history for <<-
    - [ ] reload history after <<-

- [x] Executing sources
    - [x] -c
    - [x] file.sh
    - [x] echo echo hi | ./minishell


[ ] 
    - If a command exits with C-c, the shell stops all future executions,
        unless in interactive mode, in which case, it resets, and reads the next command
    - C-c gets forwarded to the cmd in the interactive shell
    - SIGQUIT is ignored by bash
[ ] Signals (including Ctrl-\)
    [ ] For expansion, it should stop when recieving a signal

[ ] Ctrl-D / Ctrl-C management in tokenizer additional input
- Detect C-c when executing a command, and don't execute the next one.
e.g: sleep 10; sleep 10; sleep 10
- Detect C-c when executing a file, and don't even try to read the next command

[ ] Libft, update, push

[x] Close fds properly ;(
    [x] cat | ls


[x] Make sure that all the vec / deque data types can be initialized  with = {0}
    [x] In minishell
    [x] in libft

[x] Expansion of the special env vars

[x] Pasting in more then one EOF in the heredoc field, the rest should be treated as a command

[x] Subshell bug

# Memory model

Tokenizer: 1 string
|> tokens: a ref to str of tokenizer
|> Parser: owned tree, tokens are refs to tokens of tokenizer
|> ReParser: mut owned tree, tokens are refs to tokens of tokenizer
|> Heredocs: state->heredocs, nodes have refs to state->heredocs
|> exe_<node>: ref to tree nodes, owned redirects
|> expand_simple_command: ref to tree nodes
    - tilde_expand_simple_command: todo, figure out why a=~ doesn't expand, and a= does
- expand_word_fully, keep as one: ref to node, consuming, when consuming properly, it works lol

case: 
