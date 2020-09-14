# ESP-EYE Simple HTTPD Server with MIC

This sample program response an wav file, esp-eye mic inputs wav file or mic inputs wav stream to httpd client.  

    Borad: ESP-EYE  
    Build Frame work: esp-idf  
    Build tool: idf v4.0  
    Environment: Windows 10  

Base program is Simple Httpd Server and others.  
Thanks  
[github.com/MhageGH/esp32_SoundRecord](https://github.com/MhageGH/esp32_SoundRecorder)  

Configure before build  

    * Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or other:  
    
    1. Wi-Fi ssid and password  
        Example Connection Configuration >  
            WiFi SSID  
            WiFi Password  
         
    2. Increase Main task stack size  
        Component config > Common ESP-related  
            Main task stack size:5120  
         
    3. increase Flash size  
        Serial flasher Config >  
            Flash size:4 MB  
         
Build and Flash  

    1.`idf.py build`  
    2.`idf.py -p PORT flash`  

Run  

    Connect with Serial Terminal to ESP-EYE USB-Serial.
    And check ip-addres.

    Open Browser and set url address to http://ip-addres

