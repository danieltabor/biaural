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
#ifndef __TONES_H__
#define __TONES_H__

#include<portaudio.h>
#include<stdlib.h>
#include<math.h>
#include<unistd.h>

#define TONES_MAX 10
#define TONES_DEFAULT_SAMPLE_RATE 16000
#define TONES_DEFAULT_FRAMES_PER_BUFFER 1000

typedef struct {
	int active;
	float freq;
	float phase;
} envelope_t;

typedef struct {
	int active;
	float left_volume;
	float right_volume;
	float freq;
	float phase;
	envelope_t envelope;
} tone_t;

typedef enum{TONES_STARTING, TONES_RUNNING, TONES_STOPPING, TONES_STOPPED} tones_state_t;

typedef struct {
	tone_t tone[TONES_MAX];
	float volume;
	size_t sample_rate;
	size_t frames_per_buffer;
	tones_state_t state;
	PaStream* stream;
	float* buffer;
	char* err;
	size_t ii;
} tones_t;

tones_t* tones_init(size_t sample_rate,size_t frames_per_buffer);
tones_t* tones_init_default();
int tones_start_audio(tones_t* tones);
void* tones_destroy(tones_t* tones);
float* tones_generate(tones_t* tones);
void tones_generate_with_buffer(tones_t* tones, float* out, size_t frame_count);

#endif //__TONES_H__
