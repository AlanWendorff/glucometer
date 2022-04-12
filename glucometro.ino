//#include <ArduinoBLE.h>
#define ultrasoundPin 3                                            // D3 Ultrasonido
#define LEDpin 2                                                   // D2 LED NIR
#define glucoseSensor A0                                           // A0 Salida Sensor glucómetro

//BLEService glucoseService("19B10010-E8F2-537E-4F6C-D104768A1214"); // crear servicio

// crear caracteristicas del sensor y permitir a los dispositivos remotos recibir notificaciones
//BLEByteCharacteristic glucoseCharacteristic("19B10012-E8F2-537E-4F6CD104768A1214", BLERead | BLENotify);
        
// Definir variables globales
int periodo = 4;                  // periodo del temporizador encendido y apagado
int contadorNIR = 0;              // generar la onda cuadrada del IR
int contadorSensor = 0;           // leer salida del sensor
int diferenciaMaxMin = 0;         // salida sensor
int lecturaMaxima = 0;            // valor maximo
int lecturaMinima = 1023;         // valor mínimo
int lecturaSensor;                // valor que lee a la salida del sensor
int stateLEDNIR = 0;              // 0 estado apagado, 1 estado encendido
unsigned long tiempoAnterior = 0; // guarda tiempo de referencia para comparar

void setup(){
    // Configurar puerto de salida
    Serial.begin(9600);
    while (!Serial);

    // Definir altavoz,fotodiodo, LED NIR
    pinMode(ultrasoundPin, OUTPUT);
    pinMode(LEDpin, OUTPUT);
    pinMode(glucoseSensor, INPUT);
    
    // Empezar inicialización ETSI Informática Universidad de Málaga 82 
    //if (!BLE.begin()){
        //Serial.println("starting BLE failed!");
        //while (1);
    //}
    
    // Establecer nombre local de los anuncios periféricos
    //BLE.setLocalName("glucoseSensor");
    
    // Establecer el UUID para el servicio
    //BLE.setAdvertisedService(glucoseService);

    // Añadir caracteristicas al servicio
    //glucoseService.addCharacteristic(glucoseCharacteristic);
    
    // Añadir el servicio
    //BLE.addService(glucoseService);
    //glucoseCharacteristic.writeValue(0);
    
    // Empezar a anunciarse
    //BLE.advertise();
    //Serial.println("Bluetooth device active, waiting for connections...");
}

void loop(){
    tiempoAnterior = millis(); // guarda el tiempo actual como referencia
    
    // Elección para eventos BLE
    //BLE.poll();
    while (millis() - tiempoAnterior <= periodo){
        digitalWrite(ultrasoundPin, LOW);
        delayMicroseconds(12.5);
        digitalWrite(ultrasoundPin, HIGH);
        delayMicroseconds(12.5);
        if (millis() - tiempoAnterior == 2){
            digitalWrite(ultrasoundPin, LOW);
        }
    }
    
    // Almacenar los valores de la salida del sensor
    lecturaSensor = analogRead(A0);
    
    // Si los valores de la salida son mayores que la lectura máxima, estos valores son ahora los máximos 
    if (lecturaSensor > lecturaMaxima){
        lecturaMaxima = lecturaSensor;
    }
    
    // Si los valores de la salida son menores que la lectura mínima, estos valores son ahora los mínimos Anexos 83 
    if (lecturaSensor < lecturaMinima){
        lecturaMinima = lecturaSensor;
    }
    
    // Cada vez que se ejecuta el loop aumenta el contador del LED NIR en 1
    contadorNIR = contadorNIR + 1;
    
    // 125 veces*4ms=500ms encendido y 500 ms apagado
    if (contadorNIR == 125){
        contadorNIR = 0;
        if (stateLEDNIR == 0){
            digitalWrite(LEDpin, HIGH);
            stateLEDNIR = 1;
        }
        else{
            digitalWrite(LEDpin, LOW);
            stateLEDNIR = 0;
            contadorSensor = contadorSensor + 1;
            
            // cada 3 segundos se calcula la diferencia entre valores máximo y mínimo
            if (contadorSensor == 3){
                contadorSensor = 0;
                diferenciaMaxMin = lecturaMaxima - lecturaMinima;
                lecturaMaxima = 0;
                lecturaMinima = 1023;
                float voltaje = (diferenciaMaxMin * 3.3) / 1023;
                Serial.print("Nivel de glucosa: ");
                Serial.print(diferenciaMaxMin);
                Serial.print(" mg/dl");
                Serial.print(" Voltaje: ");
                Serial.println(voltaje);

                // ha cambiado el valor desde la ultima vez que fue leído
                //boolean glucoseChanged = (glucoseCharacteristic.value() != diferenciaMaxMin);
                //if (glucoseChanged){
                    // valor de glucosa cambiado, caracteristicas actualizadas
                    //glucoseCharacteristic.writeValue(diferenciaMaxMin);
                //}
            }
        }
    }
}
