// --------------------------------------
// espModeStat default lamp start effect
// --------------------------------------
void  espModeState(uint8_t color) {
  if (loadingFlag) {
    loadingFlag = false;
    step = deltaValue;
    deltaValue = 1;
    hue2 = 0;
    deltaHue2 = 1;
    DrawLine(CENTER_X_MINOR, CENTER_Y_MINOR, CENTER_X_MAJOR + 1, CENTER_Y_MINOR, CHSV(color, 255, 210));
    DrawLine(CENTER_X_MINOR, CENTER_Y_MINOR - 1, CENTER_X_MAJOR + 1, CENTER_Y_MINOR - 1, CHSV(color, 255, 210));
    // setModeSettings(128U, 128U);
    pcnt = 1;
    FastLED.clear();
  }
  if (pcnt > 0 & pcnt < 200) {
    if (pcnt != 0) {
      pcnt++;
    }

    // animation esp state ===========
    dimAll(108);
    //    if (step % 2 == 0) {
    uint8_t w = validMinMax(hue2, 0, floor(WIDTH / 2) - 1);
    uint8_t posY = validMinMax(CENTER_Y_MINOR + deltaHue2, 0, HEIGHT - 1);
    DrawLine(CENTER_X_MINOR - w, posY, CENTER_X_MAJOR + w, posY, CHSV(color, 255, (210 - deltaHue2)));
    posY = validMinMax(CENTER_Y_MINOR - 1 - deltaHue2, 1, HEIGHT - 1);
    DrawLine(CENTER_X_MINOR - w, posY, CENTER_X_MAJOR + w, posY, CHSV(color, 255, (210 - deltaHue2)));

    if (deltaHue2 == 0) {
      deltaHue2 = 1;
    }
    hue2++;
    deltaHue2 = deltaHue2 << 1;
    if (deltaHue2 == 2) {
      deltaHue2 = deltaHue2 << 1;
    }
    if (CENTER_Y_MINOR + deltaHue2 > HEIGHT) {
      deltaHue2 = 0;
      hue2 = 0;
    }
    // LOG.printf_P(PSTR("espModeState | pcnt = %05d | deltaHue2 = %03d | step %03d | ONflag • %s\n"), pcnt, deltaHue2, step, (ONflag ? "TRUE" : "FALSE"));
  } else {

#ifdef USE_NTP
    // error ntp ------------------
    color = 255;        // если при включенном NTP время не получено, будем красным цветом мигать
#else
    color = 176U;       // иначе скромно синим - нормальная ситуация при отсутствии NTP
#endif //USE_NTP
    // animtion no time -----------
    leds[XY(CENTER_X_MINOR , 0U)] = CHSV( color, 255, (step % 4 == 0) ? 200 : 128);

  }
  // clear led lamp ---------------
  if ( pcnt >= 100) {
    pcnt = 0;
    //    FastLED.clear();
    //    FastLED.delay(2);
    FastLED.clear();
    delay(2);
    FastLED.show();
    loadingFlag = false;
  }
  step++;
}

//---------------------------------------
// Global Function
//---------------------------------------
void drawRec(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, uint32_t color) {
  for (uint8_t y = startY; y < endY; y++) {
    for (uint8_t x = startX; x < endX; x++) {
      drawPixelXY(x, y, color);
    }
  }
}

//---------------------------------------
void drawRecCHSV(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, CHSV color) {
  for (uint8_t y = startY; y < endY; y++) {
    for (uint8_t x = startX; x < endX; x++) {
      drawPixelXY(x, y, color);
    }
  }
}

//--------------------------------------
uint8_t validMinMax(float val, uint8_t minV, uint32_t maxV) {
  uint8_t result;
  if (val <= minV) {
    result = minV;
  } else if (val >= maxV) {
    result = maxV;
  } else {
    result = ceil(val);
  }
  //  LOG.printf_P(PSTR( "result: %f | val: %f \n\r"), result, val);
  return result;
}

// альтернативный градиент для ламп собраных из лент с вертикальной компоновкой
// gradientHorizontal | gradientVertical менее производительный но работает на всех видах ламп
//--------------------------------------
void gradientHorizontal(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, uint8_t start_color, uint8_t end_color, uint8_t start_br, uint8_t end_br, uint8_t saturate) {
  float step_color = 0;
  float step_br = 0;
  if (startX == endX) {
    endX++;
  }
  if (startY == endY) {
    endY++;
  }
  step_color = (end_color - start_color) / abs(startX - endX);
  if (start_color >  end_color) {
    step_color -= 1.2;
  } else {
    step_color += 1.2;
  }

  step_br = (end_br - start_br) / abs(startX - endX);
  if (start_br >  end_color) {
    step_br -= 1.2;
  } else {
    step_br += 1.2;
  }

  // LOG.printf_P(PSTR( "\n step_color: %f | step_br: %f \n\n\r"), step_color, step_br);
  for (uint8_t x = startX; x < endX; x++) {
    for (uint8_t y = startY; y < endY; y++) {
      CHSV thisColor = CHSV((uint8_t) validMinMax((start_color + (x - startX) * step_color), 1, 254), saturate,
                            (uint8_t) validMinMax((start_br + (x - startX) * step_br), 0, 255) );
      drawPixelXY(x, y, thisColor);
    }
  }
}

//--------------------------------------
void gradientVertical(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY, uint8_t start_color, uint8_t end_color, uint8_t start_br, uint8_t end_br, uint8_t saturate) {
  float step_color = 0;
  float step_br = 0;
  if (startX == endX) {
    endX++;
  }
  if (startY == endY) {
    endY++;
  }
  step_color = (end_color - start_color) / abs(startY - endY);

  if (start_color >  end_color) {
    step_color -= 1.2;
  } else {
    step_color += 1.2;
  }

  step_br = (end_br - start_br) / abs(startY - endY);
  if (start_br >  end_color) {
    step_br -= 1.2;
  } else {
    step_br += 1.2;
  }
  for (uint8_t y = startY; y < endY; y++) {
    CHSV thisColor = CHSV( (uint8_t) validMinMax((start_color + (y - startY) * step_color), 0, 255), saturate,
                           (uint8_t) validMinMax((start_br + (y - startY) * step_br), 0, 255) );
    for (uint8_t x = startX; x < endX; x++) {
      drawPixelXY(x, y, thisColor);
    }
  }
}

//---------------------------------------
// gradientDownTop • более плавный градиент в отличие от gradientVertical
// но может некоректно работать на лампах собранных на ленточных светодиодах
//---------------------------------------
void gradientDownTop( uint8_t bottom, CHSV bottom_color, uint8_t top, CHSV top_color ) {
  //  FORWARD_HUES: hue always goes clockwise
  //  BACKWARD_HUES: hue always goes counter-clockwise
  //  SHORTEST_HUES: hue goes whichever way is shortest
  //  LONGEST_HUES: hue goes whichever way is longest
  if (STRIP_DIRECTION < 2) {
    // STRIP_DIRECTION to UP ========
    fill_gradient(leds, top * WIDTH, top_color, bottom * WIDTH, bottom_color, SHORTEST_HUES);
  } else {
    // STRIP_DIRECTION to DOWN ======
    fill_gradient(leds, NUM_LEDS - bottom * WIDTH - 1, bottom_color, NUM_LEDS - top * WIDTH, top_color, SHORTEST_HUES);
  }
}

//---------------------------------------
// New Effects
//---------------------------------------
uint32_t colorDimm(uint32_t colorValue, long lenght, long pixel) {

  uint8_t red = (colorValue & 0x00FF0000) >> 16;
  uint8_t green = (colorValue & 0x0000FF00) >> 8;
  uint8_t blue = (colorValue & 0x000000FF);

  double prozent = 100 / lenght;

  red = red - red * ((prozent * pixel) / 100);
  green = green - green * ((prozent * pixel) / 100);
  blue = blue - blue * ((prozent * pixel) / 100);

  // colorValue = strip.Color(red,green,blue);
  colorValue = red;
  colorValue = (colorValue << 8) + green;
  colorValue = (colorValue << 8) + blue;
  return colorValue;
}

