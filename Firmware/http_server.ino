// http_server.ino
void runServerHTTP(void) {
  // Ниже две функции предварительной настройка WiFi в режиме точки доступа
  // и установка ESP_mode -------------------------------------------------
  HTTP.on("/ESP_mode", HTTP_GET, []() {   // Установка ESP Mode
    jsonWrite(configSetup, "ESP_mode", HTTP.arg("ESP_mode").toInt());
    saveConfig();
    espMode = jsonReadtoInt(configSetup, "ESP_mode");
    HTTP.send(200, "text/plain", "OK");
  });
  
  // SSID | Pass | timout -------------------------------------------------
  HTTP.on("/ssid", HTTP_GET, []() {          // сохранение настроек роутера
    jsonWrite(configSetup, "ssid", HTTP.arg("ssid"));
    jsonWrite(configSetup, "password", HTTP.arg("password"));
    jsonWrite(configSetup, "TimeOut", HTTP.arg("TimeOut").toInt());
    ESP_CONN_TIMEOUT = jsonReadtoInt(configSetup, "TimeOut");
    saveConfig();                 // Функция сохранения данных во Flash
    HTTP.send(200, "text/plain", "OK"); // отправляем ответ о выполнении
  });
  // ======================================================================
  
  // Выдаем данные configSetup ===========
  HTTP.on("/config.setup.json", HTTP_GET, []() {
    HTTP.send(200, "application/json", configSetup);
  });

  // Обработка Restart ===================
  HTTP.on("/restart", HTTP_GET, []() {
    String restart = HTTP.arg("device");            // Получаем значение device из запроса
    if (restart == "ok") {                          // Если значение равно Ок
      HTTP.send(200, "text / plain", "Reset OK");   // Oтправляем ответ Reset OK
      ESP.restart();                                // перезагружаем модуль
    }
    else {                                          // иначе
      HTTP.send(200, "text / plain", "No Reset");   // Oтправляем ответ No Reset
    }
  });

  // Remote Control =====================
  HTTP.on("/cmd", handle_cmd);                      // web управление лампой http:ipLamp/cmd?cmd=• команда •&val=• значение •
  // ------------------------------------
  httpUpdater.setup(&HTTP);                         // Добавляем функцию Update для перезаписи прошивки по WiFi при 4М(1M SPIFFS) и выше
  HTTP.begin();                                     // Запускаем HTTP сервер
}

// ======================================
void warnDinamicColor(uint8_t val) {
  switch (val) {
    case 0: showWarning(CRGB::Blue, 1000U, 250U); break;
    case 1: showWarning(CRGB::Yellow, 1000U, 250U); break;
    case 2: showWarning(CRGB::Red, 1000U, 250U); break;
    case 3: showWarning(CRGB::Cyan, 1000U, 250U); break;
    case 4: showWarning(CRGB::Gold, 1000U, 250U); break;
    case 5: showWarning(0xFF3F00, 1000U, 250U); break; // orange
    case 6: showWarning(CRGB::Green, 1000U, 250U); break;
    case 7: showWarning(CRGB::Magenta, 1000U, 250U); break;
    default: showWarning(CRGB::White, 1000U, 250U); break;
  }
}

