#include "./msh.h"

void dbg_print(char *s) {
	if (DBG == 1) {
		write(1, s, ft_strlen(s));
	}
}

void print_err_execve(char *s) {
	char *msg = "error: cannot execute ";

	write(2, msg, ft_strlen(msg));
	write(2, s, ft_strlen(s));
	write(2, "\n", 1);
}

void print_cmds(t_cmd *cmd_lst)
{
	int i;
	char **c;
	char *del;

	i = 1;
	while (cmd_lst != NULL)
	{
		printf("cmd #%d:", i++);
		if (cmd_lst->delim == PIPE)
			del = "PIPE";
		else if (cmd_lst->delim == SEMI)
			del = "SEMI";
		if (cmd_lst->delim == LAST)
			del = "LAST";
		printf("\tdelim [%s]", del);
		printf("\tpathname [%s], args [", cmd_lst->argv[0]);
		c = cmd_lst->argv + 1;
		while (*c != NULL)
		{
			printf("(%s)", *c);
			c++;
		}
		printf("]\n");
		cmd_lst = cmd_lst->next;
	}
}

