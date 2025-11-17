#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mbedtls/aes.h"

static const char *TAG = "crypto_demo";

void demo_main(void)
{
    ESP_LOGI(TAG, "Starting AES Encryption + Decryption Demo...");

    // -------------------------------------------------------
    // 1. AES Key (16 bytes for AES-128)
    // -------------------------------------------------------
    const unsigned char aes_key[16] = {
        0x60, 0x3D, 0xEB, 0x10,
        0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0,
        0x85, 0x7D, 0x77, 0x81
    };

    // -------------------------------------------------------
    // 2. Plaintext (16 bytes block)
    // -------------------------------------------------------
    const unsigned char plaintext[16] = "HELLO_SAI_CRYPTO";

    unsigned char encrypted[16];
    unsigned char decrypted[16];

    // AES context
    mbedtls_aes_context aes;

    // -------------------------------------------------------
    // 3. Encrypt
    // -------------------------------------------------------
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, aes_key, 128);

    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, plaintext, encrypted);

    ESP_LOGI(TAG, "Encrypted Data:");
    for (int i = 0; i < 16; i++)
        printf("%02X ", encrypted[i]);
    printf("\n");

    // -------------------------------------------------------
    // 4. Decrypt
    // -------------------------------------------------------
    mbedtls_aes_setkey_dec(&aes, aes_key, 128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, encrypted, decrypted);

    ESP_LOGI(TAG, "Decrypted Data: %s", decrypted);

    // -------------------------------------------------------
    // 5. Verification
    // -------------------------------------------------------
    if (memcmp(plaintext, decrypted, 16) == 0)
        ESP_LOGI(TAG, "AES verification SUCCESS!");
    else
        ESP_LOGE(TAG, "AES verification FAILED!");

    mbedtls_aes_free(&aes);
}
