#include <pebble.h>
#include "tomato_window.h"

int main() {
  show_tomato_window();
  app_event_loop();
  return 0;
}
