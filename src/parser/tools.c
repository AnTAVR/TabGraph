/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/19 01:27:17 by gpinchon          #+#    #+#             */
/*   Updated: 2017/03/31 23:16:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char	**split_path(const char *path)
{
	int		i;
	int		j;
	int		pathlen;
	char	**final_path;

	pathlen = ft_strlen(path);
	final_path = (char **)ft_memalloc(sizeof(char *) * 3);
	i = pathlen;
	j = 0;
	while (path[i] != '/' && i > 0)
		i--;
	final_path[0] = (char *)ft_memalloc(sizeof(char) * i + 1);
	final_path[1] = (char *)ft_memalloc(sizeof(char) * (pathlen - i + 1));
	final_path[2] = NULL;
	ft_strncpy(final_path[0], path, i);
	final_path[0] = ft_strjoin(final_path[0], "/");
	while (i < pathlen)
	{
		final_path[1][j] = path[i];
		j++;
		i++;
	}
	return (final_path);
}

unsigned long long hash(unsigned char *str)
{
    unsigned long long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return (hash);
}
