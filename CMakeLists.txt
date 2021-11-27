cmake_minimum_required (VERSION 3.0.0)
project (RESET_ARDUINO_TO_BOOTLOADER)

add_compile_options(
    -Wall -Werror -Wextra -Wformat-security
    -fstack-protector-strong -D_FORTIFY_SOURCE=2 -O2 -fPIE -pie
)

add_link_options(
    -Wl,-z,now -Wl,-z,relro
)

add_executable (resetArduinoToBootloader resetArduinoToBootloader.c)

install(TARGETS resetArduinoToBootloader DESTINATION bin)