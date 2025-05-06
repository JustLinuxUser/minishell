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
[x] Builtins
    [x] echo
    [x] exit
        [x] properly free all memory
    [x] export
    [x] cd
        [x] cd -
        [x] PWD, OLDPWD
    [x] unset
    [ ] easter egg builtin with cool ascii art
        - Make it a list of all the things done here

[x] Special envvars
    [x] $$ -> /proc/self/stat, ft_split()[0]
    [x] $?
        [x] Set on syntax error
    - [ ] $RANDOM
    - [ ] _=, the last expanded argument of the last cmd
        but for the cmd it looks like the path of cmd
        - [x] `_=` for the bg cmd
        - [ ] `_=` afterwards
    [x] PWD in new processes (father)

[x] Using $IFS for word separation
    [x] ft_split with an array as the second opt
    [x] Set IFS on the startup as an unexported env var
[-] Managing syntax errors properly
    [x] Setting proper status
    [x] Proper message formating

[x] Managing HEREDOCs
    [x] <<-
    [x] quoted
    [x] unquoted
    [x] errors while writing
    [x] Expand separator, without envvars -
        [ ] Proper error handling
    [x] \char, should have it's own type, not just sqword, or mb normal word
    [x] Check for separator, after splitting by newlines, because readline can read multiple lines at once :)
    [x] `\$HOME` doesn't expand

[x] Buffered input
[x] Multiple line pasting, managing with custom readline that wraps the normal readline

[x] a=b bash, env var assignments before command
[x] Ambiguous redirect better error messages
[ ] All open / write calls should be error protected
    - Almost all of them are, too lazy to search though

[x] return proper exit status when executing, and getting a directory

- [x] Glob expansion
    - [x] sort ascii
    - [x] echo test, becomes echo test test


- [x] History
    - [x] normal history management + multi-line
    - [x] save line to history, even if it contains syntax error
    - [x] history file
        - [x] history file parsing.
        - [x] for \, I would have to escape \ with \\, and `<CR>` with `\<CR>` like zsh, kinda
        - [x] Don't save if the same as last cmd
    - [ ] clear history for <<-
    - [ ] reload history after <<-

- [x] Executing sources
    - [x] -c
    - [x] file.sh
    - [x] echo echo hi | ./minishell

[x] 
    - If a command exits with C-c, the shell stops all future executions,
        unless in interactive mode, in which case, it resets, and reads the next command
    - C-c gets forwarded to the cmd in the interactive shell
    - SIGQUIT is ignored by bash

[x] Signals (including Ctrl-\)
    [x] Fix signals with (sleep 10); echo lol
    [x] For expansion, it should stop when recieving a signal
    [x] print newline on C-c somewhere central, mb (stdout)
        [x] only if not actually exiting, because then the parrent shell will do it
    [x] don't print it in case of not readline
    [x] Restart syscalls after a signal handler stopped
        - impossible, probably
        - possible lol!! SA_RESTART

[x] prompt, change inv char to some other char

[x] Ctrl-D / Ctrl-C management in tokenizer additional input
    - [x] '<Ctrl-D>, print unexpected EOF error
    - [x] <Ctrl-D> should print `exit`, before exiting

- Detect C-c when executing a command, and don't execute the next one.
e.g: sleep 10; sleep 10; sleep 10
- Detect C-c when executing a file, and don't even try to read the next command

