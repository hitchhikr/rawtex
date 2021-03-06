/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimppixelrgn.h
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

#ifndef __GIMP_PIXEL_RGN_H__
#define __GIMP_PIXEL_RGN_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


struct _GimpPixelRgn
{
  guchar       *data;          /* pointer to region data */
  GimpDrawable *drawable;      /* pointer to drawable */
  guint         bpp;           /* bytes per pixel */
  guint         rowstride;     /* bytes per pixel row */
  guint         x, y;          /* origin */
  guint         w, h;          /* width and height of region */
  guint         dirty : 1;     /* will this region be dirtied? */
  guint         shadow : 1;    /* will this region use the shadow or normal tiles */
  guint         process_count; /* used internally */
};

#ifndef G_OS_WIN32

void      gimp_pixel_rgn_init       (GimpPixelRgn  *pr,
                                     GimpDrawable  *drawable,
                                     gint           x,
                                     gint           y,
                                     gint           width,
                                     gint           height,
                                     gint           dirty,
                                     gint           shadow);

#endif

void      gimp_pixel_rgn_resize     (GimpPixelRgn  *pr,
                                     gint           x,
                                     gint           y,
                                     gint           width,
                                     gint           height);
void      gimp_pixel_rgn_get_pixel  (GimpPixelRgn  *pr,
                                     guchar        *buf,
                                     gint           x,
                                     gint           y);
void      gimp_pixel_rgn_get_row    (GimpPixelRgn  *pr,
                                     guchar        *buf,
                                     gint           x,
                                     gint           y,
                                     gint           width);
void      gimp_pixel_rgn_get_col    (GimpPixelRgn  *pr,
                                     guchar        *buf,
                                     gint           x,
                                     gint           y,
                                     gint           height);
#ifndef G_OS_WIN32

void      gimp_pixel_rgn_get_rect   (GimpPixelRgn  *pr,
                                     guchar        *buf,
                                     gint           x,
                                     gint           y,
                                     gint           width,
                                     gint           height);

#endif

void      gimp_pixel_rgn_set_pixel  (GimpPixelRgn  *pr,
                                     const guchar  *buf,
                                     gint           x,
                                     gint           y);
void      gimp_pixel_rgn_set_row    (GimpPixelRgn  *pr,
                                     const guchar  *buf,
                                     gint           x,
                                     gint           y,
                                     gint           width);
void      gimp_pixel_rgn_set_col    (GimpPixelRgn  *pr,
                                     const guchar  *buf,
                                     gint           x,
                                     gint           y,
                                     gint           height);
void      gimp_pixel_rgn_set_rect   (GimpPixelRgn  *pr,
                                     const guchar  *buf,
                                     gint           x,
                                     gint           y,
                                     gint           width,
                                     gint           height);
gpointer  gimp_pixel_rgns_register  (gint           nrgns,
                                     ...);
gpointer  gimp_pixel_rgns_register2 (gint           nrgns,
                                     GimpPixelRgn **prs);
gpointer  gimp_pixel_rgns_process   (gpointer       pri_ptr);


G_END_DECLS

#endif /* __GIMP_PIXEL_RGN_H__ */
