#include "libft/libft.h"
#include <stdlib.h>
#include <unistd.h>

void free_tab(char** tab) {
    int i;
    i = 0;

    while (tab[i])
        free(tab[i++]);
    free(tab);
}

// 0 on success
int write_to_file(char *str, int fd)
{
	int	wrote_total;
	int	wrote;
	int	len;

	len = ft_strlen(str);
	wrote_total = 0;
	while(wrote_total != len)
	{
		wrote = write(fd, str + wrote_total, len - wrote_total);
		if (wrote < 0)
			return (1);
		wrote_total += wrote;
	}
	return 0;
}
