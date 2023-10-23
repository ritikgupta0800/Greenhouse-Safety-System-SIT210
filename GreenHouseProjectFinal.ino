// ********** Including Libraries **********
#include<LiquidCrystal.h>
#include<ArduinoHttpClient.h>
#include <Arduino_JSON.h>
#include<DHT.h>
#include <WiFiNINA.h>

// ********** Defining Hardwares **********

// Output Hardwares
#define FAN 4
#define WATER_PUMP 3
#define LIGHT 5
#define BUZZER 15

// Input Hardwares
#define LDR A1
#define MOISTURE_SENSOR A0
#define DHT_SENSOR 2


// ********** WIFI Credentials **********
#define ssid "Eespl"
#define pass "BrainDrain@123"
// #define SSID "realme 5 pro"
// #define PASS "11111111"


// ********** Server Configurations **********
char server[] = "www.techpacs.com"; //Server name
int port = 443; // HTTPS port
#define ID 2    // ID of Server Database


// ********** Object of LCD **********
LiquidCrystal lcd(8, 9, 10, 11,12, 13); // rs, en, d4, d5, d6, d7
// ********** Object of DHT11 **********
DHT dht(DHT_SENSOR, DHT11);


// ********** Object of WiFiSSLClient (For HTTPS request) **********
WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, server, 443 );


void setup() {
  lcd.begin(16, 2);
  dht.begin();
  Serial.begin(9600);
  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("  GREEN HOUSE  ");
  lcd.setCursor(0, 1);
  lcd.print("AUTOMATIC-SYSTEM");
  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LDR,INPUT);

  pinMode(MOISTURE_SENSOR, INPUT);

  pinMode(LIGHT, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(WATER_PUMP, OUTPUT);
  pinMode(FAN, OUTPUT);

  digitalWrite(LIGHT, HIGH);
  digitalWrite(BUZZER, LOW);

  digitalWrite(WATER_PUMP, HIGH);
  digitalWrite(FAN, HIGH);
  connectWIFI();
  delay(500);
}

char mode_status = 'M';
int thresh_temp = 30, thresh_moisture = 22, thresh_ldr=14;
int fanFlag,waterpumpFlag,lightFlag;

float moisture_val = 1, humid_val = 1, temp_val = 1,ldr_val = 1;
bool light_status = false;
bool pump_status = false;
bool fan_status = false;

bool light_state_changed = false;
bool pump_state_changed = false;
bool fan_state_changed = false;
bool mode_state_changed  = false;
bool thresh_temp_changed = false;
bool thresh_moisture_changed = false;
byte degree[8] = {
  B00011,
  B00011,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
int lcd_screen = 1;

// Wifi Part
IPAddress ip;

void loop() {
  if (lcd_screen == 3)
    lcd_screen = 1;
  else
    

  get_sensors_value();
  get_from_server1();
  state_change_operation();
  print_data();
  // automatic();
  if (mode_status == 'A') {
    automatic();
  }
  send_to_server(temp_val, moisture_val, humid_val);
  delay(1200);
}

void print_data() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Mode : ");
  if (mode_status == 'A') {
    lcd.print("Automatic");
  } else {
    lcd.print("Manual");
  }
  if (lcd_screen == 1) {
    screen_1();
  } else if (lcd_screen == 2) {
    screen_2();
  } else if (lcd_screen == 3) {
    screen_3();
  }
}
void screen_1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TP:");
  lcd.print(temp_val);
  lcd.createChar(0, degree);
  lcd.setCursor(8, 0);
  lcd.write(byte(0));
  lcd.setCursor(9, 0);
  lcd.print("C");

  lcd.setCursor(11, 0);
  lcd.print("H:");
  lcd.print((int)humid_val);
  lcd.setCursor(15, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("MS:");
  lcd.print((int)moisture_val);
  // lcd.setCursor(8, 1);
  lcd.print("%");

  lcd.setCursor(11, 1);
  lcd.print("L:");
  lcd.print((int)ldr_val);
  // lcd.setCursor(15, 1);
  lcd.print("%");
  delay(1000);

}
void screen_2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Threshold Values");
  lcd.setCursor(0, 1);
  lcd.print("Temp.    : ");
  lcd.print(thresh_temp);
  lcd.createChar(0, degree);
  lcd.setCursor(12, 1);
  lcd.write(byte(0));
  lcd.setCursor(14, 1);
  lcd.print("C");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("Moisture :   ");
  lcd.print(thresh_moisture);
  lcd.setCursor(15, 1);
  lcd.print("%");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("Light    :   ");
  lcd.print(thresh_ldr);
  lcd.setCursor(15, 1);
  lcd.print("%");
  delay(1000);

}
void screen_3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Fan        : ");
  if (fan_status) {
    lcd.print(" ON");
    fanFlag=1;
  } else {
    lcd.print("OFF");
    fanFlag=0;
  }
  lcd.setCursor(0, 1);
  lcd.print(" Light      : ");
  if (light_status) {
    lcd.print(" ON");
    lightFlag = 1;
  } else {
    lcd.print("OFF");
    lightFlag=0;
  }
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print(" Water Pump : ");
  if (pump_status) {
    lcd.print(" ON");
    waterpumpFlag = 1;
  } else {
    lcd.print("OFF");
    waterpumpFlag = 0;
  }
  delay(1000);
}
void automatic() {
  if ((int)temp_val >= thresh_temp) {
    if (!fan_status) {
      beep(1);
      digitalWrite(FAN, LOW);
      fan_status = true;
      print_state_thresh("temp", "Fan     ->   ON");
      fanFlag=1;
    }
  } else {
    if (fan_status) {
      beep(1);
      digitalWrite(FAN, HIGH);
      fan_status = false;
      print_state_thresh("temp", "Fan     ->  OFF");
      fanFlag=0;
    }
  }
  // if ((int)ldr_val <= thresh_ldr) {
  //   if (!light_status) {
  //     beep(1);
  //     digitalWrite(LIGHT, LOW);
  //     light_status = true;
  //     print_state_thresh("light", "Light    ->  ON");
  //     lightFlag=1;
  //   }
  // } else {
  //   if (light_status) {
  //     beep(1);
  //     digitalWrite(LIGHT, HIGH);
  //     light_status = false;
  //     print_state_thresh("light", "Light   ->  OFF");
  //     lightFlag=0;
  //   }
  // }

  if ((int)moisture_val < thresh_moisture) {

    if (!pump_status) {
      beep(1);
      digitalWrite(WATER_PUMP, LOW);
      pump_status = true;
      print_state_thresh("moist", "Water Pump-> ON");
      waterpumpFlag = 1; 
    }
  } else {

    if (pump_status) {
      beep(1);
      digitalWrite(WATER_PUMP, HIGH);
      pump_status = false;
      print_state_thresh("moist", "Water Pump->OFF");
      waterpumpFlag = 0;
    }
  }
}

