#include "caps_lock_indicator.h"

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>

#include <dt-bindings/zmk/hid_usage.h>
#include <zephyr/sys/util.h>

#include <fonts.h>
#include <sf_symbols.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct caps_lock_indicator_state {
    bool active;
};

static void caps_lock_indicator_set_active(lv_obj_t *label,
                                           struct caps_lock_indicator_state state) {
    lv_obj_set_style_text_color(label, state.active ? lv_color_hex(0x00ffe5)
                                                     : lv_color_hex(0x202020),
                                LV_PART_MAIN);
}

static void caps_lock_indicator_update_cb(struct caps_lock_indicator_state state) {
    struct zmk_widget_caps_lock_indicator *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        caps_lock_indicator_set_active(widget->obj, state);
    }
}

static struct caps_lock_indicator_state caps_lock_indicator_get_state(const zmk_event_t *eh) {
    ARG_UNUSED(eh);

    const zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    return (struct caps_lock_indicator_state){
        .active = (indicators & BIT(HID_USAGE_LED_CAPS_LOCK - 1)) != 0,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_caps_lock_indicator, struct caps_lock_indicator_state,
                            caps_lock_indicator_update_cb, caps_lock_indicator_get_state)
ZMK_SUBSCRIPTION(widget_caps_lock_indicator, zmk_hid_indicators_changed);

int zmk_widget_caps_lock_indicator_init(struct zmk_widget_caps_lock_indicator *widget,
                                        lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);
    lv_label_set_text(widget->obj, SF_SYMBOL_CAPS_LOCK);
    lv_obj_set_style_text_color(widget->obj, lv_color_hex(0x202020), LV_PART_MAIN);
    lv_obj_set_style_text_font(widget->obj, &SF_Compact_Text_Bold_32, LV_PART_MAIN);
    lv_obj_set_style_text_align(widget->obj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);

    sys_slist_append(&widgets, &widget->node);
    widget_caps_lock_indicator_init();
    return 0;
}

lv_obj_t *zmk_widget_caps_lock_indicator_obj(struct zmk_widget_caps_lock_indicator *widget) {
    return widget->obj;
}