// ======================================
void testMatrix(uint8_t val) {
  if (val == 1) {
    ONflag = false;
    FastLED.setBrightness(50);
    FastLED.clear();
    gradientHorizontal(3, 0, WIDTH - 3, 1, 90, 90, 128, 0, 255U);
    gradientHorizontal(3, HEIGHT - 1, WIDTH - 3, HEIGHT, 0, 0, 0, 128, 255U);
    drawPixelXY(0, 0, 0x00FF00);
    drawPixelXY(WIDTH - 1, HEIGHT - 1, 0xFF0000);

    drawPixelXYF(WIDTH * 0.5, HEIGHT * 0.5, 0x4F0800);
    if (WIDTH % 2 == 0) {
      drawPixelXYF(WIDTH * 0.5 - 1, HEIGHT * 0.5, 0x4F0800);
      drawPixelXYF(WIDTH * 0.5 - 1, HEIGHT * 0.5 - 1, 0xFF3F00);
      drawPixelXYF(WIDTH * 0.5, HEIGHT * 0.5 - 1, 0xFF3F00);
    }
    FastLED.show();
  } else {
    drawRec(0, 0, uint8_t WIDTH, HEIGHT, 0xFF0000);
    FastLED.show();
    delay(2000);
    drawRec(0, 0, uint8_t WIDTH, HEIGHT, 0x00FF00);
    FastLED.show();
    delay(2000);
    drawRec(0, 0, uint8_t WIDTH, HEIGHT, 0x0000FF);
    FastLED.show();
    delay(2000);
    gradientVertical(0, 0, WIDTH, HEIGHT, 0, 255, 255, 255, 255U);
    FastLED.show();
    delay(2000);
    FastLED.clear();
    FastLED.setBrightness(modes[currentMode].Brightness);
    ONflag = true;
  }
}
// ======================================
// Remote Control Command ---------------
void handle_cmd() {
  uint8_t cmd = HTTP.arg("cmd").toInt();
  String valStr = HTTP.arg("valStr");
  uint8_t val = HTTP.arg("val") ? HTTP.arg("val").toInt() : 0;
  String body = "";
  switch (cmd ) {
    case CMD_POWER:
      if (val == 3) {               // toggle
        ONflag = !ONflag;
      } else {
        if (val == 0) {
          ONflag = false;           // off
        } else {
          ONflag = true;            // on
        }
      }
      changePower();
#ifdef USE_MULTIPLE_LAMPS_CONTROL
      multipleLampControl();
#endif  //USE_MULTIPLE_LAMPS_CONTROL  
      break;

    case CMD_PREV_EFF:
      selectedSettings = 0U;
      prevEffect();
      break;

    case CMD_NEXT_EFF:
      selectedSettings = 0U;
      nextEffect();
      break;
    // case CMD_FAVORITES:
    // FavoritesManager::FavoritesRunning = val;
    // break;
    //case CMD_PC_STATE:
    //  InfoPC(valStr);
    //  break;

    case CMD_BRIGHT_UP:
      changeBrightness(true);
      break;

    case CMD_BRIGHT_DW:
      changeBrightness(false);
      break;

    case CMD_SPEED:
      modes[currentMode].Speed = val;
      loadingFlag = true;
#ifdef USE_MULTIPLE_LAMPS_CONTROL
      multipleLampControl ();
#endif  //USE_MULTIPLE_LAMPS_CONTROL
      break;

    case CMD_SCALE:
      modes[currentMode].Scale = val;
      loadingFlag = true;
#ifdef USE_MULTIPLE_LAMPS_CONTROL
      multipleLampControl ();
#endif  //USE_MULTIPLE_LAMPS_CONTROL
      break;

    case CMD_WHITE:
      currentMode = EFF_WHITE_COLOR;
      runEffect();
      break;

    case CMD_TEXT: {
        String temStr = TextTicker;
        if (valStr != "") {
          temStr = valStr;
        } else {
          temStr = jsonRead(configSetup, "run_text");
        }
        temStr.toCharArray(TextTicker, temStr.length() + 1);
        currentMode = MODE_AMOUNT - 1;
        runEffect();
      }
      break;
    case CMD_SCAN:
      //      body += "\"id\":" + String(cmd) + ",";
      //      body += getInfo();
      // showWarning(CRGB::Blue, 1000U, 500U);
      break;

    // effect ----------------
    case CMD_DEFAULT:
      modes[currentMode].Brightness = pgm_read_byte(&defaultSettings[currentMode][0]);
      modes[currentMode].Speed      = pgm_read_byte(&defaultSettings[currentMode][1]);
      modes[currentMode].Scale      = pgm_read_byte(&defaultSettings[currentMode][2]);
      runEffect();
      break;
    case CMD_RESET_EFF:
      restoreSettings();
      updateSets();
      showWarning(CRGB::Blue, 2000U, 500U);                    // мигание синим цветом 2 секунды
#ifdef USE_BLYNK
      updateRemoteBlynkParams();
#endif
      break;
    case CMD_AUTO:
      cycleEffect();
      break;
    case CMD_INTIM:
      currentMode = EFF_MATRIX + 1;
      runEffect();
      break;
    case CMD_FAV:
      currentMode = EFF_FAV;
      runEffect();
      break;
    case CMD_RANDOM:
      selectedSettings = 1U;
      updateSets();
      break;
    case CMD_LIST:
      loadingFlag = false;
      // path -----------------
      if (SPIFFS.exists("/" + valStr)) {
        body += "\"status\":\"OK\",";
        body += "\"list\":" + readFile(valStr, 4096) + ",";
      } else {
        body += "\"status\":\"Error File Not Found\",";
      }
      break;

    case CMD_SHOW_EFF:
      if (eff_auto == 1) eff_auto = 0; // off cycle effects
      currentMode = val;
      runEffect();
      break;

      // group lamps ----------
#ifdef USE_MULTIPLE_LAMPS_CONTROL
    case CMD_GROUP_INIT:
      initWorkGroup(valStr);
      warnDinamicColor(7);
      break;
    case CMD_GROUP_DESTROY:
      resetWorkGroup();
      warnDinamicColor(val);
      break;
#endif //USE_MULTIPLE_LAMPS_CONTROL
    // ----------------------

    // configure commands ---
    case CMD_CONFIG:
      warnDinamicColor(2);
      // LOG.println("config Setup:" + configSetup);
      body += "\"cfg\":" + configSetup + ",";
      body += "\"alarms\":" + readFile("alarm_config.json", 512) + ",";
      break;
    case CMD_SAVE_CFG :
      configSetup = valStr;
      LOG.println("config save:" + configSetup);
      body += "\"cfg_save\":\"OK\",";
      writeFile("config.json", configSetup);
      valStr = "";
      break;
    case CMD_SAVE_ALARMS :
      // configSetup = valStr;
      body += "\"cfg_save\":\"OK\",";
      writeFile("alarm_config.json", valStr);
      valStr = "";
      break;
    // fs commands ----------
    case CMD_FS_DIR:
      loadingFlag = false;
      body += getLampID() + ",";
      body += getDirFS();
      sendResponse(cmd, body);
      return;

    // develop commands -----
    case CMD_IP:
      showIP();
      return;
    case CMD_TEST_MATRIX:
      testMatrix(val) ;
      return;
    case CMD_CUSTOM_EFF:
      if (val == 2) {               // restore
        custom_eff = jsonReadtoInt(configSetup, "custom_eff");
        FastLED.clear();
      } else {
        custom_eff = val;           // toggle
      }
      return;
    case CMD_INFO:
      body += getLampID() + ",";
      body += getInfo();
      sendResponse(cmd, body);
      return;
    case CMD_ECHO:
      warnDinamicColor(val);
      break;
    case CMD_DEL_FILE:
      // if (valStr == "") body += "\"status\":\"Error BAD ARGS\",";
      if (valStr == "/") body += "\"status\":\"Error BAD PATH\",";
      if (SPIFFS.exists(valStr)) {
        SPIFFS.remove(valStr);
        cmd = CMD_FS_DIR;
        body += "\"status\":\"OK\",";
        // send new list files ----
        body += getDirFS() + ",";
      } else {
        if (valStr.lastIndexOf(".") == -1) {
          SPIFFS.remove(valStr + ".");
          body += "\"status\":\"Remove Directory " + valStr + "\",";
        } else {
          body += "\"status\":\"Error File Not Found\",";
        }
      }
      break;

    case CMD_RESET:
      showWarning(CRGB::MediumSeaGreen, 2000U, 500U);
      ESP.restart();
      return;

    case CMD_OTA: runOTA(); break;

    default:
      break;
  }

  body += getCurState();
  sendResponse(cmd, body);
}

