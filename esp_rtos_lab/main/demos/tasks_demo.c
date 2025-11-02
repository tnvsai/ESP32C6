#if 1 //priority testing 

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
#else  //task handler testing

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

#endif