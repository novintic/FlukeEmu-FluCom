/***************************************************************************
This work is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License Version 2 as published by the
Free Software Foundation. This work is distributed in the hope that it will
be useful, but without any warranty; without even the implied warranty of
merchantability or fitness for a particular purpose.
See the GNU General Public License for more details. You should have
received a copy of the GNU General Public License along with this program;
if not, write to:
Free Software Foundation,
Inc., 59 Temple Place, Suite 330,
Boston, MA 02111-1307 USA
****************************************************************************/
// By Clemens (novintic), 2022
// Wave header definitions
// Source: http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/

#ifndef _WAVEDEFS_H_
#define _WAVEDEFS_H_

#include <stdint.h>

// WAVE file header format
typedef struct
{
    uint8_t  riff[4];                   // RIFF string
	uint32_t overall_size;              // overall size of file in bytes
	uint8_t  wave[4];                   // WAVE string
	uint8_t  fmt_chunk_marker[4];       // fmt string with trailing null char
	uint32_t length_of_fmt;             // length of the format data
	uint16_t format_type;               // format type. 1-PCM, 3- IEEE float, 6 - 8bit A law, 7 - 8bit mu law
	uint16_t channels;                  // no.of channels
	uint32_t sample_rate;               // sampling rate (blocks per second)
	uint32_t byterate;                  // SampleRate * NumChannels * BitsPerSample/8
	uint16_t block_align;               // NumChannels * BitsPerSample/8
	uint16_t bits_per_sample;           // bits per sample, 8- 8bits, 16- 16 bits etc
	uint8_t  data_chunk_header [4];     // DATA string or FLLR string
	uint32_t data_size;                 // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
}WAVE_HEADER_t;

#endif
