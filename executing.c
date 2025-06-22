#include "./msh.h"

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

