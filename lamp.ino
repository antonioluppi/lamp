#include <CapacitiveSensor.h>

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

color_t light_green = {
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


enum cs_state_t {
	low, rising, high, falling
};

mode_t noite, arousal, rush, relax;

float touchThreshold = 0.1f;
float cs_average = 200;
float k_filter = 0.005;
long cs_debounce = 100;	//ms
long cs_time;
long cs_reset_time = 20000;
cs_state_t cs_state = low;

int mode = 0;
int red = 3;
int green = 5;
int blue = 6;
int cs_led = 13;
int timestep = 50;
CapacitiveSensor cs_4_2 = CapacitiveSensor(4,A0);        // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
int color_index = 0;
mode_t* modes[] = {&noite, &arousal, &rush, &relax};

color_t *last_color = &black;
color_t *current_color = &black;
int itr = 0;

void setup()                    
{
	noite.n_colors = 1;
	noite.colors[0] = &black;

	arousal.n_colors = 4;
	arousal.colors[0] = &light_white;
	arousal.colors[1] = &strong_blue;
	arousal.colors[2] = &light_green;
	arousal.colors[3] = &strong_white;

	rush.n_colors = 1;
	rush.colors[0] = &strong_white;

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
	pinMode(cs_led, OUTPUT);

}

void loop() {

	long current_time = millis();
	long cs_value =  cs_4_2.capacitiveSensor(200);
        cs_average = cs_value * k_filter + cs_average * (1-k_filter);
	boolean on_button_press= false;
	Serial.print(cs_value);
	Serial.print("\t");
	Serial.print(cs_average);
	Serial.print("\t");
        boolean p = (cs_value - cs_average) / cs_average > touchThreshold;
	switch(cs_state) {
		case low:
			if(p)
			{
				cs_time = current_time;
				cs_state = rising;
			}
			break;
		case rising:
			if(!p)
			{
				cs_state = low;
			}else if(current_time - cs_time > cs_debounce) {
				on_button_press = true;
				cs_state = high;
                                cs_time = current_time;
				digitalWrite(cs_led, HIGH);
			}
			break;
		case high:
			if(!p)
			{
				cs_time = current_time;
				cs_state = falling;
			}
			break;
		case falling:
			if(p)
			{
				cs_state = high;
			}else if(current_time - cs_time > cs_debounce) {
				cs_state = low;
                                cs_time = current_time;
				digitalWrite(cs_led, LOW);
			}
			break;
	};

	if (on_button_press){

		mode = (mode + 1) % 4;  
		color_index = 0;
		last_color = current_color;
		current_color = modes[mode]->colors[color_index];
		itr = 0;

		Serial.print("The current mode is...");
		Serial.println(mode);

	}
	color_t color;
	color.r = last_color->r*(255-itr)/255 + current_color->r*itr/255;
	color.g = last_color->g*(255-itr)/255 + current_color->g*itr/255;
	color.b = last_color->b*(255-itr)/255 + current_color->b*itr/255;

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

	if (itr < 255){
		itr++;
	}else if (itr == 255){
		color_index = (color_index + 1) % modes[mode]->n_colors;
		Serial.println("****************************************************");
		last_color = current_color;
		current_color = modes[mode]->colors[color_index];
		itr = 0;
	}

	delay(10);                             // arbitrary delay to limit data to serial port 
}
