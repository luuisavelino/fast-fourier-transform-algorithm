#include <math.h>

// Complex number struct
typedef struct {
    float real;
    float imag;
} Complex;

// Global variables
int counter = 0;               // Counter for the dft
int analogPin = A0;             // Analog pin for the input
hw_timer_t * timer = NULL;      // Timer for the interrupt
float** new_cos;                // Cosine matrix
float** new_sin;                // Sine matrix

// Variables for the dft
int sr = 960;                       // Sample Rate -> 960Hz
int fundamental = 60;               // Fundamental Frequency -> 60Hz
int N = round(sr / fundamental);    // Number of samples -> 16

// Function prototypes
void dft(Complex* dft_out, Complex* f, int N);  // DFT function
void getData(float* inputData);                 // Get data from the analog pin
void exposeDft();                               // Expose the dft
float** calculateCos();                         // Calculate the cosine matrix
float** calculateSin();                         // Calculate the sine matrix

// Mock data
float SenoidData[] = {0.0, 1.1043736580540338, 2.0536413177860657, 2.7144811573980587, 2.9940801852848145, 2.853169548885461, 2.311539728327368, 1.4452610223051467, 0.3759997006929136, -0.746069661494565, -1.7633557568774192, -2.5329837765060446, -2.946861752186066, -2.946861752186066, -2.532983776506047, -1.763355756877422, -0.7460696614945661};

// Get data from the analog pin
void getData(float* inputData) {
    //inputData[counter] = (((analogRead(analogPin) * 3.00) / 1024.00) - 1.00) * 100;      // Read the analog pin and convert to voltage
    inputData[counter] = SenoidData[counter];                                             // Mock read the analog pin and convert to voltage
}

// Calculate the cosine matrix
float** calculateCos() {
    float** arr = new float*[N];
    for (int i = 0; i < N; ++i) {
        arr[i] = new float[N];
        for (int j = 0; j < N; ++j) {
            arr[i][j] = cos(2 * M_PI * i * j / N);
        }
    }
    return arr;
}

// Calculate the sine matrix
float** calculateSin() {
    float** arr = new float*[N];
    for (int i = 0; i < N; ++i) {
        arr[i] = new float[N];
        for (int j = 0; j < N; ++j) {
            arr[i][j] = sin(2 * M_PI * i * j / N);
        }
    }
    return arr;
}

// Expose the DFT
void exposeDft() {
    if (counter >= N) {
        counter = 0;
    }

    float* inputData = new float[N];
    getData(inputData);

    Complex* f_DFT = new Complex[N];
    dft(f_DFT, inputData, N);

    Serial.print(sqrt(pow(f_DFT[0].real, 2) + pow(f_DFT[0].imag, 2)) / (N/2));
    for (int i = 1; i < N/2; i++) {
        Serial.print(" ");
        Serial.print(sqrt(pow(f_DFT[i].real, 2) + pow(f_DFT[i].imag, 2)) / (N/2));
    }
    Serial.println(" ");

    delete[] f_DFT;
    delete[] inputData;

    counter++;
}

// DFT function
void dft(Complex* dft_out, float* f, int N) {
    for (int k = 0; k < N; k++) {
        dft_out[k].real = 0;
        dft_out[k].imag = 0;

        for (int n = 0; n < N; n++) {
            dft_out[k].real += f[n] * new_cos[k][n];
            dft_out[k].imag += f[n] * -new_sin[k][n];
        }
    }
}

// Setup function
void setup() {
    Serial.begin(9600);         // Initialize serial communication
    pinMode(A0, INPUT);         // Set the analog pin as input

    new_cos = calculateCos();   // Calculate the cosine matrix
    new_sin = calculateSin();   // Calculate the sine matrix

    // Configuração do Timer Interrupt
    timer = timerBegin(0, 80, true);                // Initialize timerBegin() function; timer 0, prescalar: 80, UP counting
    timerAttachInterrupt(timer, &exposeDft, true);  // Attach Interrupt exposeDft() function to our timer
    timerAlarmWrite(timer, 100000, true);          // Define the match timer value -> 100000us (100ms)
    timerAlarmEnable(timer);                        // Enable Timer with interrupt (Alarm Enable)
}

// Loop function
void loop() {
    delay(1000);
}
