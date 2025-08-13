/*
 * Copyright (c) 2025, Daniel Tabor
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include"tones.h"

int tones_callback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo *timeinfo, PaStreamCallbackFlags statusFlags, void* userData) {
	size_t tonei;
	size_t samplei;
	float lsample;
	float rsample;
	tones_t* tones = (tones_t*)userData;
	float rate = tones->sample_rate;
	float volume = tones->volume;
	if( tones->state == TONES_STOPPING ) { 
		tones->state = TONES_STOPPED;
		return paAbort;
	}
	else {
		tones->state = TONES_RUNNING;
	}

	tones_generate_with_buffer(tones,(float*)output,frameCount);

	return paContinue;
}

tones_t* tones_init(size_t sample_rate,size_t frames_per_buffer) {
	tones_t* tones;
	int i;

	tones = (tones_t*)malloc(sizeof(tones_t));
	if( tones == 0 ) {
		return 0;
	}
	tones->err = 0;
	for( i=0; i<TONES_MAX; i++ ) {
		tones->tone[i].active = 0;
		tones->tone[i].left_volume = 1.0;
		tones->tone[i].right_volume = 1.0;
		tones->tone[i].freq = 1000.0;
		tones->tone[i].phase = 0;
		tones->tone[i].envelope.active = 0;
		tones->tone[i].envelope.freq = 1.0;
		tones->tone[i].envelope.phase = 0;
	}
	tones->volume = 1.0;
	tones->sample_rate = sample_rate;
	tones->frames_per_buffer = frames_per_buffer;
	tones->ii = 0;
	tones->stream = 0;
	tones->buffer = 0;


	return tones;
}

tones_t* tones_init_default() {
	tones_init(TONES_DEFAULT_SAMPLE_RATE,TONES_DEFAULT_FRAMES_PER_BUFFER);
}

int tones_start_audio(tones_t* tones) {
	PaError err;

	err = Pa_Initialize();
	if( err != paNoError ) {
		tones->err = (char*)Pa_GetErrorText(err);
		return 0;
	}

	err = Pa_OpenDefaultStream(&(tones->stream),0,2,paFloat32,tones->sample_rate,tones->frames_per_buffer,tones_callback,tones);
	if( err != paNoError ) {
		tones->err = (char*)Pa_GetErrorText(err);
		return 0;
	}

	tones->state = TONES_STARTING;
	err = Pa_StartStream(tones->stream);
	if( err != paNoError ) {
		tones->err = (char*)Pa_GetErrorText(err);
		return 0;
	}
	return 1;
}

void* tones_destroy(tones_t* tones) {
	if( tones->stream != 0 ) {
		tones->state = TONES_STOPPING;
		while( tones->state != TONES_STOPPED ) {
			sleep(1);
		}
	}
	if( tones->buffer != 0 ) {
		free(tones->buffer);
	}
	free(tones);
}


float* tones_generate(tones_t* tones) {
	if( tones->buffer == 0 ) {
		tones->buffer = (float*)malloc(sizeof(float)*2*tones->frames_per_buffer);
		if( tones->buffer == 0 ) {
			return 0;
		}
	}
	tones_generate_with_buffer(tones,tones->buffer,tones->frames_per_buffer);
	return tones->buffer;
}

void tones_generate_with_buffer( tones_t* tones, float* out, size_t frame_count ) {
	size_t tonei;
	size_t samplei;
	float lsample;
	float rsample;
	float rate = tones->sample_rate;
	float volume = tones->volume;

	for( samplei=0; samplei<frame_count; samplei++ ) {
		lsample = 0.0;
		rsample = 0.0;
		for( tonei=0; tonei<TONES_MAX; tonei++ ) {
			if( !tones->tone[tonei].active ) {
				continue;
			}

			if( tones->tone[tonei].envelope.active ) {
				lsample = lsample + volume * tones->tone[tonei].left_volume * sin(tones->tone[tonei].freq*2*M_PI*tones->ii/rate + tones->tone[tonei].phase*rate) * fabs(sin(tones->tone[tonei].envelope.freq*2*M_PI*tones->ii/rate + tones->tone[tonei].envelope.phase*rate));
				rsample = rsample + volume * tones->tone[tonei].right_volume * sin(tones->tone[tonei].freq*2*M_PI*tones->ii/rate + tones->tone[tonei].phase*rate) * fabs(sin(tones->tone[tonei].envelope.freq*2*M_PI*tones->ii/rate + tones->tone[tonei].envelope.phase*rate));
			} else {
				lsample = lsample + volume * tones->tone[tonei].left_volume * sinf((tones->tone[tonei].freq*2*M_PI*tones->ii/rate) + (tones->tone[tonei].phase*M_PI/180.0));
				rsample = rsample + volume * tones->tone[tonei].right_volume * sinf((tones->tone[tonei].freq*2*M_PI*tones->ii/rate) + (tones->tone[tonei].phase*M_PI/180.0));
			}
		}
		if( lsample > 1.0 ) lsample = 1.0;
		else if( lsample < -1.0 ) lsample = -1.0;
		if( rsample > 1.0 ) rsample = 1.0;
		else if( rsample < -1.0 ) rsample = -1.0;
		out[(2*samplei)  ] = lsample;
		out[(2*samplei)+1] = rsample;
		tones->ii++;
	}
}

