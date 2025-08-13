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
#include<ncurses.h>
#include"gui.h"
#include"file.h"

tones_t* g_tones;
int g_active_idx;
int g_active_ele;

int gui_init(tones_t* tones) {
	g_tones = tones;
	g_active_idx = -1;
	g_active_ele = 0;
	if( g_tones == 0 ) {
		printf("Unable to initialize tones structure\n");
		return 0;
	}
	if( g_tones->err ) {
		printf("Unable to initialize sound: %s\n",tones->err);
		return 0;
	}
	initscr();
	keypad(stdscr,TRUE);
	noecho();
	return 1;
}

void gui_destroy() {
	endwin();
}

void gui_draw() {
	int i;
	mvaddstr(0,0,"Master Volume");
	if( g_active_idx == -1 ) {
		attroff(A_NORMAL);
		attron(A_STANDOUT);
	} else {
		attroff(A_STANDOUT);
		attron(A_NORMAL);
	}	
	move(1,3);
	printw("<%01d.%02d>",(int)g_tones->volume,(int)(g_tones->volume*100)%100);

	attroff(A_STANDOUT);
	attron(A_NORMAL);
	mvaddstr(3,0,"Active Frequency  Phase    Left   Right   EnvAct EnvFreq  EnvPh");
	mvaddstr(4,0,"-----------------------------------------------------------------");
	for( i=0; i<TONES_MAX; i++ ) {
		/* Active */
		if( g_active_idx == i && g_active_ele == 0 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,1);
		if( g_tones->tone[i].active ) {
			addstr("[X]");
		} else {
			addstr("[ ]");
		}

		/* Frequency */
		if( g_active_idx == i && g_active_ele == 1 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,7);
		printw("<%04d.%1d>",(int)g_tones->tone[i].freq,(int)(g_tones->tone[i].freq*10)%10);
		
		/* Phase */	
		if( g_active_idx == i && g_active_ele == 2 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,17);
		printw("<%03d.%1d>",(int)g_tones->tone[i].phase,(int)(g_tones->tone[i].phase*10)%10);

		/* Left Volume */
		if( g_active_idx == i && g_active_ele == 3 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,26);
		printw("<%01d.%02d>",(int)g_tones->tone[i].left_volume,(int)(g_tones->tone[i].left_volume*100)%100);

		/* Right Volume */
		if( g_active_idx == i && g_active_ele == 4 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,34);
		printw("<%01d.%02d>",(int)g_tones->tone[i].right_volume,(int)(g_tones->tone[i].right_volume*100)%100);

		/* Envelope Active */
		if( g_active_idx == i && g_active_ele == 5 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,43);
		if( g_tones->tone[i].envelope.active ) {
			addstr("[X]");
		} else {
			addstr("[ ]");
		}

		/* Envelope Freqency */
		if( g_active_idx == i && g_active_ele == 6 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,48);
		printw("<%04d.%02d>",(int)g_tones->tone[i].envelope.freq,(int)(g_tones->tone[i].envelope.freq*100)%100);

		/* Envelope Phase */
		if( g_active_idx == i && g_active_ele == 7 ) {
			attroff(A_NORMAL);
			attron(A_STANDOUT);
		} else {
			attroff(A_STANDOUT);
			attron(A_NORMAL);
		}	
		move(5+i,58);
		printw("<%03d.%1d>",(int)g_tones->tone[i].envelope.phase,(int)(g_tones->tone[i].envelope.phase*10)%10);
	}
	refresh();
}

