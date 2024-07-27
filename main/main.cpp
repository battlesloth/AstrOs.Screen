#include "esp_log.h"
#include <WZ8048C050.h>
#include "lvgl.h"
#include "ui/ui.h"

#define TAG "AstrOs Screen"

//=========== Tasks ===========
void lvglTask(void *arg);

extern "C"
{
    void app_main(void);
}

void app_main()
{
  WZ8048C050_Init();
  ui_init(disp);

  xTaskCreate(&lvglTask, "lvglTask", 4096, NULL, 8, NULL);
}


void lvglTask(void *arg)
{
  while (1) {
    // raise the task priority of LVGL and/or reduce the handler period can
    // improve the performance
    vTaskDelay(pdMS_TO_TICKS(10));
    // The task running lv_timer_handler should have lower priority than that
    // running `lv_tick_inc`
    lv_timer_handler();
  }
}