// =============== Wine ================
//    © SlingMaster | by Alex Dovby
//               EFF_WINE
//--------------------------------------

void colorsWine() {
  uint8_t divider;
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      // scale | speed
      setModeSettings(20U + random8(200U), 200U);
    }
#endif
    loadingFlag = false;
    fillAll(CHSV(55U, 255U, 65U));
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    // minspeed 230 maxspeed 250 ============
    // minscale  40 maxscale  75 ============
    // красное вино hue > 0 & <=10
    // розовое вино hue > 10 & <=20
    // белое вино   hue > 20U & <= 40
    // шампанское   hue > 40U & <= 60

    deltaHue2 = 0U;                         // count для замедления смены цвета
    step = deltaValue;                      // чтообы при старте эффекта сразу покрасить лампу
    deltaHue = 1U;                          // direction | 0 hue-- | 1 hue++ |
    hue = 55U;                              // Start Color
    hue2 = 65U;                             // Brightness
    pcnt = 0;
  }

  deltaHue2++;
  // маштаб задает скорость изменения цвета 5 уровней
  divider = 5 - floor((modes[currentMode].Scale - 1) / 20);

  // возвращаем яркость для перехода к белому
  if (hue >= 10 && hue2 < 100U) {
    hue2++;
  }
  // уменьшаем яркость для красного вина
  if (hue < 10 && hue2 > 40U) {
    hue2--;
  }

  // изменение цвета вина -----
  if (deltaHue == 1U) {
    if (deltaHue2 % divider == 0) {
      hue++;
    }
  } else {
    if (deltaHue2 % divider == 0) {
      hue--;
    }
  }
  // --------

  // LOG.printf_P(PSTR("Wine | hue = %03d | Dir = %d | Brightness %03d | deltaHue2 %03d | divider %d | %d\n"), hue, deltaHue, hue2, deltaHue2, divider, step);

  // сдвигаем всё вверх -----------
  for (uint8_t x = 0U; x < WIDTH; x++) {
    for (uint8_t y = HEIGHT; y > 0U; y--) {
      drawPixelXY(x, y, getPixColorXY(x, y - 1U));
    }
  }

  if (hue > 40U) {
    // добавляем перляж для шампанского
    pcnt = random(0, WIDTH);
  } else {
    pcnt = 0;
  }

  // заполняем нижнюю строку с учетом перляжа
  for (uint8_t x = 0U; x < WIDTH; x++) {
    if ((x == pcnt) && (pcnt > 0)) {
      // с перляжем ------
      drawPixelXY(x, 0U, CHSV(hue, 150U, hue2 + 20U + random(0, 50U)));
    } else {
      drawPixelXY(x, 0U, CHSV(hue, 255U, hue2));
    }
  }

  // меняем направление изменения цвета вина от красного к шампанскому и обратно
  // в диапазоне шкалы HUE |0-60|
  if  (hue == 0U) {
    deltaHue = 1U;
  }
  if (hue == 60U) {
    deltaHue = 0U;
  }
  step++;
}

// ============== Swirl ================
//    © SlingMaster | by Alex Dovby
//              EFF_SWIRL
//--------------------------------------
void Swirl() {
  uint8_t divider;
  uint8_t lastHue;
  static const uint32_t colors[5][6] PROGMEM = {
    {CRGB::Blue, CRGB::DarkRed, CRGB::Aqua, CRGB::Magenta, CRGB::Gold, CRGB::Green },
    {CRGB::Yellow, CRGB::LemonChiffon, CRGB::LightYellow, CRGB::Gold, CRGB::Chocolate, CRGB::Goldenrod},
    {CRGB::Green, CRGB::DarkGreen, CRGB::LawnGreen, CRGB::SpringGreen, CRGB::Cyan, CRGB::Black },
    {CRGB::Blue, CRGB::DarkBlue, CRGB::MidnightBlue, CRGB::MediumSeaGreen, CRGB::MediumBlue, CRGB:: DeepSkyBlue },
    {CRGB::Magenta, CRGB::Red, CRGB::DarkMagenta, CRGB::IndianRed, CRGB::Gold, CRGB::MediumVioletRed }
  };
  uint32_t color;

  if (loadingFlag) {

#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      // scale | speed
      setModeSettings(50U + random8(190U), 250U);
    }
#endif
    loadingFlag = false;
    FastLED.clear();
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    step = deltaValue;                      // чтообы при старте эффекта сразу покрасить лампу
    deltaHue2 = 0U;                         // count для замедления смены цвета
    deltaHue = 0U;                          // direction | 0 hue-- | 1 hue++ |
    hue2 = 0U;                              // x
  }

  if (step >= deltaValue) {
    step = 0U;
  }
  divider = floor((modes[currentMode].Scale - 1) / 20); // маштаб задает смену палитры
  //  if (deltaValue > 50U && deltaHue2 == 0U) {
  //    hue = random8(6);                       // если низкая скорость меняем цвет после каждого витка
  //  }
  // задаем цвет и рисуем завиток --------
  color = colors[divider][hue];
  // drawPixelXY((hue2 + 1), (deltaHue2 - 1), 0x000000); // aded shadow
  drawPixelXY(hue2, deltaHue2, color);
  // LOG.printf_P(PSTR("Swirl | hue = %03d | x= %03d | y= %03d | divider %d | %d\n"), hue, hue2, deltaHue2, divider, step);
  // -------------------------------------

  hue2++;                     // x
  // два варианта custom_eff задается в сетапе лампы ----
  if (custom_eff == 1) {
    // blurScreen(beatsin8(5U, 20U, 5U));
    deltaHue2++;              // y
  } else {
    // blurScreen(10U);
    if (hue2 % 2 == 0) {
      deltaHue2++;            // y
    }
  }
  // -------------------------------------

  if  (hue2 > WIDTH) {
    hue2 = 0U;
  }

  if (deltaHue2 >= HEIGHT) {
    deltaHue2 = 0U;
    // new swirl ------------
    hue2 = random8(WIDTH - 2);
    // hue2 = hue2 + 2;
    // select new color -----
    hue = random8(6);

    if (lastHue == hue) {
      hue = hue + 1;
      if (hue >= 6) {
        hue = 0;
      }
    }
    lastHue = hue;
  }
  // blurScreen(beatsin8(5U, 20U, 5U));
  blurScreen(4U + random8(8));
  step++;
}

// -------------------------------------------
// for effect Ukraine
// -------------------------------------------
void drawCrest() {
  static const uint32_t data[9][5] PROGMEM = {
    {0x000000, 0x000000, 0xFFD700, 0x000000, 0x000000 },
    {0xFFD700, 0x000000, 0xFFD700, 0x000000, 0xFFD700 },
    {0xFFD700, 0x000000, 0xFFD700, 0x000000, 0xFFD700 },
    {0xFFD700, 0x000000, 0xFFD700, 0x000000, 0xFFD700 },
    {0xFFD700, 0x000000, 0xFFD700, 0x000000, 0xFFD700 },
    {0xFFD700, 0xFFD700, 0xFFD700, 0xFFD700, 0xFFD700 },
    {0xFFD700, 0x000000, 0xFFD700, 0x000000, 0xFFD700 },
    {0x000000, 0xFFD700, 0xFFD700, 0xFFD700, 0x000000 },
    {0x000000, 0x000000, 0xFFD700, 0x000000, 0x000000 }
  };

  uint8_t posX = CENTER_X_MAJOR - 3;
  uint8_t posY = 9;
  uint32_t color;
  if (HEIGHT > 16) {
    posY = CENTER_Y_MINOR - 1;
  }
  FastLED.clear();
  for (uint8_t y = 0U; y < 9; y++) {
    for (uint8_t x = 0U; x < 5; x++) {
      color = data[y][x];
      drawPixelXY(posX + x, posY - y, color);
    }
  }
}

