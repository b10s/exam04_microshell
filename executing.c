#include "./msh.h"

void exec_cmds(t_cmd *l, char **envp)
{
	int	prev_was_pipe = 0;
	pid_t l_pid;
	int l_pipe[2];
	int r_pipe[2];
	if (pipe(l_pipe) == -1)
		print_fatal_exit();
	if (pipe(r_pipe) == -1)
		print_fatal_exit();

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
		if (pipe(r_pipe) == -1)
			print_fatal_exit();
	}
}

void usual_cmd(t_cmd *c, char **envp) {
	pid_t cpid;

	dbg_print("[ usual cmd ]\n");
	if (strcmp(c->argv[0], "cd") == 0) {
		cd_builtin(c);
		return ;
	}
	cpid = fork();
	if (cpid == -1)
		print_fatal_exit();
	if (cpid == 0) {
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		if (waitpid(cpid, NULL, 0) == -1)
			print_fatal_exit();
	}
}

pid_t l_child(t_cmd *c, int l_pipe[2], int r_pipe[2], char **envp) {
	//printf("l [%d][%d], r [%d][%d]\n", l_pipe[0], l_pipe[1], r_pipe[0], r_pipe[1]);
	pid_t cpid;

	dbg_print("[ left child ]\n");
	cpid = fork();
	if (cpid == -1)
		print_fatal_exit();
	if (cpid == 0) {
		close(l_pipe[0]);
		close(l_pipe[1]);
		close(r_pipe[0]);
		close(1);
		if (dup2(r_pipe[1], 1) == -1)
			print_fatal_exit();
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		return (cpid);
	}
}

void r_child(t_cmd *c, int l_pipe[2], int r_pipe[2], pid_t l_pid, char **envp) {
	//printf("l [%d][%d], r [%d][%d]\n", l_pipe[0], l_pipe[1], r_pipe[0], r_pipe[1]);
	pid_t cpid;

	dbg_print("[ right child ]\n");
	cpid = fork();
	if (cpid == -1)
		print_fatal_exit();
	if (cpid == 0) {
		close(r_pipe[0]);
		close(r_pipe[1]);
		close(l_pipe[1]);
		close(0);
		if (dup2(l_pipe[0], 0) == -1)
			print_fatal_exit();
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		//printf("left child pid [%d], right child pid [%d]\n", l_pid, cpid);
		if (waitpid(l_pid, NULL, 0) == -1)
			print_fatal_exit();
		close(l_pipe[1]);
		if (waitpid(cpid, NULL, 0) == -1)
			print_fatal_exit();
	}
}

pid_t m_child(t_cmd *c, int l_pipe[2], int r_pipe[2], pid_t l_pid, char **envp) {
	//printf("l [%d][%d], r [%d][%d]\n", l_pipe[0], l_pipe[1], r_pipe[0], r_pipe[1]);
	pid_t cpid;

	dbg_print("[ middle child ]\n");
	cpid = fork();
	if (cpid == -1)
		print_fatal_exit();
	if (cpid == 0) {
		close(l_pipe[1]);
		close(r_pipe[0]);
		close(0);
		close(1);
		if (dup2(l_pipe[0], 0) == -1)
			print_fatal_exit();
		if (dup2(r_pipe[1], 1) == -1)
			print_fatal_exit();
		if (execve(c->argv[0], c->argv, envp) == -1)
			print_err_execve(c->argv[0]);
		exit(0);
	} else {
		//printf("left child pid [%d], right child pid [%d]\n", l_pid, cpid);
		if (waitpid(l_pid, NULL, 0) == -1)
			print_fatal_exit();
		return (cpid);
	}
}

void cd_builtin(t_cmd *c) {
	int res;

	res = 0;
	if (c->argv[1] != NULL && c->argv[2] == NULL) {
		res = chdir(c->argv[1]);
	} else {
		print_err_cd_wrong_args();
		return ;
	}
	if (res == -1)
		print_err_cd_failed(c->argv[1]);
}

