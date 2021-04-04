// Clignotement d'une LED (mot anglais pour DEL) à 1 Hz par une 
// interruption en provenance du timer 2,
// pendant que le programme principal fait
// clignoter une autre LED (période 10s)
 
const byte Led = 5; // Pour utiliser la LED du module
const byte Led2 = 13; // Led clignotante du programme principal
#define LedToggle digitalWrite (Led, !digitalRead(Led))
#define Led2Toggle digitalWrite (Led2, !digitalRead(Led2))
 
void setup () {
  pinMode (Led, OUTPUT);
  pinMode (Led2, OUTPUT);
  cli(); // Désactive l'interruption globale
  bitClear (TCCR2A, WGM20); // WGM20 = 0
  bitClear (TCCR2A, WGM21); // WGM21 = 0 
  TCCR2B = 0b00000110; // Clock / 256 soit 16 micro-s et WGM22 = 0
  TIMSK2 = 0b00000001; // Interruption locale autorisée par TOIE2
  sei(); // Active l'interruption globale
}
 
byte varCompteur = 0; // La variable compteur
 
// Routine d'interruption
ISR(TIMER2_OVF_vect) {
  TCNT2 = 256 - 250; // 250 x 16 µS = 4 ms
  if (varCompteur++ > 125) { // 125 * 4 ms = 500 ms (demi-période)
    varCompteur = 0;
    LedToggle;
  }
}
 
void loop () {
  // Mettre ici le programme. Exemple :
  Led2Toggle;
  delay (5000); // Demi-période de la deuxième LED
}