// ============== Ukraine ==============
//      © SlingMaster | by Alex Dovby
//              EFF_UKRAINE
//--------------------------------------
void Ukraine() {
  uint8_t divider;
  uint32_t color;
  static const uint16_t MAX_TIME = 500;
  uint16_t tMAX = 100;
  static const uint8_t timeout = 100;
  static const uint32_t colors[2][5] = {
    {CRGB::Blue, CRGB::MediumBlue, 0x0F004F, 0x02002F, 0x1F2FFF },
    {CRGB::Yellow, CRGB::Gold, 0x4E4000, 0xFF6F00, 0xFFFF2F }
  };

  // Initialization =========================
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                     scale | speed
      setModeSettings(random8(250U), 200U + random8(50U));
    }
#endif
    loadingFlag = false;
    drawCrest();
    // minspeed 200 maxspeed 250 ============
    // minscale   0 maxscale 100 ============
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    step = deltaValue;                        // чтообы при старте эффекта сразу покрасить лампу
    deltaHue2 = 0U;                           // count для замедления смены цвета
    deltaHue = 0U;                            // direction | 0 hue-- | 1 hue++ |
    hue2 = 0U;                                // Brightness
    ff_x = 1U;                                // counter
    tMAX = 100U;                              // timeout
  }
  divider = floor((modes[currentMode].Scale - 1) / 10); // маштаб задает режим рестарта
  tMAX = timeout + 100 * divider;

  if ((ff_x > timeout - 10) && (ff_x < timeout)) { // таймаут блокировки отрисовки флага
    if (ff_x < timeout - 5) {                  // размытие тризуба
      blurScreen(beatsin8(5U, 60U, 5U));
    } else {
      blurScreen(210U - ff_x);
    }
  }

  if (ff_x > tMAX) {
    if (divider == 0U) {                       // отрисовка тризуба только раз
      ff_x = 0U;
      tMAX += 20;
    } else {
      if (ff_x > tMAX + 100U * divider) {      // рестар эффект
        drawCrest();
        ff_x = 1U;
      }
    }
  }
  if ((ff_x != 0U) || (divider > 0)) {
    ff_x++;
  }

  // Flag Draw =============================
  if ((ff_x > timeout) || (ff_x == 0U))  {     // отрисовка флага
    if (step >= deltaValue) {
      step = 0U;
      hue2 = random8(WIDTH - 2);               // случайное смещение мазка по оси Y
      hue = random8(5);                        // flag color
      // blurScreen(dim8_raw(beatsin8(3, 64, 100)));
      // blurScreen(beatsin8(5U, 60U, 5U));
      // dimAll(200U);
    }
    if (step % 8 == 0 && modes[currentMode].Speed > 230) {
      blurScreen(beatsin8(5U, 5U, 72U));
    }
    hue2++;                                    // x
    deltaHue2++;                               // y

    if (hue2 >= WIDTH) {
      if (deltaHue2 > HEIGHT - 2 ) {           // если матрица высокая дорисовываем остальные мазки
        deltaHue2 = random8(5);                // изменяем положение по Y только отрисовав весь флаг
      }
      if (step % 2 == 0) {
        hue2 = 0U;
      } else {
        hue2 = random8(WIDTH);                 // смещение первого мазка по оси X
      }
    }

    if (deltaHue2 >= HEIGHT) {
      deltaHue2 = 0U;
      if (deltaValue > 200U) {
        hue = random8(5);                      // если низкая скорость меняем цвет после каждого витка
      }
    }

    if (deltaHue2 > floor(HEIGHT / 2) - 1) {    // меняем цвет для разных частей флага
      color = colors[0][hue];
    } else {
      color = colors[1][hue];
    }

    // LOG.printf_P(PSTR("color = %08d | hue2 = %d | speed = %03d | custom_eff = %d\n"), color, hue2, deltaValue, custom_eff);
    drawPixelXY(hue2, deltaHue2, color);
    // ----------------------------------
    step++;
  }
}

// ============ Oil Paints ==============
//      © SlingMaster | by Alex Dovby
//              EFF_PAINT
//           Масляные Краски
//---------------------------------------
void OilPaints() {

  uint8_t divider;
  uint8_t entry_point;
  uint16_t value;
  uint16_t max_val;
  if (loadingFlag) {

#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                          scale | speed 210
      setModeSettings(1U + random8(252U), 1 + random8(219U));
    }
#endif
    loadingFlag = false;
    FastLED.clear();
    // blurScreen(beatsin8(5U, 50U, 5U));
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    step = deltaValue;                    // чтообы при старте эффекта сразу покрасить лампу
    hue = floor(21.25 * (random8(11) + 1)); // next color
    deltaHue = hue - 22;                  // last color
    deltaHue2 = 80;                       // min bright
    max_val = pow(2, WIDTH);
    //    for ( int i = WIDTH; i < (NUM_LEDS - WIDTH); i++) {
    //      leds[i] = CHSV(120U, 24U, 64U);
    //    }
  }

  if (step >= deltaValue) {
    step = 0U;
    // LOG.printf_P(PSTR("%03d | log: %f | val: %03d\n\r"), modes[currentMode].Brightness, log(modes[currentMode].Brightness), deltaHue2);
  }

  // Create Oil Paints --------------
  // выбираем краски  ---------------
  if (step % CENTER_Y_MINOR == 0) {
    divider = floor((modes[currentMode].Scale - 1) / 10);             // маштаб задает диапазон изменения цвета
    deltaHue = hue;                                                   // set last color
    hue += 6 * divider;                                               // new color
    hue2 = 255;                                                       // restore brightness
    deltaHue2 = 80 - floor(log(modes[currentMode].Brightness) * 6);   // min bright
    entry_point = random8(WIDTH);                                     // start X position
    trackingObjectHue[entry_point] = hue;                             // set start position
    drawPixelXY(entry_point,  HEIGHT - 2, CHSV(hue, 255U, 255U));
    // !!! ********
    if (custom_eff == 1) {
      drawPixelXY(entry_point + 1,  HEIGHT - 3, CHSV(hue + 30, 255U, 255U));
    }
    // ************
    // LOG.printf_P(PSTR("BR %03d | SP %03d | SC %03d | hue %03d\n\r"), modes[currentMode].Brightness, modes[currentMode].Speed, modes[currentMode].Scale, hue);
  }

  // формируем форму краски, плавно расширяя струю ----
  if (random8(3) == 1) {
    // LOG.println("<--");
    for (uint8_t x = 1U; x < WIDTH; x++) {
      if (trackingObjectHue[x] == hue) {
        trackingObjectHue[x - 1] = hue;
        break;
      }
    }
  } else {
    // LOG.println("-->");
    for (uint8_t x = WIDTH - 1; x > 0U ; x--) {
      if (trackingObjectHue[x] == hue) {
        trackingObjectHue[x + 1] = hue;
        break;
      }
      // LOG.printf_P(PSTR("x = %02d | value = %03d | hue = %03d \n\r"), x, trackingObjectHue[x], hue);
    }
  }
  // LOG.println("------------------------------------");

  // выводим сформированную строку --------------------- максимально яркую в момент смены цвета
  for (uint8_t x = 0U; x < WIDTH; x++) {
    //                                                                                set color  next |    last  |
    drawPixelXY(x,  HEIGHT - 1, CHSV(trackingObjectHue[x], 255U, (trackingObjectHue[x] == hue) ? hue2 : deltaHue2));
  }
  //  LOG.println("");
  // уменьшаем яркость для следующих строк
  if ( hue2 > (deltaHue2 + 16)) {
    hue2 -= 16U;
  }
  // сдвигаем неравномерно поток вниз ---
  value = random16(max_val);
  //LOG.printf_P(PSTR("value = %06d | "), value);
  for (uint8_t x = 0U; x < WIDTH; x++) {
    if ( bitRead(value, x ) == 0) {
      //LOG.print (" X");
      for (uint8_t y = 0U; y < HEIGHT - 1; y++) {
        drawPixelXY(x, y, getPixColorXY(x, y + 1U));
      }
    }
  }
  // LOG.printf_P(PSTR("%02d | hue2 = %03d | min = %03d \n\r"), step, hue2, deltaHue2);
  // -------------------------------------

  step++;
}


