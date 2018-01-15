// Interlaced stereo FIR filter. For use in BIOS workshop. Feb'08, T.Cartier
// "2*" factor in data[] & r[] offsets to stride thru interlaced L&R data


void fir(short data[ ], short coeff[ ], short result[ ], int firsz, int bufsz)
{
	short	i, j; 							// loop counters
	int		sum;							// FIR filter accumulator - 32 bits
	
	for (j = 0; j < bufsz; j++) 			// for each sample received 
	{
		sum = 0;							// clear accumulator before each SoP
		for (i = 0; i < firsz; i++)			// for size of filter (# of coeffs)
			sum += data[2*(i+j)] * coeff[i];// perform sum of products (SoP)
		result[2*j] = (short)(sum >> 15);	// scale final result back to 16bits
	}
}


