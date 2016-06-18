#include "tomato_window.h"
#include <pebble.h>
#include <stdlib.h>

#define KEY_WORK_TIME 1000
  
#define PERSIST_KEY_WAKEUP_ID 0
#define PERSIST_WORK_TIME 100
#define PERSIST_PAUSE_NBR 0
#define PERSIST_WORK_NBR 0

#define TRUE 1
#define FALSE 0
#define LEN 32
#define WORK_TIME 25
#define SHORT_BREAK_TIME 5
#define LONG_BREAK_TIME 20
#define NO_TIME 0
  
static AppSync s_sync;
static uint8_t s_sync_buffer[32];
static int s_work_time;
static int s_is_paused = TRUE;
static WakeupId s_wakeup_id;
static time_t s_future_time;
static int s_minutes_remaining = 0;
static int s_pauseNbr = 0;
static int s_workNbr = 0;
// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static GBitmap *s_res_play_16;
static GBitmap *s_res_todo_list_16;
static GBitmap *s_res_pause_16;
static GBitmap *s_res_cup_16;
static GBitmap *s_res_tomato_black_50;
static BitmapLayer *s_play_bmp;
static ActionBarLayer *s_click_bar;
static TextLayer *s_work_label;
static TextLayer *s_break_label;
static BitmapLayer *s_tomato_bmp;
static TextLayer *s_time_label;
static TextLayer *s_pauseNbr_label;
static TextLayer *s_workNbr_label;

static void initialise_ui(void) {
  s_window = window_create();
  #ifndef PBL_SDK_3
    window_set_fullscreen(s_window, false);
  #endif
  
  s_res_play_16 = gbitmap_create_with_resource(RESOURCE_ID_PLAY_16);
  s_res_todo_list_16 = gbitmap_create_with_resource(RESOURCE_ID_TODO_LIST_16);
  s_res_pause_16 = gbitmap_create_with_resource(RESOURCE_ID_PAUSE_16);
  s_res_cup_16 = gbitmap_create_with_resource(RESOURCE_ID_CUP_16);
  s_res_tomato_black_50 = gbitmap_create_with_resource(RESOURCE_ID_TOMATO_BLACK_50);
  // s_play_bmp
  s_play_bmp = bitmap_layer_create(GRect(125, 67, 16, 16));
  bitmap_layer_set_bitmap(s_play_bmp, s_res_play_16);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_play_bmp);
  
  // s_click_bar
  s_click_bar = action_bar_layer_create();
  action_bar_layer_add_to_window(s_click_bar, s_window);
  action_bar_layer_set_background_color(s_click_bar, GColorBlack);
  action_bar_layer_set_icon(s_click_bar, BUTTON_ID_UP, s_res_todo_list_16);
  action_bar_layer_set_icon(s_click_bar, BUTTON_ID_SELECT, s_res_pause_16);
  action_bar_layer_set_icon(s_click_bar, BUTTON_ID_DOWN, s_res_cup_16);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_click_bar);
  
  // s_work_label
  s_work_label = text_layer_create(GRect(20, 18, 100, 20));
  text_layer_set_text(s_work_label, "Travail");
  text_layer_set_text_alignment(s_work_label, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_work_label);
  
  // s_break_label
  s_break_label = text_layer_create(GRect(20, 116, 100, 20));
  text_layer_set_text(s_break_label, "Pause");
  text_layer_set_text_alignment(s_break_label, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_break_label);
  
  // s_tomato_bmp
  s_tomato_bmp = bitmap_layer_create(GRect(37, 50, 50, 50));
  bitmap_layer_set_bitmap(s_tomato_bmp, s_res_tomato_black_50);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_tomato_bmp);
  
  // s_time_label
  s_time_label = text_layer_create(GRect(13, 75, 100, 20));
  text_layer_set_background_color(s_time_label, GColorClear);
  text_layer_set_text_color(s_time_label, GColorWhite);
  text_layer_set_text(s_time_label, "0 m");
  text_layer_set_text_alignment(s_time_label, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time_label);
  
  // s_pauseNbr_label
  s_pauseNbr_label = text_layer_create(GRect(10, 116, 20, 20));
  text_layer_set_text(s_pauseNbr_label, "0/4");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_pauseNbr_label);
  
  // s_workNbr_label
  s_workNbr_label = text_layer_create(GRect(10, 18, 20, 20));
  text_layer_set_text(s_workNbr_label, "0/4");
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_workNbr_label);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  bitmap_layer_destroy(s_play_bmp);
  action_bar_layer_destroy(s_click_bar);
  text_layer_destroy(s_work_label);
  text_layer_destroy(s_break_label);
  bitmap_layer_destroy(s_tomato_bmp);
  text_layer_destroy(s_time_label);
  text_layer_destroy(s_pauseNbr_label);
  text_layer_destroy(s_workNbr_label);
  gbitmap_destroy(s_res_play_16);
  gbitmap_destroy(s_res_todo_list_16);
  gbitmap_destroy(s_res_pause_16);
  gbitmap_destroy(s_res_cup_16);
  gbitmap_destroy(s_res_tomato_black_50);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

