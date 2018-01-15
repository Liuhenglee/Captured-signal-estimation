/* AUDIO-3.C : from LAB3 of 6437 System Integration Workshop. Mar'08 T.Cartier
   - This program demonstrates a full I-P-O in a single HWI
   - Stereo audio data is collected in an interlaced format
   - A 'ripple buffer' is used to manage the delay line
*/

// ========= INCLUSIONS =======================================================
#include	"myWorkcfg.h"						// FIRST: config file header
#include	"codec.h"							// Serial port code access
#include	"coeffs.h"							// Fir filter coeff access
#include	"dipMonitor.h"
// Necessary library file
#include 	<stdio.h>
#include	<time.h>
#include	"math.h"
#include	"stdlib.h"
// DSP/BIOS library file
#include 	<evmdm6437.h>						// Allows access to EVM init fxn
#include 	<std.h>
#include 	<log.h>
#include    <swi.h>

// ========= DEFINITIONS ======================================================
#define		FIRSZ	64							// 通道缓存
#define		Samplerate		44100				// 采样频率 Hz
#define		Npoint	1024						// 采样点数
#define		PI 		3.14159265358979323846		// 圆周率
#define		cor		0.835						// 频谱泄露矫正系数
#define		trans	7695						// 级数转换系数

typedef struct{
	float real; float imag;
} complex;										// Complex structure definition

// ========= PROTOTYPES =======================================================
void isrAudio(void);							// Parameters calculation
void estimate(void);							// Serial Port Interrupt Service
void ans(float phi, float freq, float max, float maxx);

complex cexp(complex a);						//
float short2float(short input);					// Fixed point to floating point

void fft(complex x[], int n, int isign);		// Fourier Transform
void fir(short data[ ], short coeff[ ], short result[ ], int firsz, int bufsz);

// ========= GLOBALS ==========================================================
short 	sw0 = 0;								// Controls run/bypass of FIRs
short	sw1 = 0;								// Low pass vs high pass filter

short 	counter=0;								// Sampling point count
short 	buf[2*FIRSZ];							// Interlaced stereo data buffr
short  	buffer[Npoint];							// Array of sample values cache
float  	buffer2[Npoint];

float 	A[Npoint];                     			// An array of range values
float 	phi=0, freq=0, max=0;             		// 相位，频率，幅度
float 	a=0, b=0;								// 反正切参数
float 	maxx=0;									// 时域最大值
complex s[Npoint];              				// 复数结构

// ========= EXTERNALS ========================================================
extern	cregister volatile unsigned int	CSR;	// Control status register
extern	cregister volatile unsigned int	ICR;	// Interrupt clear register
extern	cregister volatile unsigned int	IER;	// Interrupt enable reg.

// ========= MAIN - init and return to begin BIOS Scheduler environment...=====
void main(void){								// Main: setup prior to BIOS
	EVMDM6437_init();							// Init EVM6437 HW
	LOG_enable(&trace);							// Start log
	LOG_printf(&trace,"EVMDM6437 start!\n");	// Main function start

	initCodec();								// Init McBSP1; s/u AIC via I2C
	initDipSwitches(); 
	
	short i;									// Loop counter
	for (i = 0; i < 2*Npoint; i++){ 			// For size of stereo buffer
		buf[i] = 0;								// Clear delay line
		buffer[i] = 0;							// Clear channel data buffer
		buffer2[i] = 0;							// Clear float parameters buffer
		A[i] = 0;								// Clear range values
	}

	ICR  = 0x10;								// Clear INT4 (precaution)
	IER |= 0x10;								// Enable INT4 as CPU interrupt
    MCBSP1_SPCR = 0x00010001;					// Start McBSP
}

//  ===== isrAudio() - Serial Port Interrupt Service Routine: BIOS HWI ========
void isrAudio(void){							// Get, process, send data: IPO
	static	short	i;							// Loop counter
	static	int 	dataIn, dataOut;			// i/f to MCBSP read/write
	static	short	dataOutL, dataOutR;			// FIR results of L & R chans

	dataIn = MCBSP1_DRR_32BIT; 					// get one stereo sample

	buf[0] = (short)dataIn;						// add new L data to delay line
	buf[1] = (short)(dataIn >> 16);				// add new R data to delay line
	//buffer1[counter]=(short)dataIn;			// collect another channel
	buffer[counter++] = (short)(dataIn >> 16);	// add new audio data to buffer
	buffer2[counter-1] = short2float(buffer[counter-1]);

	for (i = FIRSZ-2; i >= 0; i--)  			// for 2*(#coeffs-1) 
		buf[i+2] = buf[i];						// move all data down 1 pair

	if( sw0 == 1 ){ 							// if SW0 is down...
		fir(&buf[0], &coeffs[sw1][0], &dataOutL, FIRSZ, 1); // FIR: 1 sample L
		fir(&buf[1], &coeffs[sw1][0], &dataOutR, FIRSZ, 1); // FIR: 1 sample R

		dataOut  = 0x0000FFFF & dataOutL;		// left  result is 16 LSBs
		dataOut |= 0xFFFF0000 & (dataOutR<<16);	// right result is 16 MSBs
	}
	else 										// if filtering is 'off'...
		dataOut = dataIn;						// new input copied to output

    MCBSP1_DXR_32BIT = dataOut;					// send 1 stereo value to codec
	
    if(counter==Npoint){
 	   //SWI_post(&estimate_SWI);				// Switch to software interrupt
 	   //LOG_printf(&trace,"Post to SWI.\n");	// Set up a sign to SWI
 	   estimate();
    }
}

