/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Key.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agiraude <agiraude@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 17:33:27 by agiraude          #+#    #+#             */
/*   Updated: 2022/11/28 08:43:52 by agiraude         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEY_HPP
# define KEY_HPP

# include "SDL2/SDL.h"
# include <vector>

# define K_W 1
# define K_B 0

class	Key
{
	public:

		Key(void);
		Key(int color, double freq, double ampl, SDL_Renderer *ren, SDL_Rect const & rect, int fd);
		Key(Key const & src);
		~Key(void);
		
		Key & operator=(Key const & rhs);

		SDL_Rect const &	getRect(void) const;
		void				setRect(int x, int y, int w, int h);
		void				setRect(SDL_Rect const & rect);
		int const &			getColor(void) const;
		void				setColor(int color);
		double const &		getFreq(void) const;
		void				setFreq(double freq);
		double const &		getAmpl(void) const;
		void				setAmpl(double ampl);
		void				setRen(SDL_Renderer *ren);
		void				setFdOut(int fd);
		void				addCoveringKey(Key *key);
		void				press(bool pressed);
		void				render() const;


	private:

		bool				_pressed;
		int					_color;
		double				_freq;
		double				_ampl;
		SDL_Rect			_rect;
		SDL_Renderer*		_ren;
		int					_fdOut;
		std::vector<Key *>	_coveredBy;

		int		_writeNote(bool isPressed) const;
};
#endif
