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
#include"gui.h"
#include"file.h"
#include"oggfile.h"
#include<stdio.h>
#include<string.h>

void usage(char* cmd) {
	printf("Usage:\n");
	printf("%s [-h] [-s sample_rate] [-b frame_per_buffer] [-f load/save_path]\n",cmd);
	printf("    [-w out.ogg] load.sav\n");
	printf("\n");
	printf("Defaults:\n");
	printf("  sample_rate       : %d\n",TONES_DEFAULT_SAMPLE_RATE);
	printf("  frames_per_buffer : %d\n",TONES_DEFAULT_FRAMES_PER_BUFFER);
	printf("  load/save_path    : %s\n",FILE_DEFAULT_PATH);
	printf("  out.ogg           : none\n");
	printf("\n");
	printf("load.sav (if specified) will be loaded immediately\n");
	printf("If out.ogg is specified, it will be written without audio playing\n");
	exit(1);
}

int main(int argc, char** argv) {
	int i = 1;
	size_t sr = TONES_DEFAULT_SAMPLE_RATE;
	size_t fpb = TONES_DEFAULT_FRAMES_PER_BUFFER;
        char* oggpath = 0;
	oggfile_t *oggfile = 0;
	while( i < argc ) {
		if( strcmp(argv[i],"-h") == 0 ) {
			usage(argv[0]);
		}
		if( strcmp(argv[i],"-s") == 0 && i<argc-1) {
			sr = atoi(argv[i+1]);
			i = i + 2;
		}
		else if( strcmp(argv[i],"-b") == 0 && i<argc-1) {
			fpb = atoi(argv[i+1]);
			i = i + 2;
		}
		else if( strcmp(argv[i],"-f") == 0 && i<argc-1) {
			file_set_default_path(argv[i+1]);
			i = i + 2;
		}
		else if( strcmp(argv[i],"-w") == 0 && i<argc-1) {
			oggpath = argv[i+1];
			i = i + 2;
		}
		else if( i == argc-1 ) {
			break;
		}
		else {
			usage(argv[0]);
		}
	}
	tones_t* tones = tones_init(sr,fpb);
	if( argc>1 && i==argc-1 && !file_load(tones,argv[i]) ) {
		printf("Unable to load from file: %s\n",argv[i]);
		tones_destroy(tones);
		return 1;
	}
	if( oggpath != 0 ) {
		oggfile = oggfile_init(oggpath,sr);
		if( oggfile == 0 ) {
			printf("Unable to open ogg file: %s\n",oggpath);
			tones_destroy(tones);
			return 1;
		}
		for( i=0; i<sr*10/fpb; i++ ) {
			oggfile_write(oggfile,tones_generate(tones),fpb);
		}
		oggfile_destroy(oggfile);
	}
	else {
		tones_start_audio(tones);
		if( !gui_init(tones) ) {
			return 1;
		}
		for(;;) {
			gui_draw();
			if( !gui_input() ) {
				 break;
			}
		}
		gui_destroy();
	}
	tones_destroy(tones);

	return 0;
}
