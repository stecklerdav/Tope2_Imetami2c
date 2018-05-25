#include <EEPROM.h>
//#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>

// definicion de los pines a usar en el arduino con el driver del motor de paso
const int stepPin = 11;  // pin 11
const int dirPin = 12; // pin 10
const int enPin = 13; // pin 9
const int intPin0 = 2; // pin interrupcion
const int intPin1 = 3; // pin interrupcion

//const int stepPin = 1;  // pin 1
//const int dirPin = 18; // pin 18
//const int enPin = 19; // pin 19
//const int intPin0 = 2; // pin interrupcion
//const int intPin1 = 3; // pin interrupcion

// Definimos las constantes
#define COLS 16 // Columnas del LCD
#define ROWS 4 // Filas del LCD
#define FIN_DE_CARRERA 2000 // 2 mts

String Stringout;
//int Direccion = 0;
String Val_Anterior,Distancia,Pasos_x_Cm;

String Paso = "1";
byte PasoInt = 1,Val_Anterior_int_LOW;
byte Bandera=0;
char Val_Anterior_int_HIGH;
int carrera = 0,Val_Anterior_int,DistanciaActual,aux;



// -------prototipos de funciones---------//
void MENU(void);
void LONGITUD(void);
void PUESTA_CERO(void);
void PASOSXCM(void);
//----------------------------------------//   


 //-----Definicion del Keypad------- 

  const byte filas = 4;
  const byte columnas = 3;
  char teclas[filas][columnas] ={
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
  };  
  byte pinsFilas[filas] = {10, 9, 8, 7}; // ARREGLO FILAS
  byte pinsColumnas[columnas] = {6, 5, 4}; // ARREGLO COLUMNAS
   
  Keypad keypad = Keypad ( makeKeymap (teclas), pinsFilas, pinsColumnas, filas, columnas );
  //void keypadEvent(void);
//---------------------------------- 

LiquidCrystal_I2C lcd(0x27,16,4); 
// Lo primero is inicializar la librería indicando los pins de la interfaz

//LiquidCrystal lcd(12, 11, 14, 15, 16, 17);

void setup() /// ACA COLOCO TODO LO QUE QUIERO INICIALIZAR UNA SOLA VEZ
{
  // put your setup code here, to run once:
  // asignacion de los tres pines que van al driver
  pinMode(stepPin,OUTPUT); // pin de paso debe ser de salida en el arduino
  pinMode(dirPin,OUTPUT);  // pin de dieccion debe ser de salida en el arduino
  pinMode(enPin,OUTPUT);   // pin de activacion debe ser de salida en el arduino
  digitalWrite(enPin,HIGH); //inicializo como 0 logico para iniciar con el motor apagado  
  //-----------------------------------------------

  pinMode(intPin0, INPUT_PULLUP);
  pinMode(intPin1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(intPin0), Interrupcion_0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(intPin1), Interrupcion_0, CHANGE);


 //-------------Cargar valores desde la EEPROM ------------------// 
  EEPROM.get(1,Val_Anterior_int);
  PasoInt               = EEPROM.read(16);
  Val_Anterior = (String)Val_Anterior_int;
 //--------------------------------------------------------------// 
  

  Serial.begin(9600);  
  lcd.begin(16,4);  
  lcd.clear();
  lcd.home(); // coloca el cursor en home (0,0) Columna, Fila
  lcd.init();
  lcd.backlight();   
  lcd.setCursor (0, 1); 
  lcd.print ("  IMETAM, C.A."); // Bienvenida del programa
  delay (2000);
  
  // MENU INICIAL
  
  lcd.clear();
  lcd.home(); // coloca el cursor en home (0,0) Columna, Fila
  lcd.setCursor (4, 1);
  lcd.print ("MENU"); // imprime mmenu  
  for(int y = 0;y < 8;y++)
  {
    lcd.scrollDisplayRight();
    delay(100);
  }
  for(int y = 0;y <6;y++)
  {
    lcd.scrollDisplayLeft();
    delay(100);
  }
  delay(2000);

  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print ("1.-Cortar");//centralo a tu gusto
  
  lcd.setCursor (0, 1);
  //lcd.setCursor (0, 1);
  lcd.print ("2.-Posicion Cero");//centralo a tu gusto
  
  lcd.setCursor (-4, 2);
  //lcd.setCursor (0, 2);
  lcd.print ("3.-Pasos x CM");//centralo a tu gusto

  lcd.setCursor (-4, 3);
  //lcd.setCursor (0, 3);
  lcd.print("Respuesta:");
  lcd.setCursor ( 11-4, 3);
  //lcd.setCursor ( 0, 3);  
}
void MENU()// menu al comenzar el mando
{ 
  // MENU 
  
  lcd.setCursor (0, 0);
  lcd.print ("1.-Cortar");//centralo a tu gusto
  
  lcd.setCursor (0, 1);
  //lcd.setCursor (0, 1);
  lcd.print ("2.-Posicion Cero");//centralo a tu gusto
  
  lcd.setCursor (-4, 2);
  //lcd.setCursor (0, 2);
  lcd.print ("3.-Pasos x CM");//centralo a tu gusto

  lcd.setCursor (-4, 3);
  //lcd.setCursor (0, 3);
  lcd.print("Respuesta:");
  lcd.setCursor ( 11-4, 3);
  //lcd.setCursor ( 11, 3);
}

