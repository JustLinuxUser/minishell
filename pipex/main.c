/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 02:37:16 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 18:22:50 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include "pipex.h"
#include "../libft/libft.h"

// -1 read error
// -2 write error
int	write_fd_to_fd(int src_fd, int dest_fd)
{
	int		size;
	int		total_size;
	char	buff[1024];
	int		status;

	total_size = 0;
	status = 0;
	while (1)
	{
		size = read(src_fd, buff, 1024);
		if (size < 0)
			status = 1;
		if (size <= 0)
			break ;
		total_size += size;
		size = write(dest_fd, buff, size);
		status = size < 0;
		if (size <= 0)
			break ;
	}
	close(dest_fd);
	close(src_fd);
	if (status)
		return (status);
	return (total_size);
}

// -1 eroror
// >= 0 read fd
int	get_infile_fd(t_config config, char *fname)
{
	int	fd;

	if (access(fname, R_OK))
	{
		error_errno_subj_log(config.argv[0], fname);
		fname = "/dev/null";
	}
	fd = open(fname, O_RDONLY);
	if (fd < 0)
		error_errno_subj(fname, config.argv[0]);
	return (fd);
}

int	open_outfile(t_config config)
{
	int	ret;

	if (config.is_heredoc)
		ret = open(config.argv[config.argc - 1],
				O_WRONLY | O_APPEND | O_CREAT, 0777);
	else
		ret = open(config.argv[config.argc - 1],
				O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (ret < 0)
		error_errno_subj(config.argv[0], config.argv[config.argc - 1]);
	return (ret);
}

int	cmds(t_config config, t_vec_phandle handles, int cmd_stdout, int outfile)
{
	t_phandle	temp;
	int			status;
	size_t		i;

	i = 1;
	if (config.is_heredoc)
		i++;

	while ((int)++i < config.argc - 1)
		cmd_stdout = launch_cmd(config, &handles, config.argv[i], cmd_stdout);

	write_fd_to_fd(cmd_stdout, outfile);

	i = 0;
	status = 0;
	while (i < handles.len)
	{
		temp = handles.buff[i];
		if (waitpid(temp.pid, &status, 0) == -1)
			error_errno(config.argv[0]);
		free(temp.args);
		free(temp.exe_path);
		i++;
	}
	free(handles.buff);
	if (config.is_heredoc)
		unlink("/tmp/heredoc_tmp");
	return (((status) & 0xff00) >> 8);
}

int	main(int argc, char *argv[], char *envp[])
{
	t_config		config;
	int				outfile;
	t_vec_phandle	handles;
	int				file_pipe;
	int				status;

	config = (t_config){.argc = argc, .argv = argv, .envp = envp};
	if (argc < 5 || (argc < 6 && ft_strcmp(argv[1], "here_doc") == 0))
	{
		ft_eprintf("USAGE:\n    %s <infile> <cmd1> "
			"<cmd2> [cmd_n]... <outfile>\n    %s here_doc <separator> "
			"<cmd1> <cmd2> [cmd_n]... <outfile>\n", argv[0], argv[0]);
		return (1);
	}

	vec_phandle_init(&handles);
	if (ft_strcmp(argv[1], "here_doc") == 0)
		file_pipe = get_infile_fd(config, argv[1]);
	else
		file_pipe = heredoc_fd(config);
	outfile = open_outfile(config);
	status = cmds(config, handles, file_pipe, outfile);
	return (status);
}
