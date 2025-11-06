#if 0 //priority testing 

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "PRIORITY_TEST";

// Task handles (optional, for control)
TaskHandle_t task1Handle;
TaskHandle_t task2Handle;

// ===== Task Definitions =====
void task1(void *pv)
{
    while (1) {
        ESP_LOGI(TAG, "Task 1 (Low Priority) running");
        vTaskDelay(pdMS_TO_TICKS(1000));  // 1s delay
    }
}

void task2(void *pv)
{
    while (1) {
        ESP_LOGI(TAG, "Task 2 (High Priority) running");
        vTaskDelay(pdMS_TO_TICKS(500));   // 0.5s delay
    }
}

// ===== Demo Entry =====
void demo_main(void)
{
    ESP_LOGI(TAG, "Creating tasks with different priorities...");

    // Task1 → Lower priority (1)
    xTaskCreate(task1, "Task1", 2048, NULL, 1, &task1Handle);

    // Task2 → Higher priority (3)
    xTaskCreate(task2, "Task2", 2048, NULL, 3, &task2Handle);

    // Let them run together
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Suspend high-priority task
    ESP_LOGI(TAG, "Suspending Task2 (High Priority)");
    vTaskSuspend(task2Handle);

    // Observe Task1 now runs freely
    vTaskDelay(pdMS_TO_TICKS(3000));

    // Resume Task2
    ESP_LOGI(TAG, "Resuming Task2 (High Priority)");
    vTaskResume(task2Handle);

    // Keep demo alive
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#elseif(0)  //task handler testing

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

TaskHandle_t task1Handle;  // Variable to store the task handle

void task1(void *pvParameters)
{
    while (1) {
        ESP_LOGI("Task1", "Running...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task2(void *pvParameters)
{
    while (1) {
        ESP_LOGI("Task2", "Running...");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void demo_main(void)
{
    // Create the task and store the handle
    xTaskCreate(task1, "Task1", 2048, NULL, 5, &task1Handle);
     xTaskCreate(task2, "Task2", 2048, NULL, 5, NULL);

    // Later we can use the handle to control the task
    vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 5 seconds
    vTaskSuspend(task1Handle);        // Pause task1
    ESP_LOGI("Main", "Task1 suspended");

        vTaskDelay(pdMS_TO_TICKS(5000));  // Wait 2 seconds
    vTaskResume(task1Handle);         // Resume task1
    ESP_LOGI("Main", "Task1 resumed");
}
#elseif(0) //delay vs delay until
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "DELAY_UNTIL_DEMO";

/* ================================
 * Task 1: Using vTaskDelay()
 * ================================ */
void drift_task(void *pvParameters)
{
    while (1)
    {
        TickType_t tick = xTaskGetTickCount();
        ESP_LOGI(TAG, "⏱️ Drift Task - Tick: %lu", (unsigned long)tick);
        vTaskDelay(pdMS_TO_TICKS(1000));  // Wait 1s relative to now
    }
}

/* ================================
 * Task 2: Using vTaskDelayUntil()
 * ================================ */
void precise_task(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();  // record first tick

    while (1)
    {
        TickType_t tick = xTaskGetTickCount();
        ESP_LOGI(TAG, "🎯 Precise Task - Tick: %lu", (unsigned long)tick);

        // Run exactly every 1000 ms
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000));
    }
}

/* ================================
 * Main demo entry
 * ================================ */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Delay vs DelayUntil Demo...");

    // Create two tasks: one drifts, one precise
    xTaskCreate(drift_task, "DriftTask", 2048, NULL, 1, NULL);
    xTaskCreate(precise_task, "PreciseTask", 2048, NULL, 1, NULL);
}
#else //periodic multi task scheduling
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "PERIODIC_MULTI";

/* ============================================
 * Task 1: 100 ms interval (fast loop)
 * ============================================ */
void fast_task(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1)
    {
        ESP_LOGI(TAG, "⚡ Fast Task - runs every 100ms (Tick: %lu)",
                 (unsigned long)xTaskGetTickCount());
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(100)); // precise 100 ms period
    }
}

/* ============================================
 * Task 2: 500 ms interval (medium loop)
 * ============================================ */
void medium_task(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1)
    {
        ESP_LOGI(TAG, "⚙️  Medium Task - runs every 500ms (Tick: %lu)",
                 (unsigned long)xTaskGetTickCount());
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(500)); // precise 500 ms period
    }
}

/* ============================================
 * Task 3: 1000 ms interval (slow loop)
 * ============================================ */
void slow_task(void *pvParameters)
{
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1)
    {
        ESP_LOGI(TAG, "🐢 Slow Task - runs every 1000ms (Tick: %lu)",
                 (unsigned long)xTaskGetTickCount());
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000)); // precise 1 s period
    }
}

/* ============================================
 * Main demo entry
 * ============================================ */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Multi-Task Periodic Scheduling Demo...");

    xTaskCreate(fast_task, "FastTask", 2048, NULL, 3, NULL);
    xTaskCreate(medium_task, "MediumTask", 2048, NULL, 2, NULL);
    xTaskCreate(slow_task, "SlowTask", 2048, NULL, 1, NULL);

    ESP_LOGI(TAG, "All periodic tasks created successfully!");
}

#endif