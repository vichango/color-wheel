boolean debug = true;

int ledPin = 12;
int tachoPin = 4;
int buttonPin = 2;

int potPin = A0;

boolean ledState = false;
long previousMicros = 0;

int flashLength = 100;

const int nbOfValues = 32;
long fanPulseValues[nbOfValues];
double roundsPerSecond = 25;

long flashInterval = 1000;
double flashPerSecond = 25;

int flashAt = 0;
boolean shouldFlash[6];

int count = -1;

void setup() {
	pinMode(ledPin, OUTPUT);
	pinMode(tachoPin, INPUT);
	pinMode(buttonPin, INPUT);

	if (debug) {
		Serial.begin(9600);
	}

	// Init tacho readings
	for (int i = 0; i < nbOfValues; i++) {
		fanPulseValues[i] = 0;
	}

	for (int i = 0; i < nbOfValues / 4; i++) {
		updateRoundsPerSecond();
	}

	// Init should flash
	for (int i = 0; i < 6; i++) {
		shouldFlash[i] = false;
	}

	setSpectacle();
}

void loop() {
	if (digitalRead(buttonPin)) {
		updateRoundsPerSecond();
		setSpectacle();
		
		if (debug) {
			Serial.print("roundsPerSecond=");
			Serial.print(roundsPerSecond);
			Serial.print(" flashPerSecond=");
			Serial.println(flashPerSecond);
			Serial.println();
		}

		delay(1000);
	}

	unsigned long currentMicros = micros();

	if (ledState && (currentMicros - previousMicros > flashLength)) {
		ledState = false;
		digitalWrite(ledPin, ledState);
	}

	if(currentMicros - previousMicros > flashInterval) {
		previousMicros = currentMicros;

		ledState = flashOrNot();
		digitalWrite(ledPin, ledState);
	}
}

void updateRoundsPerSecond() {
	double perThousand = map(analogRead(potPin), 0, 1023, -100, 100);
	roundsPerSecond = (1.0 + (perThousand / 1000)) * 1000000 / (4 * readPulseLength());
}

long readPulseLength() {  
	// shift 4 values on the array
	for (int i = 0; i < nbOfValues; i++) {
		fanPulseValues[i + 4] = fanPulseValues[i];
	}

	for (int i = 0; i < 2; i++) {
		fanPulseValues[2 * i] = pulseIn(tachoPin, HIGH);
		fanPulseValues[2 * i + 1] = pulseIn(tachoPin, LOW);
	}

	// if (debug) {
	// 	Serial.print("pulseValues=");
	// 	for (int i = 0; i < nbOfValues-1; i++) {
	// 		Serial.print(fanPulseValues[i]);
	// 		Serial.print(",");
	// 	}
	// 	Serial.println(fanPulseValues[nbOfValues-1]);
	// }

	long average = 0;
	for (int i = 0; i < nbOfValues; i++) {
		average += fanPulseValues[i];
	}

	return average / nbOfValues;
}

void setSpectacle() {
	if (debug) {
		Serial.print("spectacle=");
		Serial.print(count);
		Serial.println();
	}

	switch (count) {
		case 0:
			flashPerSecond = 2 * roundsPerSecond;
			for (int i = 0; i < 6; i++) {
				shouldFlash[i] = true;
			}
			break;
		case 1:
			flashPerSecond = 3 * roundsPerSecond;
			for (int i = 0; i < 6; i++) {
				shouldFlash[i] = (i % 3 != 0);
			}
			break;
		case 2:
			flashPerSecond = 5 * roundsPerSecond;
			for (int i = 0; i < 6; i++) {
				shouldFlash[i] = true;
			}
			break;
		default:
			flashPerSecond = 1 * roundsPerSecond;
			for (int i = 0; i < 6; i++) {
				shouldFlash[i] = true;
			}
			count = -1;
	}

	count = count + 1;

	// if (debug) {
	// 	Serial.print("shouldFlash=");
	// 	for (int i = 0; i < 5; i++) {
	// 		Serial.print(shouldFlash[i]);
	// 		Serial.print(",");
	// 	}
	// 	Serial.println(shouldFlash[5]);
	// }

	flashInterval = 1000000 / flashPerSecond;
}

boolean flashOrNot() {
	boolean flash = shouldFlash[flashAt];
	flashAt = (flashAt + 1) % 6;

	return flash;
}