void state_change_operation() {

  if (light_state_changed) {
    if (light_status) {
      beep(1);
      digitalWrite(LIGHT, LOW);
      print_state_changed(" Light turned  ", "Status  -> ON  ", false);
      lightFlag =1;
      
    } else {
      beep(1);
      digitalWrite(LIGHT, HIGH);
      print_state_changed(" Light turned  ", "Status  -> OFF ", false);
      lightFlag =0;
    }
    light_state_changed = false;
  }
  if (pump_state_changed) {
    if (pump_status) {
      beep(1);
      digitalWrite(WATER_PUMP, LOW);
      print_state_changed("WaterPump turn", "Status  -> ON  ", false);
      waterpumpFlag = 1;
    } else {
      beep(1);
      digitalWrite(WATER_PUMP, HIGH);
      print_state_changed("WaterPump turn", "Status -> OFF", false);
      waterpumpFlag = 0;
    }
    pump_state_changed = false;
  }
  if (fan_state_changed) {
    if (fan_status) {
      beep(1);
      digitalWrite(FAN, LOW);
      print_state_changed("  Fan turned   ", "Status -> ON", false);
      fanFlag = 1 ;
    } else {
      beep(1);
      digitalWrite(FAN, HIGH);
      print_state_changed("  Fan turned   ", "Status -> OFF", false);
      fanFlag = 0 ;

    }
    fan_state_changed = false;
  }
  if (thresh_temp_changed) {
    beep(1);
    print_state_changed("Threshold Temp.", "Changed to " + String(thresh_temp), true);
    thresh_temp_changed = false;
  }

  if (thresh_moisture_changed) {
    beep(1);
    print_state_changed("Thres. Moisture", "Changed to " + String(thresh_moisture) + " %", false);
    thresh_moisture_changed = false;
  }
  if (mode_state_changed) {
    beep(1);
    if (mode_status == 'A')
      print_state_changed("Mode changed to ", "Automatic", false);
    else
      print_state_changed("Mode changed to ", "Manual", false);
    mode_state_changed = false;
  }
}

void get_from_server1() {
  HttpClient client = HttpClient(wifi, server, 443 );
  if (WiFi.status() == WL_CONNECTED) {
    if (wifi.connect(server, port)) {
      String url = "/bgiet/iot/get_values.php?id=" + String(ID);
      // http.begin(wifiClient, url);
      client.get(url);
      int code = client.responseStatusCode();
    
    if ( code == 200) {
      String payload = client.responseBody();
      // String payload = http.getString();
      JSONVar json_object = JSON.parse(payload);
      if (JSON.typeof(json_object) == "undefined") {
        // Serial.println("Parsing input failed!");
      }

      char marray[10];

      strcpy(marray, json_object["field2"]);
      int th_temp = stoi(marray);
      if (th_temp != thresh_temp) {
        // Serial.print("Threshold Temperature Changed => ");
        // Serial.println(thresh_temp);
        thresh_temp = th_temp;
        thresh_temp_changed = true;
      }

      strcpy(marray, json_object["field4"]);
      int th_moist = stoi(marray);
      if (th_moist != thresh_moisture) {
        // Serial.print("Threshold Moisture Changed => ");
        // Serial.println(th_moist);
        thresh_moisture_changed = true;
        thresh_moisture = th_moist;
      }


      strcpy(marray, json_object["field6"]);
      if (light_status != stoi(marray)) {
        light_status = stoi(marray);
        // Serial.print("Light Status Changed => ");
        // Serial.println(light_status);
        light_state_changed = true;
      }
      if (mode_status == 'M') {
        strcpy(marray, json_object["field7"]);
        if (pump_status != stoi(marray)) {
          pump_status = stoi(marray);
          // Serial.print("Water Status Changed => ");
          // Serial.println(pump_status);
          pump_state_changed = true;
        }

        strcpy(marray, json_object["field8"]);

        if (fan_status != stoi(marray)) {
          fan_status = stoi(marray);
          // Serial.print("Fan Status Changed => ");
          // Serial.println(fan_status);
          fan_state_changed = true;
        }
      }

      strcpy(marray, json_object["field9"]);
      if (mode_status != marray[0]) {
        mode_status = marray[0];
        // Serial.print("Mode Status Changed => ");
        // Serial.println(mode_status);
        mode_state_changed = true;
      }

      client.stop();
      // http.end();
    } else {
      // Serial.println(code);
    }
    client.stop();
    // http.end();
  } else {
    connectWIFI();
  }
}
}

