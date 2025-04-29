#include "builtins.h"

int mini_atoi_base(char **str, int base, int len)
{
	int i;
	int res;
	char *digits;
	char *p;
	
	i = 0;
	res = 0;
	digits = "0123456789ABCDEF";

	while (**str && i < len)
	{
		p = ft_strchr(digits, ft_toupper(**str));
		if (!p || (p - digits) >= base)
			break;
		res = res * base + (p - digits);
		i++;
		(*str)++;
	}
	return (res);
}

void parse_numeric_escape(char **str)
{
	int base;
	int len; 
	unsigned char c;

	base = 10;
	len = 0;
	if (**str == '0')   
	{
		base = 8;
		len = 3;
	}
	else if (**str == 'x') 
	{
		base = 16;
		len = 2; 
	}
	else 
		return;
	(*str)++;
	c = mini_atoi_base(str, base, len);
	write(1, &c, 1);
}

static void backslash_writer(char *s)
{
	if	(*s == 'n')
		write(1, "\n", 1);
	else if (*s == 't')
		write(1, "\t", 1);
	else if (*s == 'a')
		write(1, "\a", 1);
	else if (*s == 'b')
		write(1, "\b", 1);
	else if (*s == 'f')
		write(1, "\f", 1);
	else if (*s == 'r')
		write(1, "\r", 1);
	else if (*s == 'v')
		write(1, "\v", 1);
	else if (*s == '\\')
		write(1, "\\", 1);
	else if (*s == 'e')
		write(1, "\033", 1);
}


int e_parser(char *s)
{
	while (*s)
	{
		if (*s == '\\' && s[1])
		{
			s++;
			backslash_writer(s);
			if (*s == 'c')
				return(1);
			else if (*s == '0' || *s == 'x')
			{
				parse_numeric_escape(&s);
				continue;
			}
			else 
			{
				write(1, "\\", 1);
				write(1, s, 1); 
			}
			s++;
		}
		else
			write(1, s++, 1);
	}
	return(0);
}
