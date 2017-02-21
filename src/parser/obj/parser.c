/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2017/02/21 20:30:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <fcntl.h>

typedef struct		s_obj_parser
{
	char			**path_split;
	int				fd;
	t_engine		*e;
	t_mesh			mesh;
	ARRAY			v;
	ARRAY			vn;
	ARRAY			vt;
	ARRAY			mtllib;
	t_vgroup		vg;
}					t_obj_parser;

t_vgroup	new_vgroup()
{
	t_vgroup	vg;

	ft_memset(&vg, 0, sizeof(t_vgroup));
	vg.v = new_ezarray(other, 0, sizeof(VEC3));
	vg.vn = new_ezarray(other, 0, sizeof(VEC3));
	vg.vt = new_ezarray(other, 0, sizeof(VEC2));
	vg.vindex = new_ezarray(other, 0, sizeof(int));
	return (vg);
}

t_mesh	new_mesh()
{
	t_mesh			m;

	ft_memset(&m, 0, sizeof(t_mesh));
	m.vgroups = new_ezarray(other, 0, sizeof(t_vgroup));
	return (m);
}

VEC3	parse_vec3(char **split)
{
	float	v[3];
	int		i;

	i = 0;
	ft_memset(v, 0, sizeof(float));
	while (split[i] && i < 3)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec3(v[0], v[1], v[2]));
}

VEC2	parse_vec2(char **split)
{
	float	v[2];
	int		i;

	i = 0;
	ft_memset(v, 0, sizeof(float));
	while (split[i] && i < 2)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec2(v[0], v[1]));
}

void	parse_vtn(t_obj_parser *p, char **split)
{
	VEC3			v;
	VEC2			vn;

	if (!ft_strcmp(split[0], "v"))
	{
		v = parse_vec3(&split[1]);
		ezarray_push(&p->v, &v);
	}
	else if (!ft_strcmp(split[0], "vn"))
	{
		v = parse_vec3(&split[1]);
		ezarray_push(&p->vn, &v);
	}
	else if (!ft_strcmp(split[0], "vt"))
	{
		vn = parse_vec2(&split[1]);
		ezarray_push(&p->vt, &vn);
	}
}

void	parse_vg(t_obj_parser *p, char **split)
{
	if (p->vg.v.length > 0)
	{
		ezarray_push(&p->mesh.vgroups, &p->vg);
		p->vg = new_vgroup();
	}
	(void)split;
}

