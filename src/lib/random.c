/*
unsigned int mocha_pseudo_random(unsigned int start, unsigned int modulus)
{
	if ((modulus % 2) == 0) {
		start++;
	}
	unsigned int value = (2557 * start);
	return value % modulus;
}
*/

unsigned int mocha_pseudo_random(unsigned int start, unsigned int modulus)
{
	unsigned int value = (214013 * start + 2531011);
	value = (value >> 16) & 0x7FFF;
	return value % modulus;
}
