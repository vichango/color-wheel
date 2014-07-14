boolean debug = false;

int ledPin = 12;
int tachoPin = 4;
int buttonPin = 2;

int potPin = A0;

boolean ledState = false;

long previousMicros = 0;
long previousUpdateMicros = 0;

int flashLength = 100;

// 32 consecutive measures are averaged to get a stable
// fan speed reading
const int nbOfValues = 32;
long fanPulseValues[nbOfValues];
double fanRevolutionsPerSecond = 25;

long flashInterval = 1000;
double flashPerSecond = 25;

// The potentionmeter can be used to slightly change the
// generated intervale between light flashes
double fixPercentage = 0.0;
long fixedFlashInterval = 1000;

// Spectacles are produced using a 18-cell table over which
// we iterate continuously to know if we should flash or not
const int nbOfFlashValues = 18;
boolean shouldFlash[nbOfFlashValues];
int flashAt = 0;

int spectacleId = -1;

void setup() {
	pinMode(ledPin, OUTPUT);
	pinMode(tachoPin, INPUT);
	pinMode(buttonPin, INPUT);

	if (debug) {
		Serial.begin(9600);
	}

	// Initialize fan lecture table
	for (int i = 0; i < nbOfValues; i++) {
		fanPulseValues[i] = 0;
	}
 	// Wait for fan to speed up
	delay(1000);
	for (int i = 0; i < nbOfValues / 4; i++) {
		updateFanRevolutionsPerSecond();
	}

	// Init spectacle configuration table
	for (int i = 0; i < 6; i++) {
		shouldFlash[i] = false;
	}

	setSpectacle();
}

void loop() {
	if (digitalRead(buttonPin)) {
		updateFanRevolutionsPerSecond();
		setSpectacle();

		delay(1000);
	}

	// Read the time in microseconds
	unsigned long currentMicros = micros();

	// If interval is longer than the length of a flash and the leds
	// are on, shut them down
	if (ledState && (currentMicros - previousMicros > flashLength)) {
		ledState = false;
		digitalWrite(ledPin, ledState);
	}

	// In any case, on or off, update the led State with the next
	// value in the spectacle table
	if (currentMicros - previousMicros > fixedFlashInterval) {
		previousMicros = currentMicros;

		ledState = flashOrNot();
		digitalWrite(ledPin, ledState);
	}

	// Read the potentiometer for fine tunning every second
	if (currentMicros - previousUpdateMicros > 1000000) {
		previousUpdateMicros = currentMicros;
		updateFixedFlashInterval();
	}
}

// Compute that number of revolutions per second by the fan
void updateFanRevolutionsPerSecond() {
	fanRevolutionsPerSecond = 1000000 / (4 * readFanPulseLength());
}

// Updates the fan speed table
long readFanPulseLength() { 
	// Shift 4 readings
	for (int i = 0; i < nbOfValues; i++) {
		fanPulseValues[i + 4] = fanPulseValues[i];
	}

	// Fill the missing 4 readings with new values
	for (int i = 0; i < 1; i++) {
		fanPulseValues[2 * i] = pulseIn(tachoPin, HIGH);
		fanPulseValues[2 * i + 1] = pulseIn(tachoPin, LOW);
		fanPulseValues[2 * i + 2] = pulseIn(tachoPin, LOW);
		fanPulseValues[2 * i + 3] = pulseIn(tachoPin, HIGH);
	}

	// Return a table average
	long average = 0;
	for (int i = 0; i < nbOfValues; i++) {
		average += fanPulseValues[i];
	}

	return average / nbOfValues;
}

// Selects the next spectacle in the list
void setSpectacle() {
	spectacleId = spectacleId + 1;

	switch (spectacleId) {
		case 1:
			flashPerSecond = 2 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = true;
			}
			break;
		case 2:
			flashPerSecond = 3 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = true;
			}
			break;
		case 3:
			flashPerSecond = 4 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = true;
			}
			break;
		case 4:
			flashPerSecond = 3 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = (i % 3 != 0);
			}
			break;
		case 5:
			flashPerSecond = 18 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = (i < 5);
			}
			break;
		case 6:
			flashPerSecond = 18 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = (i < 3);
			}
			break;
		case 7:
			flashPerSecond = 18 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = (i < 2);
			}
			break;
		case 0:
		default:
			spectacleId = 0;
			flashPerSecond = 1 * fanRevolutionsPerSecond;
			for (int i = 0; i < nbOfFlashValues; i++) {
				shouldFlash[i] = true;
			}
	}

	flashInterval = 1000000 / flashPerSecond;

	if (debug) {
		printDebugString();
	}
}

// Fixes the flash interval using a percentage value
// read from the potenciometer
void updateFixedFlashInterval() {
	// Potentiometer values are read between 0 and 1023
	// so, right now it goes from -2% to 2%
	fixedFlashInterval = (1.0 + ((analogRead(potPin) - 512.0) / (512 * 50))) * flashInterval;
}

boolean flashOrNot() {
	boolean flash = shouldFlash[flashAt];
	flashAt = (flashAt + 1) % nbOfFlashValues;

	return flash;
}

void printDebugString() {
	Serial.println();
	Serial.print("spectacle=");
	Serial.println(spectacleId);
	Serial.print("fanRevolutionsPerSecond=");
	Serial.print(fanRevolutionsPerSecond);
	Serial.print(" flashPerSecond=");
	Serial.println(flashPerSecond);
}
