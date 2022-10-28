/**
 * @file buzzergame.ino
 *
 * @brief Simple game with buzzer based on Arduino Uno
 * @date 28.10.2022
 *
 * @author Aleksandr Gordienko
 * Contact: reclaite@gmail.com
 *
 */

#define BUTTON_1 3 
#define LED_1 4
#define BUTTON_2 5
#define LED_2 6
#define PIEZO 8

int BUTTONS[] = {BUTTON_1, BUTTON_2};
int LEDS[] = {LED_1, LED_2};

const int KEY_SIZE = 2;
const int TIME_TO_PLAY = 600;

int score = 0;
int activeKey = -1;

void setup() {

for (int i = 0; i < KEY_SIZE; i++) { pinMode(BUTTONS[i], INPUT_PULLUP);
	pinMode(LEDS[i], OUTPUT);
}

pinMode(PIEZO, OUTPUT);
Serial.begin(9600);
}

void loop() {
	if (random(0, 10) == 1) {
		int selectedLED = random(0, KEY_SIZE); 
		digitalWrite(LEDS [selectedLED], HIGH);
		Serial.println("Started game");
		activeKey = selectedLED;
	} else {
		delay(100);
		return;
	}
	
	if (!checkPress()) {
		Serial.println("Finished game");
		for (int i = 0; i < KEY_SIZE; i++) {
			digitalWrite(LEDS[i], LOW);
			activeKey = -1;
		}
		delay(TIME_TO_PLAY);
	}
}

boolean checkPress() {
	int activeButton = BUTTONS [activeKey];
	for (int i = 0; i < TIME_TO_PLAY / 10; i++) { 
		if (!digitalRead(activeButton)) { 
		Serial.println("Registered button's press"); 
		Serial.print("Now your score is "); 
		Serial.println(++score);
		digitalWrite(LEDS[activeKey], LOW); 
		activeKey = -1;
		tone (PIEZO, 262, 250);
		return true;
		}
		delay(10);
	}
	return false;
}
