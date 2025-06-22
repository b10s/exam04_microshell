#include "./msh.h"

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

void add_cmd(t_cmd **cmd_lst, char **argv, int s, int e)
{
	t_cmd *c;
	t_cmd *cur;
	int i;

	//printf("dbg: cmd cnt [%d]\n", e - s);
	c = malloc(sizeof(t_cmd));
	if (c == NULL)
		print_fatal_exit();
	c->next = NULL;
	c->delim = SEMI;
	if (argv[e] == NULL)
		c->delim = LAST;
	else if (strcmp("|", argv[e]) == 0)
		c->delim = PIPE;
	c->argv = malloc(sizeof(char *) * (e - s + 1));
	if (c->argv == NULL)
		print_fatal_exit();
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

