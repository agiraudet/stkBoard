/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sound.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agiraude <agiraude@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 09:08:09 by agiraude          #+#    #+#             */
/*   Updated: 2022/11/28 09:14:27 by agiraude         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "stk/BeeThree.h"
#include "stk/RtAudio.h"
#include "stk/Messager.h"
#include "stk/SKINImsg.h"
#include <math.h>
#include <algorithm>

// The TickData structure holds all the class instances and data that
// are shared by the various processing functions.
struct	TickData
{
	stk::Instrmnt		*instrument;
	stk::Messager		messager;
	stk::Skini::Message	message;
	int					counter;
	bool				haveMessage;
	bool				done;

	TickData()
	: instrument(NULL), counter(0), haveMessage(false), done(false) {}
};

#define DELTA_CONTROL_TICKS 64 // default sample frames between control input checks

// The processMessage() function encapsulates the handling of control
// messages.  It can be easily relocated within a program structure
// depending on the desired scheduling scheme.
void	processMessage(TickData *data)
{
	stk::StkFloat	value1 = data->message.floatValues[0];
	stk::StkFloat	value2 = data->message.floatValues[1];

	switch (data->message.type)
	{
		case __SK_Exit_:
			data->done = true;
			return;
		case __SK_NoteOn_:
			if (value2 == 0.0)
				data->instrument->noteOff(0.5);
			else
			{
				stk::StkFloat	frequency = 220.0 * pow(2.0, (value1 - 57.0) / 12.0);
				data->instrument->noteOn(frequency, value2 * stk::ONE_OVER_128);
			}
			break;
		case __SK_NoteOff_:
			data->instrument->noteOff(0.5);
			break;
		case __SK_ControlChange_:
			data->instrument->controlChange((int)value1, value2);
			break;
		case __SK_AfterTouch_:
			data->instrument->controlChange(128, value1);
			break;
	}
	data->haveMessage = false;
	return;
}

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		double streamTime, RtAudioStreamStatus status, void *dataPointer)
{
	TickData		*data = (TickData *)dataPointer;
	stk::StkFloat	*samples = (stk::StkFloat *)outputBuffer;
	int				counter, nTicks = (int)nBufferFrames;

	while (nTicks > 0 && !data->done)
	{
		if (!data->haveMessage)
		{
			data->messager.popMessage(data->message);
			if (data->message.type > 0)
			{
				data->counter = (long)(data->message.time * stk::Stk::sampleRate());
				data->haveMessage = true;
			}
			else
				data->counter = DELTA_CONTROL_TICKS;
		}

		counter = std::min(nTicks, data->counter);
		data->counter -= counter;

		for (int i = 0; i < counter; i++)
		{
			*samples++ = data->instrument->tick();
			nTicks--;
		}
		if (nTicks == 0)
			break;

		if (data->haveMessage)
			processMessage(data);
	}
	return 0;
}

int sound(void)
{
	stk::Stk::setSampleRate(44100.);
	stk::Stk::setRawwavePath("rawwaves/");

	TickData	data;
	RtAudio		dac;

	RtAudio::StreamParameters	param;
	param.deviceId = dac.getDefaultOutputDevice();
	param.nChannels = 1;

	RtAudioFormat	format = (sizeof(stk::StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
	unsigned int	bufferFrames = stk::RT_BUFFER_SIZE;


	try
	{
		dac.openStream(&param, NULL, format, (unsigned int)stk::Stk::sampleRate(), &bufferFrames, &tick, (void *)&data);
	}
	catch (RtAudioError & e)
	{
		std::cerr << "openStream error:" << std::endl;
		e.printMessage();
		exit(1);
	}

	try
	{
		data.instrument = new stk::BeeThree();
	}
	catch (stk::StkError &)
	{
		std::cerr << "new Instrmnt error:" << std::endl;
		delete data.instrument;
		exit(1);
	}
	
	if (data.messager.startStdInput() == false)
	{
		std::cerr << "startStcIpnut error:" << std::endl;
		delete data.instrument;
		exit(1);
	}
	
	try
	{
		dac.startStream();
	}
	catch (RtAudioError & e)
	{
		std::cerr << "startStream error:" << std::endl;
		e.printMessage();
		delete data.instrument;
		exit(1);
	}

	while(!data.done)
	{
		stk::Stk::sleep(200);
	}

	try
	{
		dac.closeStream();
	}
	catch (RtAudioError & e)
	{
		std::cerr << "closeStream error:" << std::endl;
		e.printMessage();
		delete data.instrument;
		exit(1);
	}

	delete data.instrument;

	return 0;
}
