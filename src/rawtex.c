// ----------------------------------------------------
// RAWTEX
// Gimp 2.x plugin to create raw textures & pictures.
//
// TODO:
//  - Decorate the filenames (Width_Height_RGB.raw)
//  - Allow saving of indexed picture in the following format:
//
//    - Amiga planar. <<
//    - Amiga planar interleaved. <<
//    - Atari ST planar. <<
//    (palettes done c2p left to do)
//
//  - Investigate the possibilities of the non-interactive running mode. <<
//  - Check if there's a crash with multi layers pictures. <<
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
// linux (some gimpdevel would probably be required):
//
//  cd src
//  gimptool --install raw.c
//
// windows:
//
//  make
//
//  Note: be sure to have a mingw distribution installed & running
//        (+ eventually MSYS) with the relevant gtk and libgimp files
//        downloadable from: http://www.gimp.org/~tml/gimp/win32/downloads.html
//        Also you may want to replace some of the libgimp includes files
//        with the ones provided in the libgimp_replacement directory.
// 
//  A Visual C++ project file is provided too.

// ----------------------------------------------------
// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef G_OS_WIN32
#include <unistd.h>
#endif

#ifdef G_OS_WIN32
#include <windows.h>
#include "win32_hack.c"
#endif

#include "rawtex.h"
#include "support.c"
#include "callbacks.c"
#include "interface.c"
#include "txc_dxtn.c"
#include "config.c"
#include "idxconv.c"

// ----------------------------------------------------
// Structures
PluginValues values_defaults =
{
    GU_PSM_4444,
    GU_PSM_8888,
    0,
    TRUE,
    0x0000ff00,
    FALSE,
    TRUE,
    0,
    0,
    0
};

