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
#include"oggfile.h"
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdio.h>


oggfile_t *oggfile_init(char* path, int samplerate) {
  ogg_packet header;
  ogg_packet header_comm;
  ogg_packet header_code;
  ogg_page og;
  oggfile_t* oggfile;

  oggfile = malloc(sizeof(oggfile));
  if( oggfile == 0 ) {
    return 0;
  }

  oggfile->fp = fopen(path,"wb");
  if( oggfile->fp == 0) {
    free(oggfile);
    return 0;
  }

  vorbis_info_init(&(oggfile->vi));
  if( vorbis_encode_init_vbr(&(oggfile->vi),2,samplerate,0) ) {
    fclose(oggfile->fp);
    free(oggfile);
    return 0;
  }

  vorbis_comment_init(&(oggfile->vc));
  vorbis_comment_add_tag(&(oggfile->vc),"ENCODER","tones");

  vorbis_analysis_init(&(oggfile->vd),&(oggfile->vi));
  vorbis_block_init(&(oggfile->vd),&(oggfile->vb));

  srand(time(NULL));
  ogg_stream_init(&(oggfile->os),rand());

  vorbis_analysis_headerout(&(oggfile->vd),&(oggfile->vc),&header,&header_comm,&header_code);
  ogg_stream_packetin(&(oggfile->os),&header);
  ogg_stream_packetin(&(oggfile->os),&header_comm);
  ogg_stream_packetin(&(oggfile->os),&header_code);

  while( ogg_stream_flush(&(oggfile->os),&og) ) {
    fwrite(og.header, 1, og.header_len, oggfile->fp);
    fwrite(og.body, 1, og.body_len, oggfile->fp);
  }

  return oggfile;
}

void oggfile_destroy(oggfile_t* oggfile) {
  ogg_page og;

  while( ogg_stream_flush(&(oggfile->os),&og) ) {
    fwrite(og.header, 1, og.header_len, oggfile->fp);
    fwrite(og.body, 1, og.body_len, oggfile->fp);
  }

  //vorbis_block_clear(&(oggfile->vb));
  //vorbis_dsp_clear(&(oggfile->vd));
  //vorbis_comment_clear(&(oggfile->vc));
  //vorbis_info_clear(&(oggfile->vi));
  //ogg_stream_clear(&(oggfile->os));  
  fclose(oggfile->fp);
  free(oggfile);
}

int oggfile_write(oggfile_t* oggfile, float* samples, int samples_len) {
  int i;
  float **buffer;
  ogg_packet op;
  ogg_page og;

  // Give samples to vorbis encoder
  buffer = vorbis_analysis_buffer(&(oggfile->vd),samples_len);
  if( buffer == 0 ) {
    return 1;
  }
  memcpy(buffer[0],samples,samples_len);
  vorbis_analysis_wrote(&(oggfile->vd),samples_len);

  // While vorbis has encoded blocks to output push them through
  // the bitrate analysis
  while(vorbis_analysis_blockout(&(oggfile->vd),&(oggfile->vb))==1) {
    vorbis_analysis(&(oggfile->vb),NULL);
    vorbis_bitrate_addblock(&(oggfile->vb));
    // While vorbis has full packets to give, push them to
    // each stream
    while( vorbis_bitrate_flushpacket(&(oggfile->vd),&op) ) {
      //printf("Got op\n");
      ogg_stream_packetin(&(oggfile->os),&op);
      while( ogg_stream_pageout(&(oggfile->os),&og) ) {
	  //printf("Got og\n");
          fwrite(og.header, 1, og.header_len, oggfile->fp);
          fwrite(og.body, 1, og.body_len, oggfile->fp);
      }
    }
  }
  return 0;
}