static void update_display_counter(void) {
  
  s_minutes_remaining = (s_future_time - time(NULL)) / 60;
  static char str[LEN];
  snprintf(str, LEN, "%i m", s_minutes_remaining);
  text_layer_set_text(s_time_label, str);
}

static void delete_Wakeup_Id(void){
  if (persist_exists(PERSIST_KEY_WAKEUP_ID)) {
    s_wakeup_id = persist_read_int(PERSIST_KEY_WAKEUP_ID);
    wakeup_cancel(s_wakeup_id);
    persist_delete(PERSIST_KEY_WAKEUP_ID);
  }
}

static void update_counter(int minutes_duration) {
  delete_Wakeup_Id();
  
  // Current time + x seconds
  s_future_time = time(NULL) + (minutes_duration * 60);
  // Schedule wakeup event and keep the WakeupId in case it needs to be queried
  s_wakeup_id = wakeup_schedule(s_future_time, APP_LAUNCH_WAKEUP, true);
  // Persist to allow wakeup query after the app is closed.
  persist_write_int(PERSIST_KEY_WAKEUP_ID, s_wakeup_id);  
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Button Up !");
  s_is_paused = FALSE;
  update_counter(s_work_time);
  update_display_counter();
  if (s_workNbr < 4) {
    s_workNbr = s_workNbr + 1;
  } else {
    s_workNbr = 1;
  }
  static char str[LEN];
  snprintf(str, LEN, "%i/4", s_workNbr);
  text_layer_set_text(s_workNbr_label, str);
  
  persist_write_int(PERSIST_WORK_NBR, s_workNbr);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Button Select !");
  if (s_is_paused == FALSE) {
    s_is_paused = TRUE;
    action_bar_layer_set_icon(s_click_bar, BUTTON_ID_SELECT, s_res_play_16);
    delete_Wakeup_Id();
  } else {
    s_is_paused = FALSE;
    action_bar_layer_set_icon(s_click_bar, BUTTON_ID_SELECT, s_res_pause_16);
    update_counter(s_minutes_remaining);
    update_display_counter();
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Button Down !");
  s_is_paused = FALSE;
  update_counter(SHORT_BREAK_TIME);
  update_display_counter();

  if (s_pauseNbr < 4) {
    s_pauseNbr = s_pauseNbr + 1;
  } else {
    s_pauseNbr = 1;
  }
  static char str[LEN];
  snprintf(str, LEN, "%i/4", s_pauseNbr);
  text_layer_set_text(s_pauseNbr_label, str);
  
  persist_write_int(PERSIST_PAUSE_NBR, s_pauseNbr);
}

void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (s_is_paused == FALSE) {
    update_display_counter();
  }  
}

static void wakeup_handler(WakeupId id, int32_t reason) {  
  //Create an array of ON-OFF-ON etc durations in milliseconds
  uint32_t segments[] = {100, 200, 500, 100, 200, 500, 100, 200, 500, 100, 200, 500, 100, 200, 500, 100, 200, 500, 100, 200, 500};
 
  //Create a VibePattern structure with the segments and length of the pattern as fields
  VibePattern pattern = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
 
  //Trigger the custom pattern to be executed
  vibes_enqueue_custom_pattern(pattern);
  delete_Wakeup_Id();
}

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Type de donnee: %i", new_tuple->type);
  s_work_time = new_tuple->value->int16;
}

static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
}

void show_tomato_window(void) {
  initialise_ui();
  
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Setup initial values
  s_work_time = WORK_TIME;
  if (persist_exists(PERSIST_WORK_TIME)) {
    s_work_time = persist_read_int(PERSIST_WORK_TIME);  
  }
  
  if (persist_exists(PERSIST_PAUSE_NBR)) {
    static char str[LEN];
    s_pauseNbr = persist_read_int(PERSIST_PAUSE_NBR);  
    snprintf(str, LEN, "%i/4", s_pauseNbr);
    text_layer_set_text(s_pauseNbr_label, str);
  }
  
  if (persist_exists(PERSIST_WORK_NBR)) {
    static char str[LEN];
    s_workNbr = persist_read_int(PERSIST_WORK_NBR);  
    snprintf(str, LEN, "%i/4", s_workNbr);
    text_layer_set_text(s_workNbr_label, str);
  }

  
  Tuplet initial_values[] = {
    TupletInteger(KEY_WORK_TIME, s_work_time),
  };

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);
  
  // Was this a wakeup launch?
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    // Launch by wakeup
    WakeupId id = 0;
    int32_t reason = 0;

    // Get details and handle the wakeup
    wakeup_get_launch_event(&id, &reason);
    wakeup_handler(id, reason);
  } else {
    // Launch by user
    // TODO: récupérer état du counter
  }
  
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_set_click_config_provider(s_window, click_config_provider);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  wakeup_service_subscribe(wakeup_handler);
  window_stack_push(s_window, true);
}

void hide_tomato_window(void) {
  tick_timer_service_unsubscribe();
  window_stack_remove(s_window, true);
  // Finish using AppSync
  app_sync_deinit(&s_sync);
}