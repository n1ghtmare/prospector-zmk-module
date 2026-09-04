#include "output_indicator.h"

#include <stdio.h>

#include <zmk/display.h>
#include <zmk/endpoints.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>

#include <fonts.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct output_indicator_state {
    struct zmk_endpoint_instance endpoint;
};

static void output_indicator_set_value(lv_obj_t *label, struct output_indicator_state state) {
    char text[8];

    if (state.endpoint.transport == ZMK_TRANSPORT_USB) {
        snprintf(text, sizeof(text), "USB");
    } else {
        snprintf(text, sizeof(text), "BT %d", state.endpoint.ble.profile_index + 1);
    }
    lv_label_set_text(label, text);
}

static void output_indicator_update_cb(struct output_indicator_state state) {
    struct zmk_widget_output_indicator *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        output_indicator_set_value(widget->obj, state);
    }
}

static struct output_indicator_state output_indicator_get_state(const zmk_event_t *eh) {
    ARG_UNUSED(eh);
    return (struct output_indicator_state){.endpoint = zmk_endpoints_selected()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_indicator, struct output_indicator_state,
                            output_indicator_update_cb, output_indicator_get_state)
ZMK_SUBSCRIPTION(widget_output_indicator, zmk_endpoint_changed);
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_indicator, zmk_ble_active_profile_changed);
#endif

int zmk_widget_output_indicator_init(struct zmk_widget_output_indicator *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_obj_set_style_text_color(widget->obj, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_text_font(widget->obj, &FoundryGridnikMedium_20, LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);
    widget_output_indicator_init();
    return 0;
}

lv_obj_t *zmk_widget_output_indicator_obj(struct zmk_widget_output_indicator *widget) {
    return widget->obj;
}
