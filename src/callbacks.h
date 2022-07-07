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

// ----------------------------------------------------
// Functions
void on_CbTex_changed(GtkComboBox *combobox, gpointer user_data);
void on_CbPal_changed(GtkComboBox *combobox, gpointer user_data);
void on_CbMipmap_changed(GtkComboBox *combobox, gpointer user_data);
void on_CbDxt_changed(GtkComboBox *combobox, gpointer user_data);
void on_CbIdxType_changed(GtkComboBox *combobox, gpointer user_data);
void on_TxtColorKey_editing_done(GtkCellEditable *celleditable, gpointer user_data);
void on_ChkKey_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_ChkBGR_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_ChkSwizzle_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_ChkSrcPicture_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_ChkSrcPalette_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_BtnCancel_clicked(GtkButton *button, gpointer user_data);
void on_BtnOk_clicked(GtkButton *button, gpointer user_data);
void on_BtnColor_clicked(GtkButton *button, gpointer user_data);
void on_Color_Changed(GtkColorSelection *ColorSelection, gpointer user_data);
