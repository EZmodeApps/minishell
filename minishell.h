/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prranges <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/12 15:48:20 by prranges          #+#    #+#             */
/*   Updated: 2021/11/12 15:48:22 by prranges         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "./libft/libft.h"
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <stdlib.h>
# include <signal.h>
# include <fcntl.h>
# include <errno.h>

# define EXPORT		1
# define UNSET		2
# define ENV		3
# define PWD		4
# define ECHO		5
# define CD			6
# define EXIT		7

typedef struct s_sig
{
	int				exit_status;
	pid_t			*pid;
	char			*name;
}				t_sig;

typedef struct s_redir
{
	char			*file_name;
	int				cmd_list_num;
	int				out_in;
	int				dbl;
	struct s_redir	*next;
}				t_redir;

typedef struct s_token
{
	int				list_num;
	char			**cmd;
	t_redir			*in;
	t_redir			*out;
	int				builtin;
	struct s_token	*next;
}				t_token;

typedef struct s_env
{
	char			*key;
	char			separator;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_lexer
{
	int		j;
	int		e;
	char	*key;
	char	*before;
	char	*in;
	char	*after;
}				t_lexer;

typedef struct s_arg
{
	t_token	*tokens;
	int		num;
	t_redir	*redir;
	t_env	*env;
	char	**env_str;
	int		**fd;
	char	*home;
	int		st;
	int		ed;
	int		flag_sq;
	int		flag_dq;
}				t_arg;

extern t_sig	g_signals;

void	rl_replace_line(const char *buffer, int val);
int		preparcer(char *str);
t_token	*init_tokens(t_arg *args);
void	add_env(t_env **env, char *str, t_arg *data);
t_env	*init_env(t_arg *args);
void	env_read(t_arg *arg, char **arge);
int		add_token(t_token **lst, char **str, t_arg *args);
void	signals_ms(int i);
void	init_lexer(t_lexer *lex);
void	free_lexer(t_lexer *lex);
char	*lexe(char *str, t_arg *args, int hdoc);
char	*dollar(char *str, int *i, t_arg *args, t_lexer *lex);
void	parcer(char *str, t_arg *args);
void	print_all_lists(t_arg *args);
void	print_env(t_env *lst);
void	delete_all_tokens(t_arg *args);
int		execute(t_token *lst);
int		free_double_array(char **str);
void	print_double_array(char **sub_strs);
char	**make_substrs_pipe_devided(char *str, t_arg *args);
int		redirect(char *str, int i, t_arg *args, int num);
t_redir	*init_redir(t_arg *args);
void	delete_all_redirs(t_arg *args);
t_redir	*last_redir(t_redir *redir);
void	add_redirs_to_cmd(t_redir *redir, t_token *tokens);
int		find_number_of_parts(t_arg *args, char *s);
void	find_parts_of_str(char *s, int **start_end_i, t_arg *args, int num);
int		env_ms(t_env *env);
int		export_ms(t_arg *args, t_token *token);
void	edit_env(t_env **env, char *str, t_arg *data);
int		if_key(char c);
void	remove_env(t_arg *args, t_env *remove_list, t_env *prev_p);
int		unset_ms(t_arg *args, t_token *token);
int		pwd_ms(t_arg *args);
int		echo_ms(t_arg *args, t_token *token);
int		cd_ms(t_arg *args, t_token *token);
int		exit_ms(t_arg *args, t_token *token);
int		start_builtin(t_arg *args, t_token *token);
int		make_builtin_dup(t_token *token, t_arg *data);
void	builtin_dup_error_check(int fd, t_arg *data);
int		precreate_or_preopen(t_arg *data);
void	heredoc(char *file_name, t_arg *data);
void	env_lists_to_str(t_arg *args);
void	exec_start(t_arg *data, t_token *token);
char	*find_name_ms(char *argv);
void	free_all(t_arg *args);
void	set_quotes_flag(t_arg *args, char *str, int i);
void	delete_all_env(t_arg *args);

void	sig_init(void);
void	sig_int(int signal);
void	sig_quit(int signal);

void	my_exit(t_arg *data, char *text, int errnum, int flag);

int		pipex(t_arg *data);
void	exec_start(t_arg *data, t_token *token);
void	dup2_builtin_close_error(int file, int fd, t_arg *data);
int		make_builtin_dup(t_token *token, t_arg *data);
void	builtin_dup_error_check(int fd, t_arg *data);
int		create_file_0(t_token *token, t_arg *data);
int		create_file_1(t_token *token, t_arg *data);
int		child_process(int i, t_arg *data, int **fd, t_token *token);
void	heredoc(char *name, t_arg *data);
void	heredoc_cycle(char *name, char *line, int fd, t_arg *data);
char	**find_path(t_env *env);
char	*create_cmd_path(t_arg *data, char **all_paths, char *cmd);
char	*get_cmd_arg(t_arg *data, char **cmd);
void	check_fd_exist(int fd, char *str, t_arg *data);
void	check_minishell_in_minishell(char *cmd);
void	close_fds(t_arg *data, int **fd, int *file);
void	check_fd_exist(int fd, char *str, t_arg *data);
void	create_fd(t_arg *data);
void	create_pipe_fd(t_arg *data);
void	waitpid_pipex(t_arg *data);
int		precreate_or_preopen(t_arg *data);
void	redirect_dbl(t_redir *redirect, t_arg *data);
int		open_file(t_redir *redirect, t_arg *data);

#endif