GimpParamDef save_args[] =
{
        { GIMP_PDB_INT32,    "run_mode",     "Interactive, non-interactive" },
        { GIMP_PDB_IMAGE,    "image",        "Input image" },
        { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save" },
        { GIMP_PDB_STRING,   "filename",     "The name of the file to save" },
        { GIMP_PDB_STRING,   "raw_filename", "The name of the saved file" }
};

// ----------------------------------------------------
// Functions
void query(void);
void run(const gchar *name, gint nparams,
         const GimpParam *param, gint *nreturn_vals, GimpParam **return_vals);
gint32 save_image(gchar *filename, gint32 image_ID, gint32 drawable_ID);

// ----------------------------------------------------
// Variables
GimpPlugInInfo PLUG_IN_INFO =
{
    NULL, // init_procedure
    NULL, // quit_procedure
    query,// query_procedure
    run,  // run_procedure
};

gint mipmaps_offsets[8];

guint pow_table[] =
{
    0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192
};

// ----------------------------------------------------
// Plugin entry point.
#ifdef G_OS_WIN32
    struct HINSTANCE__;
    int _stdcall WinMain(struct HINSTANCE__ *hInstance,
                         struct HINSTANCE__ *hPrevInstance,
                         char *lpszCmdLine, int nCmdShow)
    {
        // HACK for windows
        Load_GimpLib();
        return gimp_main(&PLUG_IN_INFO, __argc, __argv);
    }
#else
    MAIN()
#endif

// ----------------------------------------------------
// Plugin dialog box
gint plugin_dialog(void)
{
    GtkWidget *dlg;
    gint run;
    gboolean ok;

    gtk_set_locale();
    gtk_init(NULL, NULL);

    gimp_ui_init(PROCEDURE_NAME, TRUE);

    dlg = gimp_dialog_new("RAWTEX", PROCEDURE_NAME,
                          NULL, (GtkDialogFlags) 0,
                          NULL, NULL,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OK, GTK_RESPONSE_OK,
                          NULL);
    create_winMain(dlg, &values_defaults);

    gtk_widget_show_all(dlg);

    ok = FALSE;
    do
    {
        run = gimp_dialog_run(GIMP_DIALOG(dlg));
        switch(run)
        {
            default:
                ok = TRUE;
                break;
        }
    } while(!ok);

    gtk_widget_destroy(dlg);
    return(run == GTK_RESPONSE_OK);
}

// ----------------------------------------------------
// Query interface
void query(void)
{
    gimp_install_procedure("file_rawtex_save",
                           "Save file in RAW format",
                           "This plug-in saves textures in several RAW formats.",
                           "Franck Charlet <hitchhikr@australia.edu>",
                           "Copyright (C) 2006-2008 Franck Charlet",
                           PLUGIN_VERSION,
                           "<Save>/RAW Texture",
                           "RGB*, INDEXED*, GRAY*",
                           GIMP_PLUGIN, 5, 0, save_args, NULL);

    gimp_register_save_handler("file_rawtex_save", "raw", "");
}

// ----------------------------------------------------
// Run interface
void run(const gchar *name, gint nparams,
         const GimpParam *param, gint *nreturn_vals,
         GimpParam **return_vals)
{
	static GimpParam values[1];
	gint run_mode;
    GimpPDBStatusType status = GIMP_PDB_SUCCESS;

    run_mode = (gint) param[0].data.d_int32;

    *nreturn_vals = 1;
    *return_vals = values;
    values[0].type = GIMP_PDB_STATUS;

    // Load the config
    Load_Config(&values_defaults);

    if(strcmp(name, "file_rawtex_save") == 0)
    {
        if(plugin_dialog())
        {
			if(!save_image(param[3].data.d_string,
                           param[1].data.d_int32,
                           param[2].data.d_int32))
            {
                status = GIMP_PDB_EXECUTION_ERROR;
            }
            Save_Config(&values_defaults);
        }
    }
    else
    {
        status = GIMP_PDB_CALLING_ERROR;
    }
    values[0].data.d_status = status;
}

// ----------------------------------------------------
// Get the real value of the palette format
gint PaletteFromCombo(gint value)
{
    switch(value)
    {
        case CB_PAL_5650:
            return GU_PSM_5650;
        case CB_PAL_5551:
            return GU_PSM_5551;
        case CB_PAL_4444:
            return GU_PSM_4444;
        case CB_PAL_8888:
            return GU_PSM_8888;
        case CB_PAL_0333:
            return GU_PSM_0333;
        case CB_PAL_0444:
            return GU_PSM_0444;
        case CB_PAL_0888:
            return GU_PSM_0888;
        case CB_PAL_5550:
            return GU_PSM_5550;
        default:
            return GU_PSM_8888;
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 5650 format
gushort Convert_5650(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;
    guchar alpha;

    alpha = *pixels++;
    red = (guchar) ((*pixels++ / 255.0) * 31.0);
    green = (guchar) ((*pixels++ / 255.0) * 63.0);
    blue = (guchar) ((*pixels++ / 255.0) * 31.0);

    if(values_defaults.Use_BGR)
    {
        return((blue << 11) | (green << 5) | red);
    }
    else
    {
        return((red << 11) | (green << 5) | blue);
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 5551 format
gushort Convert_5551(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;
    guchar alpha;

    if(*pixels++ > 0) alpha = 1;
    else alpha = 0;
    red = (guchar) ((*pixels++ / 255.0) * 31.0);
    green = (guchar) ((*pixels++ / 255.0) * 31.0);
    blue = (guchar) ((*pixels++ / 255.0) * 31.0);

    if(values_defaults.Use_BGR)
    {
        return((alpha << 15) | (blue << 10) | (green << 5) | red);
    }
    else
    {
        return((alpha << 15) | (red << 10) | (green << 5) | blue);
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 4444 format
gushort Convert_4444(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;
    guchar alpha;

    alpha = (guchar) ((*pixels++ / 255.0) * 15.0);
    red = (guchar) ((*pixels++ / 255.0) * 15.0);
    green = (guchar) ((*pixels++ / 255.0) * 15.0);
    blue = (guchar) ((*pixels++ / 255.0) * 15.0);

    if(values_defaults.Use_BGR)
    {
        return((alpha << 12) | (blue << 8) | (green << 4) | red);
    }
    else
    {
        return((alpha << 12) | (red << 8) | (green << 4) | blue);
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 0333 format
gushort Convert_0333(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;

    pixels++;
    red = (guchar) ((*pixels++ / 255.0) * 7.0);
    green = (guchar) ((*pixels++ / 255.0) * 7.0);
    blue = (guchar) ((*pixels++ / 255.0) * 7.0);

    if(values_defaults.Use_BGR)
    {
        return((blue << 8) | (green << 4) | red);
    }
    else
    {
        return((red << 8) | (green << 4) | blue);
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 5550 format
gushort Convert_5550(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;

    pixels += 1;
    red = (guchar) ((*pixels++ / 255.0) * 31.0);
    green = (guchar) ((*pixels++ / 255.0) * 31.0);
    blue = (guchar) ((*pixels++ / 255.0) * 31.0);

    return((green << 11) | (red << 6) | (blue << 1) | 0);
}

// ----------------------------------------------------
// Convert a RGBA pixel into 0444 format
gushort Convert_0444(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;

    pixels++;
    red = (guchar) ((*pixels++ / 255.0) * 15.0);
    green = (guchar) ((*pixels++ / 255.0) * 15.0);
    blue = (guchar) ((*pixels++ / 255.0) * 15.0);

    if(values_defaults.Use_BGR)
    {
        return((blue << 8) | (green << 4) | red);
    }
    else
    {
        return((red << 8) | (green << 4) | blue);
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 8888 format
guint Convert_8888(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;
    guchar alpha;

    alpha = *pixels++;
    red = *pixels++;
    green = *pixels++;
    blue = *pixels++;

    if(values_defaults.Use_BGR)
    {
        return((alpha << 24) | (blue << 16) | (green << 8) | red);
    }
    else
    {
        return((alpha << 24) | (red << 16) | (green << 8) | blue);
    }
}

// ----------------------------------------------------
// Convert a RGBA pixel into 0888 format
guint Convert_0888(guchar *pixels)
{
    guchar red;
    guchar green;
    guchar blue;

    *pixels++;
    red = *pixels++;
    green = *pixels++;
    blue = *pixels++;

    if(values_defaults.Use_BGR)
    {
        return((blue << 16) | (green << 8) | red);
    }
    else
    {
        return((red << 16) | (green << 8) | blue);
    }
}

// ----------------------------------------------------
// Convert an index pixel into half nibble format
guchar Convert_IDX4(guchar *pixels)
{
    return((guchar) ((*pixels / 255.0) * 15.0));
}

// ----------------------------------------------------
// Convert the indexed pixels buffer into the destination format
int Convert_IDX_Pic(guchar *pixels,
                    guint *dest_buffer,
                    gint width,
                    gint height,
                    gint format,
                    gint depth)
{
    gint x;
    guint x2;
    gint y;
    guint offset;
    guint offset_dest;
    guchar *bbuffer;
    gint bytes;

    bytes = 0;
    bbuffer = (guchar *) dest_buffer;

    for(y = 0; y < height; y++)
    {
        gimp_progress_update((double) y / (double) height);
        x2 = 0;
        for(x = 0; x < width; x++)
        {
            if(depth == 4) offset = ((y * width) + x) * 2;
            else offset = (y * width) + x;
            switch(format)
            {
                case GU_PSM_T4:
                    offset_dest = (y * ((width / 2) + (width % 2))) + x2;
                    // 1:2
                    bbuffer[offset_dest] = pixels[offset];
                    x++;
                    // Odd texture width
                    if(x < width)
                    {
                        if(depth == 4) offset = ((y * width) + x) * 2;
                        else offset = (y * width) + x;
                        bbuffer[offset_dest] |= pixels[offset] << 4;
                    }
                    bytes += 1;
                    break;
                case GU_PSM_T8:
                    offset_dest = (y * width) + x2;
                    // 1:1
                    bbuffer[offset_dest] = pixels[offset];
                    bytes += 1;
                    break;

                case GU_PSM_T16:            // ???
                case GU_PSM_T32:            // ???
                default:
                    return(0);
            }
            x2++;
        }
    }
    return(bytes);
}

// ----------------------------------------------------
// Convert the RGBA pixels buffer into the destination format
int Convert_RGBA_Pic(guchar *pixels,
                     guint *dest_buffer,
                     gint width,
                     gint height,
                     guchar depth,
                     gint format)
{
    gint x;
    gint y;
    guint offset;
    guint offset_dest;
    gint bytes;
    guchar *bbuffer;
    gushort *wbuffer;
    guint *dbuffer;
    
    bytes = 0;
    bbuffer = (guchar *) dest_buffer;
    wbuffer = (gushort *) dest_buffer;
    dbuffer = (guint *) dest_buffer;
    for(y = 0; y < height; y++)
    {
        gimp_progress_update((double) y / (double) height);
        for(x = 0; x < width; x++)
        {
            offset = ((y * width) + x) * depth;
            offset_dest = (y * width) + x;
            switch(format)
            {
                case GU_PSM_5650:
                    wbuffer[offset_dest] = Convert_5650(&pixels[offset]);
                    bytes += 2;
                    break;
                case GU_PSM_5551:
                    wbuffer[offset_dest] = Convert_5551(&pixels[offset]);
                    bytes += 2;
                    break;
                case GU_PSM_4444:
                    wbuffer[offset_dest] = Convert_4444(&pixels[offset]);
                    bytes += 2;
                    break;
                case GU_PSM_8888:
                case GU_PSM_DXT1:
                case GU_PSM_DXT3:
                case GU_PSM_DXT5:
                    dbuffer[offset_dest] = Convert_8888(&pixels[offset]);
                    bytes += 4;
                    break;

                default:
                    return(0);
            }
        }
    }
    return(bytes);
}

// ----------------------------------------------------
// Convert the selected color to alpha channel
void Convert_Alpha(guchar *pixels,
                   gint width,
                   gint height,
                   guchar depth,
                   guint alpha_color)
{
    gint x;
    gint y;
    guint offset;
    guchar alpha_r;
    guchar alpha_g;
    guchar alpha_b;
    guchar pixel_r;
    guchar pixel_g;
    guchar pixel_b;

    alpha_r = (alpha_color & 0xff0000) >> 16;
    alpha_g = (alpha_color & 0xff00) >> 8;
    alpha_b = (alpha_color & 0xff);
    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            offset = ((y * width) + x) * 4;
            pixel_r = pixels[offset + 1];
            pixel_g = pixels[offset + 2];
            pixel_b = pixels[offset + 3];
            if(values_defaults.Use_Alpha_Color)
            {
                if(pixel_r == alpha_r && pixel_g == alpha_g && pixel_b == alpha_b)
                {
                    pixels[offset + 1] = 0;
                    pixels[offset + 2] = 0;
                    pixels[offset + 3] = 0;
                    pixels[offset] = 0;
                }
            }
        }
    }
}

// ----------------------------------------------------
// Convert the color table to RGBA
void Palette_To_RGBA(guchar *dest_buffer,
                     guchar *color_table,
                     gint nbr_colors,
                     gint depth,
                     guint alpha_color)
{
    gint i;
    guchar alpha_r;
    guchar alpha_g;
    guchar alpha_b;
    guchar color_r;
    guchar color_g;
    guchar color_b;
    guchar color_a;

    alpha_r = (alpha_color & 0xff0000) >> 16;
    alpha_g = (alpha_color & 0xff00) >> 8;
    alpha_b = (alpha_color & 0xff);

    for(i = 0; i < nbr_colors; i++)
    {
        color_a = 0xff;
        color_r = *color_table++;
        color_g = *color_table++;
        color_b = *color_table++;
        if(values_defaults.Use_Alpha_Color)
        {
            if(color_r == alpha_r && color_g == alpha_g && color_b == alpha_b)
            {
                color_r = 0;
                color_g = 0;
                color_b = 0;
                color_a = 0;
            }
        }
        *dest_buffer++ = color_a;
        if(values_defaults.Use_BGR)
        {
            *dest_buffer++ = color_b;
            *dest_buffer++ = color_g;
            *dest_buffer++ = color_r;
        }
        else
        {
            *dest_buffer++ = color_r;
            *dest_buffer++ = color_g;
            *dest_buffer++ = color_b;
        }
    }
}

// ----------------------------------------------------
// Convert a RGB* buffer into RGBA
void RGBx_To_RGBA(guchar *dest_buffer,
                  guchar *pixels,
                  gint width,
                  gint height,
                  guchar depth)
{
    gint x;
    gint y;
    guint offset;
    guint offset_dest;
    guchar pixel_r;
    guchar pixel_g;
    guchar pixel_b;
    guchar pixel_a;

    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            offset = ((y * width) + x) * depth;     
            offset_dest = ((y * width) + x) * 4;
            if(depth == 3)
            {
                pixel_r = pixels[offset + 2];
                pixel_g = pixels[offset + 1];
                pixel_b = pixels[offset];       
            }
            else
            {
                pixel_r = pixels[offset + 2];
                pixel_g = pixels[offset + 1];
                pixel_b = pixels[offset];
            }
            dest_buffer[offset_dest + 1] = pixel_r;
            dest_buffer[offset_dest + 2] = pixel_g;
            dest_buffer[offset_dest + 3] = pixel_b;
            dest_buffer[offset_dest] = 0xff;
            if(depth == 4)
            {
                pixel_a = pixels[offset + 3];
                dest_buffer[offset_dest] = pixel_a;
            }
        }
    }
}

// ----------------------------------------------------
// Convert a gray scale buffer into alpha channel picture
void GRAY_To_ALPHA(guchar *dest_buffer,
                   guchar *pixels,
                   gint width,
                   gint height,
                   guchar depth)
{
    gint x;
    gint y;
    guint offset;
    guint offset_dest;
    guchar pixel_a;

    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            if(depth == 4) offset = ((y * width) + x) * 2;
            else offset = ((y * width) + x);
            offset_dest = ((y * width) + x) * 4;
            pixel_a = pixels[offset];
            dest_buffer[offset_dest + 1] = 0;
            dest_buffer[offset_dest + 2] = 0;
            dest_buffer[offset_dest + 3] = 0;
            dest_buffer[offset_dest] = 0xff - pixel_a;
        }
    }
}

// ----------------------------------------------------
// Swizzle the buffer
void Swizzle(guchar *out,
             guchar *in,
             guint width,
             guint height)
{
    guint i;
    guint j;
    guint rowblocks = (width / 16);
    guint blockx;
    guint blocky;
    guint x;
    guint y;
    guint block_index;
    guint block_address;

    for(j = 0; j < height; ++j)
    {
        for(i = 0; i < width; ++i)
        {
            blockx = i / 16;
            blocky = j / 8;
            x = (i - blockx * 16);
            y = (j - blocky * 8);
            block_index = blockx + (blocky * rowblocks);
            block_address = block_index * 16 * 8;
            out[block_address + x + (y * 16)] = in[i + (j * width)];
        }
    }
}

// ----------------------------------------------------
// Convert the RGBA palette
int Convert_Palette(guchar *palette_buffer,
                    guint *dest_palette_buffer,
                    gint Nbr_Colors,
                    gint Palette_Format)
{
    gint i;
    guint bytes;
    guint *ddest_palette_buffer;
    gushort *wdest_palette_buffer;

    bytes = 0;

    wdest_palette_buffer = (gushort *) dest_palette_buffer;
    ddest_palette_buffer = (guint *) dest_palette_buffer;

    for(i = 0; i < Nbr_Colors; i++)
    {
        switch(Palette_Format)
        {
            case GU_PSM_5650:
                *wdest_palette_buffer++ = Convert_5650(palette_buffer);
                bytes += 2;
                break;
            case GU_PSM_5551:
                *wdest_palette_buffer++ = Convert_5551(palette_buffer);
                bytes += 2;
                break;
            case GU_PSM_4444:
                *wdest_palette_buffer++ = Convert_4444(palette_buffer);
                bytes += 2;
                break;
            case GU_PSM_8888:
                *ddest_palette_buffer++ = Convert_8888(palette_buffer);
                bytes += 4;
                break;
            case GU_PSM_0333:
                *wdest_palette_buffer++ = Convert_0333(palette_buffer);
                bytes += 2;
                break;
            case GU_PSM_0444:
                *wdest_palette_buffer++ = Convert_0444(palette_buffer);
                bytes += 2;
                break;
            case GU_PSM_5550:
                *wdest_palette_buffer++ = Convert_5550(palette_buffer);
                bytes += 2;
                break;
            case GU_PSM_0888:
                *ddest_palette_buffer++ = Convert_0888(palette_buffer);
                bytes += 4;
                break;
        }
        palette_buffer += 4;
    }
    return(bytes);
}

// ----------------------------------------------------
// Convert the DXTx buffers for the PSP
void Convert_DXT5(unsigned char *data, unsigned int size)
{
    unsigned short *src = (unsigned short *) data;
    gint i;
    gint j;
    unsigned short converted[8];

    for(j = 0; size >= 16; size -= 16, j++)
    {
        converted[4] = src[1];
        converted[5] = src[2];
        converted[6] = src[3];
        converted[7] = src[0];

        converted[0] = src[6];
        converted[1] = src[7];
        converted[2] = src[4];
        converted[3] = src[5];
        for(i = 0; i < 8; i++) src[i] = converted[i];
        src += 8;
    }
}

void Convert_DXT3(unsigned char *data, unsigned int size)
{
    unsigned short *src = (unsigned short *) data;
    gint i;
    gint j;
    unsigned short converted[8];

    for(j = 0; size >= 16; size -= 16, j++)
    {
        converted[4] = src[0];
        converted[5] = src[1];
        converted[6] = src[2];
        converted[7] = src[3];

        converted[0] = src[6];
        converted[1] = src[7];
        converted[2] = src[4];
        converted[3] = src[5];
        for(i = 0; i < 8; i++) src[i] = converted[i];
        src += 8;
    }
}

void Convert_DXT1(unsigned char *data, unsigned int size)
{
    unsigned short *src = (unsigned short *) data;
    gint i;
    gint j;
    unsigned short converted[4];

    for(j = 0; size >= 8; size -= 8, j++)
    {
        converted[0] = src[2];
        converted[1] = src[3];
        converted[2] = src[0];
        converted[3] = src[1];
        for(i = 0; i < 4; i++) src[i] = converted[i];
        src += 4;
    }
}

// ----------------------------------------------------
// Write bytes data as binary or source code
void write_data(gchar *name,
                guchar *dest_buffer,
                gint bytes,
                FILE *out,
                gint size,
                gint source_code,
                gint msb_first)
{
    gint write_ret;
    gint i;
    guchar value;
    char data_format[16];
    gushort *wdest_buffer;
    guint *dwdest_buffer;

    wdest_buffer = (gushort *) dest_buffer;
    dwdest_buffer = (guint *) dest_buffer;

    if(source_code)
    {
        fprintf(out, "/* File produced with RAWTEX Gimp plugin v%s\n", PLUGIN_VERSION);
        fprintf(out, "   Written by Franck Charlet. */\n\n");
        fprintf(out, "#define %s_SIZE %d\n", name, bytes);
        switch(size)
        {
            case 1:
                fprintf(out, "unsigned char %s[] =\n{", name);
                break;
            case 2:
                fprintf(out, "unsigned short %s[] =\n{", name);
                break;
            case 4:
                fprintf(out, "unsigned long %s[] =\n{", name);
                break;
        }
        sprintf(data_format, "0x%%.0%dx,", size << 1);
        for(i = 0; i < bytes / size; i++)
        {
            if((i % 16) == 0) fprintf(out, "\n\t");
            switch(size)
            {
                case 1:
                    fprintf(out, data_format, dest_buffer[i]);
                    break;
                case 2:
                    fprintf(out, data_format, wdest_buffer[i]);
                    break;
                case 4:
                    fprintf(out, data_format, dwdest_buffer[i]);
                    break;
            }
        }
        fprintf(out, "\n};\n");
    }
    else
    {
        if(msb_first)
        {
            for(i = 0; i < bytes; i += 2)
            {
                value = dest_buffer[i + 1];
                dest_buffer[i + 1] = dest_buffer[i];
                dest_buffer[i] = value;
            }
        }
        write_ret = fwrite(dest_buffer, bytes, 1, out);
    }
}

// ----------------------------------------------------
// Create a label from a filename
void Create_Label(gchar *label, gchar *filename)
{
    gint len_filename;
    int j;

    strcpy(label, filename);
    len_filename = strlen(label);
    j = len_filename - 1;
    while(label[j] != '.' && label[j] != '\0')
    {
        j--;
    }
    label[j] = 0;

    len_filename = strlen(label);
    j = len_filename - 1;
    while(label[j] != '/' && label[j] != '\\' && j >= 0)
    {
        if(label[j] < 'A' || label[j] > 'Z')
        {
            if(label[j] < 'a' || label[j] > 'z')
            {
                if(label[j] < '0' || label[j] > '9') label[j] = '_';
            }
            label[j] = toupper(label[j]);
        }
        j--;
    }
    strcpy(label, &label[j + 1]);
}

// ----------------------------------------------------
// Flip a RGBA picture vertically
void Flip_Picture(gint width,
                  gint height,
                  guchar *source_Buffer,
                  guchar *dest_buffer)
{
	gint x;
	gint y;
	gint y_src;
	gint offset;
	gint offset_dest;

	y_src = height - 1;
    for(y = 0; y < height; y++)
    {
        for(x = 0; x < width; x++)
        {
            offset = ((y_src * width) + x) * 4;
            offset_dest = ((y * width) + x) * 4;
            dest_buffer[offset_dest + 1] = source_Buffer[offset + 1];
            dest_buffer[offset_dest + 2] = source_Buffer[offset + 2];
            dest_buffer[offset_dest + 3] = source_Buffer[offset + 3];
            dest_buffer[offset_dest] = source_Buffer[offset];
        }
		y_src--;
    }
	memcpy(source_Buffer, dest_buffer, width * height * 4);
}

// ----------------------------------------------------
// Save the picture
gint save_image(gchar *filename,
                gint32 image_ID,
                gint32 drawable_ID)
{
    FILE *out;
    FILE *out_palette;
    gint Return_Value;
    GimpPixelRgn pixels_rgn;
    GimpDrawable *drawable;
    guchar depth;
    guchar *pixels;
    guint *dest_buffer;
    guint *dest_buffer_final;
    guint *swizzle_buffer;
    guint *dest_buffer_rgba;
    guint *palette_buffer;
    guint *dest_palette_buffer;
    guchar *Color_Table;
    gint bytes;
    gchar *progress;
    gint Swizzle_Wide;
    gint Indexed_Mode;
    gint Grayscale_Mode;
    gint Nbr_Colors; 
    gint swizzle_width;
    gchar header_name[512];
    gchar palette_filename[512];
    gchar picture_filename[512];
    gint i;
    gint j;
    gint pal_element_size;
    gint resize_x;
    gint resize_y;
    gint len_filename;
    gint dxt_size;
    gint pic_width;
    gint pic_height;
    gint Save_Extra_Files;
    gint Half_Width;
    gint Half_Height;
    guchar *bdest_buffer;
    guchar *bpixels;
    guint *ddest_buffer;
    guint *dpixels;
    gint old_depth;
    gint pow_width;
    gint pow_height;
    gint current_offset;

    Indexed_Mode = FALSE;
    Grayscale_Mode = FALSE;
    depth = 3;
    Color_Table = NULL;
    drawable = (GimpDrawable *) gimp_drawable_get(drawable_ID);
    Return_Value = FALSE;
    Nbr_Colors = 0;

    switch(gimp_drawable_type(drawable_ID))
    {
        case GIMP_RGB_IMAGE:
            break;
        case GIMP_RGBA_IMAGE:
            depth = 4;
            break;
        case GIMP_INDEXEDA_IMAGE:
            depth = 4;
        case GIMP_INDEXED_IMAGE:
            Indexed_Mode = TRUE;
            Color_Table = (guchar *) gimp_image_get_colormap(image_ID, &Nbr_Colors);
            switch(values_defaults.Dest_Format)
            {
                case GU_PSM_DXT1:
                case GU_PSM_DXT3:
                case GU_PSM_DXT5:
                    g_message("Input format doesn't match\n");
                    return(FALSE);
            }
            break;
        case GIMP_GRAY_IMAGE:
            Grayscale_Mode = TRUE;
            break;
        case GIMP_GRAYA_IMAGE:
            depth = 4;
            Grayscale_Mode = TRUE;
            break;
        default:
            g_message("Unsupported input format\n");
            return(FALSE);
    };

    if(Nbr_Colors > 16 && Indexed_Mode && values_defaults.Dest_Format == GU_PSM_T4)
    {
        g_message("4 bits pictures require a maximum of 16 colors\n");
        return(FALSE);
    }

    memset(picture_filename, 0, sizeof(picture_filename));
    strncpy(picture_filename, filename, strlen(filename));
    if(values_defaults.Save_Picture_Source) strcat(picture_filename, ".h");

    if(strrchr(picture_filename, '/') != NULL) progress = g_strdup_printf("Saving %s:", strrchr(picture_filename, '/') + 1);
    else progress = g_strdup_printf("Saving %s:", picture_filename);

    gimp_progress_init(progress);

    pixels = g_new(guchar, drawable->width * drawable->height * depth);
    dest_buffer = g_new(guint, drawable->width * drawable->height);
    dest_buffer_rgba = g_new(guint, drawable->width * drawable->height);
    dest_buffer_final = g_new(guint, drawable->width * drawable->height);
    swizzle_buffer = g_new(guint, drawable->width * drawable->height);
    palette_buffer = g_new(guint, Nbr_Colors);
    dest_palette_buffer = g_new(guint, Nbr_Colors);

    gimp_pixel_rgn_init(&pixels_rgn, drawable, 0, 0, drawable->width, drawable->height, FALSE, FALSE);
    gimp_pixel_rgn_get_rect(&pixels_rgn, pixels, 0, 0, drawable->width, drawable->height);

    if(Indexed_Mode)
    {
        // Convert the palette to RGBA
        Palette_To_RGBA((guchar *) palette_buffer, Color_Table, Nbr_Colors, depth, values_defaults.Alpha_Color & 0x00ffffff);
    }

    old_depth = depth;
    Save_Extra_Files = TRUE;

    out = fopen(picture_filename, "wb");
    if(!out)
    {
        g_message("Unable to create file: %s\n", picture_filename);
    }
    else
    {
        pic_width = drawable->width;
        pic_height = drawable->height;
        current_offset = 0;
        for(i = 0; i < values_defaults.Nbr_MipMaps + 1; i++)
        {
            depth = old_depth;
            Create_Label(header_name, picture_filename);
            sprintf(header_name, "%s_%d", header_name, i);
            if(!Indexed_Mode)
            {
                if(Grayscale_Mode)
                {
                    // Convert gray scale to alpha channel
                    GRAY_To_ALPHA((guchar *) dest_buffer_rgba, pixels, pic_width, pic_height, depth);
                }
                else
                {
                    // Convert RGBx picture into RGBA
                    RGBx_To_RGBA((guchar *) dest_buffer_rgba, pixels, pic_width, pic_height, depth);
                }
            }

            depth = 4;

            if(!Indexed_Mode)
            {
                Convert_Alpha((guchar *) dest_buffer_rgba, pic_width, pic_height, depth, values_defaults.Alpha_Color & 0x00ffffff);
                bytes = Convert_RGBA_Pic((guchar *) dest_buffer_rgba, dest_buffer, pic_width, pic_height, depth, values_defaults.Dest_Format);
            }
            else
            {
                // (Alpha color is converted in the palette)
                bytes = Convert_IDX_Pic((guchar *) pixels,
                                        dest_buffer,
                                        pic_width,
                                        pic_height,
                                        values_defaults.Dest_Format,
                                        old_depth);
                switch(values_defaults.Idx_Type)
                {
                    case IDX_TYPE_CHUNKY:
                        // We're already in chunky mode
                        break;
                    case IDX_TYPE_AMIGA:
                        bytes = IdxToAmiga((guchar *) dest_buffer,
                                           values_defaults.Dest_Format,
                                           pic_width,
                                           pic_height);
                        break;
                    case IDX_TYPE_AMIGA_INTER:
                        bytes = IdxToAmiga_Inter((guchar *) dest_buffer,
                                                 values_defaults.Dest_Format,
                                                 pic_width,
                                                 pic_height);
                        break;
                    case IDX_TYPE_ATARI:
                        bytes = IdxToAtari((guchar *) dest_buffer,
                                           values_defaults.Dest_Format,
                                           pic_width,
                                           pic_height);
                        break;
                }
            }

            if(!bytes)
            {
                if(values_defaults.Dest_Format == GU_PSM_T4 || values_defaults.Dest_Format == GU_PSM_T8)
                {
                    g_message("Invalid output conversion: RGB > INDEXED\n");
                }
                else
                {
                    g_message("Invalid output conversion: INDEXED > RGB\n");
                }
                Save_Extra_Files = FALSE;
                break;          // stop for loop
            }
            else
            {
                swizzle_width = pic_width;
                Swizzle_Wide = 4;
                if(values_defaults.Swizzle_Texture && values_defaults.Use_Swizzle_Texture)
                {
                    switch(values_defaults.Dest_Format)
                    {
                        case GU_PSM_5650:
                        case GU_PSM_5551:
                        case GU_PSM_4444:
                            Swizzle_Wide = 2;
                            break;
                        case GU_PSM_8888:
                            break;
                        case GU_PSM_T4:
                            swizzle_width /= 2;
                        case GU_PSM_T8:
                            Swizzle_Wide = 1;
                            break;

                        case GU_PSM_T16:
                        case GU_PSM_T32:
                            break;
                        case GU_PSM_DXT1:           // Shouldn't reach this
                        case GU_PSM_DXT3:
                        case GU_PSM_DXT5:
                            g_message("Something seriously wrong & bad has occured\n");
                            goto error;
                        default:
                            Swizzle_Wide = 1;
                            break;
                    }
                    Swizzle((guchar *) swizzle_buffer, (guchar *) dest_buffer, swizzle_width * Swizzle_Wide, pic_height);
                    memcpy(dest_buffer, swizzle_buffer, (swizzle_width * Swizzle_Wide) * pic_height);
                }

                // DXTx = RGBA
                switch(values_defaults.Dest_Format)
                {
                    case GU_PSM_DXT1:
                        if(values_defaults.Dxt_Type == DXT_TYPE_OGL)
                        {
                    		Flip_Picture(pic_width, pic_height, (guchar *) dest_buffer, (guchar *) dest_buffer_final);
                    	}
                        dxt_size = tx_compress_dxtn(4, pic_width, pic_height, (guchar *) dest_buffer, GU_PSM_DXT1, (guchar *) dest_buffer_final);
                        if(values_defaults.Dxt_Type == DXT_TYPE_PSP)
                        {
                        	Convert_DXT1((guchar *) dest_buffer_final, dxt_size);
                        }
                       	memcpy(dest_buffer, dest_buffer_final, dxt_size);
                        bytes = dxt_size;
                        break;
                    case GU_PSM_DXT3:
                        if(values_defaults.Dxt_Type == DXT_TYPE_OGL)
                        {
                    		Flip_Picture(pic_width, pic_height, (guchar *) dest_buffer, (guchar *) dest_buffer_final);
                    	}
                        dxt_size = tx_compress_dxtn(4, pic_width, pic_height, (guchar *) dest_buffer, GU_PSM_DXT3, (guchar *) dest_buffer_final);
                        if(values_defaults.Dxt_Type == DXT_TYPE_PSP)
                        {
                        	Convert_DXT3((guchar *) dest_buffer_final, dxt_size);
                        }
                       	memcpy(dest_buffer, dest_buffer_final, dxt_size);
                        bytes = dxt_size;
                        break;
                    case GU_PSM_DXT5:
                        if(values_defaults.Dxt_Type == DXT_TYPE_OGL)
                        {
                    		Flip_Picture(pic_width, pic_height, (guchar *) dest_buffer, (guchar *) dest_buffer_final);
                    	}
                        dxt_size = tx_compress_dxtn(4, pic_width, pic_height, (guchar *) dest_buffer, GU_PSM_DXT5, (guchar *) dest_buffer_final);
                        if(values_defaults.Dxt_Type == DXT_TYPE_PSP)
                        {
                        	Convert_DXT5((guchar *) dest_buffer_final, dxt_size);
                        }
                        memcpy(dest_buffer, dest_buffer_final, dxt_size);
                        bytes = dxt_size;
                        break;
                    default:
                        break;
                }
                mipmaps_offsets[i] = current_offset;
                write_data(header_name, (guchar *) dest_buffer, bytes, out, 1, values_defaults.Save_Picture_Source, 0);
                current_offset += bytes;
            }

            Half_Width = pic_width >> 1;
            Half_Height = pic_height >> 1;

            // Halve the pic
            bdest_buffer = (guchar *) dest_buffer;
            bpixels = (guchar *) pixels;
            ddest_buffer = dest_buffer;
            dpixels = (guint *) pixels;
            for(resize_y = 0; resize_y < Half_Height; resize_y++)
            {
                for(resize_x = 0; resize_x < Half_Width; resize_x++)
                {
                    if(Indexed_Mode) bdest_buffer[(resize_y * Half_Width) + resize_x] = bpixels[((resize_y * 2) * pic_width) + (resize_x * 2)];
                    else
                    {
                        if(old_depth == 3)
                        {
                            bdest_buffer[(resize_y * (Half_Width * 3)) + (resize_x * 3)] = bpixels[((resize_y * 2) * (pic_width * 3)) + ((resize_x * 2) * 3)];
                            bdest_buffer[(resize_y * (Half_Width * 3)) + (resize_x * 3) + 1] = bpixels[((resize_y * 2) * (pic_width * 3)) + ((resize_x * 2) * 3) + 1];
                            bdest_buffer[(resize_y * (Half_Width * 3)) + (resize_x * 3) + 2] = bpixels[((resize_y * 2) * (pic_width * 3)) + ((resize_x * 2) * 3) + 2];
                        }
                        else
                        {
                            ddest_buffer[(resize_y * Half_Width) + resize_x] = dpixels[((resize_y * 2) * pic_width) + (resize_x * 2)];
                        }
                    }
                }
            }
            pic_width >>= 1;
            pic_height >>= 1;
            bdest_buffer = (guchar *) dest_buffer;
            bpixels = pixels;
            ddest_buffer = dest_buffer;
            dpixels = (guint *) pixels;
            for(resize_y = 0; resize_y < pic_height; resize_y++)
            {
                for(resize_x = 0; resize_x < pic_width; resize_x++)
                {
                    if(Indexed_Mode)
                    {
                        bpixels[(resize_y * pic_width) + resize_x] = bdest_buffer[(resize_y * pic_width) + resize_x];
                    }
                    else
                    {
                        if(old_depth == 3)
                        {
                            bpixels[(resize_y * (Half_Width * 3)) + (resize_x * 3)] = bdest_buffer[(resize_y * (pic_width * 3)) + (resize_x * 3)];
                            bpixels[(resize_y * (Half_Width * 3)) + (resize_x * 3) + 1] = bdest_buffer[(resize_y * (pic_width * 3)) + (resize_x * 3) + 1];
                            bpixels[(resize_y * (Half_Width * 3)) + (resize_x * 3) + 2] = bdest_buffer[(resize_y * (pic_width * 3)) + (resize_x * 3) + 2];
                        }
                        else
                        {
                            dpixels[(resize_y * pic_width) + resize_x] = ddest_buffer[(resize_y * pic_width) + resize_x];
                        }
                    }
                }
            }
        }
        fclose(out);

        if(Save_Extra_Files)
        {
            len_filename = strlen(filename);
            j = len_filename - 1;
            while(filename[j] != '.' && j >= 0)
            {
                j--;
            }

            // Save the color table
            if(Indexed_Mode)
            {
                memset(palette_filename, 0, sizeof(palette_filename));
                if(j) strncpy(palette_filename, filename, (&filename[j]) - filename);
                strcat(palette_filename, ".rawpal");
                if(values_defaults.Save_Palette_Source) strcat(palette_filename, ".h");

                out_palette = fopen(palette_filename, "wb");
                if(!out_palette)
                {
                    g_message("Unable to create file: %s\n", palette_filename);
                    Return_Value = FALSE;
                    goto error;
                }
                else
                {
                    Create_Label(header_name, palette_filename);
                    bytes = Convert_Palette((guchar *) palette_buffer,
                                            dest_palette_buffer,
                                            Nbr_Colors,
                                            PaletteFromCombo(values_defaults.Dest_Palette_Format));
                    switch(PaletteFromCombo(values_defaults.Dest_Palette_Format))
                    {
                        case GU_PSM_8888:
                        case GU_PSM_0888:
                            pal_element_size = 4;
                            break;
                        default:
                            pal_element_size = 2;
                            break;
                    }
                    write_data(header_name,
                               (guchar *) dest_palette_buffer,
                               bytes,
                               out_palette, pal_element_size,
                               values_defaults.Save_Palette_Source,
                               PaletteFromCombo(values_defaults.Dest_Palette_Format) == GU_PSM_5550);
                    fclose(out_palette);
                    Return_Value = TRUE;
                }
            }
            else
            {
                Return_Value = TRUE;
            }

            // Save the C constants
            memset(palette_filename, 0, sizeof(palette_filename));
            if(j) strncpy(palette_filename, filename, (&filename[j]) - filename);
            strcat(palette_filename, ".h");
            out_palette = fopen(palette_filename, "w");
            if(!out_palette)
            {
                g_message("Unable to create %s.h file\n", filename);
                Return_Value = FALSE;
                goto error;
            }
            else
            {
                Create_Label(header_name, palette_filename);

                fprintf(out_palette, "/* File produced with RAWTEX Gimp plugin v%s\n", PLUGIN_VERSION);
                fprintf(out_palette, "   Written by Franck Charlet. */\n\n");
                pow_width = drawable->width;
                pow_height = drawable->height;
                for(i = 0; i < (sizeof(pow_table) / sizeof(gint)) - 1; i++)
                {
                    if(drawable->width >= pow_table[i] && drawable->width <= pow_table[i + 1])
                    {
                        pow_width = pow_table[i + 1];
                        break;
                    }
                }
                for(i = 0; i < (sizeof(pow_table) / sizeof(gint)) - 1; i++)
                {
                    if(drawable->height >= pow_table[i] && drawable->height <= pow_table[i + 1])
                    {
                        pow_height = pow_table[i + 1];
                        break;
                    }
                }
                fprintf(out_palette, "#define %s_TEXTURE_WIDTH %d\n", header_name, pow_width);
                fprintf(out_palette, "#define %s_TEXTURE_REAL_WIDTH %d\n", header_name, drawable->width);
                fprintf(out_palette, "#define %s_TEXTURE_HEIGHT %d\n", header_name, pow_height);
                fprintf(out_palette, "#define %s_TEXTURE_REAL_HEIGHT %d\n", header_name, drawable->height);
                fprintf(out_palette, "#define %s_TEXTURE_FORMAT %d\n", header_name, values_defaults.Dest_Format);
                fprintf(out_palette, "#define %s_TEXTURE_MIPMAPS %d\n", header_name, values_defaults.Nbr_MipMaps);
                fprintf(out_palette, "#define %s_TEXTURE_SWIZZLE %d\n", header_name, values_defaults.Swizzle_Texture & values_defaults.Use_Swizzle_Texture);
                if(Indexed_Mode)
                {
                    fprintf(out_palette, "\n#define %s_PALETTE_FORMAT %d\n", header_name, PaletteFromCombo(values_defaults.Dest_Palette_Format));
                    fprintf(out_palette, "#define %s_PALETTE_COLORS %d\n", header_name, Nbr_Colors);
                }
                // Generate mipmaps locations inside the picture
                // (No need to generate them if the data are saved as sourcecode).
                if(!values_defaults.Save_Picture_Source)
                {
                    for(i = 0; i < values_defaults.Nbr_MipMaps + 1; i++)
                    {
                        fprintf(out_palette, "#define %s_MIPMAP%d_OFFSET %d\n", header_name, i, mipmaps_offsets[i]);
                    }
                }
                fclose(out_palette);
                Return_Value = TRUE;
            }
        }
    }
error:
    g_free(progress);
    g_free(dest_palette_buffer);
    g_free(palette_buffer);
    g_free(swizzle_buffer);
    g_free(dest_buffer_final);
    g_free(dest_buffer_rgba);
    g_free(dest_buffer);
    g_free(pixels);
    return(Return_Value);
}
