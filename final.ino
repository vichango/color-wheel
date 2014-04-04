boolean debug = false;

int ledPin = 12;
int tachoPin = 4;

bool ledState = LOW;
long previousMicros = 0;

int flashLength = 100;
long flashInterval = 1000;
double flashPerSecond = 25;

long previousMilis = 0;
double roundsPerSecond = 25;

int count = 0;

void setup() {                
	pinMode(ledPin, OUTPUT);
	pinMode(tachoPin, INPUT);

	updateRpsFps();

	if (debug) {
		Serial.begin(9600);
		Serial.print("flashPerSecond=");
		Serial.print(1000000.0 / flashInterval);
		Serial.print(" (");
		Serial.print(flashInterval);
		Serial.println(")");
	}
}

void loop() {
	unsigned long currentMicros = micros();
	unsigned long currentMillis = millis();

	if (ledState && (currentMicros - previousMicros > flashLength)) {
		ledState = false;
		digitalWrite(ledPin, ledState);

		// Update the fan RoundsPerSecond every 10 seconds
		if (currentMillis - previousMilis > 10000) {
			updateRpsFps();
			previousMilis = currentMillis;
		}
	}

	if(currentMicros - previousMicros > flashInterval) {
		ledState = true;
		digitalWrite(ledPin, ledState);

		previousMicros = currentMicros;
	}
}

void updateRpsFps() {
	roundsPerSecond = 0;
	for (int i = 0; i < 2; i++) {
		roundsPerSecond = roundsPerSecond + 1000000.0 / pulseIn(tachoPin, LOW);
		roundsPerSecond = roundsPerSecond + 1000000.0 / pulseIn(tachoPin, HIGH);
	}

	roundsPerSecond = roundsPerSecond / 16;

	flashPerSecond = roundsPerSecond;
	flashInterval = 1000000 / flashPerSecond;

	if (debug) {
		Serial.print("roundsPerSecond=");
		Serial.print(roundsPerSecond);
		Serial.print(" flashPerSecond=");
		Serial.println(flashPerSecond);
	}
}
