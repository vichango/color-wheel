boolean debug = true;

int ledPin = 12;
int tachoPin = 4;
int buttonPin = 2;

int potPin = A0;

boolean ledState = false;
long previousMicros = 0;

long previousUpdateMicros = 0;

int flashLength = 100;

const int nbOfValues = 32;
long fanPulseValues[nbOfValues];
double roundsPerSecond = 25;

long flashInterval = 1000;
double flashPerSecond = 25;

double fixPercentage = 0.0;
long fixedFlashInterval = 1000;

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

	if (currentMicros - previousMicros > fixedFlashInterval) {
		previousMicros = currentMicros;

		ledState = flashOrNot();
		digitalWrite(ledPin, ledState);
	}

	if (currentMicros - previousUpdateMicros > 1000000) {
		previousUpdateMicros = currentMicros;
		updateFixedFlashInterval();
	}
}

void updateRoundsPerSecond() {
	roundsPerSecond = 1000000 / (4 * readPulseLength());
}

long readPulseLength() { 
	if (false) {
		// Shift 4 values on the array
		for (int i = 0; i < nbOfValues; i++) {
			fanPulseValues[i + 4] = fanPulseValues[i];
		}

		for (int i = 0; i < 2; i++) {
			fanPulseValues[2 * i] = pulseIn(tachoPin, HIGH);
			fanPulseValues[2 * i + 1] = pulseIn(tachoPin, LOW);
		}

		long average = 0;
		for (int i = 0; i < nbOfValues; i++) {
			average += fanPulseValues[i];
		}

		return average / nbOfValues;
	} else {
		long average = 0;
		for (int i = 0; i < 4; i++) {
			average = average + pulseIn(tachoPin, HIGH);
			average = average + pulseIn(tachoPin, LOW);
		}

		return average / 8;
	}
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

	flashInterval = 1000000 / flashPerSecond;
}

void updateFixedFlashInterval() {
	fixedFlashInterval = (1.0 + ((analogRead(potPin) - 512.0) / 5120)) * flashInterval;
}

boolean flashOrNot() {
	boolean flash = shouldFlash[flashAt];
	flashAt = (flashAt + 1) % 6;

	return flash;
}
