#include <gtk/gtk.h>
#include "chat.h"
#include "socket.h"

extern GtkWidget *chatBox;
enum {

  LIST_ITEM = 0,
  N_COLUMNS
};

GtkWidget *list;

void welcome(){
    GtkListStore *store;
    GtkTreeIter iter;
    char msg[] = "Welcome to Chatify!";
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, LIST_ITEM, msg, -1);
}

void append_item(GtkWidget *widget, gpointer entry) {

    GtkListStore *store;
    GtkTreeIter iter;
    char msg[2024] = "[you]: ";
    gchar *str = gtk_entry_get_text(entry);

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    if(strlen(str)==0){
        return;
    }

    strcat(msg,str);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, LIST_ITEM, msg, -1);

    send_msg_handler(str);

    gtk_entry_set_text(entry, "");
}

void recv_item(char *msg) {

  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, LIST_ITEM, msg, -1);
}

void remove_item(GtkWidget *widget, gpointer selection) {

  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter  iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE) {
      return;
  }

  if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection),
         &model, &iter)) {
    gtk_list_store_remove(store, &iter);
  }
}

void remove_all(GtkWidget *widget, gpointer selection) {

  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter  iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

  if (gtk_tree_model_get_iter_first(model, &iter) == FALSE) {
      return;
  }

  gtk_list_store_clear(store);
}

void init_list(GtkWidget *list) {

  GtkCellRenderer    *renderer;
  GtkTreeViewColumn  *column;
  GtkListStore       *store;

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("List Item",
          renderer, "text", LIST_ITEM, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

  store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

  gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

  g_object_unref(store);
}

int chatForm(int argc, char *argv[]) {

  GtkWidget *sw;

  GtkWidget *remove, *add,*removeAll, *welcomeLabel;
  GtkWidget *entry;

  GtkWidget *mainBox;
  GtkWidget *hbox, *header;

  GtkTreeSelection *selection;

  gtk_init(&argc, &argv);

  chatBox = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title(GTK_WINDOW(chatBox), "Chatify");
  gtk_window_set_position(GTK_WINDOW(chatBox), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER (chatBox), 10);
  gtk_widget_set_size_request(chatBox, 370, 270);


  mainBox = gtk_vbox_new(FALSE, 0);
  header = gtk_box_new(FALSE, 5);
  welcomeLabel = gtk_label_new("Welcome To Chatify");
  remove = gtk_button_new_with_label("Remove");
  removeAll = gtk_button_new_with_label("Remove All");

  gtk_box_pack_start(GTK_BOX(header), welcomeLabel, FALSE, TRUE, 3);
  gtk_box_pack_start(GTK_BOX(header), remove, FALSE, TRUE, 3);
  gtk_box_pack_start(GTK_BOX(header), removeAll, FALSE, TRUE, 3);
  gtk_box_pack_start(GTK_BOX(mainBox), header, FALSE, TRUE, 3);

  sw = gtk_scrolled_window_new(NULL, NULL);
  list = gtk_tree_view_new();
  gtk_container_add(GTK_CONTAINER(sw), list);

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_ETCHED_IN);

  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);

  gtk_box_pack_start(GTK_BOX(mainBox), sw, TRUE, TRUE, 5);

  hbox = gtk_box_new(FALSE, 5);

  add = gtk_button_new_with_label("Send");

  entry = gtk_entry_new();
  gtk_widget_set_size_request(entry, 120, -1);

  gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 3);
  gtk_box_pack_start(GTK_BOX(hbox), add, FALSE, TRUE, 3);

  gtk_box_pack_start(GTK_BOX(mainBox), hbox, FALSE, TRUE, 3);

  gtk_container_add(GTK_CONTAINER(chatBox), mainBox);

  init_list(list);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

  g_signal_connect(G_OBJECT(add), "clicked", G_CALLBACK(append_item), entry);

  g_signal_connect(G_OBJECT(remove), "clicked", G_CALLBACK(remove_item), selection);

  g_signal_connect(G_OBJECT(removeAll), "clicked", G_CALLBACK(remove_all), selection);

  g_signal_connect(G_OBJECT(chatBox), "destroy", G_CALLBACK(gtk_main_quit), NULL);

//
//  gtk_widget_show_all(window);
//
//  gtk_main();
  return chatBox;
}
