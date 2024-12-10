/* This is a modified variant of the mixer code
 * to make it possible to autogenerate mixers with any size
 * by using the Design Tool++ by Manicksan (Jannik Svensson)
 * 
 * Original copyright note:
 * Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef mixers_h_
#define mixers_h_

#include "Arduino.h"
#include "AudioStream.h"

#if defined(__ARM_ARCH_7EM__)
#define MULTI_UNITYGAIN 65536
#define MIXERS_MULT_TYPE int32_t
#elif defined(KINETISL)
#define MULTI_UNITYGAIN 256
#define MIXERS_MULT_TYPE int16_t
#endif


class AudioMixer11 : public AudioStream
{
public:
	AudioMixer11(void) : AudioStream(11, inputQueueArray) {
		for (int i=0; i<11; i++) multiplier[i] = MULTI_UNITYGAIN;
	}
	virtual void update(void);
	void gain(unsigned int channel, float gain) {
		if (channel > (11-1))
            channel = (11-1);
		if (gain > ((MULTI_UNITYGAIN-2)/2))
            gain = ((MULTI_UNITYGAIN-2)/2);
		else if (gain < (float)((MULTI_UNITYGAIN-2)/(-2)))
            gain = (float)((MULTI_UNITYGAIN-2)/(-2));
		multiplier[channel] = gain * (float)MULTI_UNITYGAIN; // TODO: proper roundoff?
	}
    /** sets gain for all channels */
	void gain(float gain) {
	    if (gain > ((MULTI_UNITYGAIN-2)/2))
            gain = ((MULTI_UNITYGAIN-2)/2);
		else if (gain < (float)((MULTI_UNITYGAIN-2)/(-2)))
            gain = (float)((MULTI_UNITYGAIN-2)/(-2));
		for (int i=0; i<11; i++) {
            multiplier[i] = gain * (float)MULTI_UNITYGAIN; // TODO: proper roundoff?
        } 
	}
private:
	MIXERS_MULT_TYPE multiplier[11];
	audio_block_t *inputQueueArray[11];
};

#endif