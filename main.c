#include "./msh.h"

//OK: strcmp, strncmp, dup, dup2, fork, pipe, ,alloc, close, exit
// why we might need strncmp, signall and kill here?

// read (second related to fd leaks):
// check the https://github.com/shackbei/microshell-42
// https://www.thegeekstuff.com/2012/08/lsof-command-examples/


//TODO: implement cd builtin

//TODO: to stderror: if cd has wrong number of args; if cd can't go

//TODO: if any call except execve, and something ends with err - print fatal err
//	and end programm



//TODO: test with lsof and valgrind
//TODO: find way to ensure no fd leaks (watch process tree and proc fs for fds)

//TODO: try to pass norm and make code better and smaller

//TODO: check if other implementations allow execute 'ls'
//TODO: fix relative path

//TODO: optional: check what envp gives to execve and why we need to pass it?


int main(int argc, char **argv, char **envp)
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
	printf("\n");
	exec_cmds(cmd_lst, envp);
	return (0);
}

void usual_cmd(t_cmd *c, char **envp) {
	pid_t cpid;

	dbg_print("[ usual cmd ]\n");
	cpid = fork();
	if (cpid == 0) {
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		waitpid(cpid, NULL, 0);
	}
}

pid_t l_child(t_cmd *c, int l_pipe[2], int r_pipe[2], char **envp) {
	//printf("l [%d][%d], r [%d][%d]\n", l_pipe[0], l_pipe[1], r_pipe[0], r_pipe[1]);
	pid_t cpid;

	dbg_print("[ left child ]\n");
	cpid = fork();
	if (cpid == 0) {
		close(l_pipe[0]);
		close(l_pipe[1]);
		close(r_pipe[0]);
		close(1);
		dup2(r_pipe[1], 1);
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		return (cpid); //need to spawn right side of a pipe, then wait for left side
	}
}

void r_child(t_cmd *c, int l_pipe[2], int r_pipe[2], pid_t l_pid, char **envp) {
	//printf("l [%d][%d], r [%d][%d]\n", l_pipe[0], l_pipe[1], r_pipe[0], r_pipe[1]);
	pid_t cpid;

	dbg_print("[ right child ]\n");
	cpid = fork();
	if (cpid == 0) {
		close(r_pipe[0]);
		close(r_pipe[1]);
		close(l_pipe[1]);
		close(0);
		dup2(l_pipe[0], 0);
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		//printf("left child pid [%d], right child pid [%d]\n", l_pid, cpid);
		waitpid(l_pid, NULL, 0);
	}
	//printf("%p %p %p %d\n", c, l_pipe, r_pipe, l_pid);
}

pid_t m_child(t_cmd *c, int l_pipe[2], int r_pipe[2], pid_t l_pid, char **envp) {
	//printf("l [%d][%d], r [%d][%d]\n", l_pipe[0], l_pipe[1], r_pipe[0], r_pipe[1]);
	pid_t cpid;

	dbg_print("[ middle child ]\n");
	cpid = fork();
	if (cpid == 0) {
		close(l_pipe[1]);
		close(r_pipe[0]);
		close(0);
		close(1);
		dup2(l_pipe[0], 0);
		dup2(r_pipe[1], 1);
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		//printf("left child pid [%d], right child pid [%d]\n", l_pid, cpid);
		waitpid(l_pid, NULL, 0);
		return (cpid);
	}
}

void exec_cmds(t_cmd *l, char **envp)
{
	int	prev_was_pipe = 0;
	pid_t l_pid;
	int l_pipe[2];
	int r_pipe[2];
	pipe(l_pipe);
	pipe(r_pipe);

	dbg_print("\nexecuting..\n");
	while (l != NULL) {
		if (l->delim == PIPE) {
			if (prev_was_pipe == 1) {
				l_pid = m_child(l, l_pipe, r_pipe, l_pid, envp);
			} else {
				l_pid = l_child(l, l_pipe, r_pipe, envp);
			}
			prev_was_pipe = 1;
		} else {
			if (prev_was_pipe == 1) {
				r_child(l, l_pipe, r_pipe, l_pid, envp);
			} else {
				usual_cmd(l, envp);
			}
			prev_was_pipe = 0;
		}
		dbg_print("next..\n");
		l = l->next;
		close(l_pipe[0]);
		close(l_pipe[1]);
		l_pipe[0] = r_pipe[0];
		l_pipe[1] = r_pipe[1];
		pipe(r_pipe);
	}
}

