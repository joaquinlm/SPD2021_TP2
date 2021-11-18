
/*
 * TP2 SPD - 1G 2do cuatrimestre 2021
 *  Tema: Sistema de luces
 *  Grupo: "Los Borbotones"
 *  Integrantes: Áxel Claudio Melgar, Lucía Inés López, Joaquín Lubo Millán
 */

#include <LiquidCrystal.h>

#define NO_BOTON 0
#define BOTON_GIRODER 1
#define BOTON_LUZ 2
#define BOTON_FRENO1 3
#define BOTON_FRENO2 4
#define BOTON_GIROIZQ 5
#define BOTON_BALIZAS 6
#define AUTOMATICO 0
#define MANUAL 1
#define SWITCH_OFF 0
#define SWITCH_ON 1

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int fotoresistor = A5;
int toggleLuzPosicion = 0;
int lectura = 0;
int estadoDelAutomatico = 1;
int botonAhora;
int botonAntes = NO_BOTON;
int flag_Frenos = 0;
int flag_GiroIzq = 0;
int flag_GiroDer = 0;
int flag_Balizas = 0;
unsigned long millisAnterior = 0;
int estado = 0;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(A0, INPUT);
  pinMode(A5, INPUT);
  for (int i = 2; i <= 6; i++)
  {
    pinMode(i, OUTPUT);
  }
}

void loop()
{

  botonAhora = leerBoton();

  if (botonAhora != NO_BOTON && botonAhora != botonAntes)
  {

    switch (botonAhora)
    {
    case BOTON_GIRODER:
      Serial.println("BOTON 1 - Giro Derecho");
      flag_GiroDer = !flag_GiroDer;
      flag_GiroIzq = 0;
      break;

    case BOTON_LUZ:
      Serial.println("BOTON 2 - Luz");
      if (!toggleLuzPosicion)
      {
        switchLuzPosicion(&toggleLuzPosicion, MANUAL, &estadoDelAutomatico, SWITCH_ON);
      }
      else
      {
        switchLuzPosicion(&toggleLuzPosicion, MANUAL, &estadoDelAutomatico, SWITCH_OFF);
      }
      break;

    case BOTON_FRENO1:
      Serial.println("BOTON 3 - Luz Freno");
      encenderFrenos(&flag_Frenos);
      break;

    case BOTON_FRENO2:
      Serial.println("BOTON 4 - Luz Freno");
      encenderFrenos(&flag_Frenos);
      break;

    case BOTON_GIROIZQ:
      Serial.println("BOTON 5 - Giro Izquierdo");
      flag_GiroIzq = !flag_GiroIzq;
      flag_GiroDer = 0;
      break;

    case BOTON_BALIZAS:
      Serial.println("BOTON 6 - Balizas");
      flag_Balizas = !flag_Balizas;
      break;
    }
  }

  botonAntes = botonAhora;

  if (estadoDelAutomatico)
  {
    luzPosicionEnAutomatico();
  }

  if (flag_Frenos && botonAhora != BOTON_FRENO1 && botonAhora != BOTON_FRENO2)
  {
    apagarFrenos(&flag_Frenos);
  }

  gestionarBlinkers(flag_GiroIzq, flag_GiroDer, flag_Balizas);
}

// brief: Lee el valor que recibe el pin A0 y recorre rangos de valores
//  	  retornando el valor definido para el mismo. En caso que el valor
//		  recibido no esta comprendido dentro de los rangos retona 0.
//
// retorn: retorna el valor definido para el rango o 0 sino esta comprendido en estos.

int leerBoton()
{

  int valorBoton = analogRead(A0);

  // Giro Derecho /
  if (valorBoton > 757 && valorBoton < 777)
    return BOTON_GIRODER;

  // Luz Posicion /
  if (valorBoton > 502 && valorBoton < 522)
    return BOTON_LUZ;

  // Luz Freno 1
  if (valorBoton > 842 && valorBoton < 862)
    return BOTON_FRENO1;

  // Luz Freno 2
  if (valorBoton > 808 && valorBoton < 828)
    return BOTON_FRENO2;

  // Giro Izquierdo
  if (valorBoton > 867 && valorBoton < 887)
    return BOTON_GIROIZQ;

  // Balizas
  if (valorBoton > 672 && valorBoton < 692)
    return BOTON_BALIZAS;

  return NO_BOTON;
}

// Funciones intermitencia / blinkeo

// brief: Administra los componentes que titilan / blinkean (giro derecho, giro izquierdo y balizas).
//  	  Recibiendo por parametro los valores de los componentes, cambiando el estado entre encendido y apagado
//		  cada 300 milisegundos aproximadamente simula intermetencia.
//        En caso que todas los valores esten en cero (0), apaga los leds y  y su representacion en el display " ".
// param: int bandera luz de giro izquierdo
// param: int bandera luz de giro derecho
// param: int bandera luces balizas

void gestionarBlinkers(int flag_GiroIzq, int flag_GiroDer, int flag_Balizas)
{

  unsigned long millisActual = millis();
  if (millisActual - millisAnterior >= 300)
  {
    millisAnterior = millisActual;
    estado = !estado;
  }

  if (flag_Balizas)
  {
    if (estado)
    {
      balizasOn();
    }
    else
    {
      balizasOff();
    }
  }
  else if (flag_GiroIzq && !flag_Balizas)
  {
    if (estado)
    {
      giroDerOff();
      giroIzqOn();
    }
    else
    {
      giroIzqOff();
    }
  }
  else if (flag_GiroDer && !flag_Balizas)
  {
    if (estado)
    {
      giroIzqOff();
      giroDerOn();
    }
    else
    {
      giroDerOff();
    }
  }

  else if (!flag_GiroIzq && !flag_GiroDer && !flag_Balizas)
  {
    balizasOff();
  }
}

