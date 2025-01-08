
![](https://upload.wikimedia.org/wikipedia/commons/thumb/2/2b/Renesas_Electronics_logo.svg/1200px-Renesas_Electronics_logo.svg.png)

## Beschreibung
Dieser Branch dient dem Austausch der Projektdaten für den Vortrag an der FH Aachen am 6. Januar 2025. Um den Code zu nutzen, kann dieser Branch geklont werden. Bitte beachten Sie, dass folgende Änderungen vorgenommen werden müssen:

> [!TIP]
> Falls sie sich fragen, was zu hölle er da über das Klonen redet und was Github ist, schauen sie sich diesen [Guide](https://github.com/git-guides) an. Falls sie im Rahmen ihres Studiums noch nicht mit Git bzw. GitHub gearbeitet habe, lohnt es sich gerade für die Informatikstudenten hier einen genaueren Blick drauf zu werfen. Git erleichtert das arbeiten mit Code ungemein und ist ein großer benefit.


**user.h:**
```c
#define WIFI_SSID               "" // Name WIFI Hotspot
#define WIFI_PWD                ""   // Password für das WIFI

#define IO_USERNAME             ""  // Insert Adafruit Username
#define IO_PASSWORD             "" // Insert Adafruit Key

#define EXAMPLE_MQTT_HOST       "io.adafruit.com" // Hostname des Adafruit MQTT Brokers
#define EXAMPLE_MQTT_PORT       1883 // Broker Port (Ohne SSL) - sonst 8883
```
**main_thread_entry.c:**
```c
mqtt_onchip_da16xxx_cfg_t g_mqtt_onchip_da16xxx_usrcfg = g_mqtt_onchip_da16xxx_cfg;
    g_mqtt_onchip_da16xxx_usrcfg.p_host_name = (char*) &EXAMPLE_MQTT_HOST;
    // ==============USER INPUT=============
	g_mqtt_onchip_da16xxx_usrcfg.mqtt_port = (uint32_t*) EXAMPLE_MQTT_PORT;
    g_mqtt_onchip_da16xxx_usrcfg.p_mqtt_user_name = (char*) &IO_USERNAME;
    g_mqtt_onchip_da16xxx_usrcfg.p_mqtt_password = (char*) &IO_PASSWORD;
	//======================================
    /* Initialize the MQTT Client connection */
    result = RM_MQTT_DA16XXX_Open(&g_rm_mqtt_onchip_da16xxx_instance, &g_mqtt_onchip_da16xxx_usrcfg);
    if(FSP_SUCCESS != result) {
        log_error("MQTT Open failed");
        utils_halt_func();
    }

    log_info("MQTT setup successful!");

    vTaskDelay(100);
```
Bitte beachten sie nur den mit `USER INPUT` markierten Code zu verändern. 

## Sep-by-Step Guide zur manuellen Inbetriebnahme
Dieser Abschnitt ist der Step-by Step Guide zum nachmachen. Genau nach diesen Schritten habe ich das Quick Connect Kit in Betrieb genommen.

### Renesas Quick Connect Studio
1. Einloggen ins [Quick Connect Studio](https://www.renesas.com/en/software-tool/quickconnect-platform?srsltid=AfmBOoqOoXe73gcX2t_47y9pa-uIPPge-j1iXcJ2qqT1l8k9-dhf8Wwl)
2. Erstellen eines neuen Projekts über das Ordner Symbol mit dem + 
3. Auswählen des MCU Boards: Hier BGK-RA6E2
4. Auswählen der Peripherie Module (DA16600 WiFi und ZMOD4410)
5. Links eine Applikation auswählen: IoT->Other->Sensor Data to AWS Cloud (mit RTOS)
6. Generate/ Build 
7. AdafruitIO Schritt 1 und 2 bearbeiten
8. Die oben gezeigten Änderungen in der `user.h` durchführen
9. Die oben gezeigten Änderungen in der `main_thread_entry.c` durchführen
10. Builden des Projektes
11. Downloaden der e2 Studio Datei mittels des Download Symbols in der Rop leiste
12. Öffnen von e2 Studio und hinzufügen eines Projektes aus einer vorhandenen Datei (File -> Open Projects from file system) - ==ACHTUNG: lieber einen neuen Workspace erstellen==
13. FSP Konfiguration erstellen und anschließend Build 
14. Mit Debug das Programm auf den Chip ziehen


### AdafruitIO Cloud
1. Erstellen eines Accounds bei der [AdafruitIO Cloud](https://io.adafruit.com/)
2. Über das Schlüssel Symbol den Username und Active Key kopieren
3. unter "Feeds" kann ein feed mit dem Namen `led` erstellt werden
4. Erstellen eines Dashboards -Y Hinzufügen der gewünschen elemente

> [!NOTE]
> AdafruitIO erkennt neue Feeds automatisch sobald diese gepublished werden. Dies kann jedoch aufgrund des Temperieren des Gassensors ein wenig dauern. 5 Min sind hierbei normal.


## Referenzen
1. Adafruit IO Cloud: https://io.adafruit.com/
2. Renesas Quick Connect Platform: https://www.renesas.com/en/software-tool/quickconnect-platform?srsltid=AfmBOoqOoXe73gcX2t_47y9pa-uIPPge-j1iXcJ2qqT1l8k9-dhf8Wwl
3. Renesas e2 Studio: https://www.renesas.com/en/software-tool/e-studio?srsltid=AfmBOorKMyJJSAMZOAKhFQbQKmsO4UY4tBV_Dppl4pYMo_6jwVbCgF5P
4. Informationen zum Quick Connect Kit: https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/qc-bekitpocz-quickconnect-beginners-kit?srsltid=AfmBOopI0B0grAgQINxT0cq6MA0pc1WvGevrJBH-0JowJiV9Eh3Sgo2Y
5. J-Link / J-Trace Downloads: https://www.segger.com/downloads/jlink/
6. SDK Update Guide DA16200/DA16600: https://www.renesas.com/us/en/document/apn/da16200da16600-sdk-update-guide
   
