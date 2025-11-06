#if(0)
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "EVENT_OR_DEMO";

/* Define event bits */
#define WIFI_CONNECTED_BIT  (1 << 0)
#define BLE_CONNECTED_BIT   (1 << 1)

/* Global event group handle */
static EventGroupHandle_t xEventGroup;

/* ------------------------------
 * Wi-Fi Task
 * ------------------------------ */
void wifi_task(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(4000)); // Simulate Wi-Fi connect delay
        ESP_LOGI(TAG, "Wi-Fi connected!");
        xEventGroupSetBits(xEventGroup, WIFI_CONNECTED_BIT);
        vTaskDelay(pdMS_TO_TICKS(10000)); // Reconnect simulation
    }
}

/* ------------------------------
 * BLE Task
 * ------------------------------ */
void ble_task(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(7000)); // Simulate BLE connect delay
        ESP_LOGI(TAG, "BLE connected!");
        xEventGroupSetBits(xEventGroup, BLE_CONNECTED_BIT);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

/* ------------------------------
 * Main Controller Task
 * ------------------------------ */
void controller_task(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI(TAG, "Waiting for Wi-Fi or BLE connection...");

        EventBits_t bits = xEventGroupWaitBits(
            xEventGroup,
            WIFI_CONNECTED_BIT | BLE_CONNECTED_BIT, // bits to wait for
            pdTRUE,   // clear bits after catching one
            pdFALSE,  // wait for ANY (not all)
            portMAX_DELAY
        );

        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "📶 Wi-Fi event detected! Proceeding...");
        }
        else if (bits & BLE_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "🔷 BLE event detected! Proceeding...");
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // Small pause
    }
}

/* ------------------------------
 * Main Demo Entry
 * ------------------------------ */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Event Group OR-Logic Demo...");

    xEventGroup = xEventGroupCreate();
    if (xEventGroup == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        return;
    }

    xTaskCreate(wifi_task, "WiFiTask", 2048, NULL, 2, NULL);
    xTaskCreate(ble_task, "BLETask", 2048, NULL, 2, NULL);
    xTaskCreate(controller_task, "ControllerTask", 2048, NULL, 3, NULL);

    ESP_LOGI(TAG, "Tasks created successfully!");
}
#else //Barrier Synchronization, using xEventGroupSync
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

static const char *TAG = "EVENT_SYNC_DEMO";

/* Define bits for each task */
#define TASK1_BIT (1 << 0)
#define TASK2_BIT (1 << 1)
#define TASK3_BIT (1 << 2)

/* Event group handle */
static EventGroupHandle_t xSyncGroup;

/* ================================
 * Worker Tasks
 * ================================ */
void worker_task(void *pvParameters)
{
    uint32_t bit = (uint32_t)pvParameters;

    while (1)
    {
        ESP_LOGI(TAG, "Task %lu: Setup work done, waiting for others...", bit);
        vTaskDelay(pdMS_TO_TICKS(2000 + (bit * 500)));  // simulate different setup times

        // Each task sets its bit and waits for all bits to be set
        xEventGroupSync(
            xSyncGroup,
            bit,                              // set own bit
            TASK1_BIT | TASK2_BIT | TASK3_BIT, // wait for all
            portMAX_DELAY                      // block until all arrive
        );

        ESP_LOGI(TAG, "Task %lu: All ready, starting main work!", bit);

        // Simulate some work
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

/* ================================
 * Demo Main Entry
 * ================================ */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Event Group Barrier (Sync) Demo...");

    xSyncGroup = xEventGroupCreate();
    if (xSyncGroup == NULL)
    {
        ESP_LOGE(TAG, "Failed to create event group");
        return;
    }

    // Create three synchronized tasks
    xTaskCreate(worker_task, "Task1", 2048, (void *)TASK1_BIT, 2, NULL);
    xTaskCreate(worker_task, "Task2", 2048, (void *)TASK2_BIT, 2, NULL);
    xTaskCreate(worker_task, "Task3", 2048, (void *)TASK3_BIT, 2, NULL);
}
#endif
