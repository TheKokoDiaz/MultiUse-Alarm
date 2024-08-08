//Incluir Librerías para:
//Módulo de Reloj
#include <ThreeWire.h>
#include <RtcDS1302.h>

//Pantalla LCD
#include <LiquidCrystal.h>

//Reloj
//Modulo del reloj
ThreeWire myWire(9, 8, 10);   //Pines: DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);

//Tiempo configurado por el usuario
//Hora
int sec = 0;
int minutes= 0;
int hrs = 0;

//Fecha
int day = 1;
int month = 1;
int year = 2020;

//Variables Extras para mejor personalización y vista
int blink = 0;
int mode = 0;
int page = 0;
int loops = 0;
bool format = true;
bool setting = false;

//LCD
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Potenciometro
int pot = A0;
int pot_value;

//Push Buttons
//1.- Blue
int Btn_Adjust = 11;
int Btn_Adjust_Sts = 0;

//2.- Red (+)
int Btn_Add = 12;
int Btn_Add_Sts = 0;

//3.- Red (-)
int Btn_Substract = 13;
int Btn_Substract_Sts = 0;

void setup() {
  //Iniciación del Puerto Serial (Para abrirlo usa Ctrl + Shift + M)
  Serial.begin(9600);

  //Iniciación del LCD
  lcd.begin(16, 2);

  //Push Buttons
  pinMode(Btn_Adjust, INPUT);

  //Configuración de reloj
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  RtcDateTime zeroTime(2020, 1, 1, 0, 0, 0);
  Rtc.SetDateTime(zeroTime);
}

//Función con la que podemos dar formato a la hora y fecha
void PrintClock(int t) {
  //Si el número es menor a 10, se agregará un cero a la izquierda
  if(t < 10){
    lcd.print("0");
  }

  lcd.print(t);
}

//Función para imprimir la hora, fecha y más, como un reloj digital
void Clock(){
  int pm = 0;

  //Obtenemos la fecha y hora de cuando se haya subido el código
  RtcDateTime now = Rtc.GetDateTime();
 	
  //Escribir hora (HH:MM:SS)
 	lcd.setCursor(0,0);

  //Condicional para escribir en formato de 12 hrs
  if(format == true){
    if(now.Hour() == 0){
      PrintClock(12);
    } else {
      if(now.Hour() <= 12){
        PrintClock(now.Hour());
        pm = 0;
      } else {
        PrintClock(now.Hour() - 12);
        pm = 1;
      }
    }
  } else {
    PrintClock(now.Hour());
  }

 	lcd.print(":");
 	PrintClock(now.Minute());
 	lcd.print(":");
 	PrintClock(now.Second());

  //Escribe AM o PM al final de la hora si el formato es de 12hrs
  if(format == true){
    lcd.setCursor(9, 0);
    if(pm == 0){
      lcd.print("AM");
    } else {
      lcd.print("PM");
    }
  }

  //Escribir fecha (DD/MM/AAAA)
 	lcd.setCursor(0,1);
 	PrintClock(now.Day());
 	lcd.print("/");
 	PrintClock(now.Month());
 	lcd.print("/");
 	lcd.print(now.Year());

  delay(250);
}