// ======================================
String getInfo() {
  byte mac[6];
  WiFi.macAddress(mac);
  IPAddress ip = WiFi.localIP();
  String ssid = jsonRead(configSetup, "ssid");
  String lamp_info = "";
  lamp_info += "\"ssid\":\"" + ssid + "\",";
  lamp_info += "\"mac\":\"" + (String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX)) + "\",";
  lamp_info += "\"free_heap\":" + String(system_get_free_heap_size()) + ",";
  lamp_info += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  //  WIDTH | HEIGHT | MATRIX_TYPE | CONNECTION_ANGLE | STRIP_DIRECTION | COLOR_ORDER
  lamp_info += "\"matrix\":[" + String(WIDTH) + "," + String( HEIGHT) + "," + String(MATRIX_TYPE) + "," + String(CONNECTION_ANGLE) + "," + String(STRIP_DIRECTION) + "],";
  lamp_info += "\"ver\":\"" + VERSION + "\"";
  return lamp_info;
}

// ======================================
String getCurState() {
  String lamp_state = "";
  lamp_state += getLampID() + ",";
  lamp_state += "\"pass\":\"" + AP_PASS + "\",";
  lamp_state += "\"ver\":\"" + VERSION + "\",";
  lamp_state += "\"power\":" + String(ONflag) + ",";
  lamp_state += "\"workgroup\":\"" + String(WORKGROUP) + "\",";
  lamp_state += "\"custom_eff\":" + String(custom_eff) + ",";
  lamp_state += "\"eff_auto\":" + String(eff_auto) + ",";
  lamp_state += "\"cycle\":" + String(FavoritesManager::FavoritesRunning) + ",";
  lamp_state += "\"list_idx\":" + String(currentMode) + ",";
  lamp_state += "\"max_eff\":" + String(MODE_AMOUNT) + ",";
  lamp_state += "\"bright\":" + String(modes[currentMode].Brightness) + ",";
  lamp_state += "\"speed\":" + String(modes[currentMode].Speed) + ",";
  lamp_state += "\"scale\":" + String(modes[currentMode].Scale);
  return lamp_state;
}

