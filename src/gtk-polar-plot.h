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
#ifndef __GTK_POLAR_PLOT_H__
#define __GTK_POLAR_PLOT_H__ 1

#include <glib.h>
#include <glib/gi18n.h>
#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkvbox.h>
#include "gtk-sat-data.h"
#include "predict-tools.h"
#include <goocanvas.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** \brief Number of time ticks. */
#define TRACK_TICK_NUM 4


#define GTK_POLAR_PLOT(obj)          GTK_CHECK_CAST (obj, gtk_polar_plot_get_type (), GtkPolarPlot)
#define GTK_POLAR_PLOT_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, gtk_polar_plot_get_type (), GtkPolarPlotClass)
#define GTK_IS_POLAR_PLOT(obj)       GTK_CHECK_TYPE (obj, gtk_polar_plot_get_type ())
#define GTK_TYPE_POLAR_PLOT          (gtk_polar_plot_get_type ())
#define IS_GTK_POLAR_PLOT(obj)       GTK_CHECK_TYPE (obj, gtk_polar_plot_get_type ())

typedef struct _GtkPolarPlot        GtkPolarPlot;
typedef struct _GtkPolarPlotClass   GtkPolarPlotClass;



/* graph orientation; start at 12
   o'clock and go clockwise */
typedef enum {
	POLAR_PLOT_NESW = 0,  /*!< Normal / usual */
	POLAR_PLOT_NWSE = 1,
	POLAR_PLOT_SENW = 2,
	POLAR_PLOT_SWNE = 3
} polar_plot_swap_t;


	/* pole identifier */
typedef enum {
	POLAR_PLOT_POLE_N = 0,
	POLAR_PLOT_POLE_E = 1,
	POLAR_PLOT_POLE_S = 2,
	POLAR_PLOT_POLE_W = 3
} polar_plot_pole_t;


struct _GtkPolarPlot
{
	GtkVBox vbox;

	GtkWidget  *canvas;   /*!< The canvas widget */

	GooCanvasItemModel *C00, *C30, *C60; /*!< 0, 30 and 60 deg elevation circles */
	GooCanvasItemModel *hl, *vl;         /*!< horizontal and vertical lines */
	GooCanvasItemModel *N,*S,*E,*W;      /*!< North, South, East and West labels */
	GooCanvasItemModel *locnam;          /*!< Location name */
	GooCanvasItemModel *curs;            /*!< cursor tracking text */

	pass_t             *pass;
	GooCanvasItemModel *track;                  /*!< Sky track. */
	GooCanvasItemModel *trtick[TRACK_TICK_NUM]; /*!< Time ticks along the sky track */

	qth_t      *qth;      /*!< Pointer to current location. */


	guint       cx;       /*!< center X */
	guint       cy;       /*!< center Y */
	guint       r;        /*!< radius */
	guint       size;     /*!< Size of the box = min(h,w) */


	polar_plot_swap_t swap;

	gboolean    qthinfo;     /*!< Show the QTH info. */
	gboolean    cursinfo;    /*!< Track the mouse cursor. */
	gboolean    extratick;   /*!< Show extra ticks */
};

struct _GtkPolarPlotClass
{
	GtkVBoxClass parent_class;
};



GtkType        gtk_polar_plot_get_type   (void);

GtkWidget*     gtk_polar_plot_new        (qth_t *qth, pass_t *pass);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_POLAR_PLOT_H__ */