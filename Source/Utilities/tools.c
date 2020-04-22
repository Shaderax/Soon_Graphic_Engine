/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nrouzeva <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/12/01 16:08:46 by nrouzeva          #+#    #+#             */
/*   Updated: 2018/12/04 17:14:58 by nrouzeva         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.h"

void	print_mat(char *name, t_mat4 mat)
{
	printf("\n%s : \n", name);
	printf("%f, %f, %f, %f\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
	printf("%f, %f, %f, %f\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
	printf("%f, %f, %f, %f\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
	printf("%f, %f, %f, %f\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
}

int		tab_len(char **tab)
{
	int index;

	index = -1;
	while (tab[++index])
		;
	return (index);
}

int		count_char(char *line, char c)
{
	int count;
	int x;

	x = -1;
	count = 0;
	while (line[++x])
		if (line[x] == c)
			count++;
	return (count);
}

void	show_fps(double *lastime, int *nb_frame)
{
	double	currenttime;

	currenttime = glfwGetTime();
	*nb_frame = *nb_frame + 1;;
	if (currenttime - *lastime >= 1.0)
	{
		printf("%d fps\n", *nb_frame);
		*nb_frame = 0;
		*lastime = glfwGetTime();
	}
}
