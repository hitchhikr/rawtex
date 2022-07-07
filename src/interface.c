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
#include <sys/types.h>
#include <sys/stat.h>
#ifndef G_OS_WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

// ----------------------------------------------------
// Variables
GtkWidget *vbox2;
GtkWidget *ImgLogo;
GtkWidget *LblCredits;
GtkWidget *table1;
GtkWidget *CbTex;
GtkWidget *LblTex;
GtkWidget *CbPal;
GtkWidget *LblPal;
GtkWidget *CbMipmap;
GtkWidget *LblMipmap;
GtkWidget *ChkKey;
GtkWidget *LblIdxType;
GtkWidget *CbIdxType;
GtkWidget *ChkBGR;
GtkWidget *ChkSwizzle;
GtkWidget *ChkSrcPicture;
GtkWidget *ChkSrcPalette;
GtkWidget *hbox1;
GtkWidget *BtnColor;
GtkWidget *LblColor;
GtkWidget *CbDxt;
GtkWidget *LblDxt;
char string[128];

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
    g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
    g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget *create_winMain(GtkWidget *winMain, LPPluginValues Init_Values)
{
    int dest_value;

    gtk_widget_set_name(winMain, "RAWTEX");
    gtk_window_set_title(GTK_WINDOW (winMain), "RAWTEX");
    vbox2 = (GTK_DIALOG(winMain)->vbox);

    sprintf(string, "RAWTEX %s - Written by Franck Charlet.", PLUGIN_VERSION);
    LblCredits = gtk_label_new (string);
    gtk_widget_set_name(LblCredits, "LblCredits");
    gtk_widget_show(LblCredits);
    gtk_box_pack_start(GTK_BOX (vbox2), LblCredits, FALSE, FALSE, 7);

    table1 = gtk_table_new(5, 2, FALSE);
    gtk_widget_set_name(table1, "table1");
    gtk_widget_show(table1);
    gtk_box_pack_start(GTK_BOX (vbox2), table1, TRUE, TRUE, 0);
    gtk_table_set_row_spacings(GTK_TABLE (table1), 10);
    gtk_table_set_col_spacings(GTK_TABLE (table1), 6);

    CbTex = gtk_combo_box_entry_new_text ();
    gtk_widget_set_name(CbTex, "CbTex");
    gtk_widget_show(CbTex);
    gtk_table_attach(GTK_TABLE (table1), CbTex, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "RGB 5650");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "RGB 5551");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "RGB 4444");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "RGB 8888");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "Indexed 4 Bit");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "Indexed 8 Bit");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "Packed DXT1");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "Packed DXT3");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbTex), "Packed DXT5");

    LblTex = gtk_label_new("Texture format");
    gtk_widget_set_name(LblTex, "LblTex");
    gtk_widget_show(LblTex);
    gtk_table_attach(GTK_TABLE (table1), LblTex, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC (LblTex), 0, 0.5);

    CbPal = gtk_combo_box_entry_new_text();
    gtk_widget_set_name(CbPal, "CbPal");
    gtk_widget_show(CbPal);
    gtk_table_attach(GTK_TABLE (table1), CbPal, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:0 R:5 G:6 B:5");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:1 R:5 G:5 B:5");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:4 R:4 G:4 B:4");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:8 R:8 G:8 B:8");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:0 R:3 G:3 B:3");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:0 R:4 G:4 B:4");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbPal), "A:0 R:8 G:8 B:8");
    
    LblPal = gtk_label_new("Palette format");
    gtk_widget_set_name(LblPal, "LblPal");
    gtk_widget_show(LblPal);
    gtk_table_attach(GTK_TABLE (table1), LblPal, 0, 1, 1, 2,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC (LblPal), 0, 0.5);

    CbMipmap = gtk_combo_box_entry_new_text();
    gtk_widget_set_name(CbMipmap, "CbMipmap");
    gtk_widget_show(CbMipmap);
    gtk_table_attach(GTK_TABLE (table1), CbMipmap, 1, 2, 2, 3,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "(None)");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "1");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "2");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "3");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "4");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "5");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "6");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbMipmap), "7");

    LblMipmap = gtk_label_new("Mipmaps");
    gtk_widget_set_name(LblMipmap, "LblMipmap");
    gtk_widget_show(LblMipmap);
    gtk_table_attach(GTK_TABLE (table1), LblMipmap, 0, 1, 2, 3,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC (LblMipmap), 0, 0.5);
    
    CbDxt = gtk_combo_box_entry_new_text();
    gtk_widget_set_name(CbDxt, "CbDxt");
    gtk_widget_show(CbDxt);
    gtk_table_attach(GTK_TABLE (table1), CbDxt, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbDxt), "PlayStation Portable");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbDxt), "Standard OpenGL");

    LblDxt = gtk_label_new("DXT format");
    gtk_widget_set_name(LblDxt, "LblDxt");
    gtk_widget_show(LblDxt);
    gtk_table_attach(GTK_TABLE (table1), LblDxt, 0, 1, 3, 4,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC (LblDxt), 0, 0.5);

    CbIdxType = gtk_combo_box_entry_new_text();
    gtk_widget_set_name(CbIdxType, "CbIdxType");
    gtk_widget_show(CbIdxType);
    gtk_table_attach(GTK_TABLE (table1), CbIdxType, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbIdxType), "Chunky");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbIdxType), "Amiga Planar");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbIdxType), "Amiga Planar Interleaved");
    gtk_combo_box_append_text(GTK_COMBO_BOX (CbIdxType), "Atari ST Planar");

    LblIdxType = gtk_label_new("Indexed type");
    gtk_widget_set_name(LblIdxType, "LblIdxType");
    gtk_widget_show(LblIdxType);
    gtk_table_attach(GTK_TABLE (table1), LblIdxType, 0, 1, 4, 5,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment(GTK_MISC (LblIdxType), 0, 0.5);

    ChkKey = gtk_check_button_new_with_mnemonic ("Use color key");
    gtk_widget_set_name(ChkKey, "ChkKey");
    gtk_widget_show(ChkKey);
    gtk_table_attach(GTK_TABLE (table1), ChkKey, 0, 1, 5, 6,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);
    
    ChkBGR = gtk_check_button_new_with_mnemonic ("Save data as BGR");
    gtk_widget_set_name(ChkBGR, "ChkBGR");
    gtk_widget_show(ChkBGR);
    gtk_table_attach(GTK_TABLE (table1), ChkBGR, 0, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

    ChkSwizzle = gtk_check_button_new_with_mnemonic ("Swizzle mode (PSP only)");
    gtk_widget_set_name(ChkSwizzle, "ChkSwizzle");
    gtk_widget_show(ChkSwizzle);
    gtk_table_attach(GTK_TABLE (table1), ChkSwizzle, 0, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

    ChkSrcPicture = gtk_check_button_new_with_mnemonic ("Save picture data as source code");
    gtk_widget_set_name(ChkSrcPicture, "ChkSrcPicture");
    gtk_widget_show(ChkSrcPicture);
    gtk_table_attach(GTK_TABLE (table1), ChkSrcPicture, 0, 2, 8, 9,
                    (GtkAttachOptions) (GTK_FILL),
                  ( GtkAttachOptions) (0), 0, 0);

    ChkSrcPalette = gtk_check_button_new_with_mnemonic ("Save palette data as source code");
    gtk_widget_set_name(ChkSrcPalette, "ChkSrcPalette");
    gtk_widget_show(ChkSrcPalette);
    gtk_table_attach(GTK_TABLE (table1), ChkSrcPalette, 0, 2, 9, 10,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (0), 0, 0);

    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_set_name(hbox1, "hbox1");
    gtk_widget_show(hbox1);
    gtk_table_attach(GTK_TABLE (table1), hbox1, 1, 2, 5, 6,
                  (GtkAttachOptions) (GTK_FILL),
                  (GtkAttachOptions) (GTK_FILL), 0, 0);

    BtnColor = gtk_button_new_from_stock("gtk-select-color");
    gtk_widget_set_name(BtnColor, "BtnColor");
    gtk_widget_show(BtnColor);
    gtk_box_pack_start(GTK_BOX (hbox1), BtnColor, TRUE, TRUE, 20);
    if(Init_Values->Use_Alpha_Color) gtk_widget_set_sensitive(GTK_WIDGET(BtnColor), TRUE);
    else gtk_widget_set_sensitive(GTK_WIDGET(BtnColor), FALSE);
    LblColor = gtk_label_new("#000000");
    gtk_widget_set_name(LblColor, "LblColor");
    gtk_widget_show(LblColor);
    gtk_box_pack_start(GTK_BOX (hbox1), LblColor, FALSE, FALSE, 0);
    gtk_label_set_justify(GTK_LABEL (LblColor), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment(GTK_MISC (LblColor), 1, 0.48f);
    gtk_misc_set_padding(GTK_MISC (LblColor), 22, 0);
    
    g_signal_connect((gpointer) CbTex, "changed", G_CALLBACK(on_CbTex_changed), NULL);
    g_signal_connect((gpointer) CbPal, "changed", G_CALLBACK(on_CbPal_changed), NULL);
    g_signal_connect((gpointer) CbMipmap, "changed", G_CALLBACK(on_CbMipmap_changed), NULL);
    g_signal_connect((gpointer) CbDxt, "changed", G_CALLBACK(on_CbDxt_changed), NULL);
    g_signal_connect((gpointer) CbIdxType, "changed", G_CALLBACK(on_CbIdxType_changed), NULL);
    g_signal_connect((gpointer) ChkKey, "toggled", G_CALLBACK(on_ChkKey_toggled), NULL);
    g_signal_connect((gpointer) ChkSwizzle, "toggled", G_CALLBACK(on_ChkSwizzle_toggled), NULL);
    g_signal_connect((gpointer) ChkBGR, "toggled", G_CALLBACK(on_ChkBGR_toggled), NULL);
    g_signal_connect((gpointer) ChkSrcPicture, "toggled", G_CALLBACK(on_ChkSrcPicture_toggled), NULL);
    g_signal_connect((gpointer) ChkSrcPalette, "toggled", G_CALLBACK(on_ChkSrcPalette_toggled), NULL);
    g_signal_connect((gpointer) BtnColor, "clicked", G_CALLBACK(on_BtnColor_clicked), NULL);

    // Init the controls
    dest_value = GU_PSM_5650;
    switch(Init_Values->Dest_Format)
    {
        case GU_PSM_5650:
            dest_value = CB_IDX_5650;
            break;
        case GU_PSM_5551:
            dest_value = CB_IDX_5551;
            break;
        case GU_PSM_4444:
            dest_value = CB_IDX_4444;
            break;
        case GU_PSM_8888:
            dest_value = CB_IDX_8888;
            break;
        case GU_PSM_T4:
            dest_value = CB_IDX_T4;
            break;
        case GU_PSM_T8:
            dest_value = CB_IDX_T8;
            break;
        case GU_PSM_DXT1:
            dest_value = CB_IDX_DXT1;
            break;
        case GU_PSM_DXT3:
            dest_value = CB_IDX_DXT3;
            break;
        case GU_PSM_DXT5:
            dest_value = CB_IDX_DXT5;
            break;
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(CbTex), dest_value);
    gtk_combo_box_set_active(GTK_COMBO_BOX(CbPal), Init_Values->Dest_Palette_Format);
    gtk_combo_box_set_active(GTK_COMBO_BOX(CbMipmap), Init_Values->Nbr_MipMaps);
    gtk_combo_box_set_active(GTK_COMBO_BOX(CbDxt), Init_Values->Dxt_Type);
    gtk_combo_box_set_active(GTK_COMBO_BOX(CbIdxType), Init_Values->Idx_Type);
    sprintf(string, "#%.6X",  Init_Values->Alpha_Color);
    gtk_label_set_text(GTK_LABEL(LblColor), string);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ChkBGR), Init_Values->Use_BGR);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ChkSwizzle), Init_Values->Swizzle_Texture);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ChkSrcPicture), Init_Values->Save_Picture_Source);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ChkSrcPalette), Init_Values->Save_Palette_Source);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ChkKey), Init_Values->Use_Alpha_Color);

    return(winMain);
}

