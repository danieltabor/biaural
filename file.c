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
#include<stdio.h>
#include"file.h"

static char* g_path = FILE_DEFAULT_PATH;

void file_set_default_path(char* path) {
	g_path = path;
}

char* file_default_path() {
	return g_path;
}

int file_save_default(tones_t* tones) {
	return file_save(tones,g_path);
}

int file_save(tones_t* tones,char* path) {
	int i;
	FILE* fp = fopen(path,"wb");
	if( fp == 0 ) {
		return 0;
	}
	fprintf(fp,"%0.02f\n",tones->volume);
	for( i=0; i<TONES_MAX; i++ ) {
		fprintf(fp,"%d,%0.01f,%0.1f,%0.02f,%0.02f,%d,%0.02f,%0.01f\n",
			tones->tone[i].active,
			tones->tone[i].freq,
			tones->tone[i].phase,
			tones->tone[i].left_volume,
			tones->tone[i].right_volume,
			tones->tone[i].envelope.active,
			tones->tone[i].envelope.freq,
			tones->tone[i].envelope.phase);
	}
	fclose(fp);
	return 1;
}

int file_load_default(tones_t* tones) {
	return file_load(tones,g_path);
}

int file_load(tones_t* tones, char* path) {
	int i;
	int err;
	FILE* fp = fopen(path,"rb");
	if( fp == 0 ) {
		return 0;
	}
	fscanf(fp,"%f\n",&tones->volume);
	for( i=0; i<TONES_MAX; i++ ) {
		err = fscanf(fp,"%d,%f,%f,%f,%f,%d,%f,%f\n",
			&tones->tone[i].active,
			&tones->tone[i].freq,
			&tones->tone[i].phase,
			&tones->tone[i].left_volume,
			&tones->tone[i].right_volume,
			&tones->tone[i].envelope.active,
			&tones->tone[i].envelope.freq,
			&tones->tone[i].envelope.phase);
		if( err != 8 ) {
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 1;
}
