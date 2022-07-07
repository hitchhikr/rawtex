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

// Dirty hack to make it to work under windows as creating plugins under that system is a pain in the ass
// (Not to speak about compiling the libgimp.a itself).

HMODULE GimpLibBase;
HMODULE GimpBaseLibBase;
HMODULE GimpUiLibBase;
HMODULE GimpWidgetsLibBase;

void (*gimp_ui_init)(const gchar *prog_name, gboolean preview);
void (*gimp_install_procedure)(const gchar *name,
                               const gchar *blurb,
                               const gchar *help,
                               const gchar *author,
                               const gchar *copyright,
                               const gchar *date,
                               const gchar *menu_label,
                               const gchar *image_types,
                               GimpPDBProcType type,
                               gint n_params,
                               gint n_return_vals,
                               const GimpParamDef *params,
                               const GimpParamDef *return_vals);
GimpImageType (*gimp_drawable_type)(gint32 drawable_ID);
GimpDrawable *(*gimp_drawable_get)(gint32 drawable_ID);
gboolean (*gimp_progress_init)(const gchar *message);
gboolean (*gimp_progress_update)(gdouble percentage);
void (*gimp_pixel_rgn_init)(GimpPixelRgn *pr,
                            GimpDrawable *drawable,
                            gint x,
                            gint y,
                            gint width,
                            gint height,
                            gint dirty,
                            gint shadow);
void (*gimp_pixel_rgn_get_rect) (GimpPixelRgn *pr,
                            guchar *buf,
                            gint x,
                            gint y,
                            gint width,
                            gint height);
gboolean (*gimp_register_save_handler)(const gchar *procedure_name,
                                       const gchar *extensions,
                                       const gchar *prefixes);
guchar *(*gimp_image_get_colormap)(gint32 image_ID, gint *num_colors);
gint (*gimp_main)(const GimpPlugInInfo *info, gint argc, gchar *argv[]);
GtkWidget *(*gimp_dialog_new)(const gchar *title,
                              const gchar *role,
                              GtkWidget *parent,
                              GtkDialogFlags flags,
                              GimpHelpFunc help_func,
                              const gchar *help_id, ...) G_GNUC_NULL_TERMINATED;
gint (*gimp_dialog_run)(GimpDialog *dialog);
GType (*gimp_dialog_get_type)(void) G_GNUC_CONST;
const gchar *(*gimp_plug_in_directory_)(void);

int Load_GimpLib(void)
{
    int Result = TRUE;
    if(!GimpLibBase)
    {
        GimpLibBase = LoadLibrary("libgimp-2.0-0.dll");
        if(GimpLibBase)
        {
            gimp_install_procedure = (void *) GetProcAddress(GimpLibBase, "gimp_install_procedure");
            gimp_drawable_type = (void *) GetProcAddress(GimpLibBase, "gimp_drawable_type");
            gimp_drawable_get = (void *) GetProcAddress(GimpLibBase, "gimp_drawable_get");
            gimp_progress_init = (void *) GetProcAddress(GimpLibBase, "gimp_progress_init");
            gimp_progress_update = (void *) GetProcAddress(GimpLibBase, "gimp_progress_update");
            gimp_pixel_rgn_init = (void *) GetProcAddress(GimpLibBase, "gimp_pixel_rgn_init");
            gimp_pixel_rgn_get_rect = (void *) GetProcAddress(GimpLibBase, "gimp_pixel_rgn_get_rect");
            gimp_register_save_handler = (void *) GetProcAddress(GimpLibBase, "gimp_register_save_handler");
            gimp_image_get_colormap = (void *) GetProcAddress(GimpLibBase, "gimp_image_get_colormap");
            gimp_main = (void *) GetProcAddress(GimpLibBase, "gimp_main");      
        }
        else
        {
            Result = FALSE;
        }
    }
    if(!GimpBaseLibBase)
    {
        GimpBaseLibBase = LoadLibrary("libgimpbase-2.0-0.dll");
        if(GimpBaseLibBase)
        {
            gimp_plug_in_directory_ = (void *) GetProcAddress(GimpBaseLibBase, "gimp_plug_in_directory");
        }
        else
        {
            Result = FALSE;
        }
    }
    if(!GimpUiLibBase)
    {
        GimpUiLibBase = LoadLibrary("libgimpui-2.0-0.dll");
        if(GimpUiLibBase)
        {
            gimp_ui_init = (void *) GetProcAddress(GimpUiLibBase, "gimp_ui_init");
        }
        else
        {
            Result = FALSE;
        }
    }
    if(!GimpWidgetsLibBase)
    {
        GimpWidgetsLibBase = LoadLibrary("libgimpwidgets-2.0-0.dll");
        if(GimpWidgetsLibBase)
        {
            gimp_dialog_new = (void *) GetProcAddress(GimpWidgetsLibBase, "gimp_dialog_new");
            gimp_dialog_run = (void *) GetProcAddress(GimpWidgetsLibBase, "gimp_dialog_run");
            gimp_dialog_get_type = (void *) GetProcAddress(GimpWidgetsLibBase, "gimp_dialog_get_type");
        }
        else
        {
            Result = FALSE;
        }
    }
    return(Result);
}
