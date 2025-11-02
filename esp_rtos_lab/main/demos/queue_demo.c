#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "QUEUE_DEMO";

/* Queues */
static QueueHandle_t xQueue = NULL;     // normal queue (5 items)
static QueueHandle_t xLatest = NULL;    // single-slot latest value queue

/* ================================
 * Producer Task — pushes values
 * ================================ */
void producer_task(void *pvParameters)
{
    int value = 0;

    while (1) {
        if (xQueueSend(xQueue, &value, pdMS_TO_TICKS(100)) == pdPASS) {
            ESP_LOGI(TAG, "Producer: Sent value %d", value);
            value++;
        } else {
            ESP_LOGW(TAG, "Producer: Queue full, could not send value %d", value);
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* ================================
 * Consumer Task — receives values
 * ================================ */
void consumer_task(void *pvParameters)
{
    int received = 0;

    while (1) {
        if (xQueueReceive(xQueue, &received, pdMS_TO_TICKS(1000)) == pdPASS) {
            ESP_LOGI(TAG, "Consumer: Received value %d", received);
        } else {
            ESP_LOGW(TAG, "Consumer: Timeout waiting for data");
        }

        // Peek without removing
        int peeked = 0;
        if (xQueuePeek(xQueue, &peeked, 0) == pdPASS) {
            ESP_LOGI(TAG, "Consumer: Peeked next value %d", peeked);
        } else {
            ESP_LOGI(TAG, "Consumer: Queue empty, nothing to peek");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ================================
 * Controller Task — monitors queue
 * ================================ */
void controller_task(void *pvParameters)
{
    while (1) {
        UBaseType_t waiting = uxQueueMessagesWaiting(xQueue);
        UBaseType_t space = uxQueueSpacesAvailable(xQueue);

        ESP_LOGI(TAG, "Controller: Messages waiting = %u, Space = %u",
                 (unsigned int)waiting, (unsigned int)space);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/* ================================
 * Overwrite Task — always keeps the latest value
 * ================================ */
void overwrite_task(void *pvParameters)
{
    int overwrite_value = 1000;

    while (1) {
        if (xQueueOverwrite(xLatest, &overwrite_value) == pdPASS) {
            ESP_LOGI(TAG, "Overwrite: Latest value updated to %d", overwrite_value);
        } else {
            ESP_LOGE(TAG, "Overwrite: Failed to update latest value");
        }
        overwrite_value++;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ================================
 * Latest Reader — reads the latest value queue
 * ================================ */
void latest_reader_task(void *pvParameters)
{
    int latest = 0;

    while (1) {
        if (xQueuePeek(xLatest, &latest, pdMS_TO_TICKS(1000)) == pdPASS) {
            ESP_LOGI(TAG, "Latest Reader: Current latest = %d", latest);
        } else {
            ESP_LOGW(TAG, "Latest Reader: No latest value yet");
        }

        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

/* ================================
 * Main demo entry
 * ================================ */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting FreeRTOS Queue Demo...");

    // Create queues
    xQueue = xQueueCreate(5, sizeof(int));   // 5-slot normal queue
    xLatest = xQueueCreate(1, sizeof(int));  // 1-slot overwrite queue

    if (xQueue == NULL || xLatest == NULL) {
        ESP_LOGE(TAG, "Failed to create queues");
        return;
    }

    // Create tasks
    xTaskCreate(producer_task, "Producer", 2048, NULL, 3, NULL);
    xTaskCreate(consumer_task, "Consumer", 2048, NULL, 2, NULL);
    xTaskCreate(controller_task, "Controller", 2048, NULL, 1, NULL);
    xTaskCreate(overwrite_task, "Overwrite", 2048, NULL, 2, NULL);
    xTaskCreate(latest_reader_task, "LatestReader", 2048, NULL, 2, NULL);

    ESP_LOGI(TAG, "All tasks created successfully");
}

