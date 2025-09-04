#include <gtk/gtk.h>
#include "ui.h"

/* ------------ Translation table ------------ */

typedef struct { const char *key; const char *en; const char *es; const char *ja; } TRow;

static const TRow STR[] = {
    {"title_app",         "Sample HMI",            "HMI de Ejemplo",          "サンプルHMI"},
    {"menu_dashboard",    "Dashboard",             "Panel",                   "ダッシュボード"},
    {"menu_weldlog",      "Weld Log",              "Registro de Soldadura",   "溶接ログ"},
    {"menu_alarms",       "Alarms",                "Alarmas",                 "アラーム"},
    {"menu_trends",       "Trends",                "Tendencias",              "トレンド"},
    {"btn_language",      "Language",              "Idioma",                  "言語"},
    {"title_language",    "Choose Language",       "Elegir idioma",           "言語を選択"},
    {"prompt_language",   "Select a language:",    "Seleccione un idioma:",   "言語を選択してください:"},
    {"splash_loading",    "Loading...",            "Cargando...",             "読み込み中..."}
};

static const TRow* find_row(const char *key) {
    for (guint i = 0; i < G_N_ELEMENTS(STR); ++i) {
        if (g_strcmp0(STR[i].key, key) == 0) return &STR[i];
    }
    return NULL;
}

static const char* tr(const char *key, AppLanguage lang) {
    const TRow *r = find_row(key);
    if (!r) return key;
    switch (lang) {
        case LANG_EN: return r->en;
        case LANG_ES: return r->es;
        case LANG_JA: return r->ja;
        default: return r->en;
    }
}

/* ------------ UI update helpers ------------ */

static void apply_language(AppWidgets *w) {
    // Window title
    gtk_window_set_title(GTK_WINDOW(w->window), tr("title_app", w->lang));

    // Menus / labels
    gtk_label_set_text(GTK_LABEL(w->lbl_menu_dashboard), tr("menu_dashboard", w->lang));
    gtk_label_set_text(GTK_LABEL(w->lbl_menu_weldlog),   tr("menu_weldlog",   w->lang));
    gtk_label_set_text(GTK_LABEL(w->lbl_menu_alarms),    tr("menu_alarms",    w->lang));
    gtk_label_set_text(GTK_LABEL(w->lbl_menu_trends),    tr("menu_trends",    w->lang));

    // Buttons
    gtk_button_set_label(GTK_BUTTON(w->btn_language), tr("btn_language", w->lang));
}

/* ------------ Callbacks ------------ */

static gboolean switch_to_homepage_cb(gpointer data) {
    AppWidgets *w = (AppWidgets*)data;
    gtk_stack_set_visible_child(GTK_STACK(w->stack), w->home_box);
    return G_SOURCE_REMOVE; // one-shot
}

static void on_language_clicked(GtkButton *btn, gpointer user_data) {
    AppWidgets *w = (AppWidgets*)user_data;

    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        tr("title_language", w->lang),
        GTK_WINDOW(w->window),
        GTK_DIALOG_MODAL,
        "_OK", GTK_RESPONSE_OK,
        "_Cancel", GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(vbox, 12);
    gtk_widget_set_margin_bottom(vbox, 12);
    gtk_widget_set_margin_start(vbox, 12);
    gtk_widget_set_margin_end(vbox, 12);

    GtkWidget *prompt = gtk_label_new(tr("prompt_language", w->lang));
    gtk_box_append(GTK_BOX(vbox), prompt);

    // Radio buttons for 3 languages
    GtkWidget *rb_en = gtk_check_button_new_with_label("English");
    GtkWidget *rb_es = gtk_check_button_new_with_label("Spanish");
    GtkWidget *rb_ja = gtk_check_button_new_with_label("Japanese");

    // Make them behave like radio buttons by grouping via action group (GTK3: use gtk_radio_button_*)
    // Since we target GTK3 per your note, use GtkRadioButton API:
#if !GTK_CHECK_VERSION(4,0,0)
    gtk_widget_unparent(rb_en); gtk_widget_unparent(rb_es); gtk_widget_unparent(rb_ja);
    GtkWidget *r_en = gtk_radio_button_new_with_label(NULL, "English");
    GtkWidget *r_es = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(r_en), "Spanish");
    GtkWidget *r_ja = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(r_en), "Japanese");
    rb_en = r_en; rb_es = r_es; rb_ja = r_ja;
