/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
    Gpredict: Real-time satellite tracking and orbit prediction program

    Copyright (C)  2001-2007  Alexandru Csete, OZ9AEC.

    Authors: Alexandru Csete <oz9aec@gmail.com>

    Comments, questions and bugreports should be submitted via
    http://sourceforge.net/projects/groundstation/
    More details can be found at the project home page:

            http://groundstation.sourceforge.net/
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
  
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
  
    You should have received a copy of the GNU General Public License
    along with this program; if not, visit http://www.fsf.org/
*/
#ifndef PASS_TO_TXT_H
#define PASS_TO_TXT_H 1

#include <gtk/gtk.h>
#include "sat-pass-dialogs.h"
#include "predict-tools.h"
#include "gtk-sat-data.h"


gchar *pass_to_txt_pgheader (pass_t *pass, qth_t *qth, gint fields);
gchar *pass_to_txt_tblheader (pass_t *pass, qth_t *qth, gint fields);
gchar *pass_to_txt_tblcontents (pass_t *pass, qth_t *qth, gint fields);

gchar *passes_to_txt_pgheader (GSList *passes, qth_t *qth, gint fields);
gchar *passes_to_txt_tblheader (GSList *passes, qth_t *qth, gint fields);
gchar *passes_to_txt_tblcontents (GSList *passes, qth_t *qth, gint fields);


#endif