int		ft_chartablen(char **s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	vindex_in_array(ARRAY a, int index)
{
	int	i;
	int	*value;

	i = 0;
	while ((value = ezarray_get_index(a, i)))
	{
		if (*value == index)
			return (1);
		i++;
	}
	return (0);
}

void	parse_v(t_obj_parser *p, char **split)
{
	VEC3	v;
	VEC3	vn;
	VEC2	vt;
	int		i;
	int		vindex;
	int		tablen;
	int		slash;
	char	**fsplit;

	i = 0;
	while (split[i])
	{
		fsplit = ft_strsplit(split[i], '/');
		slash = count_char(split[i], '/');
		tablen = ft_chartablen(fsplit);
		vindex = ft_atoi(fsplit[0]);
		v = *((VEC3*)ezarray_get_index(p->v, vindex - 1));
		if (tablen == 3 && slash == 2)
		{
			vt = *((VEC2*)ezarray_get_index(p->vt, ft_atoi(fsplit[1]) - 1));
			vn = *((VEC3*)ezarray_get_index(p->vn, ft_atoi(fsplit[2]) - 1));
		}
		else if (tablen == 2 && slash == 2)
		{
			vn = *((VEC3*)ezarray_get_index(p->vn, ft_atoi(fsplit[1]) - 1));
			vt = new_vec2(0, 0);
		}
		else if (tablen == 2 && slash == 1)
		{
			vn = new_vec3(0, 0, 0);
			vt = *((VEC2*)ezarray_get_index(p->vt, ft_atoi(fsplit[1]) - 1));
		}
		if (tablen == 1 && !vindex_in_array(p->vg.vindex, vindex))
		{
			ezarray_push(&p->vg.v, &v);
			v = new_vec3(0, 0, 0);
			ezarray_push(&p->vg.vn, &v);
			ezarray_push(&p->vg.vt, &v);
		}
		else if (tablen > 1)
		{
			ezarray_push(&p->vg.v, &v);
			ezarray_push(&p->vg.vt, &vt);
			ezarray_push(&p->vg.vn, &vn);
			vindex = p->vg.v.length;
		}
		ezarray_push(&p->vg.vindex, &vindex);
		ft_free_chartab(fsplit);
		i++;
	}
}

void	parse_f(t_obj_parser *p, char **split)
{
	int		chartablen;

	chartablen = ft_chartablen(split);
	if (chartablen >= 3)
		parse_v(p, (char*[4]){split[0], split[1], split[2], NULL});
	if (chartablen == 4)
		parse_v(p, (char*[4]){split[0], split[2], split[3], NULL});
}

void	get_material(t_obj_parser *p, char **split)
{
	p->vg.mtl_id = hash((unsigned char *)split[0]);
	p->vg.mtl_index = get_material_index_by_id(p->e->materials, p->vg.mtl_id);
}

int	start_obj_parsing(t_obj_parser *p, char *path)
{
	char	**split;
	char	*line;
	int		fd;

	if (access(path, F_OK | W_OK))
		return (-1);
	//
	p->path_split = split_path(path);
	p->fd = open(path, O_RDONLY);
	p->mesh = new_mesh();
	p->vg = new_vgroup();
	p->v = new_ezarray(other, 0, sizeof(VEC3));
	p->vn = new_ezarray(other, 0, sizeof(VEC3));
	p->vt = new_ezarray(other, 0, sizeof(VEC3));
	fd = p->fd;
	while (get_next_line(fd, &line))
	{
		split = ft_strsplitwspace((const char *)line);
		if (split && split[0] && split[0][0] != '#')
		{
			if (split[0][0] == 'v')
				parse_vtn(p, split);
			else if (split[0][0] == 'f')
				parse_f(p, &split[1]);
			else if (split[0][0] == 'g'
			|| split[0][0] == 'o'
			|| !ft_strcmp(split[0], "usemtl"))
				parse_vg(p, split);
			if (!ft_strcmp(split[0], "usemtl"))
				get_material(p, &split[1]);
		}
		ft_free_chartab(split);
		free(line);
	}
	ft_free_chartab(p->path_split);
	destroy_ezarray(&p->v);
	destroy_ezarray(&p->vn);
	destroy_ezarray(&p->vt);
	return (0);
}

int	get_mtllib(t_obj_parser *p, char *path)
{
	char	**split;
	ARRAY	mtl_pathes;
	char	*line;
	int		fd;
	unsigned int i;
	STRING	s;

	if (access(path, F_OK | W_OK))
		return (-1);
	fd = open(path, O_RDONLY);
	mtl_pathes = new_ezarray(other, 0, sizeof(STRING));
	while (get_next_line(fd, &line))
	{
		split = ft_strsplitwspace((const char *)line);
		if (split && split[0] && split[0][0] != '#')
		{
			if (!ft_strcmp(split[0], "mtllib"))
			{
				s = new_ezstring(split[1]);
				ezarray_push(&mtl_pathes, &s);
			}
		}
		ft_free_chartab(split);
		free(line);
	}
	close(fd);
	i = 0;
	while (i < mtl_pathes.length)
	{
		s = *((STRING *)ezarray_get_index(mtl_pathes, i));
		printf("mtllib : %s\n", s.tostring);
		load_mtllib(p->e, ft_strjoin(p->path_split[0], s.tostring));
		destroy_ezstring(&s);
		i++;
	}
	ft_free_chartab(p->path_split);
	return (0);
}

int	load_obj(t_engine *engine, char *path)
{
	t_obj_parser	p;
	TRANSFORM		t;

	ft_memset(&p, 0, sizeof(t_obj_parser));
	p.e = engine;
	p.path_split = split_path(path);
	get_mtllib(&p, path);
	if (start_obj_parsing(&p, path))
		return (-1);
	if (!p.mesh.vgroups.length && p.vg.v.length)
		ezarray_push(&p.mesh.vgroups, &p.vg);
	p.mesh.transform_index = create_transform(e, new_vec3(0, 0, 0),
		new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	ezarray_push(&engine->meshes, &p.mesh);
	return (engine->meshes.length);
}