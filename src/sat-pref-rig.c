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
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#ifdef HAVE_CONFIG_H
#  include <build-config.h>
#endif
#include "gpredict-utils.h"
#include "sat-cfg.h"
#include "sat-log.h"
#include "compat.h"
#include "radio-conf.h"
#include "sat-pref-rig-data.h"
#include "sat-pref-rig-editor.h"
#include "sat-pref-rig.h"


extern GtkWidget *window; /* dialog window defined in sat-pref.c */


/* private function declarations */
static void          create_rig_list       (void);
static GtkTreeModel *create_and_fill_model (void);
static GtkWidget    *create_buttons        (void);

static void add_cb    (GtkWidget *button, gpointer data);
static void edit_cb   (GtkWidget *button, gpointer data);
static void delete_cb (GtkWidget *button, gpointer data);

static void render_name (GtkTreeViewColumn *col,
                         GtkCellRenderer   *renderer,
                         GtkTreeModel      *model,
                         GtkTreeIter       *iter,
                         gpointer           column);
static void render_civ (GtkTreeViewColumn *col,
                        GtkCellRenderer   *renderer,
                        GtkTreeModel      *model,
                        GtkTreeIter       *iter,
                        gpointer           column);
static void render_dtr_rts (GtkTreeViewColumn *col,
                            GtkCellRenderer   *renderer,
                            GtkTreeModel      *model,
                            GtkTreeIter       *iter,
                            gpointer           column);
static void render_rig_type (GtkTreeViewColumn *col,
                             GtkCellRenderer   *renderer,
                             GtkTreeModel      *model,
                             GtkTreeIter       *iter,
                             gpointer           column);

/* global objects */
static GtkWidget *addbutton;
static GtkWidget *editbutton;
static GtkWidget *delbutton;
static GtkWidget *riglist;


/** \brief Create and initialise widgets for the radios tab. */
GtkWidget *sat_pref_rig_create ()
{
    GtkWidget *vbox;    /* vbox containing the list part and the details part */
    GtkWidget *swin;


    vbox = gtk_vbox_new (FALSE, 10);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);

    /* create qth list and pack into scrolled window */
    create_rig_list ();
    swin = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_add (GTK_CONTAINER (swin), riglist);

    gtk_box_pack_start (GTK_BOX (vbox), swin, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), create_buttons (), FALSE, FALSE, 0);

    return vbox;
}


/** \brief Create Radio configuration list widget.
 *
 */
static void create_rig_list ()
{
    GtkTreeModel      *model;
    GtkCellRenderer   *renderer;
    GtkTreeViewColumn *column;


    riglist = gtk_tree_view_new ();

    model = create_and_fill_model ();
    gtk_tree_view_set_model (GTK_TREE_VIEW (riglist), model);
    g_object_unref (model);

    /* Conf name */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Config Name"), renderer,
                                                        "text", RIG_LIST_COL_NAME,
                                                        NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_name,
                                             GUINT_TO_POINTER(RIG_LIST_COL_NAME),
                                             NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);

    /* Model */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Model"), renderer,
                                                       "text", RIG_LIST_COL_MODEL,
                                                        NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* Type */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Type"), renderer,
                                                       "text", RIG_LIST_COL_TYPE,
                                                       NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_rig_type,
                                             GUINT_TO_POINTER(RIG_LIST_COL_TYPE),
                                             NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* Port */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Port"), renderer,
                                                       "text", RIG_LIST_COL_PORT,
                                                        NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* Speed */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Speed"), renderer,
                                                       "text", RIG_LIST_COL_SPEED,
                                                       NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* Icom CI-V */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("Icom CI-V"), renderer,
                                                       "text", RIG_LIST_COL_CIV,
                                                       NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_civ,
                                             GUINT_TO_POINTER(RIG_LIST_COL_CIV),
                                             NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* DTR */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("DTR Line"), renderer,
                                                       "text", RIG_LIST_COL_DTR,
                                                       NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_dtr_rts,
                                             GUINT_TO_POINTER(RIG_LIST_COL_DTR),
                                             NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
    /* RTS */
    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes (_("RTS Line"), renderer,
                                                       "text", RIG_LIST_COL_RTS,
                                                       NULL);
    gtk_tree_view_column_set_cell_data_func (column, renderer,
                                             render_dtr_rts,
                                             GUINT_TO_POINTER(RIG_LIST_COL_RTS),
                                             NULL);
    gtk_tree_view_insert_column (GTK_TREE_VIEW (riglist), column, -1);
    
}