// ========== Botswana Rivers ===========
//      © SlingMaster | by Alex Dovby
//              EFF_RIVERS
//            Реки Ботсваны

//---------------------------------------
void flora() {
  uint32_t  FLORA_COLOR = 0x2F1F00;
  uint8_t posX =  floor(CENTER_X_MINOR - WIDTH * 0.3);
  uint8_t h =  random8(HEIGHT - 6U) + 4U;
  DrawLine(posX + 1, 1U, posX + 1, h - 1, 0x000000);
  DrawLine(posX + 2, 1U, posX + 2, h, FLORA_COLOR );
  drawPixelXY(posX + 2, h - random8(floor(h * 0.5)), random8(2U) == 1 ? 0xFF00E0 :  random8(2U) == 1 ? 0xFFFF00 : 0x00FF00);
  drawPixelXY(posX + 1, h - random8(floor(h * 0.25)), random8(2U) == 1 ? 0xFF00E0 : 0xFFFF00);
  if (random8(2U) == 1) {
    drawPixelXY(posX + 1, floor(h * 0.5), random8(2U) == 1 ? 0xEF001F :  0x9FFF00);
  }
  h =  floor(h * 0.65);
  if (WIDTH > 8) {
    DrawLine(posX - 1, 1U, posX - 1, h - 1, 0x000000);
  }
  DrawLine(posX, 1U, posX, h, FLORA_COLOR);
  drawPixelXY(posX, h - random8(floor(h * 0.5)), random8(2U) == 1 ? 0xFF00E0 : 0xFFFF00);
}

//---------------------------------------
void animeBobbles() {
  // сдвигаем всё вверх ----
  for (uint8_t x = CENTER_X_MAJOR; x < WIDTH; x++) {
    for (uint8_t y = HEIGHT; y > 0U; y--) {
      if (getPixColorXY(x, y - 1) == 0xFFFFF7) {
        drawPixelXY(x, y, 0xFFFFF7);
        drawPixelXY(x, y - 1, getPixColorXY(0, y - 1));
      }
    }
  }
  // ----------------------
  if ( step % 4 == 0) {
    drawPixelXY(CENTER_X_MAJOR + random8(5), 0U, 0xFFFFF7);
    if ( step % 12 == 0) {
      drawPixelXY(CENTER_X_MAJOR + 2 + random8(3), 0U, 0xFFFFF7);
    }
  }
}

//---------------------------------------
void createScene(uint8_t idx) {
  switch (idx) {
    case 0:     // blue green ------
      gradientDownTop(floor((HEIGHT - 1) * 0.5), CHSV(96, 255, 100), HEIGHT, CHSV(160, 255, 255));
      gradientDownTop(0, CHSV(96, 255, 255), CENTER_Y_MINOR, CHSV(96, 255, 100));
      break;
    case 1:     // aquamarine green
      gradientDownTop(floor((HEIGHT - 1) * 0.3), CHSV(96, 255, 100), HEIGHT, CHSV(130, 255, 220));
      gradientDownTop(0, CHSV(96, 255, 255), floor(HEIGHT * 0.3), CHSV(96, 255, 100));
      break;
    case 2:     // blue aquamarine -
      gradientDownTop(floor((HEIGHT - 1) * 0.5), CHSV(170, 255, 100), HEIGHT, CHSV(160, 255, 200));
      gradientDownTop(0, CHSV(100, 255, 255), CENTER_Y_MINOR, CHSV(170, 255, 100));
      break;
    case 3:     // yellow green ----
      gradientDownTop(floor((HEIGHT - 1) * 0.5), CHSV(95, 255, 55), HEIGHT, CHSV(70, 255, 200));
      gradientDownTop(0, CHSV(95, 255, 255), CENTER_Y_MINOR, CHSV(100, 255, 55));
      break;
    case 4:     // sea green -------
      gradientDownTop(floor((HEIGHT - 1) * 0.3), CHSV(120, 255, 55), HEIGHT, CHSV(175, 255, 200));
      gradientDownTop(0, CHSV(120, 255, 255), floor(HEIGHT * 0.3), CHSV(120, 255, 55));
      break;
    default:
      gradientDownTop(floor((HEIGHT - 1) * 0.25), CHSV(180, 255, 85), HEIGHT, CHSV(160, 255, 200));
      gradientDownTop(0, CHSV(80, 255, 255), floor(HEIGHT * 0.25), CHSV(180, 255, 85));
      break;
  }
  flora();
}

//---------------------------------------
void createSceneM(uint8_t idx) {
  switch (idx) {
    case 0:     // blue green ------
      gradientVertical(0, CENTER_Y_MINOR, WIDTH, HEIGHT, 96, 150, 100, 255, 255U);
      gradientVertical(0, 0, WIDTH, CENTER_Y_MINOR, 96, 96, 255, 100, 255U);
      break;
    case 1:     // aquamarine green
      gradientVertical(0, floor(HEIGHT  * 0.3), WIDTH, HEIGHT, 96, 120, 100, 220, 255U);
      gradientVertical(0, 0, WIDTH, floor(HEIGHT  * 0.3), 96, 96, 255, 100, 255U);
      break;
    case 2:     // blue aquamarine -
      gradientVertical(0, CENTER_Y_MINOR, WIDTH, HEIGHT, 170, 160, 100, 200, 255U);
      gradientVertical(0, 0, WIDTH, CENTER_Y_MINOR, 100, 170, 255, 100, 255U);
      break;
    case 3:     // yellow green ----
      gradientVertical(0, CENTER_Y_MINOR, WIDTH, HEIGHT, 95, 65, 55, 200, 255U);
      gradientVertical(0, 0, WIDTH, CENTER_Y_MINOR, 95, 100, 255, 55, 255U);
      break;
    case 4:     // sea green -------
      gradientVertical(0, floor(HEIGHT  * 0.3), WIDTH, HEIGHT, 120, 160, 55, 200, 255U);
      gradientVertical(0, 0, WIDTH, floor(HEIGHT  * 0.3), 120, 120, 255, 55, 255U);
      break;
    default:
      drawRec(0, 0, WIDTH, HEIGHT, 0x000050);
      break;
  }
  flora();
}

//---------------------------------------
void BotswanaRivers() {
  // альтернативный градиент для ламп собраных из лент с вертикальной компоновкой
  // для корректной работы ALT_GRADIENT = true
  // для ламп из лент с горизонтальной компоновкой и матриц ALT_GRADIENT = false
  // ALT_GRADIENT = false более производительный и более плавная растяжка
  //------------------------------------------------------------------------------
  static const bool ALT_GRADIENT = false;

  uint8_t divider;
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                          scale | speed 210
      setModeSettings(1U + random8(252U), 20 + random8(180U));
    }
