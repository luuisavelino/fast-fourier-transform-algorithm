#include <math.h>

typedef struct {
    float real;
    float imag;
} Complex;

int analogPin = A0;
float period = 1.0 / 60;
int sr = 960;
float ts = 1.0 / sr;
int N = round(period * sr);
float SenoidData[] = {0.0, 1.1043736580540338, 2.0536413177860657, 2.7144811573980587, 2.9940801852848145, 2.853169548885461, 2.311539728327368, 1.4452610223051467, 0.3759997006929136, -0.746069661494565, -1.7633557568774192, -2.5329837765060446, -2.946861752186066, -2.946861752186066, -2.532983776506047, -1.763355756877422, -0.7460696614945661};

Complex* DFT(Complex* f, int N);
float* GetData();

void setup() {
    Serial.begin(9600);
}

float* GetData() {
    float* inputData = new float[N];
    unsigned long int start = micros();

    for (int i = 0; i < N; i++) {
        unsigned long int start = micros();

        //inputData[i] = (((analogRead(analogPin) * 5.00) / 1024.00) - 1.00) * 100;
        inputData[i] = SenoidData[i];
        delayMicroseconds(1030 + start - micros());
    }

    unsigned long int ends = micros();
    return inputData;
}

void loop() {
    float* inputData = new float[N];
    inputData = GetData();

    Complex* f_FFT = new Complex[N];

    unsigned long int start = micros();
    f_FFT = FFT(inputData, N);
    unsigned long int ends = micros();

    Serial.println(ends-start);


    // Serial.print(sqrt(pow(f_FFT[0].real, 2) + pow(f_FFT[0].imag, 2)) / (N/2));
    // for (int i = 1; i < N/2; i++) {
    //     Serial.print(" ");
    //     Serial.print(sqrt(pow(f_FFT[i].real, 2) + pow(f_FFT[i].imag, 2)) / (N/2));
    // }
    // Serial.println(" ");

    delete[] f_FFT;
    delete[] inputData;
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
