#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10 megohm between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50 kilohm - 50 megohm. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 * Best results are obtained if sensor foil and wire is covered with an insulator such as paper or plastic sheet
 */
struct color_t {
  byte r,g,b;
};

struct mode_t {
  color_t *colors[10];
  byte n_colors;
};

color_t black = {
  .r=0,
  .g=0,
  .b=0,
};

color_t light_white = {
  .r=127,
  .g=127,
  .b=127,
};

color_t strong_blue = {
  .r=0,
  .g=142,
  .b=255,
};


color_t strong_green = {
  .r=13,
  .g=255,
  .b=233,
};

color_t strong_white = {
  .r=255,
  .g=255,
  .b=255,
};

color_t light_orange = {
  .r=251,
  .g=202,
  .b=124,
};

color_t deep_orange = {
  .r=251,
  .g=104,
  .b=25,
};

color_t strong_red = {
  .r=255,
  .g=43,
  .b=0,
};

mode_t noite, arousal, relax;

byte targetBrightness = 0;
byte currentBrightness = 0;
int touchThreshold_high = 200;
int touchThreshold_low = 100;
int mode = 0;
int value = 255;
int red = 5;
int green = 3;
int blue = 6;
int button_led = 13;
int timestep = 50;
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,A0);        // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
int color_index = 0;
long last_time;
long mode_time_hop = 30000;
mode_t* modes[] = {&noite, &arousal, &relax};

color_t *last_color = &black;
color_t *current_color = &black;
int itr = 0;
long button_int = 0;
long lastbutton_int = 0;
long lastlastbutton_int = 0;

void setup()                    
{
   noite.n_colors = 1;
   noite.colors[0] = &black;
   
   arousal.n_colors = 4;
   arousal.colors[0] = &light_white;
   arousal.colors[1] = &strong_blue;
   arousal.colors[2] = &strong_green;
   arousal.colors[3] = &strong_white;
   
   relax.n_colors = 4;
   relax.colors[0] = &light_orange;
   relax.colors[1] = &deep_orange;
   relax.colors[2] = &strong_red;
   relax.colors[3] = &black;
   
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   Serial.begin(9600);
   pinMode(red, OUTPUT);     
   pinMode(green, OUTPUT);
   pinMode(blue, OUTPUT);
   pinMode(button_led, OUTPUT);  

  last_time = millis();
}

void loop(){
  lastlastbutton_int = lastbutton_int;
  lastbutton_int = button_int;
  button_int =  cs_4_2.capacitiveSensor(30); 
  Serial.println(button_int);
  digitalWrite(button_led, button_int > touchThreshold_high ? HIGH : LOW); // mostra quando o sensor esta sendo ativado
  
  if (button_int > touchThreshold_high && lastbutton_int < touchThreshold_low && lastlastbutton_int < touchThreshold_low) {
    mode = (mode + 1) % 3;  
       /* if (mode == 0) targetBrightness = 0;
        if (mode == 1) targetBrightness = 255;
        if (mode == 2) targetBrightness = 128;
        if (mode == 3) targetBrightness = 64;
       */
        Serial.print("The current mode is..."); //Serial monitor to bebug mode increases
        Serial.println(mode);  //print value of mode to seial monitor
    }
    
    long current_time = millis();
    if (current_time - last_time > mode_time_hop){
      last_time = current_time;
      color_index = (color_index + 1) % modes[mode]->n_colors;
      
      last_color = current_color;
      current_color = modes[mode]->colors[color_index];
      itr = 0;
      
    }
    
    color_t color;
    color.r = last_color->r*(255-itr)/255 + current_color->r*itr/255;
    color.g = last_color->g*(255-itr)/255 + current_color->g*itr/255;
    color.b = last_color->b*(255-itr)/255 + current_color->b*itr/255;
    if (itr <= 255){
      itr++;
    }
    
    Serial.print("current color:\t");
    Serial.print(current_color->r);
    Serial.print("\t");
    Serial.print(current_color->g);
    Serial.print("\t");
    Serial.print(current_color->b);
    Serial.print("\t");

    analogWrite(red, color.r); 
    analogWrite(green, color.g);
    analogWrite(blue, color.b);
    
    
    Serial.print("color:\t");
    Serial.print(color.r);
    Serial.print("\t");
    Serial.print(color.g);
    Serial.print("\t");
    Serial.print(color.b);
    Serial.print("\n");
    

    
    
    delay(10);                             // arbitrary delay to limit data to serial port 
}
