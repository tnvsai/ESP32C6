#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_log.h"

static const char *TAG = "TIMER_DEMO";

/* Timer handles */
TimerHandle_t periodicTimer;
TimerHandle_t oneShotTimer;

/* ============================================
 * Timer callback functions
 * ============================================ */
void periodic_timer_callback(TimerHandle_t xTimer)
{
    ESP_LOGI(TAG, "🔁 Periodic Timer triggered! (every 2 seconds)");
}

void one_shot_timer_callback(TimerHandle_t xTimer)
{
    ESP_LOGI(TAG, "🎯 One-Shot Timer triggered! (only once)");
}

/* ============================================
 * Main demo entry
 * ============================================ */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting FreeRTOS Timer Demo...");

    // Create timers
    periodicTimer = xTimerCreate(
        "PeriodicTimer",             // Name
        pdMS_TO_TICKS(2000),         // 2 seconds period
        pdTRUE,                      // Auto-reload (periodic)
        (void *)0,                   // Timer ID
        periodic_timer_callback      // Callback function
    );

    oneShotTimer = xTimerCreate(
        "OneShotTimer",              // Name
        pdMS_TO_TICKS(5000),         // 5 seconds delay
        pdFALSE,                     // One-shot
        (void *)0,                   // Timer ID
        one_shot_timer_callback      // Callback
    );

    if (periodicTimer == NULL || oneShotTimer == NULL)
    {
        ESP_LOGE(TAG, "Failed to create timers!");
        return;
    }

    // Start timers
    xTimerStart(periodicTimer, 0);
    xTimerStart(oneShotTimer, 0);

    ESP_LOGI(TAG, "Timers started successfully!");
}
