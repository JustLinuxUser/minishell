/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 18:44:57 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 04:02:31 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H
# include "vec_phandle.h"

typedef struct s_config
{
	int		is_heredoc;
	int		argc;
	char	**argv;
	char	**envp;
}	t_config;

void	error_errno(char *progname);
void	error_custom(char *e);
void	error_errno_subj(char *progname, char *subject);
void	error_errno_subj_exit_code(char *progname, char *subject, int code);
void	error_errno_subj_log(char *progname, char *subject);
char	*exe_path(t_config config, char *exe_name);
int		launch_cmd(t_config config, t_vec_phandle *hndl, char *cmd, int in_fd);
int		heredoc_fd(t_config config);
#endif
