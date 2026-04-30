/*
 * ============================================================
 *  Lab 2 - Jogo da Memória com LEDs
 *  Etapa 3: Jogo com 4 LEDs, 4 botoes e contador de rodadas
 * ============================================================
 *
 *  Mapeamento de pinos:
 *    LED Azul      -> D8    |   Botao Azul (S1)      -> D5
 *    LED Verde     -> D9    |   Botao Verde (S2)     -> D4
 *    LED Amarelo   -> D10   |   Botao Amarelo (S3)   -> D3
 *    LED Vermelho  -> D11   |   Botao Vermelho (S4)  -> D2
 *
 *  Display de 7 segmentos:
 *    Segmento A -> D6
 *    Segmento B -> D7
 *    Segmento C -> D12
 *    Segmento D -> D13
 *    Segmento E -> A0
 *    Segmento F -> A1
 *    Segmento G -> A2
 *
 *  Para display de catodo comum (K no GND): HIGH acende.
 *  Para display de anodo comum (no 5V): trocar HIGH<->LOW.
 * ============================================================
 */

#define NUM_CORES        4
#define TAMANHO_MAXIMO   100
#define NIVEL_VITORIA    10

// LEDs e botoes
const int ledPins[NUM_CORES]   = {8, 9, 10, 11};
const int botaoPins[NUM_CORES] = {5, 4, 3, 2};
const char* nomeCores[NUM_CORES] = {"AZUL", "VERDE", "AMARELO", "VERMELHO"};

// Display 7 segmentos: ordem A, B, C, D, E, F, G
const int segPins[7] = {6, 7, 12, 13, A0, A1, A2};

// Tabela de digitos 0-9 (catodo comum: 1 = aceso)
//  segmentos:    A  B  C  D  E  F  G
const byte digitos[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

// Tempos (em ms)
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

  // Botoes
  for (int i = 0; i < NUM_CORES; i++) {
    pinMode(botaoPins[i], INPUT_PULLUP);
  }

  // Display de 7 segmentos
  for (int i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }
  desligarDisplay();

  // Semente aleatoria a partir de um pino analogico nao usado
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

  // Mostra a rodada atual no display (limita ao digito 9)
  if (rodada <= 9) {
    mostrarDigito(rodada);
  } else {
    mostrarDigito(9);
  }

  // Sorteia a proxima cor
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

// ============================================================
//  Funcoes do jogo
// ============================================================

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

// Animacao de vitoria: display piscando + LEDs em sequencia
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
  // Acende todos os LEDs juntos no final
  for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], HIGH);
  mostrarDigito(0);
  delay(1500);
  for (int i = 0; i < NUM_CORES; i++) digitalWrite(ledPins[i], LOW);
}

// ============================================================
//  Funcoes do display de 7 segmentos
// ============================================================

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
