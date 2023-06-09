//Motor pins
const int LEFT_DIRECTION = 13;
const int LEFT_PWM = 11;
const int LEFT_BRAKE = 8;

const int RIGHT_DIRECTION = 12;
const int RIGHT_PWM = 3;
const int RIGHT_BRAKE = 9;

#define SPEED 35

//ping sensor setup
const int PING_PIN_TRIGGER = 0;
const int PING_PIN_ECHO = 1;
const int PING_MAX_DIST = 20; //maximale afstand in centimeters

NewPing sonar(PING_PIN_TRIGGER, PING_PIN_ECHO, PING_MAX_DIST);


//Lijn sensor pins.
const int LINE_SENSOR_PINS[5] = {
	A0,
	A1,
	A2,
	A3,
	A4,

};

//extra const ints om motor richting te verduidelijken
const int MOTOR_COUNTER_CLOCKWISE = HIGH;
const int MOTOR_CLOCKWISE = LOW;

/*
BEGIN WAARDES -> ACTIE  
De waardes zijn hier in base2 (binary) waarbij 0 gelijk is aan zwart en 1 gelijk is aan wit.
Ook zijn de waardes gegroepeerd bij actie. Zie het technisch ontwerp hoofdstuk 3 voor meer toelichting.
De reden om uint8_t's te gebruiken ipv bijvoorbeeld strings is omdat number comparisons over het algemeen sneller zijn.
*/
const uint8_t FINISH_VALUE = 0b00000;

const uint8_t VALUES_TURN_LEFT[12] = {
	0b00001,
	0b00010,
	0b00011,
	0b00100,
	0b00101,
	0b00110,
	0b00111,
	0b10001,
	0b10010,
	0b10011,
	0b10110,
	0b10111
};



const uint8_t VALUES_TURN_RIGHT[10] = {
	0b01000,
	0b01001,
	0b01100,
	0b01101,
	0b10000,
	0b10100,
	0b11000,
	0b11001,
	0b11100,
	0b11101
};

const uint8_t VALUES_UTURN[5] = {

	0b11110,
	0b11111,
	0b01110,
	0b01111,
	0b10101
};

const uint8_t VALUES_FORWARD[4] = {

	0b11010,
	0b11011,
	0b01010,
	0b01011
};

/*
	EIND WAARDES -> ACTIE
*/

//Time tracker variabele.
unsigned long time_used = 0;

//Utility function om te kunnen kijken of een sensor waarde bij een gegeven actie hoort.
//
bool in_array(uint8_t arr[], uint8_t value, uint16_t size){
	for(uint16_t i = 0; i < size; i++){
		if(arr[i] == value){
			return true;

		}

	}

	return false;

}

void setup_motors(){
	pinmode(LEFT_DIRECTION, OUTPUT);
	pinmode(LEFT_PWM, OUTPUT);
	pinmode(LEFT_BRAKE, OUTPUT);

	pinmode(RIGHT_DIRECTION, OUTPUT);
	pinmode(RIGHT_PWM, OUTPUT);
	pinmode(RIGHT_BRAKE, OUTPUT);
}

void setup_line_sensor(){
	for(int i = 0; i < 5; i++){
		pinMode(LINE_SENSOR_PINS[i], INPUT);
	}


}

/*
* Function om de lijn sensors uit te lezen.
* Hierbij worden de waardes van iedere sensor samengevoegd in een uint8_t
*/

//Helper function omdat arduino geen pulse out heeft.
void pulseOut(int pin, unsigned long duration){
	digitalWrite(pin, LOW); //We zetten de pin op low voor de zekerheid.
	delayMicroseconds(2); 
	digitalWrite(pin, HIGH);
	delayMicroseconds(duration);
	digitalWrite(pin, LOW);
}

//Code is op basis van de basic 2 stamp voorbeeld in de datasheet voor ping sensor ( PING))) tm Ultrasonic Distance Sensor #28015)
uint32_t getPing(){
	pulseOut(PING_PIN, 10);
	uint32_t duration = pulseIn(PING_PIN, HIGH);
	return Math.pow(2260, duration); //de waarde 2260 is de cm constant uit de basic stamp 2 voorbeeld (zie brightspace voor datasheet).



}

uint8_t read_line_sensor(){
	uint8_t result = 0;
	for(int i = 0; i < 5; i++){
		result |= digitalRead(LINE_SENSOR_PINS[i]) << i;

	}

	return result;

}

void setup(){
	setup_motors();
	setup_line_sensors();
	//PWM-Frequentie (Zie opdracht beschrijving hoofdstuk 4.1.2)
	TCCR2B = TCCR2B & B11111000 | B00000111; // Frequentie is nu 30.64hz

}


void loop(){
	uint8_t line_data = read_line_sensors();

	if(line_data == FINISH_VALUE){
		finish();

	}else if(in_array(VALUES_FORWARD, line_data, 4)){
		forward();
	}else if(in_array(VALUES_TURN_RIGHT, line_data, 10)){
		right();

	}else if(in_array(VALUES_TURN_LEFT, line_data, 11)){
		left();

	}else if(in_array(VALUES_UTURN, line_data, 5)){
		uturn();

	}else{ //else voor wanneer een niet gedlfineerde line waarde wordt opgepakt.
		uturn();
	}

	uint32_t ping_distance = sonar.ping_cm();

	if(ping_distance <= 10){
		uturn();

	}


}

void forward(){
	digitalWrite(LEFT_DIRECTION, MOTOR_COUNTER_CLOCKWISE);
	analogWrite(LEFT_DIRECTION, SPEED);
	digitalWrite(RIGHT_DIRECTION, MOTOR_CLOCKWISE);
	analogWrite(RIGHT_SPEED, SPEED);

}

void left(){
	digitalWrite(RIGHT_DIRECTION, MOTOR_CLOCKWISE);
	analogWrite(LEFT_SPEED, 0);
	analogWrite(RIGHT_SPEED, SPEED);

}

void right(){
	digitalWrite(LEFT_DIRECTION, MOTOR_CLOCKWISE);
	analogWrite(LEFT_SPEED, SPEED);
	analogWrite(RIGHT_SPEED, 0);

}

void uturn(){
	digitalWrite(LEFT_DIRECTION, MOTOR_CLOCKWISE);
	analogWrite(LEFT_DIRECTION, SPEED);
	digitalWrite(RIGHT_DIRECTION, MOTOR_CLOCKWISE);
	analogWrite(RIGHT_SPEED, SPEED);
}

void stop(){
	digitalWrite(LEFT_DIRECTION, MOTOR_COUNTER_CLOCKWISE);
	analogWrite(LEFT_DIRECTION, 0);
	digitalWrite(RIGHT_DIRECTION, MOTOR_CLOCKWISE);
	analogWrite(RIGHT_SPEED, 0);
}

void finish(){
	forward();
	uint8_t line_data = read_line_sensors();
	if(line_data != FINISH_VALUE){
		return;

	}

	//Einde doolhof

}