int stoi(char *string) {
  int num = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    num = num * 10 + (string[i] - 48);
  }
  return num;
}
void send_to_server(int field1, int field3, int field5) {
  HttpClient client = HttpClient(wifi, server, 443 );
  if (WiFi.status() == WL_CONNECTED) {
    String url = "/bgiet/iot/set_values.php?id=" + String(ID) +
                "&field1=" + String(field1) +
                "&field3=" + String(field3) +
                "&field5=" + String(field5);
    client.get(url);
    int code = client.responseStatusCode();
    if ( code == 200) {
      // Serial.println("Server Hit Succesfully");
      client.stop();
    } else {
      // Serial.println("Failed to connect to server");
    }
  } else {
    // Serial.println("WiFi not connected, retrying...");
    connectWIFI();
  }

}

void print_state_changed(String msg_2, String msg_3, bool isTemp) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data received from");
  lcd.setCursor(0, 1);
  lcd.print("IOT Web Server");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg_2);

  lcd.setCursor(0, 1);
  lcd.print(msg_3);

  if (isTemp) {
    lcd.createChar(0, degree);
    lcd.setCursor(14, 1);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.print("C");
    lcd.clear();
  }
  delay(1000);

}

void print_state_thresh(String type, String msg_3) {
  lcd.clear();
  if (type.equalsIgnoreCase("temp")) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tem. Val: ");
    lcd.print(temp_val);
    // lcd.print(" %");
    // lcd.setCursor(0, 1);
    // lcd.print("Threshold: ");
    // lcd.print(thresh_temp);
    lcd.createChar(0, degree);
    lcd.setCursor(14, 0);
    lcd.write(byte(0));
    lcd.setCursor(15, 0);
    lcd.print("C");
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(msg_3);
    delay(2000);
  } else if(type.equalsIgnoreCase("moist")){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Mois. Val:");
    // lcd.setCursor(0, 1);
    // lcd.print("Value : ");
    lcd.print(moisture_val);
    lcd.print(" %");
    lcd.setCursor(0, 1);
    lcd.print(msg_3);
    delay(2000);
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light Val:");
    lcd.print(ldr_val);
    lcd.print(" %");
    lcd.setCursor(0, 1);
    lcd.print(msg_3);
    delay(2000);
  }
  delay(1000);
}

void beep(int times) {
  while (times--) {
    digitalWrite(BUZZER, HIGH);
    delay(100);
    digitalWrite(BUZZER, LOW);
    delay(100);
  }
}

void connectWIFI() {
  // Attempt to connect to Wi-Fi
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    // Serial.println("Connecting to WiFi...");
  }

  // Serial.println("Connected to WiFi");
}



void get_sensors_value() {
  //  moisture_val = ((4095 - analogRead(MOISTURE_SENSOR)) / 4095.0) * 100;
  moisture_val = analogRead(MOISTURE_SENSOR);
  // if (moisture_val < 3200)
    // moisture_val = 3200;
  moisture_val = map(moisture_val, 0, 1023, 0, 100);
  moisture_val = 100 - moisture_val;

  // Serial.print("Moisture is : ");
  // Serial.println(moisture_val);

  humid_val = dht.readHumidity();
  temp_val = dht.readTemperature();


  //LDR Value Percentage
  ldr_val = get_ldr_val();
  ldr_val = map(ldr_val, 0, 1023, 0, 100);

  Serial.print(moisture_val);
  Serial.print(", ");
  Serial.print(humid_val);
  Serial.print(", ");
  Serial.print(temp_val);
  Serial.print(", ");
  Serial.print(ldr_val);
  Serial.print(", ");
  Serial.print(waterpumpFlag);
  Serial.print(", ");
  Serial.print(fanFlag);
  Serial.print(", ");
  Serial.println(lightFlag);


  if (isnan(humid_val) || isnan(temp_val)) {
    humid_val = 0;
    temp_val = 0;
  }
}

int get_ldr_val(){
  int value = analogRead(A1);
  return value;
}

