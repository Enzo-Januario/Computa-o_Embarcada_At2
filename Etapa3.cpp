
#define NUM_CORES        4
#define TAMANHO_MAXIMO   100
#define NIVEL_VITORIA    10

// LEDs e botoes
const int ledPins[NUM_CORES]   = {8, 9, 10, 11};
const int botaoPins[NUM_CORES] = {5, 4, 3, 2};
const char* nomeCores[NUM_CORES] = {"AZUL", "VERDE", "AMARELO", "VERMELHO"};

const int segPins[7] = {6, 7, 12, 13, A0, A1, A2};

const byte digitos[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, 
  {0, 1, 1, 0, 0, 0, 0}, 
  {1, 1, 0, 1, 1, 0, 1}, 
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1}, 
  {1, 0, 1, 1, 0, 1, 1}, 
  {1, 0, 1, 1, 1, 1, 1}, 
  {1, 1, 1, 0, 0, 0, 0}, 
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 0, 1, 1} 
};

const int TEMPO_LED_LIGADO   = 450;
const int TEMPO_ENTRE_LEDS   = 200;
const int TEMPO_ANTES_JOGAR  = 700;
const int TEMPO_DEBOUNCE     = 50;

int sequencia[TAMANHO_MAXIMO];
int rodada = 0;

void setup() {
  Serial.begin(9600);

  // LEDs
  for (int i = 0; i < NUM_CORES; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  for (int i = 0; i < NUM_CORES; i++) {
    pinMode(botaoPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
  desligarDisplay();

  randomSeed(analogRead(A5));

  Serial.println("============================================");
  Serial.println("   JOGO DA MEMORIA - Etapa 3 (com display)");
  Serial.println("============================================");
  Serial.println("Cores: AZUL, VERDE, AMARELO, VERMELHO");
  Serial.println("Objetivo: chegar ao nivel 10!");
  Serial.println("Pressione qualquer botao para comecar...");

  mostrarDigito(0);
  esperarQualquerBotao();
  iniciarJogo();
}

void loop() {
  rodada++;

  if (rodada <= 9) {
    mostrarDigito(rodada);
  } else {
    mostrarDigito(9);
  }

  sequencia[rodada - 1] = random(0, NUM_CORES);

  Serial.println();
  Serial.print(">>> Rodada ");
  Serial.println(rodada);

  Serial.print("Sequencia: ");
  for (int i = 0; i < rodada; i++) {
    Serial.print(nomeCores[sequencia[i]]);
    if (i < rodada - 1) Serial.print(" -> ");
  }
  Serial.println();

  mostrarSequencia();

  Serial.println("Sua vez! Repita a sequencia...");
  delay(TEMPO_ANTES_JOGAR);

  if (!lerRespostaJogador()) {
    Serial.println("### ERROU! Reiniciando o jogo... ###");
    Serial.print("Voce chegou na rodada ");
    Serial.println(rodada);
    animacaoErro();
    rodada = 0;
    mostrarDigito(0);
    delay(1500);
    Serial.println();
    Serial.println("Pressione qualquer botao para comecar de novo...");
    esperarQualquerBotao();
    iniciarJogo();
    return;
  }

  Serial.println("Acertou!");

  // Verifica vitoria
  if (rodada >= NIVEL_VITORIA) {
    Serial.println();
    Serial.println("############################################");
    Serial.println("###   PARABENS!! VOCE VENCEU O JOGO!!!   ###");
    Serial.println("############################################");
    animacaoVitoria();
    rodada = 0;
    mostrarDigito(0);
    delay(1500);
    Serial.println();
    Serial.println("Pressione qualquer botao para jogar de novo...");
    esperarQualquerBotao();
    iniciarJogo();
  } else {
    delay(900);
  }
}


void iniciarJogo() {
  rodada = 0;
  mostrarDigito(0);
  for (int i = 0; i < NUM_CORES; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(120);
    digitalWrite(ledPins[i], LOW);
  }
  for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], HIGH);
  delay(250);
  for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], LOW);
  delay(500);
}

void mostrarSequencia() {
  for (int i = 0; i < rodada; i++) {
    int cor = sequencia[i];
    digitalWrite(ledPins[cor], HIGH);
    delay(TEMPO_LED_LIGADO);
    digitalWrite(ledPins[cor], LOW);
    delay(TEMPO_ENTRE_LEDS);
  }
}

bool lerRespostaJogador() {
  for (int i = 0; i < rodada; i++) {
    int botaoPressionado = esperarBotao();

    digitalWrite(ledPins[botaoPressionado], HIGH);

    while (digitalRead(botaoPins[botaoPressionado]) == LOW) {
      delay(10);
    }
    digitalWrite(ledPins[botaoPressionado], LOW);

    Serial.print("  -> Jogador apertou: ");
    Serial.println(nomeCores[botaoPressionado]);

    if (botaoPressionado != sequencia[i]) {
      return false;
    }
  }
  return true;
}

int esperarBotao() {
  while (true) {
    for (int i = 0; i < NUM_CORES; i++) {
      if (digitalRead(botaoPins[i]) == LOW) {
        delay(TEMPO_DEBOUNCE);
        if (digitalRead(botaoPins[i]) == LOW) {
          return i;
        }
      }
    }
  }
}

void esperarQualquerBotao() {
  int b = esperarBotao();
  while (digitalRead(botaoPins[b]) == LOW) {
    delay(10);
  }
  delay(200);
}

void animacaoErro() {
  for (int k = 0; k < 8; k++) {
    for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], HIGH);
    delay(100);
    for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], LOW);
    delay(100);
  }
}

void animacaoVitoria() {
  for (int repeat = 0; repeat < 3; repeat++) {
    // Acende LEDs em sequencia
    for (int i = 0; i < NUM_CORES; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(150);
    }
    // Pisca o display
    for (int p = 0; p < 4; p++) {
      mostrarDigito(0);
      delay(150);
      desligarDisplay();
      delay(150);
    }
    // Apaga LEDs em sequencia
    for (int i = NUM_CORES - 1; i >= 0; i--) {
      digitalWrite(ledPins[i], LOW);
      delay(100);
    }
  }
 
  for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], HIGH);
  mostrarDigito(0);
  delay(1500);
  for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], LOW);
}

void mostrarDigito(int n) {
  if (n < 0 || n > 9) {
    desligarDisplay();
    return;
  }
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], digitos[n][i] ? HIGH : LOW);
  }
}

void desligarDisplay() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segPins[i], LOW);
  }
}
