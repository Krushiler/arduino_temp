int delayDuration = 100;
int layers[] = {A0, A1};
int columns[] = {2, 3, 4, 5};
int layerCount = sizeof(layers) / sizeof(layers[0]);
int columnCount = sizeof(columns) / sizeof(columns[0]);

void setup() {
  for (int i = 0; i < layerCount; i++) {
    pinMode(layers[i], OUTPUT);
  }
  for (int i = 0; i < columnCount; i++) {
    pinMode(columns[i], OUTPUT);
  }
}

void showCombinations(int layerCount, int columnCount, int delayDuration) {
  for (int l = 0; l < layerCount; l++) {
    for (int c2 = 0; c2 < columnCount; c2++) {
      for (int i = 0; i < layerCount; i++) {
        digitalWrite(layers[i], l == i ? HIGH : LOW);
      }
      for (int j = 0; j < columnCount; j++) {
        digitalWrite(columns[j], c2 == j ? HIGH : LOW);
      }
      delay(delayDuration);
      for (int j = 0; j < columnCount; j++) {
        digitalWrite(columns[j], LOW);
      }
    }
  }
}

void loop() {
  showCombinations(layerCount, columnCount, delayDuration);
}