void loop() {
  //Lectura de datos
  Btn_Adjust_Sts = digitalRead(Btn_Adjust);
  Btn_Add_Sts = digitalRead(Btn_Add);
  Btn_Substract_Sts = digitalRead(Btn_Substract);
  lcd.clear();

  //Verifica si el botón de configuración/modo se mantiene presionado
  if(Btn_Adjust_Sts == LOW){
    loops++;

    //Si se mantiene pulsado por 10 "loops", el reloj entrará en el modo de configuración
    if(loops >= 10){
      setting = true;
      loops = 0;

      //Guardando la ultima hora y fecha para modificarla
      RtcDateTime now = Rtc.GetDateTime();
      sec = now.Second();
      minutes= now.Minute();
      hrs = now.Hour();

      day = now.Day();
      month = now.Month();
      year = now.Year();
    }
  } else {
    loops = 0;
  }
  
  //Permite cambiar entre el formato de 12 y 24 horas
  if(Btn_Add_Sts == LOW && setting == false){
    if(format == false){
      format = true;
    } else {
      if(format == true){
        format = false;
      }
    }
  }

  //Modo de Configuración:
  if(setting == true){
    int pm = 0;

    lcd.setCursor(13,0);
    lcd.print("|*|");

    lcd.setCursor(0,0);

    //Modo (Alterna entre horas, minutos, segundos...)
    if(Btn_Adjust_Sts == LOW){
      mode++;

      if(mode >= 6){
        mode = 0;
      }
    }

    //Hora (hrs)
    if(mode == 0){
      if(Btn_Add_Sts == LOW){
        hrs++;
      } else {
        if(Btn_Substract_Sts == LOW){
          hrs--;
        }
      }

      if(hrs <= -1){ hrs = 23; }
      if(hrs > 23){ hrs = 0; }
    }

    //Condicional para escribir en formato de 12 hrs
    if(blink == 0 && mode == 0){
      lcd.print("  ");

      if(hrs <= 12){
        pm = 0;
      } else {
        pm = 1;
      }
    } else {
      if(format == true){
        if(hrs == 0){
          PrintClock(12);
        } else {
          if(hrs <= 12){
            PrintClock(hrs);
            pm = 0;
          } else {
            PrintClock(hrs - 12);
            pm = 1;
          }
        }
      } else {
        PrintClock(hrs);
      }
    }

    lcd.print(":");

    //Minutos (min)
    if(mode == 1){
      if(Btn_Add_Sts == LOW){
        minutes++;
      } else {
        if(Btn_Substract_Sts == LOW){
          minutes--;
        }
      }
      
      if(minutes<= -1){ minutes= 59; }
      if(minutes> 59){ minutes= 0; }
    }

    if(blink == 0 && mode == 1){
      lcd.print("  ");
    } else {
      PrintClock(minutes);
    }

    lcd.print(":");

    //Segundos (sec)
    if(mode == 2){
      if(Btn_Add_Sts == LOW){
        sec++;
      } else {
        if(Btn_Substract_Sts == LOW){
          sec--;
        }
      }
      
      if(sec <= -1){ sec = 59; }
      if(sec > 59){ sec = 0; }
    }

    if(blink == 0 && mode == 2){
      lcd.print("  ");
    } else {
      PrintClock(sec);
    }

    //Escribe AM o PM al final de la hora si el formato esta en 12 hrs
    if(format == true){
      lcd.setCursor(9, 0);
      if(pm == 0){
        lcd.print("AM");
      } else {
        lcd.print("PM");
      }
    }

    //Escribir fecha
    lcd.setCursor(0,1);

    //Días (day)
    if(mode == 3){
      if(Btn_Add_Sts == LOW){
        day++;
      } else {
        if(Btn_Substract_Sts == LOW){
          day--;
        }
      }
      
      if(day <= 0){ day = 31; }
      if(day > 31){ day = 1; }
    }

    if(blink == 0 && mode == 3){
      lcd.print("  ");
    } else {
      PrintClock(day);
    }

    lcd.print("/");
    
    //Meses (month)
    if(mode == 4){
      if(Btn_Add_Sts == LOW){
        month++;
      } else {
        if(Btn_Substract_Sts == LOW){
          month--;
        }
      }
      
      if(month <= 0){ month = 12; }
      if(month > 12){ month = 1; }
    }

    if(blink == 0 && mode == 4){
      lcd.print("  ");
    } else {
      PrintClock(month);
    }

    lcd.print("/");

    //Año (year)
    if(mode == 5){
      if(Btn_Add_Sts == LOW){
        year++;
      } else {
        if(Btn_Substract_Sts == LOW){
          if(year >= 2000){ year--; }
        }
      }
    }

    if(blink == 0 && mode == 5){
      lcd.print("  ");
    } else {
      lcd.print(year);
    }

    //Configurar la nueva hora
    if(Btn_Adjust_Sts == LOW){
    loops++;
      if(loops >= 10 && Btn_Adjust_Sts == LOW){
        setting = false;
        RtcDateTime newTime(year, month, day, hrs, minutes, sec);
        Rtc.SetDateTime(newTime);
        loops = 0;
      }
    } else {
      loops = 0;
    }

    //Variables para el "parpadeo" del campo a editar
    if(blink == 0){
      blink++;
    } else {
      blink--;
    }

    delay(200);
  } else {
    Clock();  
  }
}