void LONGITUD() 
{     
  char k = keypad.getKey(); 
  lcd.clear();
  lcd.home();
  EEPROM.get(1,aux);
  Val_Anterior =(String) aux;
  while ( (k != '*') )
  { 
   String myString = "";     
   k = keypad.getKey();
   lcd.clear();
   lcd.home(); 
   while((k != '#')&&(k != '*') )   
   {
    lcd.setCursor(0,0);
    lcd.print ("Posicion  Actual");
    lcd.setCursor(6,1); 
    lcd.print (Val_Anterior);//centralo a tu gusto 
    lcd.setCursor(1-4,2);
    lcd.print ("Nueva Posicion");
    lcd.setCursor(6-4,3);   
    lcd.print (myString);//centralo a tu gusto    
    k = keypad.waitForKey();   
    if ((k != NO_KEY) && (k != '#') )
    {        
      myString.concat(k);        
      lcd.setCursor(6-4,3);                     
    }     
    if ((k != NO_KEY) &&(k == '*'))
    {          
      lcd.clear();
      lcd.home();           
      myString = "";      
    }  
  }
  if((k == '#'))
  {             
    DistanciaActual = myString.toInt(); // de String  a Int
    EEPROM.put(1,DistanciaActual);// guardo la distancia actualmente introducida 
    carrera = (DistanciaActual - Val_Anterior_int);
    Val_Anterior_int = DistanciaActual;
    Val_Anterior = (String)Val_Anterior_int;     
    lcd.clear();
    lcd.home();
    lcd.setCursor (1, 1);
    lcd.print ("En Movimiento");//centralo a tu gusto
    lcd.clear();
    lcd.home();
    delay(1000);
    lcd.setCursor (1, 1);
    lcd.print ("En Movimiento");//centralo a tu gusto
  if((PasoInt*carrera) < 0)
  {  
    digitalWrite(dirPin,HIGH); 
  }
  else
  {
    digitalWrite(dirPin,LOW); 
  }  
  digitalWrite(enPin,LOW); //inicializo como 1 logico para iniciar con el motor encendido  
  for(unsigned int x = 0; x < abs(PasoInt*carrera); x++) 
    {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(500); 
    }     
  digitalWrite(enPin,HIGH); //inicializo como 0 logico para iniciar con el motor apagado
   
  }
 }
    delay (500);  
    lcd.clear();
}
void PUESTA_CERO()
{   
  lcd.clear();
  lcd.home(); 
  lcd.setCursor(3,1); 
  lcd.print("CALIBRANDO");
  lcd.setCursor(5-4,2);
  lcd.print("A CERO");
  //lcd.setCursor(8,1);
  EEPROM.get(1,aux);
  PasoInt = EEPROM.read(16);   
  digitalWrite(dirPin,HIGH);  
  digitalWrite(enPin,LOW); //inicializo como 1 logico para iniciar con el motor encendido  
  for(unsigned int x = 0; x < abs(PasoInt*aux); x++) 
    {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(500); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(500); 
    }     
  digitalWrite(enPin,HIGH); //inicializo como 0 logico para iniciar con el motor apagado
  EEPROM.put(1,0x0000);// guardo la distancia actualmente introducida 
  lcd.clear();
}
void PASOSXCM()
{ 
  lcd.clear();
  lcd.home();  
  Paso = readVal("PASOS X CM:");
  PasoInt = Paso.toInt();
  EEPROM.write(16,PasoInt); 
  lcd.clear(); 
}
void DESPLI_VAR()
{
  lcd.clear();
  lcd.home();  
  lcd.print("DISTANCIA:");
  lcd.setCursor(11,0);
  EEPROM.get(1,aux);  
  lcd.print(aux);
  lcd.setCursor(0-4,2);
  lcd.print("PASOS X CM:");
  lcd.setCursor(12-4,2);  
  Pasos_x_Cm =  EEPROM.read(16);
  lcd.print(Pasos_x_Cm);
  delay(2000);
  lcd.clear();
}
//Data input: Enter the values and press "#"

String readVal(String cadena)
{
  String myString = ""; 
  char keyPressed = keypad.getKey(); 
  lcd.print(cadena); 
  delay(500);
  while (keyPressed != '#')
  { 
    keyPressed = keypad.waitForKey();    
    lcd.home(); 
    lcd.print(cadena);  
    lcd.setCursor(11,0);
    myString.concat(keyPressed); 
    if ((keyPressed != NO_KEY) && (keyPressed == '*'))
    {
      lcd.clear();
      lcd.home(); 
      lcd.print(cadena);  
      lcd.setCursor(11,0);
      myString = "";    
      lcd.print(myString); 
    }       
    if ((keyPressed != NO_KEY) && (keyPressed != '#'))
    {      
      lcd.print(myString);       
    }   
  
 }
 myString = myString.substring(0, myString.length() - 1);    
  return(myString);  
}

void loop()
{
   char Key = keypad.getKey();
   switch (Key)
  { 
    lcd.print(Key);
    delay(200);     
    case '1': LONGITUD();    break; 
    case '2': PUESTA_CERO(); break;
    case '3': PASOSXCM();    break;
    case '*': DESPLI_VAR();  break;
    default: break;    
  }  
  MENU();
}

void Interrupcion_0()//interrupcion a cero
{
  digitalWrite(enPin,HIGH); //inicializo como 0 logico para iniciar con el motor apagado
  EEPROM.put(1,0x0000);// debido a que golpea el pulsador en 0 debo guardar el valor nuevo de posicion
  
}
void Interrupcion_1()//interrupcion a fin de carrera 
{
  digitalWrite(enPin,HIGH); //inicializo como 0 logico para iniciar con el motor apagado
  EEPROM.put(1,FIN_DE_CARRERA);// debido a que golpea el pulsador en 0 debo guardar el valor nuevo de posicion
}



