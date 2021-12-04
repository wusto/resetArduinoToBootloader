# Helper to reset your arduino to bootloader

Based on the source of [arduino-cli](https://github.com/arduino/arduino-cli/blob/7415e269061ff4d90e6356f6c1156941122d58c3/arduino/serialutils/serialutils.go#L30).

According to the arduino-cli tool source code the Arduino is reset to bootloader mode (the mode used for flashing) by pulling the DTR line to low at 1200 bauds per second.

This program is a small helper doing exactly this.

This might be handy when not using the C++/Arduino SDK for developing firmware.

I originially developed this to flash firmware programmed in Rust.

## How to compile
```
mkdir build
cd build
cmake ..
make
```

## How to install
As root execute
```
make install
```

# How to use
```
resetArduinoToBootloader /dev/ttyACM0
```

Afterwards you can execute your flash script. E.g.:
```
resetArduinoToBootloader /dev/ttyACM0 && avrdude -v -V -patmega32u4 -cavr109 -P/dev/ttyACM0 -b57600 -D -Uflash:w:<YOUR_ELF_FILE_TO_FLASH>
```

Please be aware that the standard avrdude somehow has problems flashing software via the Arduino bootloader. Up to now I was too lazy to figure out why. :)

I used the avrdude provided with the arduino sdk. E.g.:
```
./resetArduinoToBootloader /dev/ttyACM0 && /home/<YOUR_USER_NAME>/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude -C/home/<YOUR_USER_NAME>/.arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf -v -V -patmega32u4 -cavr109 -P/dev/ttyACM0 -b57600 -D -Uflash:w:<THE_ELF_FILE_TO_FLASH>
```
