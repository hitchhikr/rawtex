/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimp.h
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GIMP_H__
#define __GIMP_H__

#include <glib-object.h>

#include <libgimpbase/gimpbase.h>
#include <libgimpcolor/gimpcolor.h>
#include <libgimpconfig/gimpconfig.h>
#include <libgimpmath/gimpmath.h>

#include <libgimp/gimpenums.h>
#include <libgimp/gimptypes.h>

#include <libgimp/gimpbrushes.h>
#include <libgimp/gimpbrushselect.h>
#include <libgimp/gimpchannel.h>
#include <libgimp/gimpdrawable.h>
#include <libgimp/gimpfontselect.h>
#include <libgimp/gimpgimprc.h>
#include <libgimp/gimpgradients.h>
#include <libgimp/gimpgradientselect.h>
#include <libgimp/gimpimage.h>
#include <libgimp/gimplayer.h>
#include <libgimp/gimppalette.h>
#include <libgimp/gimppalettes.h>
#include <libgimp/gimppaletteselect.h>
#include <libgimp/gimppatterns.h>
#include <libgimp/gimppatternselect.h>
#include <libgimp/gimppixelfetcher.h>
#include <libgimp/gimppixelrgn.h>
#include <libgimp/gimpplugin.h>
#include <libgimp/gimpproceduraldb.h>
#include <libgimp/gimpprogress.h>
#include <libgimp/gimpregioniterator.h>
#include <libgimp/gimpselection.h>
#include <libgimp/gimptile.h>

#include <libgimp/gimp_pdb.h>

#ifdef G_OS_WIN32
#include <stdlib.h> /* For __argc and __argv */
#endif


G_BEGIN_DECLS


#define gimp_get_data         gimp_procedural_db_get_data
#define gimp_get_data_size    gimp_procedural_db_get_data_size
#define gimp_set_data         gimp_procedural_db_set_data


typedef void (* GimpInitProc)  (void);
typedef void (* GimpQuitProc)  (void);
typedef void (* GimpQueryProc) (void);
typedef void (* GimpRunProc)   (const gchar      *name,
                                gint              n_params,
                                const GimpParam  *param,
                                gint             *n_return_vals,
                                GimpParam       **return_vals);


struct _GimpPlugInInfo
{
  /* called when the gimp application initially starts up */
  GimpInitProc  init_proc;

  /* called when the gimp application exits */
  GimpQuitProc  quit_proc;

  /* called by the gimp so that the plug-in can inform the
   *  gimp of what it does. (ie. installing a procedure database
   *  procedure).
   */
  GimpQueryProc query_proc;

  /* called to run a procedure the plug-in installed in the
   *  procedure database.
   */
  GimpRunProc   run_proc;
};

struct _GimpParamDef
{
  GimpPDBArgType  type;
  gchar          *name;
  gchar          *description;
};

struct _GimpParamRegion
{
  gint32 x;
  gint32 y;
  gint32 width;
  gint32 height;
};

union _GimpParamData
{
  gint32            d_int32;
  gint16            d_int16;
  guint8            d_int8;
  gdouble           d_float;
  gchar            *d_string;
  gint32           *d_int32array;
  gint16           *d_int16array;
  guint8           *d_int8array;
  gdouble          *d_floatarray;
  gchar           **d_stringarray;
  GimpRGB           d_color;
  GimpParamRegion   d_region;
  gint32            d_display;
  gint32            d_image;
  gint32            d_layer;
  gint32            d_layer_mask;
  gint32            d_channel;
  gint32            d_drawable;
  gint32            d_selection;
  gint32            d_boundary;
  gint32            d_path; /* deprecated */
  gint32            d_vectors;
  gint32            d_unit;
  GimpParasite      d_parasite;
  gint32            d_tattoo;
  GimpPDBStatusType d_status;
};

struct _GimpParam
{
  GimpPDBArgType type;
  GimpParamData  data;
};



/**
 * MAIN():
 *
 * A macro that expands to the appropriate main() function for the
 * platform being compiled for.
 *
 * To use this macro, simply place a line that contains just the code
 * MAIN() at the toplevel of your file.  No semicolon should be used.
 *
 */

#ifdef G_OS_WIN32

/* Define WinMain() because plug-ins are built as GUI applications. Also
 * define a main() in case some plug-in still is built as a console
 * application.
 */
#  ifdef __GNUC__
#    ifndef _stdcall
#      define _stdcall __attribute__((stdcall))
#    endif
#  endif

#  define MAIN()                                        \
   struct HINSTANCE__;                                  \
                                                        \
   int _stdcall                                         \
   WinMain (struct HINSTANCE__ *hInstance,              \
            struct HINSTANCE__ *hPrevInstance,          \
            char *lpszCmdLine,                          \
            int   nCmdShow);                            \
                                                        \
   int _stdcall                                         \
   WinMain (struct HINSTANCE__ *hInstance,              \
            struct HINSTANCE__ *hPrevInstance,          \
            char *lpszCmdLine,                          \
            int   nCmdShow)                             \
   {                                                    \
     return gimp_main (&PLUG_IN_INFO, __argc, __argv);  \
   }                                                    \
                                                        \
   int                                                  \
   main (int argc, char *argv[])                        \
   {                                                    \
     return gimp_main (&PLUG_IN_INFO, argc, argv);      \
   }
