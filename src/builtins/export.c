#include "builtins.h"

void parse_export_arg(char *str, char **ident, char **val)
{
	char *eq = ft_strchr(str, '=');
	if (eq)
	{	*ident = ft_strndup(str, eq - str);
		*val   = ft_strdup(eq + 1); }
	else
	{	*ident = ft_strdup(str); *val = NULL; }
}

static bool is_valid_ident(char *id)
{
	int i = 0;
	if (!is_var_name_p1(id[0])) return false;
	while (id[i] && is_var_name_p2(id[i])) i++;
	return !id[i];
}

int	builtin_export(t_state *st, t_vec_str av)
{
	size_t i = 1; int status = 0;
	if (av.len == 1) return builtin_env(st, av);

	while (i < av.len)
	{
		char *id, *val; parse_export_arg(av.buff[i], &id, &val);
		if (is_valid_ident(id))
		{
			if (!val)
			{	t_env *e = env_get(&st->env, id);
				if (e) e->exported = true;
				free(id); }
			else
				env_set(&st->env, (t_env){true, id, val});
		}
		else
		{	ft_eprintf("%s: %s: `%s' not valid identifier\n",
				st->context, av.buff[0], av.buff[i]);
			free(id); free(val); status = 1; }
		i++;
	}
	return status;
}