GtkWidget *create_winColor(GtkWidget *winColor)
{
    GtkWidget *color_selection1;
    GtkWidget *vColorbox;
    GdkColor color_value;
    double val_red;
    double val_green;
    double val_blue;

    gtk_widget_set_name(winColor, "COLOR");
    gtk_window_set_title(GTK_WINDOW (winColor), "COLOR");
    vColorbox = (GTK_DIALOG(winColor)->vbox);

    color_selection1 = gtk_color_selection_new();
    if(color_selection1)
    {
        gtk_widget_set_name (color_selection1, "color_selection1");
        gtk_widget_show (color_selection1);
        gtk_box_pack_start (GTK_BOX (vColorbox), color_selection1, TRUE, TRUE, 0);
        gtk_misc_set_alignment (GTK_MISC (vColorbox), 1, 0.5);
        gtk_color_selection_set_has_opacity_control (GTK_COLOR_SELECTION (color_selection1), FALSE);
        color_value.pixel = values_defaults.Alpha_Color;
        val_red = ((values_defaults.Alpha_Color >> 16) & 0xff);
        color_value.red = (guint16) ((val_red * 65535.0) / 255.0);
        val_green = ((values_defaults.Alpha_Color >> 8) & 0xff);
        color_value.green = (guint16) ((val_green * 65535.0) / 255.0);
        val_blue = (values_defaults.Alpha_Color & 0xff);
        color_value.blue = (guint16) ((val_blue * 65535.0) / 255.0);
        gtk_color_selection_set_current_color(GTK_COLOR_SELECTION(color_selection1), &color_value);

        g_signal_connect((gpointer) color_selection1, "color_changed", G_CALLBACK(on_Color_Changed), NULL);
    }
    return(winColor);
}
