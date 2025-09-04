#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

typedef enum {
    LANG_EN = 0,
    LANG_ES = 1,
    LANG_JA = 2
} AppLanguage;

typedef struct {
    GtkApplication *app;
    GtkWidget *window;
    GtkWidget *stack;      // splash/home
    GtkWidget *splash_box;
    GtkWidget *home_box;

    // Menu labels that will be updated at runtime
    GtkWidget *lbl_menu_dashboard;
    GtkWidget *lbl_menu_weldlog;
    GtkWidget *lbl_menu_alarms;
    GtkWidget *lbl_menu_trends;

    // Buttons / controls that will be updated at runtime
    GtkWidget *btn_language;

    // current language
    AppLanguage lang;
} AppWidgets;

void build_ui(GtkApplication *app);

#endif
