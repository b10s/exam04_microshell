#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE 1
#define SEMI 2
#define LAST 3

typedef struct cmd {
	char	**argv;
	int	delim;
	struct cmd *next;
} t_cmd;

void add_cmd(t_cmd **cmd_lst, char **argv, int s, int e);
int ft_strlen(char *s);
void ft_print_str(char *s);
void ft_print_strln(char *s);
void print_cmds(t_cmd *cmd_lst);
void exec_cmds(t_cmd *l);

