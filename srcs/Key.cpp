/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Key.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agiraude <agiraude@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 17:43:35 by agiraude          #+#    #+#             */
/*   Updated: 2022/11/28 09:52:17 by agiraude         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Key.hpp"
#include <sstream>
#include <unistd.h>
#include <iostream>

Key::Key(void)
: _pressed(false), _color(K_W), _freq(0.), _ampl(0.), _ren(NULL), _fdOut(STDIN_FILENO)
{
	this->setRect(0., 0., 0., 0.);
}

Key::Key(int color, double freq, double ampl, SDL_Renderer *ren, SDL_Rect const & rect, int fd)
: _pressed(false), _color(color), _freq(freq), _ampl(ampl), _ren(ren), _fdOut(fd)
{
	this->setRect(rect);
}

Key::Key(Key const & src)
{
	*this = src;
}

Key::~Key(void)
{
}

Key & Key::operator=(Key const & rhs)
{
	if (this == &rhs)
		return *this;
	this->_pressed = rhs._pressed;
	this->_color = rhs._color;
	this->_freq = rhs._freq;
	this->_ampl = rhs._ampl;
	this->setRect(rhs.getRect());
	this->_ren = rhs._ren;
	this->_fdOut = rhs._fdOut;
	return *this;
}

SDL_Rect const &	Key::getRect(void) const
{
	return this->_rect;
}

void	Key::setRect(int x, int y, int w, int h)
{
	this->_rect.x = x;
	this->_rect.y = y;
	this->_rect.h = h;
	this->_rect.w = w;
}

void	Key::setRect(SDL_Rect const & rect)
{
	this->_rect.x = rect.x;
	this->_rect.y = rect.y;
	this->_rect.h = rect.h;
	this->_rect.w = rect.w;
}

int const &	Key::getColor(void) const
{
	return this->_color;
}

void	Key::setColor(int color)
{
	this->_color = color;
}

double const &	Key::getFreq(void) const
{
	return this->_freq;
}

void	Key::setFreq(double freq)
{
	this->_freq = freq;
}

double const &	Key::getAmpl(void) const
{
	return this->_ampl;
}

void	Key::setAmpl(double ampl)
{
	this->_ampl = ampl;
}

void	Key::setRen(SDL_Renderer *ren)
{
	this->_ren = ren;
}

void	Key::setFdOut(int fd)
{
	this->_fdOut = fd;
}

void	Key::addCoveringKey(Key *key)
{
	if (!key)
		return;
	this->_coveredBy.push_back(key);
}

void	Key::press(bool isPressed)
{
	this->_writeNote(isPressed);
	this->_pressed = isPressed;
	this->render();
}

int	Key::_writeNote(bool isPressed) const
{
	if (this->_pressed == isPressed)
		return 0;
	std::string			msg;
	std::stringstream	ss;

	ss << (isPressed ? "NoteOn" : "NoteOff");
	ss <<  msg << " 0.0 1 " << this->_freq << " " << this->_ampl << "\n";
	msg = ss.str();
	return (write(this->_fdOut, msg.c_str(), msg.size()));
}

void	Key::render(void) const
{
	if (this->_pressed)
		SDL_SetRenderDrawColor(this->_ren, 255, 0, 0 ,0);
	else if (this->_color == K_W)
		SDL_SetRenderDrawColor(this->_ren, 255, 255, 255 ,0);
	else
		SDL_SetRenderDrawColor(this->_ren, 0, 0, 0 ,0);

	SDL_RenderFillRect(this->_ren, &(this->_rect));
}

