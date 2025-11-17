#include <stdio.h>
#include <string.h>
#include "mbedtls/aes.h"
#include "esp_log.h"

void demo_main(void)
{
    const char *TAG = "crypto_cbc_demo";

    // 128-bit (16-byte) key
    uint8_t key[16] = "MySecretAESKey12";

    // Input plaintext (must be multiple of 16 bytes for AES)
    uint8_t input[32] = "SecurityWithESP32C6!FreeRTOS!";
    uint8_t encrypted[32];
    uint8_t decrypted[32];

    // 16-byte IV (Initialization Vector) - must be random in real use
    uint8_t iv_enc[16] = "RandomInitVector";
    uint8_t iv_dec[16];
    memcpy(iv_dec, iv_enc, 16);  // CBC needs same IV for decryption

    ESP_LOGI(TAG, "Starting AES-128 CBC encryption demo...");

    // Create AES context
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // ---- ENCRYPTION ----
    mbedtls_aes_setkey_enc(&aes, key, 128);  // Set encryption key
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, sizeof(input), iv_enc, input, encrypted);

    ESP_LOGI(TAG, "Encrypted data (hex):");
    for (int i = 0; i < sizeof(input); i++) printf("%02X ", encrypted[i]);
    printf("\n");

    // ---- DECRYPTION ----
    mbedtls_aes_setkey_dec(&aes, key, 128);  // Set decryption key
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, sizeof(encrypted), iv_dec, encrypted, decrypted);

    ESP_LOGI(TAG, "Decrypted text: %s", decrypted);

    // Clean up
    mbedtls_aes_free(&aes);
    ESP_LOGI(TAG, "AES CBC demo complete!");
}
