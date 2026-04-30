/*
 * Lab 2 - Jogo da Memória com LEDs
 * Etapa 1: Jogo Básico com 2 LEDs e 2 botões
 *
 * Mapeamento:
 *   LED Azul   -> D8   |   Botão Azul   -> D5
 *   LED Verde  -> D9   |   Botão Verde  -> D4
 */

#define NUM_CORES        2
#define TAMANHO_MAXIMO   50

const int ledPins[NUM_CORES] = {8, 9};
const int botaoPins[NUM_CORES] = {5, 4};
const char* nomeCores[NUM_CORES] = {"AZUL", "VERDE"};

const int TEMPO_LED_LIGADO   = 500;
const int TEMPO_ENTRE_LEDS   = 250;
const int TEMPO_ANTES_JOGAR  = 800;
const int TEMPO_DEBOUNCE     = 50;

int sequencia[TAMANHO_MAXIMO];
int rodada = 0;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < NUM_CORES; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  for (int i = 0; i < NUM_CORES; i++) {
    pinMode(botaoPins[i], INPUT_PULLUP);
  }

  randomSeed(analogRead(A5));

  Serial.println("============================================");
  Serial.println("   JOGO DA MEMORIA - Etapa 1 (2 cores)");
  Serial.println("============================================");
  Serial.println("Pressione qualquer botao para comecar...");

  esperarQualquerBotao();
  iniciarJogo();
}

void loop() {
  rodada++;

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
    animacaoErro();
    rodada = 0;
    delay(1500);
    Serial.println();
    Serial.println("Pressione qualquer botao para comecar de novo...");
    esperarQualquerBotao();
    iniciarJogo();
  } else {
    Serial.println("Acertou! Proxima rodada...");
    delay(1000);
  }
}

void iniciarJogo() {
  rodada = 0;
  for (int i = 0; i < NUM_CORES; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(150);
    digitalWrite(ledPins[i], LOW);
  }
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
  for (int k = 0; k < 6; k++) {
    for (int i = 0; i < NUM_CORES; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(120);
    for (int i = 0; i < NUM_CORES; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(120);
  }
}
