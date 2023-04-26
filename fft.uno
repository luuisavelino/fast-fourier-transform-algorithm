#include <Arduino.h>
#include <math.h>

typedef struct {
    float real;
    float imag;
} Complex;

typedef struct {
    float amplitute;
    float freq;
} Transformada;

int analogPin = A0;

float period = 1.0 / 60;
int sr = 1000;
float ts = 1.0 / sr;

int N = round(period * sr);

float SenoidData[] = {0.0, 1.1043736580540338, 2.0536413177860657, 2.7144811573980587, 2.9940801852848145, 2.853169548885461, 2.311539728327368, 1.4452610223051467, 0.3759997006929136, -0.746069661494565, -1.7633557568774192, -2.5329837765060446, -2.946861752186066, -2.946861752186066, -2.532983776506047, -1.763355756877422, -0.7460696614945661};

Complex* FFT(Complex* f, int N);

void setup() {
    Serial.begin(9600);
}

Complex* GetData() {
    Complex* inputData = new Complex[N];

    unsigned long int start = micros();
    for (int i = 0; i < N; i++) {
        unsigned long int start = micros();

        inputData[i].real = ((analogRead(analogPin) * 5.00) / 1024.00) - 1.00;
        //inputData[i].real = SenoidData[i];
        inputData[i].imag = 0;

        delayMicroseconds(990 + start - micros());
    }

    unsigned long int ends = micros();

    Serial.print("Tempo de coleta do ciclo foi de ");
    Serial.print((ends - start)/1000.0);
    Serial.println("ms");

    return inputData;
}

void loop() {
    Complex* inputData = new Complex[N];
    inputData = GetData();

    Complex* f_FFT = new Complex[N];
    f_FFT = FFT(inputData, N);

    // Normaliza a FFT
    Transformada transf[N/2];
    for (int i = 0; i < N/2; i++) {
        transf[i].freq = i * round(1/period);
        transf[i].amplitute = sqrt(pow(f_FFT[i].real, 2) + pow(f_FFT[i].imag, 2)) / (N/2);
    }

    Serial.println("\tFreq [Hz]\tAmp [V]\t");
    Serial.println("::====================================::");
    for (int i = 0; i < N/2; i++) {
        Serial.print("\t ");
        Serial.print(transf[i].freq);
        Serial.print("\t   ||\t ");
        Serial.print(transf[i].amplitute);
        Serial.println("");
    }
    Serial.println("::====================================::\n\n");

    delete[] f_FFT;
    delete[] inputData;

    delay(500);
}

Complex* FFT(Complex* f, int N) {
    if (N <= 1) {
        return f;
    }

    Complex* even = new Complex[(N+1)/2];
    Complex even_aux[(N+1)/2];
    for (int i = 0; i < N; i += 2) {
        even_aux[i/2] = f[i];
    }
    even = FFT(even_aux,(N+1)/2);

    Complex* odd = new Complex[N/2];
    Complex odd_aux[N/2];
    for (int i = 1; i < N; i += 2) {
        odd_aux[(i-1)/2] = f[i];
    }
    odd = FFT(odd_aux,N/2);

    Complex w;
    for (int u = 0; u < N/2; u++) {       
        w.real = cos(-2 * M_PI * u / N);
        w.imag = sin(-2 * M_PI * u / N);

        f[u].real = even[u].real + w.real * odd[u].real - w.imag * odd[u].imag;
        f[u].imag = even[u].imag + w.real * odd[u].imag + w.imag * odd[u].real;

        f[u + N/2].real = even[u].real - w.real * odd[u].real + w.imag * odd[u].imag;
        f[u + N/2].imag = even[u].imag - w.real * odd[u].imag - w.imag * odd[u].real;
    }

    return f;
}