int gui_input() {
	int ch = getch();
	if( ch == 0x01B || ch == 0x071 || ch == 0x051 ) { /* QUIT */
		return 0;
	}
	else if( ch == 0x073 || ch == 0x053 ) { /* SAVE */
		file_save_default(g_tones);
	}
	else if( ch == 0x06C || ch == 0x04C ) { /* LOAD */
		if( !file_load_default(g_tones) ) {
			mvprintw(0,25,"Failed to load: %s",file_default_path());
		}
	}
	else if( ch == 0x103 ) { /* UP */
		g_active_idx = g_active_idx - 1;
		if( g_active_idx < -1 ) {
			g_active_idx = TONES_MAX-1;
		}
	}
	else if( ch == 0x102 ) { /* DOWN */
		g_active_idx = g_active_idx + 1;
		if( g_active_idx >= TONES_MAX ) {
			g_active_idx = -1;
		}
	}
	else if( ch == 0x104 ) { /* LEFT */
		g_active_ele = g_active_ele - 1;
		if( g_active_ele < 0 ) {
			g_active_ele = 0;
		}
	}
	else if( ch == 0x105 ) { /* RIGHT */ 
		g_active_ele = g_active_ele + 1;
		if( g_active_ele > 7 ) {
			g_active_ele = 7;
		}
	}
	else if( ch == 0x009 ) { /* NEXT */
		if( g_active_idx == -1 ) {
			g_active_idx++;
			g_active_ele = 0;
		}
		else if( g_active_ele < 7 ) {
			g_active_ele++;
		}
		else {
			g_active_idx = g_active_idx + 1;
			if( g_active_idx >= TONES_MAX ) {
				g_active_idx = -1;
			}
			g_active_ele = 0;
		}
	}
	else if( ch == 0x020 || ch == 0x00A ) { /* ACTIVATE */
		if( g_active_idx != -1 ) {
			if( g_active_ele == 5 ) {
				g_tones->tone[g_active_idx].envelope.active = 1 - g_tones->tone[g_active_idx].envelope.active;
			}
			else {
				g_tones->tone[g_active_idx].active = 1 - g_tones->tone[g_active_idx].active;
			}
		}
	}
	else if( ch == 0x02E ) { /* LESS */
		if( g_active_idx == -1 ) {
			g_tones->volume = g_tones->volume - 0.10;
			if( g_tones->volume < 0.0 ) {
				g_tones->volume = 0.0;
			}
		}
		else if( g_active_ele == 1 ) {
			g_tones->tone[g_active_idx].freq = g_tones->tone[g_active_idx].freq - 10.0;
			if( g_tones->tone[g_active_idx].freq < 0.0  ) {
				g_tones->tone[g_active_idx].freq = 0.0;
			}
		}
		else if( g_active_ele == 2 ) {
			g_tones->tone[g_active_idx].phase = g_tones->tone[g_active_idx].phase - 10.0;
			if( g_tones->tone[g_active_idx].phase < 0.0 ) {
				g_tones->tone[g_active_idx].phase = 0.0;
			}
		}
		else if( g_active_ele == 3 ) {
			g_tones->tone[g_active_idx].left_volume = g_tones->tone[g_active_idx].left_volume - 0.10;
			if( g_tones->tone[g_active_idx].left_volume < 0.0 ) {
				g_tones->tone[g_active_idx].left_volume = 0.0;
			}
		}
		else if( g_active_ele == 4 ) {
			g_tones->tone[g_active_idx].right_volume = g_tones->tone[g_active_idx].right_volume - 0.10;
			if( g_tones->tone[g_active_idx].right_volume < 0.0 ) {
				g_tones->tone[g_active_idx].right_volume = 0.0;
			}
		}
		else if( g_active_ele == 6 ) {
			g_tones->tone[g_active_idx].envelope.freq = g_tones->tone[g_active_idx].envelope.freq - 1.0;
			if( g_tones->tone[g_active_idx].envelope.freq < 0.0 ) {
				g_tones->tone[g_active_idx].envelope.freq = 0.0;
			}
		}
		else if( g_active_ele == 7 ) {
			g_tones->tone[g_active_idx].envelope.phase = g_tones->tone[g_active_idx].envelope.phase - 1.0;
			if( g_tones->tone[g_active_idx].envelope.phase < 0.0 ) {
				g_tones->tone[g_active_idx].envelope.phase = 0.0;
			}
		}
	}
	else if( ch == 0x03E ) { /* LITTLE LESS */
		if( g_active_idx == -1 ) {
			g_tones->volume = g_tones->volume - 0.01;
			if( g_tones->volume < 0.0 ) {
				g_tones->volume = 0.0;
			}
		}
		else if( g_active_ele == 1 ) {
			g_tones->tone[g_active_idx].freq = g_tones->tone[g_active_idx].freq - 0.1;
			if( g_tones->tone[g_active_idx].freq < 0.0  ) {
				g_tones->tone[g_active_idx].freq = 0.0;
			}
		}
		else if( g_active_ele == 2 ) {
			g_tones->tone[g_active_idx].phase = g_tones->tone[g_active_idx].phase - 0.1;
			if( g_tones->tone[g_active_idx].phase < 0.0 ) {
				g_tones->tone[g_active_idx].phase = 0.0;
			}
		}
		else if( g_active_ele == 3 ) {
			g_tones->tone[g_active_idx].left_volume = g_tones->tone[g_active_idx].left_volume - 0.01;
			if( g_tones->tone[g_active_idx].left_volume < 0.0 ) {
				g_tones->tone[g_active_idx].left_volume = 0.0;
			}
		}
		else if( g_active_ele == 4 ) {
			g_tones->tone[g_active_idx].right_volume = g_tones->tone[g_active_idx].right_volume - 0.01;
			if( g_tones->tone[g_active_idx].right_volume < 0.0 ) {
				g_tones->tone[g_active_idx].right_volume = 0.0;
			}
		}
		else if( g_active_ele == 6 ) {
			g_tones->tone[g_active_idx].envelope.freq = g_tones->tone[g_active_idx].envelope.freq - 0.01;
			if( g_tones->tone[g_active_idx].envelope.freq < 0.0 ) {
				g_tones->tone[g_active_idx].envelope.freq = 0.0;
			}
		}
		else if( g_active_ele == 7 ) {
			g_tones->tone[g_active_idx].envelope.phase = g_tones->tone[g_active_idx].envelope.phase - 0.1;
			if( g_tones->tone[g_active_idx].envelope.phase < 0.0 ) {
				g_tones->tone[g_active_idx].envelope.phase = 0.0;
			}
		}
	}
	else if( ch == 0x02C ) { /* MORE */
		if( g_active_idx == -1 ) {
			g_tones->volume = g_tones->volume + 0.10;
			if( g_tones->volume > 1.0 ) {
				g_tones->volume = 1.0;
			}
		}
		else if( g_active_ele == 1 ) {
			g_tones->tone[g_active_idx].freq = g_tones->tone[g_active_idx].freq + 10.0;
			if( g_tones->tone[g_active_idx].freq > 9999.9  ) {
				g_tones->tone[g_active_idx].freq = 9999.9;
			}
		}
		else if( g_active_ele == 2 ) {
			g_tones->tone[g_active_idx].phase = g_tones->tone[g_active_idx].phase + 10.0;
			if( g_tones->tone[g_active_idx].phase > 360.0 ) {
				g_tones->tone[g_active_idx].phase = 360.0;
			}
		}
		else if( g_active_ele == 3 ) {
			g_tones->tone[g_active_idx].left_volume = g_tones->tone[g_active_idx].left_volume + 0.10;
			if( g_tones->tone[g_active_idx].left_volume > 1.0 ) {
				g_tones->tone[g_active_idx].left_volume = 1.0;
			}
		}
		else if( g_active_ele == 4 ) {
			g_tones->tone[g_active_idx].right_volume = g_tones->tone[g_active_idx].right_volume + 0.10;
			if( g_tones->tone[g_active_idx].right_volume > 1.0 ) {
				g_tones->tone[g_active_idx].right_volume = 1.0;
			}
		}
		else if( g_active_ele == 6 ) {
			g_tones->tone[g_active_idx].envelope.freq = g_tones->tone[g_active_idx].envelope.freq + 1.0;
			if( g_tones->tone[g_active_idx].envelope.freq > 9999.9 ) {
				g_tones->tone[g_active_idx].envelope.freq = 9999.9;
			}
		}
		else if( g_active_ele == 7 ) {
			g_tones->tone[g_active_idx].envelope.phase = g_tones->tone[g_active_idx].envelope.phase + 1.0;
			if( g_tones->tone[g_active_idx].envelope.phase > 360.0 ) {
				g_tones->tone[g_active_idx].envelope.phase = 360.0;
			}
		}
	}
	else if( ch == 0x03C ) { /* LITTLE MORE */
		if( g_active_idx == -1 ) {
			g_tones->volume = g_tones->volume + 0.01;
			if( g_tones->volume > 1.0 ) {
				g_tones->volume = 1.0;
			}
		}
		else if( g_active_ele == 1 ) {
			g_tones->tone[g_active_idx].freq = g_tones->tone[g_active_idx].freq + 0.1;
			if( g_tones->tone[g_active_idx].freq > 9999.9  ) {
				g_tones->tone[g_active_idx].freq = 9999.9;
			}
		}
		else if( g_active_ele == 2 ) {
			g_tones->tone[g_active_idx].phase = g_tones->tone[g_active_idx].phase + 0.1;
			if( g_tones->tone[g_active_idx].phase > 360.0 ) {
				g_tones->tone[g_active_idx].phase = 360.0;
			}
		}
		else if( g_active_ele == 3 ) {
			g_tones->tone[g_active_idx].left_volume = g_tones->tone[g_active_idx].left_volume + 0.01;
			if( g_tones->tone[g_active_idx].left_volume > 1.0 ) {
				g_tones->tone[g_active_idx].left_volume = 1.0;
			}
		}
		else if( g_active_ele == 4 ) {
			g_tones->tone[g_active_idx].right_volume = g_tones->tone[g_active_idx].right_volume + 0.01;
			if( g_tones->tone[g_active_idx].right_volume > 1.0 ) {
				g_tones->tone[g_active_idx].right_volume = 1.0;
			}
		}
		else if( g_active_ele == 6 ) {
			g_tones->tone[g_active_idx].envelope.freq = g_tones->tone[g_active_idx].envelope.freq + 0.01;
			if( g_tones->tone[g_active_idx].envelope.freq > 9999.9 ) {
				g_tones->tone[g_active_idx].envelope.freq = 999.9;
			}
		}
		else if( g_active_ele == 7 ) {
			g_tones->tone[g_active_idx].envelope.phase = g_tones->tone[g_active_idx].envelope.phase + 0.1;
			if( g_tones->tone[g_active_idx].envelope.phase > 360.0 ) {
				g_tones->tone[g_active_idx].envelope.phase = 360.0;
			}
		}
	}
	
	return 1;
}
