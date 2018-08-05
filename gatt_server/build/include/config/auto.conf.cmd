deps_config := \
	/Users/naboo/esp/esp-idf/components/app_trace/Kconfig \
	/Users/naboo/esp/esp-idf/components/aws_iot/Kconfig \
	/Users/naboo/esp/esp-idf/components/bt/Kconfig \
	/Users/naboo/esp/esp-idf/components/driver/Kconfig \
	/Users/naboo/esp/esp-idf/components/esp32/Kconfig \
	/Users/naboo/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/Users/naboo/esp/esp-idf/components/esp_http_client/Kconfig \
	/Users/naboo/esp/esp-idf/components/ethernet/Kconfig \
	/Users/naboo/esp/esp-idf/components/fatfs/Kconfig \
	/Users/naboo/esp/esp-idf/components/freertos/Kconfig \
	/Users/naboo/esp/esp-idf/components/heap/Kconfig \
	/Users/naboo/esp/esp-idf/components/http_server/Kconfig \
	/Users/naboo/esp/esp-idf/components/libsodium/Kconfig \
	/Users/naboo/esp/esp-idf/components/log/Kconfig \
	/Users/naboo/esp/esp-idf/components/lwip/Kconfig \
	/Users/naboo/esp/gatt_server/main/Kconfig \
	/Users/naboo/esp/esp-idf/components/mbedtls/Kconfig \
	/Users/naboo/esp/esp-idf/components/mdns/Kconfig \
	/Users/naboo/esp/esp-idf/components/openssl/Kconfig \
	/Users/naboo/esp/esp-idf/components/pthread/Kconfig \
	/Users/naboo/esp/esp-idf/components/spi_flash/Kconfig \
	/Users/naboo/esp/esp-idf/components/spiffs/Kconfig \
	/Users/naboo/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/Users/naboo/esp/esp-idf/components/vfs/Kconfig \
	/Users/naboo/esp/esp-idf/components/wear_levelling/Kconfig \
	/Users/naboo/esp/esp-idf/Kconfig.compiler \
	/Users/naboo/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/Users/naboo/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/Users/naboo/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/Users/naboo/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