#endif
    loadingFlag = false;
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    step = deltaValue;                                          // чтообы при старте эффекта сразу покрасить лампу
    divider = floor((modes[currentMode].Scale - 1) / 20);       // маштаб задает смену палитры воды
    if (ALT_GRADIENT) {
      createSceneM(divider);
    } else {
      createScene(divider);
    }
  }

  if (step >= deltaValue) {
    step = 0U;
  }

  // restore scene after power on ---------
  if (getPixColorXY(0U, HEIGHT - 2) == CRGB::Black) {
    if (ALT_GRADIENT) {
      createSceneM(divider);
    } else {
      createScene(divider);
    }
  }

  // light at the bottom ------------------
  if (!ALT_GRADIENT) {
    if (step % 2 == 0) {
      if (random8(6) == 1) {
        //fill_gradient(leds, NUM_LEDS - WIDTH, CHSV(96U, 255U, 200U), NUM_LEDS, CHSV(50U, 255U, 255U), SHORTEST_HUES);
        if (STRIP_DIRECTION < 2) {
          fill_gradient(leds, 0, CHSV(96U, 255U, 190U), random8(WIDTH + random8(6)), CHSV(90U, 200U, 255U), SHORTEST_HUES);
        } else {
          fill_gradient(leds, NUM_LEDS - random8(WIDTH + random8(6)), CHSV(96U, 255U, 190U), NUM_LEDS, CHSV(90U, 200U, 255U), SHORTEST_HUES);
        }
      } else {
        //fill_gradient(leds, NUM_LEDS - WIDTH, CHSV(50U, 128U, 255U), NUM_LEDS, CHSV(90U, 255U, 180U), SHORTEST_HUES);
        if (STRIP_DIRECTION < 2) {
          fill_gradient(leds, 0, CHSV(85U, 128U, 255U), random8(WIDTH), CHSV(90U, 255U, 180U), SHORTEST_HUES);
        } else {
          fill_gradient(leds, NUM_LEDS - random8(WIDTH), CHSV(85U, 128U, 255U), NUM_LEDS, CHSV(90U, 255U, 180U), SHORTEST_HUES);
        }
      }
    }
  }

  // LOG.printf_P(PSTR("%02d | hue2 = %03d | min = %03d \n\r"), step, hue2, deltaHue2);
  // -------------------------------------
  animeBobbles();
  if (custom_eff == 1) {
    blurRows(leds, WIDTH, 3U, 10U);
    // blurScreen(beatsin8(0U, 5U, 0U));
  }
  step++;
}


// ============ Watercolor ==============
//      © SlingMaster | by Alex Dovby
//            EFF_WATERCOLOR
//               Акварель
//---------------------------------------
void SmearPaint(uint8_t obj[trackingOBJECT_MAX_COUNT]) {
  uint8_t divider;
  int temp;
  static const uint32_t colors[6][8] PROGMEM = {
    {0x2F0000,  0xFF4040, 0x6F0000, 0xAF0000, 0xff5f00, CRGB::Red, 0x480000, 0xFF0030},
    {0x002F00, CRGB::LawnGreen, 0x006F00, 0x00AF00, CRGB::DarkMagenta, 0x00FF00, 0x004800, 0x00FF30},
    {0x002F1F, CRGB::DarkCyan, 0x00FF7F, 0x007FFF, 0x20FF5F, CRGB::Cyan, 0x004848, 0x7FCFCF },
    {0x00002F, 0x5030FF, 0x00006F, 0x0000AF, CRGB::DarkCyan, 0x0000FF, 0x000048, 0x5F5FFF},
    {0x2F002F, 0xFF4040, 0x6F004A, 0xFF0030, CRGB::DarkMagenta, CRGB::Magenta, 0x480048, 0x3F00FF},
    {CRGB::Blue, CRGB::Red, CRGB::Gold, CRGB::Green, CRGB::DarkCyan, CRGB::DarkMagenta, 0x000000, 0xFF7F00 }
  };
  if (trackingObjectHue[5] == 1) {  // direction >>>
    obj[1]++;
    if (obj[1] >= obj[2]) {
      trackingObjectHue[5] = 0;     // swap direction
      obj[3]--;                     // new line
      if (step % 2 == 0) {
        obj[1]++;
      } else {
        obj[1]--;
      }

      obj[0]--;
    }
  } else {                          // direction <<<
    obj[1]--;
    if (obj[1] <= (obj[2] - obj[0])) {
      trackingObjectHue[5] = 1;     // swap direction
      obj[3]--;                     // new line
      if (obj[0] >= 1) {
        temp = obj[0] - 1;
        if (temp < 0) {
          temp = 0;
        }
        obj[0] = temp;
        obj[1]++;
      }
    }
  }

  if (obj[3] == 255) {
    deltaHue = 255;
  }

  divider = floor((modes[currentMode].Scale - 1) / 16.7);
  if ( (obj[1] >= WIDTH) || (obj[3] == obj[4]) ) {
    // deltaHue value == 255 activate -------
    // set new parameter for new smear ------
    deltaHue = 255;
  }
  drawPixelXY(obj[1], obj[3], colors[divider][hue]);

  // alternative variant without dimmer effect
  // uint8_t h = obj[3] - obj[4];
  // uint8_t br = 266 - 12 * h;
  // if (h > 0) {
  // drawPixelXY(obj[1], obj[3], makeDarker(colors[divider][hue], br));
  // } else {
  // drawPixelXY(obj[1], obj[3], makeDarker(colors[divider][hue], 240));
  // }
}



//---------------------------------------
void Watercolor() {
  // #define DIMSPEED (254U - 500U / WIDTH / HEIGHT)
  uint8_t divider;
  if (loadingFlag) {

#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                          scale | speed 250
      setModeSettings(1U + random8(252U), 1 + random8(250U));
    }
#endif
    loadingFlag = false;
    FastLED.clear();
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    step = deltaValue;                    // чтообы при старте эффекта сразу покрасить лампу
    hue = 0;
    deltaHue = 255;                       // last color
    trackingObjectHue[1] = floor(WIDTH * 0.25);
    trackingObjectHue[3] = floor(HEIGHT * 0.25);
  }

  if (step >= deltaValue) {
    step = 0U;
    // LOG.printf_P(PSTR("%03d | log: %f | val: %03d | divider: %d \n\r"), modes[currentMode].Brightness, log(modes[currentMode].Brightness), deltaHue2, divider);
  }

  // ******************************
  // set random parameter for smear
  // ******************************
  if (deltaHue == 255) {

    trackingObjectHue[0] = 4 + random8(floor(WIDTH * 0.25));                // width
    trackingObjectHue[1] = random8(WIDTH - trackingObjectHue[0]);           // x
    int temp =  trackingObjectHue[1] + trackingObjectHue[0];
    if (temp >= (WIDTH - 1)) {
      temp = WIDTH - 1;
      if (trackingObjectHue[1] > 1) {
        trackingObjectHue[1]--;
      } else {
        trackingObjectHue[1]++;
      }
    }
    trackingObjectHue[2] = temp;                                            // x end
    trackingObjectHue[3] = 3 + random8(HEIGHT - 4);                         // y
    temp = trackingObjectHue[3] - random8(3) - 3;
    if (temp <= 0) {
      temp = 0;
    }
    trackingObjectHue[4] = temp;                                            // y end
    trackingObjectHue[5] = 1;
    divider = floor((modes[currentMode].Scale - 1) / 16.7);                 // маштаб задает смену палитры
    hue = random8(8);
    //    if (step % 127 == 0) {
    //      LOG.printf_P(PSTR("BR %03d | SP %03d | SC %03d | divider %d | [ %d ]\n\r"), modes[currentMode].Brightness, modes[currentMode].Speed, modes[currentMode].Scale, divider, hue);
    //    }
    hue2 = 255;
    deltaHue = 0;
  }
  // ******************************
  SmearPaint(trackingObjectHue);

  // LOG.printf_P(PSTR("%02d | hue2 = %03d | min = %03d \n\r"), step, hue2, deltaHue2);
  // -------------------------------------
  //  if (custom_eff == 1) {
  // dimAll(DIMSPEED);
  if (step % 2 == 0) {
    blurScreen(beatsin8(1U, 1U, 6U));
    // blurRows(leds, WIDTH, 3U, 10U);
  }
  //  }
  step++;
}

