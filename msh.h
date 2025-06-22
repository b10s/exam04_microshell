#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE 1
#define SEMI 2
#define LAST 3

#define DBG 1

typedef struct cmd {
	char	**argv;
	int	delim;
	struct cmd *next;
} t_cmd;

void add_cmd(t_cmd **cmd_lst, char **argv, int s, int e);
void exec_cmds(t_cmd *l, char **envp);
void usual_cmd(t_cmd *c, char **envp);
pid_t l_child(t_cmd *c, int l_pipe[2], int r_pipe[2], char **envp);
pid_t m_child(t_cmd *c, int l_pipe[2], int r_pipe[2], pid_t l_pid, char **envp);
void r_child(t_cmd *c, int l_pipe[2], int r_pipe[2], pid_t l_pid, char **envp);
void cd_builtin(t_cmd *c);
void print_cmds(t_cmd *cmd_lst);
void dbg_print(char *s);
void print_err_execve(char *s);
void print_err_cd_wrong_args();
void print_err_cd_failed(char *s);
void print_fatal_exit();
int ft_strlen(char *s);

