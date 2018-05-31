/*

MIT License

Copyright (c) 2013 Peter Bjorklund

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include "fast_atan2.h"

int fast_atan2(signed int x, signed int y) // these hold the XY vector at the start
{
	unsigned char negflag;
	unsigned char tempdegree;
	unsigned char comp;
	unsigned int degree; // this will hold the result
						 // (and they will be destroyed)
	unsigned int ux;
	unsigned int uy;

	// Save the sign flags then remove signs and get XY as unsigned ints
	negflag = 0;
	if (x < 0) {
		negflag += 0x01; // x flag bit
		x = (0 - x);	 // is now +
	}
	ux = x; // copy to unsigned var before multiply
	if (y < 0) {
		negflag += 0x02; // y flag bit
		y = (0 - y);	 // is now +
	}
	uy = y; // copy to unsigned var before multiply

	// 1. Calc the scaled "degrees"
	if (ux > uy) {
		degree = (uy * 45) / ux; // degree result will be 0-45 range
		negflag += 0x10;		 // octant flag bit
	} else {
		degree = (ux * 45) / uy; // degree result will be 0-45 range
	}

	// 2. Compensate for the 4 degree error curve
	comp = 0;
	tempdegree = degree; // use an unsigned char for speed!
	if (tempdegree > 22) // if top half of range
	{
		if (tempdegree <= 44)
			comp++;
		if (tempdegree <= 41)
			comp++;
		if (tempdegree <= 37)
			comp++;
		if (tempdegree <= 32)
			comp++; // max is 4 degrees compensated
	} else			// else is lower half of range
	{
		if (tempdegree >= 2)
			comp++;
		if (tempdegree >= 6)
			comp++;
		if (tempdegree >= 10)
			comp++;
		if (tempdegree >= 15)
			comp++; // max is 4 degrees compensated
	}
	degree += comp; // degree is now accurate to +/- 1 degree!

	// Invert degree if it was X>Y octant, makes 0-45 into 90-45
	if (negflag & 0x10)
		degree = (90 - degree);

	// 3. Degree is now 0-90 range for this quadrant,
	// need to invert it for whichever quadrant it was in
	if (negflag & 0x02) // if -Y
	{
		if (negflag & 0x01) // if -Y -X
			degree = (180 + degree);
		else // else is -Y +X
			degree = (180 - degree);
	} else // else is +Y
	{
		if (negflag & 0x01) // if +Y -X
			degree = (360 - degree);
	}

	return degree;
}
