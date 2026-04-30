/*
 * ============================================================
 *  Lab 2 - Jogo da Memória com LEDs
 *  Etapa 2: Jogo Avançado com 4 LEDs e 4 botões
 * ============================================================
 *
 *  Mapeamento de pinos:
 *    LED Azul      -> D8    |   Botão Azul (S1)      -> D5
 *    LED Verde     -> D9    |   Botão Verde (S2)     -> D4
 *    LED Amarelo   -> D10   |   Botão Amarelo (S3)   -> D3
 *    LED Vermelho  -> D11   |   Botão Vermelho (S4)  -> D2
 *
 *  Botões: usam INPUT_PULLUP (ligar entre o pino e GND).
 * ============================================================
 */

#define NUM_CORES        4
#define TAMANHO_MAXIMO   100

// LEDs: 0=Azul, 1=Verde, 2=Amarelo, 3=Vermelho
const int ledPins[NUM_CORES]   = {8, 9, 10, 11};
const int botaoPins[NUM_CORES] = {5, 4, 3, 2};
const char* nomeCores[NUM_CORES] = {"AZUL", "VERDE", "AMARELO", "VERMELHO"};

// Tempos (em ms)
const int TEMPO_LED_LIGADO   = 450;
const int TEMPO_ENTRE_LEDS   = 200;
const int TEMPO_ANTES_JOGAR  = 700;
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

  // Semente aleatoria a partir de um pino analogico nao usado
  randomSeed(analogRead(A5));

  Serial.println("============================================");
  Serial.println("   JOGO DA MEMORIA - Etapa 2 (4 cores)");
  Serial.println("============================================");
  Serial.println("Cores: AZUL, VERDE, AMARELO, VERMELHO");
  Serial.println("Pressione qualquer botao para comecar...");

  esperarQualquerBotao();
  iniciarJogo();
}

void loop() {
  rodada++;

  // Sorteia a proxima cor
  sequencia[rodada - 1] = random(0, NUM_CORES);

  Serial.println();
  Serial.print(">>> Rodada ");
  Serial.println(rodada);

  // Mostra a sequencia no Monitor Serial
  Serial.print("Sequencia: ");
  for (int i = 0; i < rodada; i++) {
    Serial.print(nomeCores[sequencia[i]]);
    if (i < rodada - 1) Serial.print(" -> ");
  }
  Serial.println();

  // Pisca a sequencia nos LEDs
  mostrarSequencia();

  // Vez do jogador
  Serial.println("Sua vez! Repita a sequencia...");
  delay(TEMPO_ANTES_JOGAR);

  if (!lerRespostaJogador()) {
    Serial.println("### ERROU! Reiniciando o jogo... ###");
    Serial.print("Voce chegou na rodada ");
    Serial.println(rodada);
    animacaoErro();
    rodada = 0;
    delay(1500);
    Serial.println();
    Serial.println("Pressione qualquer botao para comecar de novo...");
    esperarQualquerBotao();
    iniciarJogo();
  } else {
    Serial.println("Acertou! Proxima rodada...");
    delay(900);
  }
}

// ============================================================
//  Funcoes auxiliares
// ============================================================

void iniciarJogo() {
  rodada = 0;
  // Animacao de inicio: acende os 4 LEDs em sequencia
  for (int i = 0; i < NUM_CORES; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(120);
    digitalWrite(ledPins[i], LOW);
  }
  // Pisca os 4 juntos
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

    // Feedback visual: acende o LED da cor enquanto segura
    digitalWrite(ledPins[botaoPressionado], HIGH);

    // Espera soltar o botao
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
  // Pisca todos os LEDs rapidamente
  for (int k = 0; k < 8; k++) {
    for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], HIGH);
    delay(100);
    for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], LOW);
    delay(100);
  }
}