/** \brief Create data storage for radio configuration list. */
static GtkTreeModel *create_and_fill_model ()
{
    GtkListStore *liststore;    /* the list store data structure */
    GtkTreeIter  item;      /* new item added to the list store */
    GDir         *dir = NULL;   /* directory handle */
    GError       *error = NULL; /* error flag and info */
    gchar        *cfgdir;
    gchar        *dirname;      /* directory name */
    gchar       **vbuff;
    const gchar  *filename;     /* file name */
    radio_conf_t  conf;

    /* create a new list store */
    liststore = gtk_list_store_new (RIG_LIST_COL_NUM,
                                    G_TYPE_STRING,    // name
                                    G_TYPE_STRING,    // model
                                    G_TYPE_INT,       // hamlib id
                                    G_TYPE_INT,       // radio type
                                    G_TYPE_STRING,    // port
                                    G_TYPE_INT,       // speed
                                    G_TYPE_INT,       // Icom CI-V
                                    G_TYPE_INT,       // DTR line
                                    G_TYPE_INT        // RTS line
                                   );

    /* open configuration directory */
    cfgdir = get_conf_dir ();
    dirname = g_strconcat (cfgdir, G_DIR_SEPARATOR_S,
                           "hwconf", NULL);
    g_free (cfgdir);
    
    dir = g_dir_open (dirname, 0, &error);
    if (dir) {
        /* read each .rig file */
        while ((filename = g_dir_read_name (dir))) {
            
            if (g_strrstr (filename, ".rig")) {
                
                vbuff = g_strsplit (filename, ".rig", 0);
                conf.name = g_strdup (vbuff[0]);
                g_strfreev (vbuff);
                if (radio_conf_read (&conf)) {
                    /* insert conf into liststore */
                    gtk_list_store_append (liststore, &item);
                    gtk_list_store_set (liststore, &item,
                                        RIG_LIST_COL_NAME, conf.name,
                                        RIG_LIST_COL_MODEL, conf.model,
                                        RIG_LIST_COL_ID, conf.id,
                                        RIG_LIST_COL_TYPE, conf.type,
                                        RIG_LIST_COL_PORT, conf.port,
                                        RIG_LIST_COL_SPEED, conf.speed,
                                        RIG_LIST_COL_CIV, conf.civ,
                                        RIG_LIST_COL_DTR, conf.dtr,
                                        RIG_LIST_COL_RTS, conf.rts,
                                        -1);
                    
                    sat_log_log (SAT_LOG_LEVEL_DEBUG,
                                 _("%s:%d: Read %s"),
                                 __FILE__, __LINE__, filename);
                    /* clean up memory */
                    if (conf.name)
                        g_free (conf.name);
                    
                    if (conf.model)
                        g_free (conf.model);
                    
                    if (conf.port)
                        g_free (conf.port);
                }
                else {
                    /* there was an error */
                    sat_log_log (SAT_LOG_LEVEL_ERROR,
                                 _("%s:%d: Failed to read %s"),
                                 __FILE__, __LINE__, conf.name);
                    
                    g_free (conf.name);
                }
            }
        }
    }
    else {
        sat_log_log (SAT_LOG_LEVEL_ERROR,
                     _("%s:%d: Failed to open hwconf dir (%s)"),
                       __FILE__, __LINE__, error->message);
        g_clear_error (&error);
    }

    g_free (dirname);
    g_dir_close (dir);

    return GTK_TREE_MODEL (liststore);
}


