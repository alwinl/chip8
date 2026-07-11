#include <iostream>
#include <cstdint>
#include <bitset>

using namespace std;


void prng()
{
	bitset<8> start_state = 0b0000;
	bitset<8> reg = start_state;
	unsigned int period = 0;

	do {

		bitset<1> bit = bitset<1>( ~(reg[7] ^ reg[5] ^ reg[4] ^ reg[3]) );

		reg <<= 1;
		reg[0] = bit[0];

		cout << "Current value: " << reg << endl;
		++period;

	} while( (reg != start_state) && (period <=500) );

	cout << "Period: " << period << endl;

}

int lfsr()
{
	bitset<4> start_state = 0b0000;
	bitset<4> reg = start_state;
	unsigned int period = 0;

	do {

		bitset<1> bit_part1 = bitset<1>( ~(reg[2] ^ reg[3]) );
		bitset<1> bit_part2a = bitset<1>( reg[2] & reg[1] & reg[0] );
		bitset<1> bit_part2b = bitset<1>( reg[3] & reg[2] & ~reg[1] & reg[0] );

		bitset<1> bit = bit_part1 ^ (bit_part2a | bit_part2b);

		reg <<= 1;
		reg[0] = bit[0];

		cout << "Current value: " << reg << endl;
		++period;

	} while( (reg != start_state) && (period <=20) );

	cout << "Period: " << period << endl;

    return 0;
}

int main()
{
	prng();
	return 0;
}
