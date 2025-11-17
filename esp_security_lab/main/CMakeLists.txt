# Select which demo to build (pass via -DDEMO_NAME=<name>, e.g., secure_boot_demo)
set(DEMO_NAME "crypto_demo" CACHE STRING "Demo to build (e.g., crypto_demo, tls_demo, secure_boot_demo)")

# Pick the selected demo source file
file(GLOB DEMO_SOURCE "demos/${DEMO_NAME}.c")

idf_component_register(
    SRCS "main.c" ${DEMO_SOURCE}
    INCLUDE_DIRS "."
    REQUIRES esp_event nvs_flash esp_wifi esp_system mbedtls
    PRIV_REQUIRES efuse
)
