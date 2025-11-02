#if 0 //BinarySemaphore
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "BINARY_SEM_DEMO";

SemaphoreHandle_t xBinarySemaphore;

// Task 1 waits for semaphore
void task_waiter(void *pvParameters)
{
    while (1)
    {
        ESP_LOGI(TAG, "Task 1 waiting for signal...");

        // Wait for semaphore with 5s timeout
        if (xSemaphoreTake(xBinarySemaphore, pdMS_TO_TICKS(5000)) == pdTRUE)
        {
            ESP_LOGI(TAG, "Task 1 got signal!");
        }
        else
        {
            ESP_LOGW(TAG, "Timeout waiting for signal.");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task 2 gives semaphore every 3 seconds
void task_signaler(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000)); // Wait 3s before signaling
        ESP_LOGI(TAG, "Task 2 sending signal...");
        xSemaphoreGive(xBinarySemaphore);
    }
}

void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Binary Semaphore Demo...");

    // Create binary semaphore
    xBinarySemaphore = xSemaphoreCreateBinary();

    if (xBinarySemaphore == NULL)
    {
        ESP_LOGE(TAG, "Failed to create semaphore!");
        return;
    }

    // Give once initially to make it available
    xSemaphoreGive(xBinarySemaphore);

    // Create tasks
    xTaskCreate(task_waiter, "WaiterTask", 2048, NULL, 2, NULL);
    xTaskCreate(task_signaler, "SignalerTask", 2048, NULL, 1, NULL);
}
#elseif(0)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "COUNT_SEM_DEMO";

SemaphoreHandle_t xCountingSemaphore;

void worker_task(void *pvParameters)
{
    int id = (int)pvParameters;

    while (1)
    {
        ESP_LOGI(TAG, "Task %d waiting for resource...", id);

        if (xSemaphoreTake(xCountingSemaphore, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "Task %d got resource!", id);
            vTaskDelay(pdMS_TO_TICKS(2000)); // Simulate work

            ESP_LOGI(TAG, "Task %d releasing resource...", id);
            xSemaphoreGive(xCountingSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void demo_main(void)
{
    ESP_LOGI(TAG, "Starting Counting Semaphore Demo...");

    // Create a counting semaphore with max count = 3, initial count = 3
    xCountingSemaphore = xSemaphoreCreateCounting(3, 3);

    if (xCountingSemaphore == NULL)
    {
        ESP_LOGE(TAG, "Failed to create semaphore!");
        return;
    }

    // Create 4 tasks competing for 3 resources
    for (int i = 1; i <= 4; i++)
    {
        xTaskCreate(worker_task, "WorkerTask", 2048, (void *)i, 1, NULL);
    }
}
#else
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

SemaphoreHandle_t mutex;

void writer_task(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(mutex, portMAX_DELAY))
        {
            ESP_LOGI("Writer", "Writing to shared resource...");
            vTaskDelay(pdMS_TO_TICKS(1000));
            ESP_LOGI("Writer", "Done writing, releasing mutex.");
            xSemaphoreGive(mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void reader_task(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(mutex, portMAX_DELAY))
        {
            ESP_LOGI("Reader", "Reading from shared resource...");
            vTaskDelay(pdMS_TO_TICKS(700));
            ESP_LOGI("Reader", "Done reading, releasing mutex.");
            xSemaphoreGive(mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void demo_main(void)
{
    mutex = xSemaphoreCreateMutex();

    if (mutex == NULL)
    {
        ESP_LOGE("Main", "Failed to create mutex!");
        return;
    }

    xTaskCreate(writer_task, "WriterTask", 2048, NULL, 2, NULL);
    xTaskCreate(reader_task, "ReaderTask", 2048, NULL, 1, NULL);
}

#endif 