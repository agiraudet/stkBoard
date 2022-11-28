/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agiraude <agiraude@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 09:10:07 by agiraude          #+#    #+#             */
/*   Updated: 2022/11/28 10:09:19 by agiraude         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "stkboard.hpp"
#include <unistd.h>

int main(void)
{
	int		pfd[2];

	pipe(pfd);
	pid_t	pid = fork();
	if (pid < 0)
	{
		return 2;
	}
	else if (pid == 0)
	{
		close(pfd[0]);
		gui(pfd[1]);
	}
	else
	{
		close(pfd[1]);
		dup2(pfd[0], STDIN_FILENO);
		sound();
	}
	return 0;
}