// =========== FeatherCandle ============
//         адаптация © SottNick
//    github.com/mnemocron/FeatherCandle
//      modify & design © SlingMaster
//           EFF_FEATHER_CANDLE
//                Свеча
//---------------------------------------
// FeatherCandle animation data
const uint8_t  level = 160;
const uint8_t  low_level = 110;
const uint8_t *ptr  = anim;                     // Current pointer into animation data
const uint8_t  w    = 7;                        // image width
const uint8_t  h    = 15;                       // image height
uint8_t        img[w * h];                      // Buffer for rendering image
uint8_t        deltaX = CENTER_X_MINOR - 4;     // position img
uint8_t last_brightness;
void FeatherCandleRoutine() {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
  if (selectedSettings) {
    // brightness | scale | speed
    // { 21, 220,  40}
    setModeSettings(1U + random8(99U), 190U + random8(65U));
  }
#endif
  if (loadingFlag) {
    FastLED.clear();
    hue = 0;
    trackingObjectState[0] = low_level;
    trackingObjectState[1] = low_level;
    trackingObjectState[2] = low_level;
    trackingObjectState[4] = CENTER_X_MINOR;
    loadingFlag = false;
  }

  uint8_t a = pgm_read_byte(ptr++);     // New frame X1/Y1
  if (a >= 0x90) {                      // EOD marker? (valid X1 never exceeds 8)
    ptr = anim;                         // Reset animation data pointer to start
    a   = pgm_read_byte(ptr++);         // and take first value
  }
  uint8_t x1 = a >> 4;                  // X1 = high 4 bits
  uint8_t y1 = a & 0x0F;                // Y1 = low 4 bits
  a  = pgm_read_byte(ptr++);            // New frame X2/Y2
  uint8_t x2 = a >> 4;                  // X2 = high 4 bits
  uint8_t y2 = a & 0x0F;                // Y2 = low 4 bits

  // Read rectangle of data from anim[] into portion of img[] buffer
  for (uint8_t y = y1; y <= y2; y++)
    for (uint8_t x = x1; x <= x2; x++) {
      img[y * w + x] = pgm_read_byte(ptr++);
    }
  int i = 0;
  uint8_t color = (modes[currentMode].Scale - 1U) * 2.57;



  // draw flame -------------------
  for (uint8_t y = 1; y < h; y++) {
    if ((HEIGHT < 15) || (WIDTH < 9)) {
      // for small matrix -----
      if (y % 2 == 0) {
        leds[XY(CENTER_X_MINOR - 1, 7)] = CHSV(color, 255U, 55 + random8(200));
        leds[XY(CENTER_X_MINOR, 6)] = CHSV(color, 255U, 160 + random8(90));
        leds[XY(CENTER_X_MINOR + 1, 6)] = CHSV(color, 255U, 205 + random8(50));
        leds[XY(CENTER_X_MINOR - 1, 5)] = CHSV(color, 255U, 155 + random8(100));
        leds[XY(CENTER_X_MINOR, 5)] = CHSV(color - 10U , 255U, 120 + random8(130));
        leds[XY(CENTER_X_MINOR, 4)] = CHSV(color - 10U , 255U, 100 + random8(120));
        DrawLine(0, 2U, WIDTH - 1, 2U, 0x000000);
      }
    } else {
      for (uint8_t x = 0; x < w; x++) {
        uint8_t brightness = img[i];
        leds[XY(deltaX + x, y)] = CHSV(brightness > 240 ? color : color - 10U , 255U, brightness);
        i++;
      }
    }

    // draw body FeatherCandle ------
    if (y <= 3) {
      if (y % 2 == 0) {
        gradientVertical(0, 0, WIDTH, 2, color, color, 48, 128, 20U);
      }
    }

    // drops of wax move -------------
    switch (hue ) {
      case 0:
        if (trackingObjectState[0] < level) {
          trackingObjectState[0]++;
        }
        break;
      case 1:
        if (trackingObjectState[0] > low_level) {
          trackingObjectState[0] --;
        }
        if (trackingObjectState[1] < level) {
          trackingObjectState[1] ++;
        }
        break;
      case 2:
        if (trackingObjectState[1] > low_level) {
          trackingObjectState[1] --;
        }
        if (trackingObjectState[2] < level) {
          trackingObjectState[2] ++;
        }
        break;
      case 3:
        if (trackingObjectState[2] > low_level) {
          trackingObjectState[2] --;
        } else {
          hue++;
          // set random position drop of wax
          trackingObjectState[4] = CENTER_X_MINOR - 3 + random8(6);
        }
        break;
    }

    if (hue > 3) {
      hue++;
    } else {
      // LOG.printf_P(PSTR("[0] = %03d | [1] = %03d | [2] = %03d \n\r"), trackingObjectState[0], trackingObjectState[1], trackingObjectState[2]);
      if (hue < 2) {
        leds[XY(trackingObjectState[4], 2)] = CHSV(50U, 20U, trackingObjectState[0]);
      }
      if ((hue == 1) || (hue == 2)) {
        leds[XY(trackingObjectState[4], 1)] = CHSV(50U, 15U, trackingObjectState[1]); // - 10;
      }
      if (hue > 1) {
        leds[XY(trackingObjectState[4], 0)] = CHSV(50U, 5U, trackingObjectState[2]); // - 20;
      }
    }
  }

  // next -----------------
  if ((trackingObjectState[0] == level) || (trackingObjectState[1] == level) || (trackingObjectState[2] == level)) {
    hue++;
  }
}

// ============= Hourglass ==============
//             © SlingMaster
//             EFF_HOURGLASS
//             Песочные Часы
//---------------------------------------

void Hourglass() {
  uint8_t divider;
  uint8_t h;
  float SIZE = 0.43;
  const uint8_t topPos  = HEIGHT - floor(HEIGHT * SIZE);
  const uint8_t route = HEIGHT - floor(HEIGHT * SIZE) + 1;
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                          scale | speed 210
      setModeSettings(15U + random8(225U), random8(255U));
    }
#endif
    loadingFlag = false;
    deltaValue = 255U - modes[currentMode].Speed + 1U;
    step = deltaValue;                                          // чтообы при старте эффекта сразу покрасить лампу
    deltaHue2 = 0;
    h = floor(NUM_LEDS * SIZE / WIDTH) * WIDTH;
    FastLED.clear();
    hue2 = 0;
  }
  if (hue2 == 0) {


    divider = modes[currentMode].Scale;
    if (divider < 10) {
      hue = step;
    } else {
      hue = divider * 2.55;
    }

    if (STRIP_DIRECTION < 2) {
      // STRIP_DIRECTION to UP ========
      fill_gradient(leds, NUM_LEDS - h, CHSV(hue, 255, 254), NUM_LEDS - deltaHue2, CHSV(hue, 255, 30), SHORTEST_HUES);
      fill_gradient(leds, 0, CHSV(hue, 255, 254), deltaHue2, CHSV(hue, 255, 30), SHORTEST_HUES);
    } else {
      // STRIP_DIRECTION to DOWN ======
      fill_gradient(leds, deltaHue2, CHSV(hue, 255, 30), h, CHSV(hue, 255, 254), SHORTEST_HUES);
      fill_gradient(leds, NUM_LEDS - deltaHue2, CHSV(hue, 255, 30), NUM_LEDS, CHSV(hue, 255, 254), SHORTEST_HUES);
    }

    if (custom_eff == 1) {
      if (deltaHue2 < WIDTH) {
        blurScreen(8U);
      }
    }
    deltaHue2++;
    if (deltaHue2 > h) {
      deltaHue2 = 0;
      hue2 = 1;
      // борьба с артифактом -----
      DrawLine(0, topPos, WIDTH - 1, topPos, 0x000000);
    }
    step++;
  }

  // сдвигаем всё вверх -----------
  // имитация переворота песочных часов
  if (hue2 > 0) {
    for (uint8_t x = 0U; x < WIDTH; x++) {
      for (uint8_t y = HEIGHT; y > 0U; y--) {
        drawPixelXY(x, y, getPixColorXY(x, y - 1U));
        drawPixelXY(x, y - 1, 0x000000);
      }
    }
    hue2++;
    if (hue2 > route) {
      hue2 = 0;
    }
  }
}

// ============== Spectrum ==============
//             © SlingMaster
//              EFF_SPECTRUM
//                Spectrum
//---------------------------------------

void  Spectrum() {
  uint8_t divider;
  uint8_t deltaY;
  uint8_t posX;

  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                     scale | speed 210
      setModeSettings(random8(100U), 180U + random8(75U));
    }
