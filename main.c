#include <gtk/gtk.h>
#include "ui.h"

static void on_app_activate(GtkApplication *app, gpointer user_data) {
    build_ui(app);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.gtk-i18n-demo", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
