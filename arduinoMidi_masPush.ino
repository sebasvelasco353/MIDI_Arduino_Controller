/*
   Codigo que se encarga de recibir informacion desde knobs a los pines analogos
   y desde botones arcade push a los digitales, despues envia esa informacion a travez
   del puerto serial a 9600 de speed.

   Realizado por Sebastian Velasco Borrero bcause i can and im not afraid.
*/


//Referente a los potenciometros
int valoresNuevos [3];
int valoresViejos [3];

//referente a los arcade
int velocity = 100;
int noteON = 144;//144 = 10010000 in binary, note on command
int noteOFF = 128;//128 = 10000000 in binary, note off command
boolean encendido [6]; //cambiar de noteOn a noteOff

void setup()
{
  Serial.begin(9600);       // Set the speed of the midi port to the same as we will be using in the Hairless Midi software
  for (int i = 2; i < 8; i++) {
    pinMode(i, INPUT_PULLUP);
  }

  for (int i = 0; i < 3; i++) { //inicializo todos los valores de los knobs a la posicion actual del potenciometro
    valoresNuevos [i] = analogRead(i);
    valoresViejos [i] = analogRead(i);
  }

  for (int i = 0; i < 6; i++) { //inicializo todos los valores de los knobs a la posicion actual del potenciometro
    encendido [i] = false; //siempre son falsos menos cuando dan al push, ahi cambia a true, cuando sueltan el push envia note off y cambia a false
  }
}

void loop()
{
  controlKnobs (); //metodo que se encarga de revisar si hay cambios en los knobs o potenciometros
  controlArcade (); // metodo que se encarga de revisar si hay cambios en los botones de arcade
}

void controlKnobs () {
  for (int i = 0; i < 6; i++) {
    valoresNuevos [i] = analogRead(i) / 8; // Divide by 8 to get range of 0-127 for midi
    if (valoresNuevos [i] != valoresViejos [i]) // If the value does not = the last value the following command is made. This is because the pot has been turned. Otherwise the pot remains the same and no midi message is output.
    {
      MIDImessage(176, i + 1, valoresNuevos [i]);
    }         // 176 = CC command (channel 1 control change), 1 = Which Control, val = value read from Potentionmeter 1 NOTE THIS SAYS VAL not VA1 (lowercase of course)
    valoresViejos [i] = valoresNuevos[i];
    delay (10);
  }
}

void controlArcade () {
  for (int i = 2; i < 8; i++) {
    if (digitalRead (i) == LOW) {
      MIDImessage(noteON, 60 + i, velocity);
      encendido [i - 2] = true;
    } else if (encendido [i - 2] == true) {
      MIDImessage(noteOFF, 60 + i, velocity);
      encendido [i - 2] = false;
    } else {
      // No haga nada.
    }
    delay (10);
  }
  // delay (100);
}

void MIDImessage(byte command, byte data1, byte data2) //pass values out through standard Midi Command
{
  Serial.write(command);
  Serial.write(data1);
  Serial.write(data2);
}