#endif
    loadingFlag = false;
    step = deltaValue;
    hue = 120;
    hue2 = hue + 30;
    posX = 0;
    deltaHue2 = 0;
    divider = floor(modes[currentMode].Scale / 12.5);
    if (divider % 2 == 0) {
      blurScreen(dim8_raw(beatsin8(3, 64, 3)));
      DrawLine(3, 0, 4, 0 , 0x00FF00);
    } else {
      FastLED.clear();
      DrawLine(3, 0, 4, 0 , 0xFF3F00);
    }
  }

  deltaHue2++;
  if (deltaHue2 >= WIDTH) {
    deltaHue2 = 0;
  }
  divider = floor(modes[currentMode].Scale / 12.5);
  posX = deltaHue2;
  deltaY = floor((HEIGHT - validMinMax(random8(HEIGHT - 1), 3, HEIGHT)) * 0.5);

  switch (divider ) {
    case 0:
    case 1:
      if (step % 32 == 0) {
        hue++;
      }
      break;
    case 2:
    case 3:
      if (step % 2 == 0) {
        hue++;
      }
      break;
    case 4:
    case 5:
      if (step == 0) {
        hue += 30;
      }
      break;
    default:
      if (step == 0) {
        hue += 10;
        hue2 -= 10;
      }
      break;
  }

  if (divider < 6) {
    if (hue >= 225) {
      hue2 = 224;
    } else {
      hue2 = hue + 30;
    }
  }

  // if (step % 32 == 0) {
  //   LOG.printf_P(PSTR("divider = %01d | hue = %03d | hue2 = %03d | scale = %03d | BR = %03d\n\r"), divider, hue, hue2, modes[currentMode].Scale, (deltaValue));
  // }

  deltaValue = random8(128);          // delta brightness
  if (posX % 2 == 0) {
    if (divider < 6) {
      gradientVertical(posX, CENTER_Y_MINOR, posX + 1, CENTER_Y_MINOR + random8(2), hue + 15, hue2 - 15, 128 + deltaValue, 255 - deltaValue, 255U);
    }
    DrawLine(posX, HEIGHT - deltaY, posX, HEIGHT - 1, 0x000000);
    DrawLine(posX, 0, posX, deltaY , 0x000000);
  } else {
    gradientVertical(posX, deltaY, posX + 1, HEIGHT - deltaY, hue, hue2, 128 + deltaValue, 255 - deltaValue, 255U);
  }

  if (divider % 2 == 0) {
    blurScreen(6U);
  } else {
    DrawLine(posX, HEIGHT - deltaY, posX, HEIGHT - 1, 0x000000);
    DrawLine(posX, 0, posX, deltaY , 0x000000);
    dimAll(255 - posX);
  }
  step++;
}

// ============ Lotus Flower ============
//             © SlingMaster
//               EFF_LOTUS
//             Цветок Лотоса
//---------------------------------------
void  Flower() {
  uint8_t br;
  if (step < 128) {
    br = 255 - step;  // 255 >> 128
  } else {
    br = step;        // 128 >> 255
  }
  if (modes[currentMode].Scale > 10) {
    dimAll(90);
    hue = floor(modes[currentMode].Scale * 1.9) + floor(br / 4);
  } else {
    FastLED.clear();
    hue = step;
  }
  if (step > 190) {
    hue2 = validMinMax(hue - 64 + floor(br / 4), 190, 250);
  } else {
    hue2 = hue + 64 - floor(br / 4);
  }

  for (uint8_t x = 0U ; x < WIDTH ; x++) {
    if (x % 6 == 0) {
      gradientVertical( x - deltaValue, 2U, x + 1 - deltaValue, HEIGHT - floor((255 - br) / 24) - random8(2), hue, hue2, 255, floor(br * 0.5), 255U);
      gradientVertical( x + 3U - deltaValue, 0U, x + 4U - deltaValue, HEIGHT - floor(br / 24) + random8(3), hue, hue2, 255, floor((255 - br * 0.5)), 255U);
      drawPixelXY(x - deltaValue, 0, 0x005F00);
    }
  }
}

//---------------------------------------
void LotusFlower() {
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                     scale | speed
      setModeSettings(random8(100U), 50U + random8(190U));
    }
#endif
    loadingFlag = false;
    deltaValue = 0;
    step = deltaValue;
    deltaValue = 0;
    hue = 120;
    hue2 = 0;
    deltaHue = 0;
    deltaHue2 = 0;
    FastLED.clear();
  }

  Flower();
  if (deltaHue == 0) {               // rotation
    deltaValue--;
    if (deltaValue <= 0) {
      deltaValue = 3;
    }
  } else {
    deltaValue++;
    if (deltaValue >= 3) {
      deltaValue = 0;
    }
  }
  deltaHue2++;
  if (deltaHue2 >= 18) {           // swap direction rotate
    deltaHue2 = 0;
    deltaHue = (deltaHue == 0) ? 1 : 0;
  }
  step++;
}

// =========== Christmas Tree ===========
//             © SlingMaster
//           EFF_CHRISTMAS_TREE
//            Новогодняя Елка
//---------------------------------------
void VirtualSnow() {
  for (uint8_t x = 0U; x < WIDTH; x++) {
    for (uint8_t y = 0U; y < HEIGHT - 1; y++) {
      noise3d[0][x][y] = noise3d[0][x][y + 1];
      if (noise3d[0][x][y] > 0) {
        drawPixelXY(x, y, CHSV(170, 5U, 127 + random8(128)));
      }
    }
  }
  uint8_t posX = random(WIDTH);
  for (uint8_t x = 0U; x < WIDTH; x++) {
    // заполняем случайно верхнюю строку
    if (posX == x) {
      if (step % 3 == 0) {
        noise3d[0][x][HEIGHT - 1U] = 1;
      } else {
        noise3d[0][x][HEIGHT - 1U]  = 0;
      }
    } else {
      noise3d[0][x][HEIGHT - 1U]  = 0;
    }
  }
}

//---------------------------------------
void GreenTree() {
  hue = floor(step / 32) * 32U;
  FastLED.clear();
  for (uint8_t x = 0U; x < WIDTH + 1 ; x++) {
    if (x % 8 == 0) {
      if (modes[currentMode].Scale < 60) {
        // nature -----
        DrawLine(x - 1U - deltaValue, floor(HEIGHT * 0.70), x + 1U - deltaValue, floor(HEIGHT * 0.70), 0x002F00);
        DrawLine(x - 1U - deltaValue, floor(HEIGHT * 0.55), x + 1U - deltaValue, floor(HEIGHT * 0.55), 0x004F00);
        DrawLine(x - 2U - deltaValue, floor(HEIGHT * 0.35), x + 2U - deltaValue, floor(HEIGHT * 0.35), 0x005F00);
        DrawLine(x - 2U - deltaValue, floor(HEIGHT * 0.15), x + 2U - deltaValue, floor(HEIGHT * 0.15), 0x007F00);
        drawPixelXY(x - 3U - deltaValue, floor(HEIGHT * 0.15), 0x001F00);
        drawPixelXY(x + 3U - deltaValue, floor(HEIGHT * 0.15), 0x001F00);
        gradientVertical( x - deltaValue, 0U, x + 1U - deltaValue, HEIGHT, 96U, 97U, 190U, 32U, 255U);
      } else {
        // holiday -----
        //        drawPixelXY(x - 1 - deltaValue, CENTER_Y_MINOR, CHSV(floor(random8(255) / 32) * 32U, 255U, 128 + random8(128)));
        //        drawPixelXY(x + 1 - deltaValue, CENTER_Y_MINOR, CHSV(floor(random8(90) / 32) * 32U, 255U, 128 + random8(128)));
        //        drawPixelXY(x - deltaValue, floor(HEIGHT * 0.2), CHSV(floor(random8(255) / 32) * 32U, 255U, 128 + random8(128)));
        //        drawPixelXY(x - 2 - deltaValue, floor(HEIGHT * 0.25), CHSV(floor(random8(90) / 32) * 32U, 255U, 128 + random8(128)));
        //        drawPixelXY(x + 2 - deltaValue, floor(HEIGHT * 0.25), CHSV(floor(random8(255) / 32) * 32U, 255U, 128 + random8(128)));
        //        drawPixelXY(x - deltaValue, 0U, CHSV(96U, 255U, 200U));

        drawPixelXY(x - 1 - deltaValue, floor(HEIGHT * 0.6), CHSV(step, 255U, 128 + random8(128)));
        drawPixelXY(x + 1 - deltaValue, floor(HEIGHT * 0.6), CHSV(step, 255U, 128 + random8(128)));

        drawPixelXY(x - deltaValue, floor(HEIGHT * 0.4), CHSV(step, 255U, 200U));

        drawPixelXY(x - deltaValue, floor(HEIGHT * 0.2), CHSV(step, 255U, 190 + random8(65)));
        drawPixelXY(x - 2 - deltaValue, floor(HEIGHT * 0.25), CHSV(step, 255U, 96 + random8(128)));
        drawPixelXY(x + 2 - deltaValue, floor(HEIGHT * 0.25), CHSV(step, 255U, 96 + random8(128)));

        drawPixelXY(x - 2 - deltaValue, 1U, CHSV(step, 255U, 200U));
        drawPixelXY(x - deltaValue, 0U, CHSV(step, 255U, 250U));
        drawPixelXY(x + 2 - deltaValue, 1U, CHSV(step, 255U, 200U));

        gradientVertical( x - deltaValue, floor(HEIGHT * 0.75), x + 1U - deltaValue, HEIGHT,  hue, hue + 2, 250U, 0U, 128U);
      }
    }
  }
}

