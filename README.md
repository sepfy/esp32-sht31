# esp32-sht31
An example to read temperature and humidity from sht31 sensor by ESP32. I implement simple protocol to read sensor data by i2c in a header file sht31.h. This code can be integrated to esp-idf. More sensor information can reference [here](http://wiki.seeedstudio.com/Grove-TempAndHumi_Sensor-SHT31/)

### Build
Please download esp-idf project and copy this project to esp-idf.
Do following commands:
```bash
$ source export.sh
$ cd esp32-sht31
$ idf.py build
$ idf.py -p <your device node> flash
$ idf.py monitor
```

### Notice
My i2c pins are SDA 21 and SCL 22.
