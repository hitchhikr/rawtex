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
#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "rawtex.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"

// ----------------------------------------------------
// Constants
#define COLOR_NAME "COLOR"

// ----------------------------------------------------
// Functions
#ifdef G_OS_WIN32
extern void (*gimp_ui_init)(const gchar *prog_name, gboolean preview);
extern GtkWidget *(*gimp_dialog_new)(const gchar *title, const gchar *role, GtkWidget *parent, GtkDialogFlags flags, GimpHelpFunc help_func, const gchar *help_id, ...);
extern gint (*gimp_dialog_run)(GimpDialog *dialog);
extern GType (*gimp_dialog_get_type)(void) G_GNUC_CONST;
#endif

// ----------------------------------------------------
// Variables
extern GtkWidget *vbox2;
extern GtkWidget *ImgLogo;
extern GtkWidget *LblCredits;
extern GtkWidget *table1;
extern GtkWidget *CbTex;
extern GtkWidget *LblTex;
extern GtkWidget *CbPal;
extern GtkWidget *LblPal;
extern GtkWidget *CbMipmap;
extern GtkWidget *LblMipmap;
extern GtkWidget *CbDxt;
extern GtkWidget *LblIdxType;
extern GtkWidget *CbIdxType;
extern GtkWidget *LblDxt;
extern GtkWidget *ChkKey;
extern GtkWidget *ChkBGR;
extern GtkWidget *ChkSwizzle;
extern GtkWidget *ChkSrcPicture;
extern GtkWidget *ChkSrcPalette;
extern GtkWidget *hbox1;
extern GtkWidget *BtnColor;
extern GtkWidget *LblColor;
extern char string[128];

void on_CbTex_changed(GtkComboBox *combobox, gpointer user_data)
{
    int cb_value;

    cb_value = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
    switch(cb_value)
    {
        case CB_IDX_5650:
            values_defaults.Dest_Format = GU_PSM_5650;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = TRUE;
            break;
        case CB_IDX_5551:
            values_defaults.Dest_Format = GU_PSM_5551;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = TRUE;
            break;
        case CB_IDX_4444:
            values_defaults.Dest_Format = GU_PSM_4444;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = TRUE;
            break;
        case CB_IDX_8888:
            values_defaults.Dest_Format = GU_PSM_8888;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = TRUE;
            break;
        case CB_IDX_T4:
            values_defaults.Dest_Format = GU_PSM_T4;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), FALSE);
            values_defaults.Use_Swizzle_Texture = TRUE;
            break;
        case CB_IDX_T8:
            values_defaults.Dest_Format = GU_PSM_T8;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), FALSE);
            values_defaults.Use_Swizzle_Texture = TRUE;
            break;
        case CB_IDX_DXT1:
            values_defaults.Dest_Format = GU_PSM_DXT1;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = FALSE;
            break;
        case CB_IDX_DXT3:
            values_defaults.Dest_Format = GU_PSM_DXT3;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = FALSE;
            break;
        case CB_IDX_DXT5:
            values_defaults.Dest_Format = GU_PSM_DXT5;
            gtk_widget_set_sensitive(GTK_WIDGET(CbPal), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbIdxType), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSwizzle), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkSrcPalette), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(CbDxt), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ChkBGR), TRUE);
            values_defaults.Use_Swizzle_Texture = FALSE;
            break;
    }       
}

void on_CbPal_changed(GtkComboBox *combobox, gpointer user_data)
{
     values_defaults.Dest_Palette_Format = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
}

void on_CbMipmap_changed(GtkComboBox *combobox, gpointer user_data)
{
     values_defaults.Nbr_MipMaps = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
}

void on_CbDxt_changed(GtkComboBox *combobox, gpointer user_data)
{
     values_defaults.Dxt_Type = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
}

void on_CbIdxType_changed(GtkComboBox *combobox, gpointer user_data)
{
     values_defaults.Idx_Type = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
}

void on_ChkKey_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    int chk_value;

    chk_value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
    values_defaults.Use_Alpha_Color = chk_value;
    gtk_widget_set_sensitive(GTK_WIDGET(BtnColor), chk_value);
}

void on_ChkBGR_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    int chk_value;

    chk_value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
    values_defaults.Use_BGR = chk_value;
}

void on_ChkSwizzle_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    int chk_value;

    chk_value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
    values_defaults.Swizzle_Texture = chk_value;

    switch(values_defaults.Dest_Format)
    {
        case GU_PSM_DXT1:
        case GU_PSM_DXT3:
        case GU_PSM_DXT5:
            values_defaults.Use_Swizzle_Texture = FALSE;
            break;
        default:
            values_defaults.Use_Swizzle_Texture = chk_value;
            break;
    }       
}

void on_ChkSrcPicture_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    int chk_value;

    chk_value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
    values_defaults.Save_Picture_Source = chk_value;
}

void on_ChkSrcPalette_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    int chk_value;

    chk_value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton));
    values_defaults.Save_Palette_Source = chk_value;
}

void on_BtnColor_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *dlg;
    gint run;
    gboolean ok;
    guint old_color;

    old_color = values_defaults.Alpha_Color;

    gimp_ui_init(COLOR_NAME, TRUE);
    dlg = gimp_dialog_new("Choose the color key", COLOR_NAME,
                          NULL, (GtkDialogFlags) 0,
                          NULL, NULL,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OK, GTK_RESPONSE_OK,
                          NULL);
    create_winColor (dlg);

    gtk_widget_show_all(dlg);

    ok = FALSE;
    do
    {
        run = gimp_dialog_run(GIMP_DIALOG(dlg));
        switch (run)
        {
            default:
                ok = TRUE;
                break;
        }
    } while(!ok);

    gtk_widget_destroy(dlg);
    if(run != GTK_RESPONSE_OK)
    {
        values_defaults.Alpha_Color = old_color;
        sprintf(string, "#%.6X",  old_color);
        gtk_label_set_text(GTK_LABEL(LblColor), string);
    }
}

void on_Color_Changed(GtkColorSelection *ColorSelection, gpointer user_data)
{
    GdkColor color_value;
    double val_red;
    double val_green;
    double val_blue;
    int ired;
    int igreen;
    int iblue;

    gtk_color_selection_get_current_color(GTK_COLOR_SELECTION(ColorSelection), &color_value);
    val_red = ((color_value.red * 256.0) / 65536.0);
    val_green = (((color_value.green * 256.0) / 65536.0));
    val_blue = ((color_value.blue * 256.0) / 65536.0);
    ired = (int) val_red << 16;
    igreen = (int) val_green << 8;
    iblue = (int) val_blue;
    values_defaults.Alpha_Color = ired;
    values_defaults.Alpha_Color |= igreen;
    values_defaults.Alpha_Color |= iblue;
    sprintf(string, "#%.6X", (guint) values_defaults.Alpha_Color);
    gtk_label_set_text(GTK_LABEL(LblColor), string);
}
