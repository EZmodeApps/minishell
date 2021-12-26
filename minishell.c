/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: prranges <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/12 15:48:07 by prranges          #+#    #+#             */
/*   Updated: 2021/11/12 15:48:09 by prranges         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_sig	g_signals;

void	init_args(t_arg *args)
{
	args->tokens = NULL;
	args->num = 0;
	args->redir = NULL;
	args->env = NULL;
	args->env_str = NULL;
	args->fd = NULL;
	args->home = NULL;
}

void	parcer(char *str, int *num, t_arg *args)
{
	int		s;
	char	**sub_strs;
	int		number_of_parts;
	int		**start_end_i;
	char	**cmd;

	sub_strs = make_substrs_pipe_devided(str, args);
	while (*sub_strs)
	{
		(*num)++;
		number_of_parts = find_number_of_parts(*sub_strs);
		start_end_i = malloc(sizeof(int **));
		if (!(start_end_i))
			my_exit(args, "malloc", 12);
		start_end_i[0] = malloc(sizeof(int *) * number_of_parts);
		if (!(start_end_i[0]))
			my_exit(args, "malloc", 12);
		start_end_i[1] = malloc(sizeof(int *) * number_of_parts);
		if (!(start_end_i[1]))
			my_exit(args, "malloc", 12);
		cmd = malloc(sizeof(char **) * number_of_parts + 1);
		if (!cmd || !start_end_i || !start_end_i[0] || !start_end_i[1])
			my_exit(args, "malloc", 12);
		find_parts_of_str(*sub_strs, start_end_i, args, *num);
		s = -1;
		while (number_of_parts > ++s)
		{
			cmd[s] = ft_substr(sub_strs[0], start_end_i[0][s], start_end_i[1][s] - start_end_i[0][s]);
			if (!(cmd[s]))
				my_exit(args, "malloc", 12);
			cmd[s] = lexe(cmd[s], args,0);
		}
		cmd[s] = NULL;
		args->num = add_token(&args->tokens, cmd, args);
		free(start_end_i[0]);
		free(start_end_i[1]);
		free(start_end_i);
		sub_strs++;
	}
	add_redirs_to_cmd(args->redir, args->tokens);
}

int	start_builtin(t_arg *args, t_token *token)
{
	if (token->builtin == EXPORT)
		return (export_ms(args, token));
	else if (token->builtin == UNSET)
		return (unset_ms(args, token));
	else if (token->builtin == ENV)
		return (env_ms(args->env));
	else if (token->builtin == PWD)
		return (pwd_ms(args));
	else if (token->builtin == ECHO)
		return (echo_ms(args, token));
	else if (token->builtin == CD)
		return (cd_ms(args, token));
	else if (token->builtin == EXIT)
		return (exit_ms(args, token));
	return (0);
}

void free_all(t_arg *args)
{
	delete_all_redirs(args);
	delete_all_tokens(args);
	if (!access("heredoc_file", F_OK))
		unlink("heredoc_file");
}

char	*find_name_ms(char *argv) //exit 12 тк выполняется до создания структуры
{
	int		i;
	char 	*res;

	i = (int)ft_strlen(argv) - 1;
	while (argv[i] != '/' && i)
		i--;
	if (argv[i] == '/')
		i++;
	res = ft_strdup(argv + i);
	if (!res)
		exit(12);
	return (res);
}

int	main(int argc, char **argv, char **arge)
{
	char	*str;
	int		num;
	t_arg	*args;

	(void)argc;
	(void)argv;
	rl_outstream = stderr;
	g_signals.name = find_name_ms(argv[0]);
	if (!(g_signals.name))
		exit(12);
	args = (t_arg *)malloc(sizeof(t_arg));
	if (!args)
		my_exit(args, "malloc", 12);
	init_args(args);
	env_read(args, arge);
	g_signals.exit_status = 0;
	while (1)
	{
		num = 0;
		args->num = 0;
		sig_init();
		signal(SIGINT, &sig_int);
		signal(SIGQUIT, &sig_quit);
		if (!(str = readline("🔷 minishell-0.70$ ")))
		{
			ft_putstr_fd("\033[A🔷 minishell-0.70$ exit\n", 1);
			rl_redisplay();
			exit (0);
		}
		else if (str && str[0])
			add_history(str);
		if (preparcer(str) == 1)
			ft_putstr_fd("minishell: syntax error near unexpected token\n", 2);
		else if (preparcer(str) == 2)
			ft_putstr_fd("minishell: syntax error near unclosed quotes\n", 2);
		else if (!preparcer(str))
			parcer(str, &num, args);
		free(str);
//		print_all_lists(args);
		env_lists_to_str(args);
//		print_double_array(args->env_str);
		precreate_or_preopen(args);
		if (num == 1 && args->tokens->builtin && !args->redir)
			g_signals.exit_status = start_builtin(args, args->tokens);
		else
			pipex(args);
		free_all(args);
	}
	return (0);
}