/** \brief Create buttons.
 *  \param riglist The GtkTreeView widget containing the rig data.
 *  \return A button box containing the buttons.
 *
 * This function creates and initialises the three buttons below the rig list.
 * The treeview widget is needed by the buttons when they are activated.
 *
 */
static GtkWidget *create_buttons (void)
{
    GtkWidget *box;

    /* add button */
    addbutton = gpredict_hstock_button (GTK_STOCK_ADD,
                                        _("Add New"),
                                        _("Add a new radio to the list"));
    g_signal_connect (addbutton, "clicked", G_CALLBACK (add_cb), NULL);

    /* edit button */
    editbutton = gpredict_hstock_button (GTK_STOCK_EDIT,
                                         _("Edit"),
                                         _("Edit the currently selected radio"));
    g_signal_connect (editbutton, "clicked", G_CALLBACK (edit_cb), NULL);

    /* delete button; don't forget to delete file.... */
    delbutton = gpredict_hstock_button (GTK_STOCK_DELETE,
                                        _("Delete"),
                                        _("Delete the selected radio"));
    g_signal_connect (delbutton, "clicked", G_CALLBACK (delete_cb), NULL);

    /* vertical button box */
    box = gtk_hbutton_box_new ();
    gtk_button_box_set_layout (GTK_BUTTON_BOX (box), GTK_BUTTONBOX_START);

    gtk_container_add (GTK_CONTAINER (box), addbutton);
    gtk_container_add (GTK_CONTAINER (box), editbutton);
    gtk_container_add (GTK_CONTAINER (box), delbutton);


    return box;
}



/** \brief User pressed cancel. Any changes to config must be cancelled.
 */
void sat_pref_rig_cancel ()
{
}


/** \brief User pressed OK. Any changes should be stored in config.
 * 
 * First, all .grc files are deleted, whereafter the radio configurations in
 * the riglist are saved one by one.
 */
void sat_pref_rig_ok     ()
{
    GDir         *dir = NULL;   /* directory handle */
    GError       *error = NULL; /* error flag and info */
    gchar        *buff,*dirname;
    const gchar  *filename;
    GtkTreeIter   iter;      /* new item added to the list store */
    GtkTreeModel *model;
    guint         i,n;
    
    radio_conf_t conf = {
        .name  = NULL,
        .model = NULL,
        .id    = 0,
        .type  = RADIO_TYPE_RX,
        .port  = NULL,
        .speed = 0,
        .civ   = 0,
        .dtr   = LINE_UNDEF,
        .rts   = LINE_UNDEF,
    };

    
    /* delete all .rig files */
    buff = get_conf_dir ();
    dirname = g_strconcat (buff, G_DIR_SEPARATOR_S,
                           "hwconf", NULL);
    g_free (buff);
    
    dir = g_dir_open (dirname, 0, &error);
    if (dir) {
        /* read each .rig file */
        while ((filename = g_dir_read_name (dir))) {

            if (g_strrstr (filename, ".rig")) {

                buff = g_strconcat (dirname, G_DIR_SEPARATOR_S, filename, NULL);
                g_remove (buff);
                g_free (buff);
            }
        }
    }

    g_free (dirname);
    g_dir_close (dir);
    
    /* create new .rig files for the radios in the riglist */
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (riglist));
    n = gtk_tree_model_iter_n_children (model, NULL);
    for (i = 0; i < n; i++) {
        
        /* get radio conf */
        if (gtk_tree_model_iter_nth_child (model, &iter, NULL, i)) {
        
            /* store conf */
            gtk_tree_model_get (model, &iter,
                                RIG_LIST_COL_NAME, &conf.name,
                                RIG_LIST_COL_MODEL, &conf.model,
                                RIG_LIST_COL_ID, &conf.id,
                                RIG_LIST_COL_TYPE, &conf.type,
                                RIG_LIST_COL_PORT, &conf.port,
                                RIG_LIST_COL_SPEED, &conf.speed,
                                RIG_LIST_COL_CIV, &conf.civ,
                                RIG_LIST_COL_DTR, &conf.dtr,
                                RIG_LIST_COL_RTS, &conf.rts,
                                -1);
            radio_conf_save (&conf);
        
            /* free conf buffer */
            if (conf.name)
                g_free (conf.name);
            
            if (conf.model)
                g_free (conf.model);
            
            if (conf.port)
                g_free (conf.port);
        }
        else {
            sat_log_log (SAT_LOG_LEVEL_ERROR,
                         _("%s: Failed to get RIG %s"),
                           __FUNCTION__, i);
        }
    }
    
}


