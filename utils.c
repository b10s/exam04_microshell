#include "./msh.h"

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

void add_cmd(t_cmd **cmd_lst, char **argv, int s, int e)
{
	t_cmd *c;
	t_cmd *cur;
	int i;

	//printf("dbg: cmd cnt [%d]\n", e - s);
	c = malloc(sizeof(t_cmd));
	c->next = NULL;
	c->delim = SEMI;
	if (argv[e] == NULL)
		c->delim = LAST;
	else if (strcmp("|", argv[e]) == 0)
		c->delim = PIPE;
	c->argv = malloc(sizeof(char *) * (e - s + 1));
	i = 0;
	while (s < e)
		c->argv[i++] = argv[s++];
	c->argv[i] = NULL;
	if (*cmd_lst == NULL)
		*cmd_lst = c;
	else {
		cur = *cmd_lst;
		while (cur->next !=NULL)
			cur = cur->next;
		cur->next = c;
	}
}

void ft_print_str(char *s)
{
	write(1, s, ft_strlen(s));
}

void ft_print_strln(char *s)
{
	ft_print_str(s);
	write(1, "\n", 1);
}

int ft_strlen(char *s)
{
	int	res;

	res = 0;
	while (*s != '\0')
	{
		res++;
		s++;
	}
	return (res);
}

