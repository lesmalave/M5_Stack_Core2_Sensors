#include <M5Core2.h>
#include <Wire.h>             // I2C comunication
#include <Adafruit_BMP280.h>  //I2C Pressure sensor
#include <Adafruit_ADS1X15.h> //I2C ADC 16 bits
#include <DHT.h>              // Humidity & temperature sensor
#include <FastLED.h>          // LED GRB SK6812

#define NUM_LEDS 10 // leds SK6812
#define DATA_PIN 25 // Pin output to SK6812
#define PIR 36      //Digital pin M5Stack2 Pin 1 Port B
#define DHTPIN 26   //Digital pin M5Stack2 Pin 2 Port B
#define DHTTYPE DHT11

//Class objects
Adafruit_BMP280 bme;
Adafruit_ADS1115 ads;
CRGB leds[NUM_LEDS];
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  pinMode(PIR, INPUT); //Set pin 36 to input mode.
  M5.begin();          //Init M5Core2.
  Wire.begin();        //Wire init, adding the I2C bus.
  FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, NUM_LEDS);
  dht.begin(); // Init DHT11
  Serial.begin(115200);

  while (!bme.begin(0x76))
  { //Init this sensor,True if the init was successful, otherwise false.
    M5.Lcd.println("Could not find a valid BMP280 sensor, check wiring!");
  }
  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS1115.");
    while (1)
      ;
  }

  M5.Lcd.clear(); //Clear the screen.
}

float pressure, Temp; //Store the value of pressure and Temperature.

void loop()
{
  //Condition for the PIR sensor, it also enables the internal LEDs of the M5
  if (digitalRead(PIR) == 1)
  { //If pin 36 reads a value of 1.
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(75, 170);
    M5.Lcd.printf("%-10s", " Sensing");
    leds[0] = CRGB::BlueViolet;
    leds[4] = CRGB::GreenYellow;
    leds[9] = CRGB::BlueViolet;
    leds[5] = CRGB::GreenYellow;
    FastLED.show();
  }
  else
  {
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(75, 170);
    M5.Lcd.printf("%-10s", "Not Sensed");
    leds[0] = CRGB::Black;
    leds[4] = CRGB::Black;
    leds[9] = CRGB::Black;
    leds[5] = CRGB::Black;
    FastLED.show();
  }
  pressure = bme.readPressure();
  Temp = bme.readTemperature();

  int16_t adc0, adc1; //adc2, adc3
  float volts0, Lux;
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);

  volts0 = ads.computeVolts(adc0);
  Lux = 100000 * ads.computeVolts(adc1) / 5000; // Percent Lux sensor

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  delay(500);
  // Show measures on display
  M5.Lcd.setCursor(0, 0); //Set the cursor to (0,0)
  M5.Lcd.setTextSize(2);  //Set the font size to 2
  M5.Lcd.printf("Pressure:%2.0fPa\nTemperature:%4.2fC", pressure, Temp);

  M5.Lcd.setCursor(0, 50);
  M5.Lcd.setTextSize(2); //Set the font size to 2
  M5.Lcd.printf("MQ135: %5d %5.2fV \nLDR:   %5d %6.2f%% Lux", adc0, volts0, adc1, Lux);

  M5.Lcd.setCursor(0, 100);
  M5.Lcd.setTextSize(2); //Set the font size to 2
  M5.Lcd.printf("Humidity: %.0f%% \nTemperature:%.2fC  %3.2fF\nHeat Index: %.2fC %6.2fF", h, t, f, hic, hif);

  M5.update(); //Update M5Stack Core2

  delay(100);
  //
  //
}
