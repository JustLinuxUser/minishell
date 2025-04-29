#include "builtins.h"

int	parse_flags(t_vec_str argv, int *n, int *e)
{
	size_t	i;
	size_t	j;
	
	i = 1;
	while (i < argv.len && argv.buff[i][0] == '-' && argv.buff[i][1])
	{
		j = 1;
		while (argv.buff[i][j] && ft_strchr("nEe", argv.buff[i][j]))
			j++;
		if (argv.buff[i][j])
			break ;
		j = 1;
		while (argv.buff[i][j])
		{
			if (argv.buff[i][j] == 'n')
				*n = 1;
			if (argv.buff[i][j] == 'e')
				*e = 1;
			if (argv.buff[i][j] == 'E')
				*e = 0;
			j++;
		}
		i++;
	}
	return (i);
}

int		print_args(int e, t_vec_str argv, size_t i)
{
	while (i < argv.len)
	{
		if (e)
		{
			if (e_parser(argv.buff[i]))
				return(1);
		}
		else
			ft_printf("%s", argv.buff[i]);
		if (++i < argv.len)
			ft_printf(" ");
	}
	return 0;
}