//---------------------------------------
void ChristmasTree() {
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                     scale | speed
      setModeSettings(random8(100U), 10U + random8(128U));
    }
#endif
    loadingFlag = false;
    deltaValue = 0;
    step = deltaValue;
    FastLED.clear();
  }

  GreenTree();
  if (modes[currentMode].Scale < 60) {
    VirtualSnow();
  }
  if (modes[currentMode].Scale > 30) {
    deltaValue++;
  }
  if (deltaValue >= 8) {
    deltaValue = 0;
  }
  step++;
}

// ============== ByEffect ==============
//             © SlingMaster
//             EFF_BY_EFFECT
//            Побочный Эффект
// --------------------------------------
void ByEffect() {
  uint8_t saturation;
  uint8_t delta;
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      //                     scale | speed 210
      setModeSettings(random8(100U), random8(200U));
    }
#endif
    loadingFlag = false;
    deltaValue = 0;
    step = deltaValue;
    FastLED.clear();
  }

  hue = floor(step / 32) * 32U;
  dimAll(180);
  // ------
  saturation = 255U;
  delta = 0;
  for (uint8_t x = 0U; x < WIDTH + 1 ; x++) {
    if (x % 8 == 0) {
      gradientVertical( x - deltaValue, floor(HEIGHT * 0.75), x + 1U - deltaValue, HEIGHT,  hue, hue + 2, 250U, 0U, 255U);
      if (modes[currentMode].Scale > 50) {
        delta = random8(200U);
      }
      drawPixelXY(x - 2 - deltaValue, floor(HEIGHT * 0.7), CHSV(step, saturation - delta, 128 + random8(128)));
      drawPixelXY(x + 2 - deltaValue, floor(HEIGHT * 0.7), CHSV(step, saturation, 128 + random8(128)));

      drawPixelXY(x - deltaValue, floor(HEIGHT * 0.6), CHSV(hue, 255U, 190 + random8(65)));
      if (modes[currentMode].Scale > 50) {
        delta = random8(200U);
      }
      drawPixelXY(x - 1 - deltaValue, CENTER_Y_MINOR, CHSV(step, saturation, 128 + random8(128)));
      drawPixelXY(x + 1 - deltaValue, CENTER_Y_MINOR, CHSV(step, saturation - delta, 128 + random8(128)));

      drawPixelXY(x - deltaValue, floor(HEIGHT * 0.4), CHSV(hue, 255U, 200U));
      if (modes[currentMode].Scale > 50) {
        delta = random8(200U);
      }
      drawPixelXY(x - 2 - deltaValue, floor(HEIGHT * 0.3), CHSV(step, saturation - delta, 96 + random8(128)));
      drawPixelXY(x + 2 - deltaValue, floor(HEIGHT * 0.3), CHSV(step, saturation, 96 + random8(128)));

      gradientVertical( x - deltaValue, 0U, x + 1U - deltaValue, floor(HEIGHT * 0.25),  hue + 2, hue, 0U, 250U, 255U);

      if (modes[currentMode].Scale > 50) {
        drawPixelXY(x + 3 - deltaValue, HEIGHT - 3U, CHSV(step, 255U, 255U));
        drawPixelXY(x - 3 - deltaValue, CENTER_Y_MINOR, CHSV(step, 255U, 255U));
        drawPixelXY(x + 3 - deltaValue, 2U, CHSV(step, 255U, 255U));
      }
    }
  }
  // ------
  deltaValue++;
  if (deltaValue >= 8) {
    deltaValue = 0;
  }
  step++;
}


// =====================================
//            Строб Хаос Дифузия
//          Strobe Haos Diffusion
//             © SlingMaster
// =====================================
/*должен быть перед эффектом Матрицf бегунок Скорость не регулирует задержку между кадрами,
  но меняет частоту строба*/
void StrobeAndDiffusion() {
  const uint8_t SIZE = 3U;
  const uint8_t DELTA = 1U;         // центровка по вертикали
  uint8_t STEP = 2U;
  if (loadingFlag) {
#if defined(USE_RANDOM_SETS_IN_APP) || defined(RANDOM_SETTINGS_IN_CYCLE_MODE)
    if (selectedSettings) {
      // scale | speed
      setModeSettings(1U + random8(100U), 1U + random8(150U));
    }
#endif
    loadingFlag = false;
    hue2 = 1;
    FastLED.clear();
  }

  STEP = floor((255 - modes[currentMode].Speed) / 64) + 1U; // for strob
  if (modes[currentMode].Scale > 50) {
    // diffusion ---
    blurScreen(beatsin8(3, 64, 80));
    STEP = 1U;
    if (modes[currentMode].Scale < 75) {
      // chaos ---
      VirtualSnow();
    }

  } else {
    // strob -------
    if (modes[currentMode].Scale > 25) {
      dimAll(200);
    } else {
      dimAll(240);
    }
  }

  const uint8_t rows = (HEIGHT + 1) / 3U;
  deltaHue = floor(modes[currentMode].Speed / 64) * 48;
  bool dir = false;
  for (uint8_t y = 0; y < rows; y++) {
    if (dir) {
      if ((step % STEP) == 0) {   // small layers
        drawPixelXY(WIDTH - 1, y * 3 + DELTA, CHSV(step, 255U, 255U ));
      } else {
        drawPixelXY(WIDTH - 1, y * 3 + DELTA, CHSV(170U, 255U, 1U));
      }
    } else {
      if ((step % STEP) == 0) {   // big layers
        drawPixelXY(0, y * 3 + DELTA, CHSV((step + deltaHue), 255U, 255U));
      } else {
        drawPixelXY(0, y * 3 + DELTA, CHSV(0U, 255U, 0U));
      }
    }

    // сдвигаем слои  ------------------
    for (uint8_t x = 0U ; x < WIDTH; x++) {
      if (dir) {  // <==
        drawPixelXY(x - 1, y * 3 + DELTA, getPixColorXY(x, y * 3 + DELTA));
      } else {    // ==>
        drawPixelXY(WIDTH - x, y * 3 + DELTA, getPixColorXY(WIDTH - x - 1, y * 3 + DELTA));
      }
    }
    dir = !dir;
  }

  if (hue2 == 1) {
    step ++;
    if (step >= 254) hue2 = 0;
  } else {
    step --;
    if (step < 1) hue2 = 1;
  }
}

// =====================================
//
// =====================================