/** \brief Add a new radio configuration
 * \param button Pointer to the Add button.
 * \param data User data (null).
 * 
 * This function executes the radio configuration editor with the "new"
 * flag set to TRUE.
 */
static void add_cb    (GtkWidget *button, gpointer data)
{
    GtkTreeIter  item;      /* new item added to the list store */
    GtkListStore *liststore;
    
    radio_conf_t conf = {
        .name  = NULL,
        .model = NULL,
        .id    = 0,
        .type  = RADIO_TYPE_RX,
        .port  = NULL,
        .speed = 0,
        .civ   = 0,
        .dtr   = LINE_UNDEF,
        .rts   = LINE_UNDEF,
    };
    
    /* run rig conf editor */
    sat_pref_rig_editor_run (&conf);
    
    /* add new rig to the list */
    if (conf.name != NULL) {
        liststore = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (riglist)));
        gtk_list_store_append (liststore, &item);
        gtk_list_store_set (liststore, &item,
                            RIG_LIST_COL_NAME, conf.name,
                            RIG_LIST_COL_MODEL, conf.model,
                            RIG_LIST_COL_ID, conf.id,
                            RIG_LIST_COL_TYPE, conf.type,
                            RIG_LIST_COL_PORT, conf.port,
                            RIG_LIST_COL_SPEED, conf.speed,
                            RIG_LIST_COL_CIV, conf.civ,
                            RIG_LIST_COL_DTR, conf.dtr,
                            RIG_LIST_COL_RTS, conf.rts,
                            -1);
        
        g_free (conf.name);
        
        if (conf.model != NULL)
            g_free (conf.model);
        
        if (conf.port != NULL)
            g_free (conf.port);
    }
}


/** \brief Add a new radio configuration
 * \param button Pointer to the Add button.
 * \param data User data (null).
 * 
 * This function executes the radio configuration editor with the "new"
 * flag set to TRUE.
 */
