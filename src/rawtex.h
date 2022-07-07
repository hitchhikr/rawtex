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

#ifndef _RAW_H_
#define _RAW_H_

// ----------------------------------------------------
// Includes
#include <gtk/gtk.h>

// ----------------------------------------------------
// Constants
#define PLUGIN_VERSION "2.0"
#define PROCEDURE_NAME "RAWTEX"

#define GU_PSM_5650 0
#define GU_PSM_5551 1
#define GU_PSM_4444 2
#define GU_PSM_8888 3
#define GU_PSM_T4 4
#define GU_PSM_T8 5
#define GU_PSM_T16 6
#define GU_PSM_T32 7
#define GU_PSM_DXT1 8
#define GU_PSM_DXT3 9
#define GU_PSM_DXT5 10
#define GU_PSM_0333 11
#define GU_PSM_0444 12
#define GU_PSM_0888 13

#define CB_IDX_5650 0
#define CB_IDX_5551 1
#define CB_IDX_4444 2
#define CB_IDX_8888 3
#define CB_IDX_T4 4
#define CB_IDX_T8 5
#define CB_IDX_DXT1 6
#define CB_IDX_DXT3 7
#define CB_IDX_DXT5 8

#define CB_PAL_5650 0
#define CB_PAL_5551 1
#define CB_PAL_4444 2
#define CB_PAL_8888 3
#define CB_PAL_0333 4
#define CB_PAL_0444 5
#define CB_PAL_0888 6

#define DXT_TYPE_PSP 0
#define DXT_TYPE_OGL 1

#define IDX_TYPE_CHUNKY 0
#define IDX_TYPE_AMIGA 1
#define IDX_TYPE_AMIGA_INTER 2
#define IDX_TYPE_ATARI 3

// ----------------------------------------------------
// Structures
typedef struct
{
    gint Dest_Format;
    gint Dest_Palette_Format;
    gint Nbr_MipMaps;
    gint Use_Alpha_Color;
    gint Alpha_Color;
    gint Swizzle_Texture;
    gint Use_Swizzle_Texture;
    gint Save_Picture_Source;
    gint Save_Palette_Source;
    gint Dxt_Type;
    gint Use_BGR;
    gint Idx_Type;
} PluginValues, *LPPluginValues;

// ----------------------------------------------------
// Variables
extern PluginValues values_defaults;

#endif
