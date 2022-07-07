// libtxc_dxtn
// Version: 0.1b
//
// FC: Fixed some bugs with dxt1 compression.
//
// mailto:charlet.franck@wanadoo.fr
// http://franck.charlet.pagesperso-orange.fr/
//
// Copyright (C) 2004 Roland Scheidegger
// All Rights Reserved.
// Copyright (C) 2006-2022 Franck Charlet.
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _TXC_DXTN_H_
#define _TXC_DXTN_H_

#ifdef G_OS_WIN32
#include <windows.h>
#endif

typedef unsigned char GLchan;
#define UBYTE_TO_CHAN(b) (b)
#define CHAN_MAX 255
#define RCOMP 0
#define GCOMP 1
#define BCOMP 2
#define ACOMP 3

void fetch_2d_texel_rgb_dxt1(int srcRowStride, const unsigned char *pixdata, int i, int j, void *texel);
void fetch_2d_texel_rgba_dxt1(int srcRowStride, const unsigned char *pixdata, int i, int j, void *texel);
void fetch_2d_texel_rgba_dxt3(int srcRowStride, const unsigned char *pixdata, int i, int j, void *texel);
void fetch_2d_texel_rgba_dxt5(int srcRowStride, const unsigned char *pixdata, int i, int j, void *texel);

int tx_compress_dxtn(int srccomps, int width, int height, const unsigned char *srcPixData, unsigned int destformat, unsigned char *dest);

#endif
