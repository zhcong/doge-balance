#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_doge_ground_balance_layer;
static TextLayer *s_doge_balance_layer;
static char s_doge_balance[10];

/**
 * send to app for request data
 */
static bool request_data()
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, 0, "amount");
  dict_write_end(iter);
  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[doge-balance]send request.");
  return true;
}

static void in_received_handler(DictionaryIterator *iter, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[doge-balance]get message.");
  Tuple *s_doge_balance_tuple = dict_find(iter, 0);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "[doge-balance]get %s", s_doge_balance_tuple->value->cstring);

  if (s_doge_balance_tuple)
  {
    text_layer_set_text(s_doge_balance_layer, s_doge_balance_tuple->value->cstring);
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[doge-balance]message dropped. " + reason);
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[doge-balance]message send failed. " + reason);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
  text_layer_set_text(s_doge_balance_layer, "...");
  request_data();
}

static void click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_doge_ground_balance_layer = text_layer_create(GRect(0, 0, bounds.size.w, 100));
  text_layer_set_background_color(s_doge_ground_balance_layer, GColorChromeYellow);
  layer_add_child(window_layer, text_layer_get_layer(s_doge_ground_balance_layer));

  s_doge_balance_layer = text_layer_create(GRect(0, 100, bounds.size.w, 68));
  text_layer_set_background_color(s_doge_balance_layer, GColorChromeYellow);
  text_layer_set_text_color(s_doge_balance_layer, GColorBlack);
  text_layer_set_text(s_doge_balance_layer, "-");
  text_layer_set_text_alignment(s_doge_balance_layer, GTextAlignmentCenter);
  text_layer_set_font(s_doge_balance_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(s_doge_balance_layer));

  request_data();
}

static void window_unload(Window *window)
{
  text_layer_destroy(s_doge_balance_layer);
  text_layer_destroy(s_doge_ground_balance_layer);
}

static void init(void)
{
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);

  app_message_open(64, 64);

  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers){
                                                .load = window_load,
                                                .unload = window_unload,
                                            });
  window_stack_push(s_main_window, true);
}

static void deinit(void)
{
  window_destroy(s_main_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}