#else
#  define MAIN()                                        \
   int                                                  \
   main (int argc, char *argv[])                        \
   {                                                    \
     return gimp_main (&PLUG_IN_INFO, argc, argv);      \
   }
#endif


/* The main procedure that must be called with the PLUG_IN_INFO structure
 * and the 'argc' and 'argv' that are passed to "main".
 */
#ifndef G_OS_WIN32

gint           gimp_main                (const GimpPlugInInfo *info,
                                         gint                  argc,
                                         gchar                *argv[]);

#endif

/* Forcefully causes the gimp library to exit and
 *  close down its connection to main gimp application.
 */
void           gimp_quit                (void) G_GNUC_NORETURN;


/* Install a procedure in the procedure database.
 */
#ifndef G_OS_WIN32

void           gimp_install_procedure   (const gchar        *name,
                                         const gchar        *blurb,
                                         const gchar        *help,
                                         const gchar        *author,
                                         const gchar        *copyright,
                                         const gchar        *date,
                                         const gchar        *menu_label,
                                         const gchar        *image_types,
                                         GimpPDBProcType     type,
                                         gint                n_params,
                                         gint                n_return_vals,
                                         const GimpParamDef *params,
                                         const GimpParamDef *return_vals);
#endif


/* Install a temporary procedure in the procedure database.
 */
void           gimp_install_temp_proc   (const gchar        *name,
                                         const gchar        *blurb,
                                         const gchar        *help,
                                         const gchar        *author,
                                         const gchar        *copyright,
                                         const gchar        *date,
                                         const gchar        *menu_label,
                                         const gchar        *image_types,
                                         GimpPDBProcType     type,
                                         gint                n_params,
                                         gint                n_return_vals,
                                         const GimpParamDef *params,
                                         const GimpParamDef *return_vals,
                                         GimpRunProc         run_proc);

/* Uninstall a temporary procedure
 */
void           gimp_uninstall_temp_proc (const gchar        *name);

/* Notify the main GIMP application that the extension is ready to run
 */
void           gimp_extension_ack       (void);

/* Enable asynchronous processing of temp_procs
 */
void           gimp_extension_enable    (void);

/* Process one temp_proc and return
 */
void           gimp_extension_process   (guint            timeout);

/* Run a procedure in the procedure database. The parameters are
 *  specified via the variable length argument list. The return
 *  values are returned in the 'GimpParam*' array.
 */
GimpParam    * gimp_run_procedure       (const gchar     *name,
                                         gint            *n_return_vals,
                                         ...);

/* Run a procedure in the procedure database. The parameters are
 *  specified as an array of GimpParam.  The return
 *  values are returned in the 'GimpParam*' array.
 */
GimpParam    * gimp_run_procedure2      (const gchar     *name,
                                         gint            *n_return_vals,
                                         gint             n_params,
                                         const GimpParam *params);

/* Destroy the an array of parameters. This is useful for
 *  destroying the return values returned by a call to
 *  'gimp_run_procedure'.
 */
void           gimp_destroy_params      (GimpParam       *params,
                                         gint             n_params);

/* Destroy the an array of GimpParamDef's. This is useful for
 *  destroying the return values returned by a call to
 *  'gimp_query_procedure'.
 */
void           gimp_destroy_paramdefs   (GimpParamDef    *paramdefs,
                                         gint             n_params);


/* Return various constants given by the GIMP core at plug-in config time.
 */
guint          gimp_tile_width          (void) G_GNUC_CONST;
guint          gimp_tile_height         (void) G_GNUC_CONST;
gint           gimp_shm_ID              (void) G_GNUC_CONST;
guchar       * gimp_shm_addr            (void) G_GNUC_CONST;
gdouble        gimp_gamma               (void) G_GNUC_CONST;
gboolean       gimp_install_cmap        (void) G_GNUC_CONST;
gint           gimp_min_colors          (void) G_GNUC_CONST;
gboolean       gimp_show_tool_tips      (void) G_GNUC_CONST;
gboolean       gimp_show_help_button    (void) G_GNUC_CONST;
GimpCheckSize  gimp_check_size          (void) G_GNUC_CONST;
GimpCheckType  gimp_check_type          (void) G_GNUC_CONST;
gint32         gimp_default_display     (void) G_GNUC_CONST;
const gchar  * gimp_wm_class            (void) G_GNUC_CONST;
const gchar  * gimp_display_name        (void) G_GNUC_CONST;
gint           gimp_monitor_number      (void) G_GNUC_CONST;

const gchar  * gimp_get_progname        (void) G_GNUC_CONST;

gboolean       gimp_attach_new_parasite (const gchar    *name,
                                         gint            flags,
                                         gint            size,
                                         gconstpointer   data);


G_END_DECLS

#endif /* __GIMP_H__ */