static void edit_cb   (GtkWidget *button, gpointer data)
{
    GtkTreeModel     *model = gtk_tree_view_get_model (GTK_TREE_VIEW(riglist));
    GtkTreeModel     *selmod;
    GtkTreeSelection *selection;
    GtkTreeIter       iter;
    
    radio_conf_t conf = {
        .name  = NULL,
        .model = NULL,
        .id    = 0,
        .type  = RADIO_TYPE_RX,
        .port  = NULL,
        .speed = 0,
        .civ   = 0,
        .dtr   = LINE_UNDEF,
        .rts   = LINE_UNDEF,
    };

    
    /* If there are no entries, we have a bug since the button should 
    have been disabled. */
    if (gtk_tree_model_iter_n_children (model, NULL) < 1) {

        sat_log_log (SAT_LOG_LEVEL_BUG,
                     _("%s:%s: Edit button should have been disabled."),
                       __FILE__, __FUNCTION__);
        //gtk_widget_set_sensitive (button, FALSE);
        
        return;
    }
    
    /* get selected row
    FIXME: do we really need to work with two models?
    */
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (riglist));
    if (gtk_tree_selection_get_selected(selection, &selmod, &iter)) {
        gtk_tree_model_get (model, &iter,
                            RIG_LIST_COL_NAME, &conf.name,
                            RIG_LIST_COL_MODEL, &conf.model,
                            RIG_LIST_COL_ID, &conf.id,
                            RIG_LIST_COL_TYPE, &conf.type,
                            RIG_LIST_COL_PORT, &conf.port,
                            RIG_LIST_COL_SPEED, &conf.speed,
                            RIG_LIST_COL_CIV, &conf.civ,
                            RIG_LIST_COL_DTR, &conf.dtr,
                            RIG_LIST_COL_RTS, &conf.rts,
                            -1);

    }
    else {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                         GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                        _("Select the radio you want to edit\n"\
                                          "and try again!"));
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        return;
    }

    /* run radio configuration editor */
    sat_pref_rig_editor_run (&conf);
    
    /* apply changes */
    if (conf.name != NULL) {
        gtk_list_store_set (GTK_LIST_STORE(model), &iter,
                            RIG_LIST_COL_NAME, conf.name,
                            RIG_LIST_COL_MODEL, conf.model,
                            RIG_LIST_COL_ID, conf.id,
                            RIG_LIST_COL_TYPE, conf.type,
                            RIG_LIST_COL_PORT, conf.port,
                            RIG_LIST_COL_SPEED, conf.speed,
                            RIG_LIST_COL_CIV, conf.civ,
                            RIG_LIST_COL_DTR, conf.dtr,
                            RIG_LIST_COL_RTS, conf.rts,
                            -1);
        
    }

    /* clean up memory */
    if (conf.name)
        g_free (conf.name);
        
    if (conf.model != NULL)
        g_free (conf.model);
        
    if (conf.port != NULL)
        g_free (conf.port);

}


/** \brief Delete selected radio configuration
 *
 * This function is called when the user clicks the Delete button.
 * 
 */
static void delete_cb (GtkWidget *button, gpointer data)
{
    GtkTreeModel     *model = gtk_tree_view_get_model (GTK_TREE_VIEW(riglist));
    GtkTreeSelection *selection;
    GtkTreeIter       iter;


    /* If there are no entries, we have a bug since the button should 
       have been disabled. */
    if (gtk_tree_model_iter_n_children (model, NULL) < 1) {

        sat_log_log (SAT_LOG_LEVEL_BUG,
                     _("%s:%s: Delete button should have been disabled."),
                     __FILE__, __FUNCTION__);
        //gtk_widget_set_sensitive (button, FALSE);
        
        return;
    }
    
    /* get selected row */
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (riglist));
    if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
        gtk_list_store_remove (GTK_LIST_STORE(model), &iter);
    }
    else {
        GtkWidget *dialog;
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                         GTK_DIALOG_MODAL |
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                         _("Select the radio you want to delete\n"\
                                           "and try again!"));
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

    }
}


/** \brief Render configuration name.
 * \param col Pointer to the tree view column.
 * \param renderer Pointer to the renderer.
 * \param model Pointer to the tree model.
 * \param iter Pointer to the tree iterator.
 * \param column The column number in the model.
 * 
 * This function renders the configuration name onto the riglist. Although
 * the configuration name is a plain string, it contains the file name of the
 * configuration file and we want to strip the .rig extension.
*/
static void render_name (GtkTreeViewColumn *col,
                         GtkCellRenderer   *renderer,
                         GtkTreeModel      *model,
                         GtkTreeIter       *iter,
                         gpointer           column)
{
    gchar  *fname;
    gchar **buff;
    guint   coli = GPOINTER_TO_UINT (column);
    
    gtk_tree_model_get (model, iter, coli, &fname, -1);

    buff = g_strsplit (fname, ".rig", 0);
    g_object_set (renderer, "text", buff[0], NULL);
    
    g_strfreev (buff);
    g_free (fname);
}