// brief: Apaga las luces de las balizas en el circuito, esto es apagar los leds
//  	  correspondientes (giro derecho e izquierdo) y su representacion en el
//        display " ".
//

void balizasOff()
{

  lcd.setCursor(0, 0);
  lcd.print(' ');
  lcd.setCursor(15, 0);
  lcd.print(' ');
  digitalWrite(6, LOW);
  digitalWrite(2, LOW);
}

// brief: Enciende las luces de las balizas en el circuito, esto es enciende los leds
//  	  correspondientes (giro derecho e izquierdo) y su representacion en el
//        display "<" y ">".
//
void balizasOn()
{

  lcd.setCursor(0, 0);
  lcd.print('<');
  lcd.setCursor(15, 0);
  lcd.print('>');
  digitalWrite(6, HIGH);
  digitalWrite(2, HIGH);
}

// brief: Enciende las luces del giro izquierdo en el circuito, esto es enciende el led
//  	  correspondiente y su representacion en el display "<".
//

void giroIzqOn()
{

  lcd.setCursor(0, 0);
  lcd.print('<');
  digitalWrite(6, HIGH);
}

// brief: Apaga las luces del giro izquierdo en el circuito, esto es apaga el led
//  	  correspondiente y su representacion en el display " ".
//

void giroIzqOff()
{

  lcd.setCursor(0, 0);
  lcd.print(' ');
  digitalWrite(6, LOW);
}

// brief: Enciende las luces del giro derecho en el circuito, esto es enciende el led
//  	  correspondiente y su representacion en el display ">".
//

void giroDerOn()
{

  lcd.setCursor(15, 0);
  lcd.print('>');
  digitalWrite(2, HIGH);
}

// brief: Apaga las luces del giro derecho en el circuito, esto es apaga el led
//  	  correspondiente y su representacion en el display " ".
//

void giroDerOff()
{

  lcd.setCursor(15, 0);
  lcd.print(' ');
  digitalWrite(2, LOW);
}

// Funciones frenos

// brief: Enciende las luces de freno en el circuito, esto es enciende el led
//  	  y muestra su representacion en el display "!!". Actualiza el valor
//		  de flag frenos a activo (1)
//
// param: int* flagFrenos puntero a entero

void encenderFrenos(int *flagFrenos)
{

  digitalWrite(4, HIGH);
  lcd.setCursor(7, 0);
  lcd.print('!');
  lcd.setCursor(8, 0);
  lcd.print('!');
  *flagFrenos = 1;
}

// brief: Apaga las luces de freno en el circuito, esto es apagar el led
//  	  y su representacion en el display " ". Actualiza el valor
//		  de flag frenos a inactivo (0)
//
// param: int* flagFrenos puntero a entero

void apagarFrenos(int *flagFrenos)
{

  digitalWrite(4, LOW);
  lcd.setCursor(7, 0);
  lcd.print(' ');
  lcd.setCursor(8, 0);
  lcd.print(' ');
  *flagFrenos = 0;
}

// Funciones Luz de posición

// brief: Función encargada de registrar y devolver la lectura del fotorresistor. No es llamada directamente en main sino que es usada por el módulo de posición en automático (linea 363)
// Está activa sólo si el flag que representa al estado del modo automático es true.
// return: entero mapeado devuelto por el fotorresistor

int lecturaFotoresistor()
{
  int lectura = analogRead(fotoresistor);
  lectura = map(lectura, 0, 1023, 0, 255);
  return lectura;
}

// brief: Función encargada cambiar el estado de las luces de posición. Imprime en el lcd el estado actual de las luces(ON/OFF)
// param: int *pToggleLuzPosicion puntero al estado actual de la luz de posición
// param: int modo entero para seleccionar el modo. cuando la función se llama con modo manual se baja la bandera del modo automático (en true por default)
// param: int *pEstadoDelAutomatico puntero del estado del modo automático
// param: int posicionSwitch para indicar si apagamos o encendemos la luz

void switchLuzPosicion(int *pToggleLuzPosicion, int modo, int *pEstadoDelAutomatico, int posicionSwitch)
{
  lcd.setCursor(0, 1);
  lcd.print("LUCES:");
  if (posicionSwitch == 1)
  {
    digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    lcd.setCursor(7, 1);
    lcd.print("ON ");
    *pToggleLuzPosicion = 1;
  }

  else if (posicionSwitch == 0)
  {
    digitalWrite(3, LOW);
    digitalWrite(5, LOW);
    lcd.setCursor(7, 1);
    lcd.print("OFF");

    *pToggleLuzPosicion = 0;
  }

  if (modo == 1)
  {
    lcd.setCursor(14, 1);
    lcd.print("  ");
    *pEstadoDelAutomatico = 0;
  }
}

void luzPosicionEnAutomatico()
{
  lectura = lecturaFotoresistor();
  if (lectura > 113 && !toggleLuzPosicion && estadoDelAutomatico)
  {

    switchLuzPosicion(&toggleLuzPosicion, AUTOMATICO, &estadoDelAutomatico, SWITCH_ON);
  }
  else if (lectura < 114 && toggleLuzPosicion && estadoDelAutomatico)
  {
    switchLuzPosicion(&toggleLuzPosicion, AUTOMATICO, &estadoDelAutomatico, SWITCH_OFF);
  }
  lcd.setCursor(14, 1);
  lcd.print("PA");
}

// brief: Módulo autómatico de luz de posición. Sólo activa si la bandera de autómatico está en true (true por default). Consiste en en llamadas a switchLuzPosicion() condicionales a la lectura que devuelve lecturaFotoresistor() y los parametros variables de switches (on/off). En caso de estar activa, imprime en el vértice inferior derecho la leyenda "PA" por "posición en automático"