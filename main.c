#include "./msh.h"

//OK: strcmp, strncmp, dup, dup2, fork, pipe, ,alloc, close, exit
// why we might need strncmp, signall and kill here?

//TODO: find way to ensure no fd leaks (watch process tree and proc fs for fds)
//TODO: to stderror: if cd has wrong number of args; if cd can't go
//TODO: if any call except execve, and something ends with err - print fatal err
//	and end programm
//TODO: if execve can't find executable - print to stderr?

//int main(int argc, char **argv, char **envp)
int main(int argc, char **argv)
{
	int	i;
	int after_last_delim;
	t_cmd *cmd_lst;

	cmd_lst = NULL;
	i = 1;
	after_last_delim = 1;
	while(i < argc)
	{
		if (strcmp(";", argv[i]) == 0 || strcmp("|", argv[i]) == 0) {
			add_cmd(&cmd_lst, argv, after_last_delim, i);
			after_last_delim = i + 1;
		}
		i++;
	}
	add_cmd(&cmd_lst, argv, after_last_delim, i);
	print_cmds(cmd_lst);
	return (0);
}


