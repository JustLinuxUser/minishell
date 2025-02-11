#include "libft/libft.h"
#include "minishell.h"
#include "vec_env.h"

t_env str_to_env(char* str) {
    t_env ret;
    char* key_pos;

    key_pos = ft_strchr(str, '=') + 1;

    ret.exported = true;
    ret.key = malloc(key_pos - str);
    ft_strlcpy(ret.key, str, key_pos - str);
    ret.value = malloc(ft_strlen(key_pos) + 1);
    ft_strlcpy(ret.value, key_pos, ft_strlen(key_pos) + 1);
    return (ret);
}

t_vec_env env_to_vec_env(char** envp) {
    t_vec_env ret;

    vec_env_init(&ret);
    while (*envp) {
        vec_env_push(&ret, str_to_env(*envp));
        envp++;
    }
    return (ret);
}

t_env* env_get(t_vec_env* env, char* key) {
    t_env* curr;
    int i;

    i = env->len - 1;
    while (i >= 0) {
        curr = vec_env_idx(env, i);
        if (ft_strcmp(key, curr->key) == 0)
            return (curr);
        i--;
    }
    return (0);
}

void free_env(t_vec_env *env)
{
	size_t	i;
	t_env	curr;

	i = 0;
	while (i < env->len)
	{
		curr = env->buff[i];
		free(curr.key);
		free(curr.value);
		i++;
	}
	free(env->buff);
	env->buff = 0;
}
