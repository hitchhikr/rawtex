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
#include <gtk/gtk.h>
#include "support.h"

GtkWidget *lookup_widget(GtkWidget *widget, const gchar *widget_name)
{
    GtkWidget *parent;
    GtkWidget *found_widget;

    for(;;)
    {
        if(GTK_IS_MENU (widget)) parent = gtk_menu_get_attach_widget(GTK_MENU(widget));
        else parent = widget->parent;
        if(!parent) parent = (GtkWidget *) g_object_get_data(G_OBJECT (widget), "GladeParentKey");
        if(parent == NULL) break;
        widget = parent;
    }

    found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (widget), widget_name);
    if(!found_widget) g_warning("Widget not found: %s", widget_name);
    return(found_widget);
}

/* This is used to set ATK action descriptions. */
void glade_set_atk_action_description(AtkAction *action, const gchar *action_name, const gchar *description)
{
    gint n_actions;
    gint i;

    n_actions = atk_action_get_n_actions(action);
    for(i = 0; i < n_actions; i++)
    {
        if(!strcmp (atk_action_get_name(action, i), action_name)) atk_action_set_description(action, i, description);
    }
}