//  ===== estimate() - Parameter estimation and preparation work: BIOS SWI ====
void estimate(void){
	static short k = 0;                      	// The Max range value to the point
	static short j = 0;							// Cycle counter

	// Store floating-point values to complex structures
	for(j = 0; j < Npoint; j++){
		buffer2[j]=short2float(buffer[j]);
		s[j].real=buffer2[j];					// Assignment to the real part
		s[j].imag=0;							// The imaginary part is zero
		buffer[j]=0;							// clear data buffer
	}

	// Convert to frequency domain analysis
	fft(s, Npoint, -1);							// FFT calculation
	max=0; k=0; maxx=0;							// Parameter initialization
	static short q=0;							// Cycle counter
	for (q = 0; q < Npoint; q++){				// Gets the spectral value
		A[q] = sqrt(s[q].real*s[q].real + s[q].imag*s[q].imag);
		if (A[q] > max) {
			max = A[q];							// Look for maximum spectrum
			k = q;								// Find the corresponding point
		}
	}
	max = max * 2 / Npoint /cor;				// Convert to maximum range

	// Direct search for the maximum time domain
	for(q = 0; q < Npoint; q++){
		if(fabs(buffer2[q])>maxx) {
			maxx=fabs(buffer2[q]);
		}
	}

	// Frequency calculation using correction parameters
	float o=0;
	if (s[k].real > s[k].imag)
		o = (s[k + 1].real - s[k - 1].real) / (2 * s[k].real - s[k - 1].real - s[k + 1].real);
	else
		o = (s[k + 1].imag - s[k - 1].imag) / (2 * s[k].imag - s[k - 1].imag - s[k + 1].imag);
	if(2*k>Npoint){k = Npoint - k; o = fabs(o);}// Relative orthodontic correction
	freq = (k - o)*Samplerate / Npoint;			// Sinusoidal frequency

	// Using accurate frequency value to calculate phase
	for (q = 0; q < Npoint; q++) {
		a = a + buffer2[q] * cos(2 * PI*freq*q / Samplerate);
		b = b + buffer2[q] * sin(2 * PI*freq*q / Samplerate);
	}
	phi = atan(a / b);							// Calculat initial phase

	ans(phi, freq, max, maxx);					// Show results
	counter=0;									// Restart the sampling process
}

void ans(float phi, float freq, float max, float maxx){
	LOG_printf(&trace,"Phase=%f。\n",phi);		// Phase print
	LOG_printf(&trace,"Frequency=%f。\n",freq);	// Frequency print
	LOG_printf(&trace,"Range=%f。\n",max);		// Range print
	LOG_printf(&trace,"Maximum=%f。\n",maxx);	// Maximum value print
}

//  ===== short2float() - Get the corresponded floating point value  ==========
float short2float(short input){
	float out=0;
	out=((float)input)/trans;
	return out;
}

//  ===== cexp() - Complex exponential operation ==============================
complex cexp(complex a){
	complex z;
	z.real = exp(a.real)*cos(a.imag);
	z.imag = exp(a.real)*sin(a.imag);
	return(z);
}

//  ===== fft() - Fourier transform computations ==============================
void fft(complex x[], int n, int isign) {
	complex t, z, ce;
	int nn;
	z.real = 0.0;

	short r;
	for (r = 1; r <= 16; r++) {
		nn = (int)pow(2, r);					// Power operation of 2
		if (n == nn) break;						//
	}
	if (r>16) {
		//printf("?N?is?not?a?power?of?2?!?\n");
		return;
	}

	double pisign;
	pisign = 4 * isign*atan(1.);

	int l = 0;
	int m = 1;
	int mr = 0;
	for (m = 1; m<n; m++) {
		l = n;
		while (mr + l >= n) l = l / 2;
		mr = mr % l + l;
		if (mr <= m) continue;
		t.real = x[m].real; t.imag = x[m].imag;
		x[m].real = x[mr].real; x[m].imag = x[mr].imag;
		x[mr].real = t.real; x[mr].imag = t.imag;
	}

	int q = 0;
	int h = 0;
	l = 1;
	while (1) {
		if (l >= n) {
			if (isign == -1) return;
			for (q = 0; q<n; q++) {
				x[q].real = x[q].real / n;
				x[q].imag = x[q].imag / n;
			}
			return;
		}

		for (m = 0; m<l; m++) {
			for (h = m; h<n; h = h + 2 * l) {
				z.imag = m * pisign / l;
				ce = cexp(z);
				t.real = x[h + l].real*ce.real - x[h + l].imag*ce.imag;
				t.imag = x[h + l].real*ce.imag + x[h + l].imag*ce.real;
				x[h + l].real = x[h].real - t.real;
				x[h + l].imag = x[h].imag - t.imag;
				x[h].real = x[h].real + t.real;
				x[h].imag = x[h].imag + t.imag;
			}
		}
		l = 2 * l;
	}
}

