/**
 * @file indicatorgame.ino
 *
 * @brief Simple game with 7-segment indicator based on Arduino Uno
 * @date 28.10.2022
 *
 * @author Aleksandr Gordienko
 * Contact: reclaite@gmail.com
 *
 */

#define A 2
#define B 3
#define C 4
#define D 5
#define E 6
#define F 7
#define G 8

#define D1 A0
#define D2 A1
#define D3 A2
#define D4 A3

const byte SEGMENTS[] = {A, B, C, D, E, F, G}; 
const byte DIGITS[] = {D1, D2, D3, D4};

#define BTN1 12
#define BTN2 9 
#define BTN3 11
#define BTN4 10

const byte BUTTONS[] = {BTN1, BTN2, BTN3, BTN4};

// Пин пьезодинамика
#define BUZZ 13

// Время для игры (с каждым нажатием постепенно уменьшается
// на difference миллисекунд) 
int difference = 100;
int timeToPlay = 3000;

// Поля для хранения количества очков, сколько "промахов"
// можно и уже сделано
int score = 0; 
byte missesToLose = 3;
byte missedTimes = 0;

// Отвечает за состояние проигрыша игры
boolean lost = false;

void setup() {
	// Регистрация пинов сегментов индикатора на OUTPUT
	for (int i = 0; i < sizeof(SEGMENTS); i++)
		pinMode(SEGMENTS[1], OUTPUT);
	
	// Регистрация пинов цифр индикатора на OUTPUT 
	for (int i = 0; i < sizeof(DIGITS); i++) 
		pinMode(DIGITS[i], OUTPUT);
	
	// Регистрация пинов кнопок на INPUT (принятие сигнала) 
	for (int i=0; i < sizeof (BUTTONS); i++) 
		pinMode(BUTTONS[1], INPUT_PULLUP);
	
	// Регистрация пьезодинамика
	pinMode(BUZZ, OUTPUT); 
	
	Serial.begin(9600);
}

void loop() {
	// Если игрок уже проиграл, то сразу же заканчиваем игру
	if (lost) 
		return;
	
	// Даем шанс 1/10 на включение игры, иначе ждем
	// небольшое время и повторяем попытку 
	if (random(10) == 1) { 
		// Получаем из метода сhесkpress делитель очков.
		// Чем быстрее была нажата кнопка, тем больше по итогу
		// будет очков (максимум 10) 
		// Если же был вернут делитель 0, это означает отсутствие клика, либо
		// нажатие неправильной и мы засчитываем промах
		byte chosenDigit = random(sizeof(DIGITS)); 
		lightUp(chosenDigit);
		byte divider = checkPress(chosenDigit);
	
		if (divider != 0) {
		score += (int) 10 / divider; 
		timeToPlay -= difference;
		Serial.print("You pressed the right button! Now your score is ");
		Serial.println(score); 
		tone(BUZZ, 2000, 100);
		} else {
			if (missesToLose == ++missedTimes)
				lose();
			tone(BUZZ, 300, 100); 
			} 
		// Тушим индикатор, чтобы между играми он не горел
		lightUp(-1);
		delay(500);
	} else {
	    delay(300);
	}
}

byte checkPress(byte digit) {
	byte activeButton = BUTTONS[digit];
	int divider = 1;
	// Разбиваем отрезок времени игры на кусочки по 10 миллисекунд 
	// Так как не работаем асинхронно, то нам приходится так делать
	for (int i = 0; i < timeToPlay / 10; i++) { 
		for (int k = 0; k < sizeof(BUTTONS); k++) {
			byte button = BUTTONS[k];
			// Проходимся по всем кнопкам и проверяем их на полученный сигнал нак 
			if (!digitalRead(button)) {
				// Проверяем, ту ли кнопку нажал игрок 
				// Если да, то засчитываем очки, если нет, засчитываем промах
				if (button == activeButton)
					return divider;
				else {
					Serial.println("You pressed the wrong button! +1 miss");
					return 0;
				}
			}
		}
		if (i % 50 == 0)
			divider++;
		delay(10);
	}
	Serial.println("You didn't press the button. +1 miss");
	return 0;
}

// Данный метод используется для полного зажигания одной из цифр индикатора
// В параметр digit передается номер цифры (порядок начинается с 0)
void lightUp(byte digit) {
  for (int i = 0; i < sizeof(SEGMENTS); i++)
    digitalWrite(SEGMENTS[i], HIGH);
  for (int i = 0; i < sizeof(DIGITS); i++)
    digitalWrite(DIGITS[i], i == digit ? LOW : HIGH);
}

// Дополнительная логика при проигрыше игрока в игре
// Выводится сообщение о том, что игрок проиграл, выводится
// число очков и грустный звук
// Состояние проигрыша меняется на true, то есть теперь
// нужно только перезагружать плату для повторной игры
// Либо дописывать отдельную кнопку, переключающую состояние lost
void lose() {
	Serial.println("Unfortunately, you lost the game...");
	Serial.print("Your final score is ");
	Serial.println(score);
	for (int i = 1200; i > 0; i -= 100) {
		tone(BUZZ, 500 + i, 100);
		delay(100);
	}
	lightUp(-1);
	lost = true;
}
