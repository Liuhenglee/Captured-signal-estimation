/* AUDIO-3.C : from LAB3 of 6437 System Integration Workshop. Mar'08 T.Cartier
   - This program demonstrates a full I-P-O in a single HWI
   - Stereo audio data is collected in an interlaced format
   - A 'ripple buffer' is used to manage the delay line
*/

// ========= INCLUSIONS =======================================================
#include	"myWorkcfg.h"						// FIRST: config file header
#include	"codec.h"							// serial port code access
#include	"coeffs.h"							// fir filter coeff access
#include	"dipMonitor.h"

#include 	<stdio.h>
#include	"math.h"
#include	"stdlib.h"

// FFT���ͷ�ļ�
#include 	"c6x.h"
#include   	"DSP_fft16x16.h"
#include   	"DSP_q15tofl.h"
#include 	"gen_twiddle_fft16x16.h"

// BIOS���ͷ�ļ�
#include 	<evmdm6437.h>						// allows access to EVM init fxn
#include 	<std.h>
#include 	<log.h>
#include    <swi.h>

// ========= DEFINITIONS ======================================================
#define		FIRSZ	64							// ͨ������
#define		Samplerate		44100				// ����Ƶ�� Hz
#define		Npoint	1024						// ��������
#define		PI 		3.14159265358979323846		// Բ����

typedef struct{float real,image;} complex;

// ========= PROTOTYPES =======================================================
void isrAudio(void);							// Parameters calculation
void estimate(void);							// Serial Port Interrupt Service
void curvefitpara(void);
void fir(short data[ ], short coeff[ ], short result[ ], int firsz, int bufsz);

// ========= GLOBALS ==========================================================
short 	sw0 = 0;								// controls run/bypass of FIRs
short	sw1 = 0;								// low pass vs high pass filter

short 	counter=0;								// ��������
short 	buf[2*FIRSZ];							// interlaced stereo data buffr
short  	buffer[Npoint]={0};							// ��������
float  	buffer2[Npoint]={0};
short 	k=0;                      				//�����ȵ���

short	fftoutdata[2*Npoint];       			//FFT������
short 	w_16x16[2*Npoint];          			//��ת����
short 	fftindata[2*Npoint];        			//����ṹ(ʵ���鲿������,�ȷ�ʵ��)

float 	fftdata[2*Npoint];        				//FFT������
float 	A[Npoint];                     			//����ֵ����
float 	phi=0, freq=0, max=0;             		//��λ��Ƶ�ʣ�����
complex s[Npoint];              				//�����ṹ

// ========= EXTERNALS ========================================================
extern	cregister volatile unsigned int	CSR;	// control status register
extern	cregister volatile unsigned int	ICR;	// interrupt clear register
extern	cregister volatile unsigned int	IER;	// interrupt enable reg.

float short2float(short input)
{
	float out=0;
	//out=((float)input)/10000;
	out=(float)input;
	return out;
}

// ========= MAIN - init and return to begin BIOS Scheduler environment...=====
void main(void){								// main: setup prior to BIOS
	EVMDM6437_init();							// init EVM6437 HW
	LOG_enable(&trace);
	LOG_printf(&trace,"EVMDM6437 start!\n");	// Main function start

	initCodec();								// init McBSP1; s/u AIC via I2C
	initDipSwitches(); 
	
	short i;									// loop counter
	for (i = 0; i < 2*Npoint; i++){ 				// for size of stereo buffer
		buf[i] = 0;								// clear delay line
		buffer[i] = 0;							// clear data buffer
		buffer2[i]=0;
	}

	ICR  = 0x10;								// clear INT4 (precaution)
	IER |= 0x10;								// enable INT4 as CPU interrupt
    MCBSP1_SPCR = 0x00010001;					// start McBSP
}

//  ===== isrAudio() - Serial Port Interrupt Service Routine: BIOS HWI ========
void isrAudio(void){							// get, process, send data: IPO
	static	short	i;							// loop counter
	static	int 	dataIn, dataOut;			// i/f to MCBSP read/write
	static	short	dataOutL, dataOutR;			// FIR results of L & R chans

	dataIn = MCBSP1_DRR_32BIT; 					// get one stereo sample

	buf[0] = (short)dataIn;						// add new L data to delay line
	buf[1] = (short)(dataIn >> 16);				// add new R data to delay line
	//buffer1[counter]=(short)dataIn;			// collect another channel
	buffer[counter++] = (short)(dataIn >> 16);	// add new audio data to buffer

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
 	   SWI_post(&estimate_SWI);					// Switch to software interrupt
 	   LOG_printf(&trace,"Post to SWI.\n");		// Set up a sign to SWI
    }
}

//  ===== estimate() - parameters calculation algorithm: BIOS SWI =============
void estimate(void){
	static short j=0;
	for(j=0;j<1024;j++) {
		buffer2[j]=((float)((int)buffer[j]))/10000;
		LOG_printf(&trace,"value=%d��\n",buffer[j]);
	}

	curvefitpara();								// parameters calculation
	for ( ; counter >=0; counter--) 			// for size of stereo buffer
		buffer[counter] = 0;					// clear data buffer
}

void curvefitpara(void){
	static short q=0;
	for(q=0;q<Npoint;q++){          //�����ṹ��ʼ��
		s[q].real=0;
		s[q].image=0;
	}
	for(q=0;q<2*Npoint;q++){        //�������ݴ��
		fftindata[q]=buffer[q++];   //ʵ������
		fftindata[q]=0;             //�鲿����
	}

	static short p=0;
	for( p = 0; p < Npoint; p++ ){
		fftindata[2*p] = fftindata[2*p]>>7;                 //ѹ����������7bit����ֹFFT��������
	}
	
	gen_twiddle_fft16x16(w_16x16, Npoint);                  //��ת���ӵĲ���
	DSP_fft16x16(w_16x16, Npoint,fftindata, fftoutdata);    //FFT�任(��ת����,����,��������,�������)
	DSP_q15tofl(fftoutdata, fftdata, Npoint);               //���ת����

	//fft�任��ת�����ṹ
	static short r=0;
	for (q = 0, r = 0; r < Npoint; q+=2, r++) {
		s[r].real = fftdata[q];         //���ʵ��
		s[r].image = fftdata[q + 1];    //����鲿
	}

	static short findmax=0;
	for(findmax=0;findmax<Npoint;findmax++){
		AA[findmax]=sqrt(s[findmax].real*s[findmax].real+s[findmax].image*s[findmax].image);
		if(A[findmax]>max){
			k=findmax;
			max=A[findmax];
		}
	}
	max=max*2/Npoint;                           //���ȹ���ֵ

	float op;
	if(s[k].image<s[k].real){
		op=(s[k+1].real-s[k-1].real)/(2*s[k].real-s[k-1].real-s[k+1].real);
	}
	else op=(s[k+1].image-s[k-1].image)/(2*s[k].image-s[k-1].image-s[k+1].image);
	freq=(k-op)*Samplerate/Npoint;              //Ƶ�ʹ���ֵ

	for (i = 0; i < N; i++) {
		a = a + fftdata[2*i] * cos(2 * PI*f00*i / fs);
		b = b + fftdata[2*i] * sin(2 * PI*f00*i / fs);
	}
	phi = atan(a / b);							//��λ����ֵ

	//���ؽ��
	LOG_printf(&trace,"Phase=%f��\n",phi);
	LOG_printf(&trace,"Frequency=%f��\n",freq);
	LOG_printf(&trace,"Range=%f��\n",max);
}