// ======================================
String getLampID() {
  IPAddress ip = WiFi.localIP();
  String id = "\"name\":\"" + LAMP_NAME + "\",";
  id += "\"ip\":\"" + ipToString(ip) + "\"";
  return id;
}

// ======================================
String ipToString(IPAddress ip) {
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

// ======================================
String getDirFS() {
  //  if (!HTTP.hasArg("dir")) {
  //    HTTP.send(500, "text/plain", "BAD ARGS");
  //    return;
  //  }
  String path = ""; //HTTP.arg("dir");
  Dir dir = SPIFFS.openDir(path);
  path = String();
  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
#if defined (USE_LittleFS)
    output += "\",\"name\":\"" + String(entry.name()).substring(0);
#else
    output += "\",\"name\":\"" + String(entry.name()).substring(1);
#endif
    output += "\",\"size\":\"" + String(entry.size());
    output += "\"}";
    entry.close();
  }
  output += "]";
  return "\"fs\":" + output;
}

// ======================================
void sendResponse(uint8_t cmd, String json) {
  String body = "\"id\":" + String(cmd) + ",";
  body += "\"json\":{";
  body += json;
  body += "}";
  sendHTML(body);
}

// ======================================
void sendHTML(String body) {
  String output = "<!DOCTYPE html>\r\n";
  output += "<html lang='en'>\r\n";
  output += "<head>\r\n<meta charset='utf-8'>\r\n<title>ESP-8266</title>\r\n";
  output += "<script>window.onload=function(){window.parent.postMessage(document.body.innerHTML, '*')};</script>\r\n";
  output += "</head>\r\n";
  output += "<body>{" + body + "}</body>";
  output += "</html>\r\n";
  HTTP.send(200, "text/html", output);
}