/** \brief Render CIV address.
 * \param col Pointer to the tree view column.
 * \param renderer Pointer to the renderer.
 * \param model Pointer to the tree model.
 * \param iter Pointer to the tree iterator.
 * \param column The column number in the model.
 *
 * This function is used to render the Icom CI-V address of the radio.
 * The CI-V adress is store as an integer and we want to display it as a
 * HEX number.
 */
static void render_civ (GtkTreeViewColumn *col,
                        GtkCellRenderer   *renderer,
                        GtkTreeModel      *model,
                        GtkTreeIter       *iter,
                        gpointer           column)
{
    guint   number;
    gchar  *buff;
    guint   coli = GPOINTER_TO_UINT (column);
    
    gtk_tree_model_get (model, iter, coli, &number, -1);

    if (number > 0)
        buff = g_strdup_printf ("0x%X", number);
    else
        buff = g_strdup_printf (" ");
        
    g_object_set (renderer, "text", buff, NULL);
    g_free (buff);
}

/** \brief Render DTR or RTS columns address.
 * \param col Pointer to the tree view column.
 * \param renderer Pointer to the renderer.
 * \param model Pointer to the tree model.
 * \param iter Pointer to the tree iterator.
 * \param column The column number in the model.
 * 
 * This function renders the DTR and RTS line settings onto the radio list.
 * The DTR and RTS states are stored as enum; however, we want to display them
 * using some escriptive text, e.g. "ON", "OFF", "PTT", and so on
 */
static void render_dtr_rts (GtkTreeViewColumn *col,
                            GtkCellRenderer   *renderer,
                            GtkTreeModel      *model,
                            GtkTreeIter       *iter,
                            gpointer           column)
{
    guint    number;
    guint   coli = GPOINTER_TO_UINT (column);
    
    gtk_tree_model_get (model, iter, coli, &number, -1);

    switch (number) {
                        
        case LINE_ON:
            g_object_set (renderer, "text", "ON", NULL);
            break;
                            
        case LINE_PTT:
            g_object_set (renderer, "text", "PTT", NULL);
            break;
                            
        case LINE_CW:
            g_object_set (renderer, "text", "CW", NULL);
            break;
                            
        default:
            g_object_set (renderer, "text", "UNDEF", NULL);
            break;
        
    }
    
}


/** \brief Render radio type.
 * \param col Pointer to the tree view column.
 * \param renderer Pointer to the renderer.
 * \param model Pointer to the tree model.
 * \param iter Pointer to the tree iterator.
 * \param column The column number in the model.
 * 
 * This function renders the radio type onto the radio list.
 * The radio type is stored as enum; however, we want to display it
 * using some escriptive text, e.g. "Received", "Full Duplex", and so on
 */
static void render_rig_type (GtkTreeViewColumn *col,
                             GtkCellRenderer   *renderer,
                             GtkTreeModel      *model,
                             GtkTreeIter       *iter,
                             gpointer           column)
{
    guint    number;
    guint   coli = GPOINTER_TO_UINT (column);
    
    gtk_tree_model_get (model, iter, coli, &number, -1);

    switch (number) {

        case RADIO_TYPE_RX:
            g_object_set (renderer, "text", "RX", NULL);
            break;

        case RADIO_TYPE_TX:
            g_object_set (renderer, "text", "TX", NULL);
            break;

        case RADIO_TYPE_TRX:
            g_object_set (renderer, "text", "RX/TX", NULL);
            break;

        case RADIO_TYPE_FULL_DUP:
            g_object_set (renderer, "text", "FULL DUPL", NULL);
            break;

        default:
            sat_log_log (SAT_LOG_LEVEL_ERROR,
                         _("%s:%s: Invalid type: %d. Using RX."),
                           __FILE__, __FUNCTION__, number);
            g_object_set (renderer, "text", "RX", NULL);
            break;
        
    }
    
}




