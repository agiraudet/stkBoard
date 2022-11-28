/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gui.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agiraude <agiraude@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 11:16:20 by agiraude          #+#    #+#             */
/*   Updated: 2022/11/28 10:17:09 by agiraude         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Key.hpp"
#include "SDL2/SDL.h"
#include <iostream>
#include <map>
#include <unistd.h>

# define SCWD 400
# define SCHG 150

void	usage(void)
{
	std::cout << std::endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << std::endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << std::endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << std::endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << std::endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << std::endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << std::endl << std::endl;
	std::cout << "PRESS Q TO EXIT" << std::endl;
}


void	initKeyboard(std::map<int, Key> & kb, SDL_Renderer *ren, int fd)
{
	int	whiteSym[] = {SDLK_z, SDLK_x, SDLK_c, SDLK_v, SDLK_b, SDLK_n, SDLK_m, SDLK_COMMA, SDLK_PERIOD, SDLK_SLASH};
	double	whiteFreq[] = {27.5, 30.8677, 32.7032, 36.7081, 41.2034, 43.6535, 48.9994, 55., 61.7354, 65.4064}; 
	int	blackSym[] = {SDLK_s, SDLK_f, SDLK_g, SDLK_j, SDLK_k, SDLK_l};
	double	blackFreq[] = {29.1352, 34.6478, 38.8909, 46.2493, 51.9131, 58.2705};
	int nbWhiteKey = 10;
	int outline = 2;
	int	ii = 0;

	for (int i = 0; i < nbWhiteKey; i++)
	{
		SDL_Rect	wk;
		wk.x = i * (SCWD / nbWhiteKey) + outline / 2;
		wk.y = 0;
		wk.h = SCHG;
		wk.w = (SCWD / nbWhiteKey) - outline;
		Key wKey(K_W, whiteFreq[i], 100., ren, wk, fd);
		kb[whiteSym[i]] = wKey;
		if (i == 1 || i == 3 || i == 4 || i == 6 || i == 7 || i == 8)
		{
			SDL_Rect	bk;
			bk.x = (wk.x - wk.w / 2) + outline / 2;
			bk.y = 0;
			bk.h = SCHG / 2;
			bk.w = wk.w - outline / 2;
			Key bKey(K_B, blackFreq[ii], 100., ren, bk, fd);
			kb[blackSym[ii++]] = bKey;
		}
	}
}

void	renderKeyboard(std::map<int, Key> const & kb, SDL_Renderer *ren)
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 0);
	SDL_RenderClear(ren);
	std::map<int, Key>::const_iterator	it = kb.begin();
	std::map<int, Key>::const_iterator	ite = kb.end();
	for (; it != ite; it++)
	{
		if (it->second.getColor() == K_W)
			it->second.render();
	}
	it = kb.begin();
	for (; it != ite; it++)
	{
		if (it->second.getColor() == K_B)
			it->second.render();
	}
	SDL_RenderPresent(ren);
}

int gui(int fd)
{
	SDL_Event				event;
	SDL_Window				*win;
	SDL_Renderer			*ren;
	std::map<int, Key>		keyboard;
	bool					loop = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL_Init() error" << std::endl;
		exit(1);
	}
	win = SDL_CreateWindow("keyboad", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, SCWD, SCHG, 0);
	ren = SDL_CreateRenderer(win, -1, 0);
	if (!win || !ren)
	{
		SDL_Quit();
		std::cerr << "SDL_Create() error" << std::endl;
		exit(1);
	}
	
	usage();
	initKeyboard(keyboard, ren, fd);
	renderKeyboard(keyboard, ren);

	while (loop)
	{
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
		{
			write(fd, "Exit\n", 5);
			break;
		}
		else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
		{
			int	key = event.key.keysym.sym;
			std::map<int, Key>::iterator	it = keyboard.begin();
			std::map<int, Key>::iterator	ite = keyboard.end();
			for (; it != ite; it++)
			{
				if (it->first != key)
					it->second.press(false);
			}
			if (key == SDLK_q)
				break;
			bool	isPressed = event.type == SDL_KEYDOWN ? true : false;
			if (keyboard.find(key) != keyboard.end())
				keyboard[key].press(isPressed);
			renderKeyboard(keyboard, ren);
		}
	}
	close(fd);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();

	exit(0);
}