#endif

    // Pre-select current language
    switch (w->lang) {
        case LANG_EN: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_en), TRUE); break;
        case LANG_ES: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_es), TRUE); break;
        case LANG_JA: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_ja), TRUE); break;
    }

    gtk_box_append(GTK_BOX(vbox), rb_en);
    gtk_box_append(GTK_BOX(vbox), rb_es);
    gtk_box_append(GTK_BOX(vbox), rb_ja);

    gtk_box_append(GTK_BOX(area), vbox);

    gtk_widget_show(dialog);
    gint resp = gtk_dialog_run(GTK_DIALOG(dialog));

    if (resp == GTK_RESPONSE_OK) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_en))) w->lang = LANG_EN;
        else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_es))) w->lang = LANG_ES;
        else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_ja))) w->lang = LANG_JA;
        apply_language(w);
    }

    gtk_widget_destroy(dialog);
}

/* ------------ Builders ------------ */

static GtkWidget* build_splash(AppWidgets *w) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

    // Replace with your image path/resource as needed
    GtkWidget *image = gtk_image_new_from_icon_name("image-x-generic");
    GtkWidget *label = gtk_label_new(tr("splash_loading", w->lang));

    gtk_box_append(GTK_BOX(box), image);
    gtk_box_append(GTK_BOX(box), label);

    return box;
}

static GtkWidget* make_menu_item_label(const char *text) {
    GtkWidget *lbl = gtk_label_new(text);
    gtk_widget_set_margin_top(lbl, 6);
    gtk_widget_set_margin_bottom(lbl, 6);
    gtk_widget_set_margin_start(lbl, 6);
    gtk_widget_set_margin_end(lbl, 6);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    return lbl;
}

static GtkWidget* build_home(AppWidgets *w) {
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Top bar with Language button
    GtkWidget *topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_top(topbar, 8);
    gtk_widget_set_margin_bottom(topbar, 8);
    gtk_widget_set_margin_start(topbar, 8);
    gtk_widget_set_margin_end(topbar, 8);

    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);

    w->btn_language = gtk_button_new_with_label(tr("btn_language", w->lang));
    g_signal_connect(w->btn_language, "clicked", G_CALLBACK(on_language_clicked), w);

    gtk_box_append(GTK_BOX(topbar), spacer);
    gtk_box_append(GTK_BOX(topbar), w->btn_language);

    // Content: simple left menu list to demonstrate switching
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_margin_start(content, 8);
    gtk_widget_set_margin_end(content, 8);
    gtk_widget_set_margin_bottom(content, 8);

    GtkWidget *menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    w->lbl_menu_dashboard = make_menu_item_label(tr("menu_dashboard", w->lang));
    w->lbl_menu_weldlog   = make_menu_item_label(tr("menu_weldlog",   w->lang));
    w->lbl_menu_alarms    = make_menu_item_label(tr("menu_alarms",    w->lang));
    w->lbl_menu_trends    = make_menu_item_label(tr("menu_trends",    w->lang));

    gtk_box_append(GTK_BOX(menu_box), w->lbl_menu_dashboard);
    gtk_box_append(GTK_BOX(menu_box), w->lbl_menu_weldlog);
    gtk_box_append(GTK_BOX(menu_box), w->lbl_menu_alarms);
    gtk_box_append(GTK_BOX(menu_box), w->lbl_menu_trends);

    GtkWidget *dummy_area = gtk_frame_new(NULL);
    gtk_widget_set_hexpand(dummy_area, TRUE);
    gtk_widget_set_vexpand(dummy_area, TRUE);

    gtk_box_append(GTK_BOX(content), menu_box);
    gtk_box_append(GTK_BOX(content), dummy_area);

    gtk_box_append(GTK_BOX(root), topbar);
    gtk_box_append(GTK_BOX(root), content);

    return root;
}

/* ------------ Public entry ------------ */

void build_ui(GtkApplication *app) {
    AppWidgets *w = g_new0(AppWidgets, 1);
    w->app = app;
    w->lang = LANG_EN; // default

    w->window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(w->window), 900, 540);
    gtk_window_set_title(GTK_WINDOW(w->window), tr("title_app", w->lang));

    w->stack = gtk_stack_new();
    gtk_window_set_child(GTK_WINDOW(w->window), w->stack);

    // Splash + Home
    w->splash_box = build_splash(w);
    w->home_box   = build_home(w);

    gtk_stack_add_named(GTK_STACK(w->stack), w->splash_box, "splash");
    gtk_stack_add_named(GTK_STACK(w->stack), w->home_box,   "home");

    gtk_stack_set_visible_child(GTK_STACK(w->stack), w->splash_box);

    gtk_widget_show(w->window);

    // Switch to home after 5 seconds
    g_timeout_add(5000, switch_to_homepage_cb, w);
}
