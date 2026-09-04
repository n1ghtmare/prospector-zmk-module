#include "wpm_indicator.h"

#include <stdio.h>

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include <fonts.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct wpm_indicator_state {
    uint8_t wpm;
};

static void wpm_indicator_set_value(lv_obj_t *label, struct wpm_indicator_state state) {
    char text[8];
    snprintf(text, sizeof(text), "WPM %u", state.wpm);
    lv_label_set_text(label, text);
}

static void wpm_indicator_update_cb(struct wpm_indicator_state state) {
    struct zmk_widget_wpm_indicator *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        wpm_indicator_set_value(widget->obj, state);
    }
}

static struct wpm_indicator_state wpm_indicator_get_state(const zmk_event_t *eh) {
    ARG_UNUSED(eh);
    return (struct wpm_indicator_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_indicator, struct wpm_indicator_state,
                            wpm_indicator_update_cb, wpm_indicator_get_state)
ZMK_SUBSCRIPTION(widget_wpm_indicator, zmk_wpm_state_changed);

int zmk_widget_wpm_indicator_init(struct zmk_widget_wpm_indicator *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_style_text_color(widget->obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(widget->obj, &FoundryGridnikMedium_20, LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);
    widget_wpm_indicator_init();
    return 0;
}

lv_obj_t *zmk_widget_wpm_indicator_obj(struct zmk_widget_wpm_indicator *widget) {
    return widget->obj;
}
