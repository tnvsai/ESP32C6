#if(0) //Simple Task Notification
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "TASK_NOTIFY_DEMO";

/* Task Handles */
static TaskHandle_t xTask1Handle = NULL;
static TaskHandle_t xTask2Handle = NULL;

/* ==========================================
 * Task 1 — Producer (sends notifications)
 * ========================================== */
void producer_task(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI(TAG, "Producer: Doing some work...");
        vTaskDelay(pdMS_TO_TICKS(2000));

        // Notify Task2 (increment its internal counter)
        xTaskNotifyGive(xTask2Handle);
        ESP_LOGI(TAG, "Producer: Sent notification to Consumer");
    }
}

/* ==========================================
 * Task 2 — Consumer (waits for notification)
 * ========================================== */
void consumer_task(void *pvParameters)
{
    while (1)
    {
        // Wait indefinitely for a notification
        uint32_t notifyCount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (notifyCount > 0)
        {
            ESP_LOGI(TAG, "Consumer: Received notification! Count=%lu", notifyCount);
        }
        else
        {
            ESP_LOGW(TAG, "Consumer: Timeout or no notification");
        }
    }
}

/* ==========================================
 * Main demo entry
 * ========================================== */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Task Notification Demo...");

    // Create tasks
    xTaskCreate(producer_task, "ProducerTask", 2048, NULL, 2, &xTask1Handle);
    xTaskCreate(consumer_task, "ConsumerTask", 2048, NULL, 2, &xTask2Handle);

    ESP_LOGI(TAG, "All tasks created successfully");
}
#elseif(0)
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "TASK_NOTIFY_DATA";

/* Task Handles */
static TaskHandle_t xSenderHandle = NULL;
static TaskHandle_t xReceiverHandle = NULL;

/* ==========================================
 * Sender Task — sends numeric data
 * ========================================== */
void sender_task(void *pvParameters)
{
    uint32_t counter = 0;

    while (1)
    {
        counter++;

        // Send data to receiver (overwrite old value)
        xTaskNotify(xReceiverHandle, counter, eSetValueWithOverwrite);
        ESP_LOGI(TAG, "Sender: Sent value %lu", counter);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ==========================================
 * Receiver Task — receives the latest data
 * ========================================== */
void receiver_task(void *pvParameters)
{
    uint32_t receivedValue = 0;

    while (1)
    {
        // Wait indefinitely for new value
        if (xTaskNotifyWait(0, 0, &receivedValue, portMAX_DELAY) == pdPASS)
        {
            ESP_LOGI(TAG, "Receiver: Got value %lu", receivedValue);
        }
        else
        {
            ESP_LOGW(TAG, "Receiver: Timeout waiting for data");
        }
    }
}

/* ==========================================
 * Main demo entry
 * ========================================== */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Task Notification (Data) Demo...");

    // Create tasks
    xTaskCreate(sender_task, "SenderTask", 2048, NULL, 2, &xSenderHandle);
    xTaskCreate(receiver_task, "ReceiverTask", 2048, NULL, 2, &xReceiverHandle);

    ESP_LOGI(TAG, "All tasks created successfully");
}
#else // all three types
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "NOTIFY_DEMO";

/* Task Handles */
static TaskHandle_t xBinaryReceiverHandle = NULL;
static TaskHandle_t xCountingReceiverHandle = NULL;
static TaskHandle_t xValueReceiverHandle = NULL;

/* =======================================================
 * Binary Notification — behaves like a binary semaphore
 * ======================================================= */
void binary_sender_task(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI(TAG, "Binary Sender: Sending signal...");
        xTaskNotifyGive(xBinaryReceiverHandle);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void binary_receiver_task(void *pvParameters)
{
    while (1)
    {
        // Wait for one notification (signal)
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Binary Receiver: Received signal!");
    }
}

/* =======================================================
 * Counting Notification — multiple notifications stack up
 * ======================================================= */
void counting_sender_task(void *pvParameters)
{
    while (1)
    {
        for (int i = 0; i < 3; i++)
        {
            xTaskNotifyGive(xCountingReceiverHandle);
            ESP_LOGI(TAG, "Counting Sender: Sent notification #%d", i + 1);
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        ESP_LOGI(TAG, "Counting Sender: Sent 3 signals\n");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void counting_receiver_task(void *pvParameters)
{
    while (1)
    {
        uint32_t count = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Counting Receiver: Got %lu accumulated notifications", count);
    }
}

/* =======================================================
 * Value Notification — sends 32-bit data values
 * ======================================================= */
void value_sender_task(void *pvParameters)
{
    uint32_t value = 0;

    while (1)
    {
        value++;
        // Send new value, overwrite old if not read yet
        xTaskNotify(xValueReceiverHandle, value, eSetValueWithOverwrite);
        ESP_LOGI(TAG, "Value Sender: Sent value %lu", value);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void value_receiver_task(void *pvParameters)
{
    uint32_t received = 0;

    while (1)
    {
        if (xTaskNotifyWait(0, 0, &received, portMAX_DELAY) == pdPASS)
        {
            ESP_LOGI(TAG, "Value Receiver: Received latest value %lu", received);
        }
    }
}

/* =======================================================
 * Main demo entry
 * ======================================================= */
void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Task Notification Combined Demo...");

    // Create receivers FIRST (so their handles are valid)
    xTaskCreate(binary_receiver_task,  "BinaryReceiver",   2048, NULL, 3, &xBinaryReceiverHandle);
    xTaskCreate(counting_receiver_task, "CountingReceiver",2048, NULL, 2, &xCountingReceiverHandle);
    xTaskCreate(value_receiver_task,    "ValueReceiver",    2048, NULL, 2, &xValueReceiverHandle);

    // Then create senders
    xTaskCreate(binary_sender_task,     "BinarySender",     2048, NULL, 3, NULL);
    xTaskCreate(counting_sender_task,   "CountingSender",   2048, NULL, 2, NULL);
    xTaskCreate(value_sender_task,      "ValueSender",      2048, NULL, 2, NULL);

    ESP_LOGI(TAG, "All tasks created successfully");
}

#endif

