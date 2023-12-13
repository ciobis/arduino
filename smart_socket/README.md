# Smart Socket
Turn on electricity based on device availability in the local network. 

## Motivation
Primary purpose of the project is to automatically turn on a media server when TV is turned on. This media server is nothing else than an old laptop. Laptop is hidden out of plain view therefore it is too much effort to reach it to manually turn it on every time I want to consume media. Luckily the laptop has a support of "AC Power Recovery" which basically turns on the device when electricity is resumed. This is where smart socket comes into play, it detects that TV is turned on, turns on electricity and laptop automatically turns on as well.

## Requirements
- Socket turned on when TV is turned on.
- PC turns off automatically when TV is turned off.
- Socket turns off only when TV and PC are off - it is important to wait till PC gracefully shuts down.
- Ignore short random glitches in the network.

# Solution
Solution has 2 parts:
1. Smart socket. 
   - [Circuit design and finished product](docs/circuit_design.md)
   - [Code](smart_socket.ino)
2. PC script.
   - Cron [script](power_off.sh) to periodically check if TV online.
   - Install: `sudo crontab -e` add `*/5 * * * * /home/john/dev/smart_socket/power_off.sh`


# Libraries used
- https://www.arduino.cc/reference/en/libraries/espping/
- https://github.com/mmurdoch/arduinounit/tree/master

# How-to
- https://lastminuteengineers.com/esp32-arduino-ide-tutorial/
- https://roboticsbackend.com/install-arduino-library-from-github/
- https://www.instructables.com/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/