[x] Close fds properly ;(
    [x] cat | ls


[x] git submodule
    - Seams fine
[ ] Makefiles leak 

[x] Make sure that all the vec / deque data types can be initialized  with = {0}
    [x] In minishell
    [x] in libft

[x] libft fix dyn array types
    - [x] dyn str, is using more the the malloc on init
    - [x] resonable initial alloc size
    - [-] mb make normal types just be wrappers against central vec type
    - [-] str slices
    - [-] more algorithms

[x] Libft checked atoi, bit 3
    - [x] modify exit to use the proper nums

- [x] env, don't search in reverse

[x] Expansion of the special env vars

[x] Pasting in more then one EOF in the heredoc field, the rest should be treated as a command

[x] Subshell bug

# Memory model (to be expanded)

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

- [ ] Delete prohibited functions
    - [x] assert -> ft_assert
    - [ ] fopen, fprintf -> open, ft_fdprintf

- [ ] print_ast_dot should be a compile time define
    - [ ] figure out a good way to manage compile time defines with make

# bugs:
- unexpected EOF, for lexer,
    - [x] Only sets the error code if there is no prev error code
    - [x] and parser
        - [x] Always sets exit code to SYNTAX_ERR
- [x] printing exit on Ctrl-D, only when interactive though
- printing exit on exit, only when interactive,
    - [x] on Ctrl-D
    - [x] exit builtin
- [x] heredoc ctrl-d warning, (interactive and non interactive)
- [x] heredoc doesn't work rn
-   ```bash
    echo hello \
    <CTRL-D>
    ```
    [x] should execute `echo hello`, no warnings, generalazing, we should try lexing after ctrl-d, and the tokenizer should print an error, when something goes wrong, in the last pass, and if it works, we should still stop, mb by setting some flag in the buff_readline,
    it doesn't print exit though, Hmm

    - About printing `exit` on Ctrl-D, it's kinda easy, we only do that if the command is empty, and we don't even try to execute the command if hte the eof flag is set
    - So, we have 3 exit paths:
        - [x] Ctrl-D, empty command, prints exit in interactive mode
        - [x] Exiting with a final command after unfinished \ + Ctrl-D
        - Exiting wit han exit builtin, prints exit in interactive mode
    - [x] Parser is easy, propagate the error code from buff_readline, and we are golden, because we always print the same error there
    - [x] Lexer, we need to do a similar thing, but, also set what are we looking for

    - [x] shouldn't parse or print errors if C-c

    - Parser unexpencted token, newline is special
    - Parser syntax error should also print a full line, only in non interactive mode
    - Parser errors have different context then the rest of the errors in -c
        - Ignore this, I don't care enough
    - [x] \\ doesn't equal to \, idk why
        - Fixed

    - -c doesn't add a \n in the end,
    - buff_readline doesn't always expect a newline to be there
    - [x] \ without any char afterwards is just a \
    - [x] parser doesn't expect a newline at the end to be there always, doesn't execute if the flag is already set
    - [x] parser should not reset the status of the command unless there is at least one token
    - [x] `; ls` fails critically
    - [-] check all the parser code, to ensure that the unexpected is called everywere
    - [x] ` ` fails critically, and the newline token gets printed, why?
    - [x] `echo hi | (echo hello | cat -e)`, cat, bad file desc
    - [x] close all unnesessary fds
    - [x] check if `echo '\''hello'`
        - fails critically
    - [x] Heredoc sep, in files, and in heredocs, related to searching for sep, when getting a newline terminated line
    - [x] backslashes don't work well with newline additions
    - [x] multiline string should have nls
    - [x] ./minishell -c '\'
        - does nothing
    - [x] ./minishell -c '\\'
        - fails critically
        - [x] fixed, now also command not found
    - [x] ./minishell -c 'echo \' doesn't print anything
        - [x] now prints \
    - [x] ./minishell -c 'echo \\' fails
    - [x] redo the checking if the line is empty
    - [x] extend bs behaves differently if there actually is a newline afterwards or not, and for some reason when reading a file, it assumes that one is always there

    - [x] syntax error for some reason
    ```bash
    (ls -la
    



    )
    ```


    - [x] heredocs, are reversed, the last redir in should count only, same with hrdocs
    - [ ] don't segfault on ft_assert, do an exit, only segfault with an option set
    - [x] `cat /dev/urandom > /dev/null | cat` dies


## Handling of Ctrl-D
- Ctrl-D in get_tokens prints exit on interactive
- Ctrl-D always sets EOF flag
- Heredoc prints warning on EOF, and resets the EOF flag
- On bs, we execute on Ctrl-D
    - By checking if input is empty? Mb?

- [x] better error handling
- [x] redirect special error when redirect points t o a directory
- [x] redirect special error when permission denied
- [ ] the expansion of globs takes too much memory, and that means that I need to do something about it, for example run it in a different process, or use bigger arenas so I get the whole pages a time
    - Ignore that, I think, in release mode it takes a long time to slow down enough for it to be painfull

- 
```bash
andrii@arch> echo hello\<Return>
> <Ctrl-d>
> <Ctrl-d>
hello
<Exits here>
```
It's a bug, I don't replicate clear bugs

```bash
andrii@arch> echo hello \<Return>
> <Ctrl-d>
hello
<Exits here>
```

- Ok, the bash bug is a problem, and my flag idea is also a problem,
because you can terminate heredocs
    - Fixed by resetting the flag when reading heredocs

```bash
andrii@arch> echo hello\<Return>
> <Ctrl-d>
> world
hello world
andrii@arch> <Up arrow>
andrii@arch> echo helloworld <Return>
helloworld
```
- [ ] command not found with newlines

- [x] doesn't append hello world more then once
```bash
(echo hello world) >> file1 && cat file1
```
- [x] double check all redirects, and that they start at the same thing
- [ ] backslashes in double quotes
