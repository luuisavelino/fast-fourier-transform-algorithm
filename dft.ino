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

void DFT(Complex* dft_out, Complex* f, int N);
void GetData(float* inputData);
float** calculate_cos();
float** calculate_sin();

float** calculate_cos() {
    float** arr = new float*[N];
    for (int i = 0; i < N; ++i) {
        arr[i] = new float[N];
        for (int j = 0; j < N; ++j) {
            arr[i][j] = cos(2 * M_PI * i * j / N);
        }
    }
    return arr;
}
float** new_cos = calculate_cos();

float** calculate_sin() {
    float** arr = new float*[N];
    for (int i = 0; i < N; ++i) {
        arr[i] = new float[N];
        for (int j = 0; j < N; ++j) {
            arr[i][j] = sin(2 * M_PI * i * j / N);
        }
    }
    return arr;
}
float** new_sin = calculate_sin();

void setup() {
    Serial.begin(9600);
}

void loop() {
    expose_dft();
    delay(1000);
}

int contador = 0;

void GetData(float* inputData) {
    //inputData[contador] = (((analogRead(analogPin) * 5.00) / 1024.00) - 1.00) * 100;
    inputData[contador] = SenoidData[contador];
}

void expose_dft() {
    if (contador >= N) {
        contador = 0;
    }

    float* inputData = new float[N];
    GetData(inputData);

    Complex* f_DFT = new Complex[N];
    DFT(f_DFT, inputData, N);

    Serial.print(sqrt(pow(f_DFT[0].real, 2) + pow(f_DFT[0].imag, 2)) / (N/2));
    for (int i = 1; i < N/2; i++) {
        Serial.print(" ");
        Serial.print(sqrt(pow(f_DFT[i].real, 2) + pow(f_DFT[i].imag, 2)) / (N/2));
    }
    Serial.println(" ");

    delete[] f_DFT;
    delete[] inputData;

    contador++;
}

void DFT(Complex* dft_out, float* f, int N) {
    for (int k = 0; k < N; k++) {
        dft_out[k].real = 0;
        dft_out[k].imag = 0;

        for (int n = 0; n < N; n++) {
            dft_out[k].real += f[n] * new_cos[k][n];
            dft_out[k].imag += f[n] * -new_sin[k][n];
        }
    }
}
