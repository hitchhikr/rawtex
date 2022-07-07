// ----------------------------------------------------
// RAWTEX
// Gimp 2.x plugin to create raw textures & pictures.
//
// mailto:charlet.franck@wanadoo.fr
// http://franck.charlet.pagesperso-orange.fr/
//
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
// ----------------------------------------------------

// ----------------------------------------------------
// Includes
#include "idxconv.h"

// ----------------------------------------------------
// Variables
static gint pos_nibble;
static gint pic_len;
static gint nbr_cols;

// ----------------------------------------------------
// Read a byte or 4 bit from thre source
static guchar *ReadData(guchar *buffer, gint format, guchar *byte)
{
    *byte = 0;
    switch(format)
    {
        case GU_PSM_T4:
            if(pos_nibble)
            {
                *byte = (*buffer >> 4) & 0xf;
                buffer++;
            }
            else
            {
                *byte = *buffer & 0xf;
            }
            pos_nibble ^= 1;
            break;
        case GU_PSM_T8:
            *byte = *buffer++;
            break;
    }
    return buffer;
}

// ----------------------------------------------------
// Get the number of colors of the picture
gint GetPicCols(guchar *buffer, gint format)
{
    gint i;
    guchar byte;
    gint cols;

    cols = 0;
    for(i = 0; i < pic_len; i++)
    {
        buffer = ReadData(buffer, format, &byte);
        if((int) byte > cols) cols = byte;
    }
    return cols;
}

void StorePicLen(gint pic_width, gint pic_height, gint format)
{
    switch(format)
    {
        case GU_PSM_T4:
            pic_len = (pic_width << 1) * pic_height;
            break;
        case GU_PSM_T8:
            pic_len = pic_width * pic_height;
            break;
    }
}

// ----------------------------------------------------
// Chunky to Amiga planar
guint IdxToAmiga(guchar *buffer, gint format, gint pic_width, gint pic_height)
{
    pos_nibble = 0;
    StorePicLen(pic_width, pic_height, format);

    return 0;
}

// ----------------------------------------------------
// Chunky to Interleaved Amiga planar
guint IdxToAmiga_Inter(guchar *buffer, gint Format, gint pic_width, gint pic_height)
{
    pos_nibble = 0;

    return 0;
}

// ----------------------------------------------------
// Chunky to Atari ST planar
guint IdxToAtari(guchar *buffer, gint Format, gint pic_width, gint pic_height)
{
    pos_nibble = 0;

    return 0;
}
