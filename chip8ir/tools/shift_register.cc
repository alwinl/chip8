#include <iostream>
#include <cstdint>
#include <bitset>


void prng()
{
	const std::bitset<8> start_state = 0b0000;
	std::bitset<8> reg = start_state;
	unsigned int period = 0;

	do {

		std::bitset<1> bit = std::bitset<1>( ~(reg[7] ^ reg[5] ^ reg[4] ^ reg[3]) );

		reg <<= 1;
		reg[0] = bit[0];

		std::cout << "Current value: " << reg << std::endl;
		++period;

	} while( (reg != start_state) && (period <=500) );

	std::cout << "Period: " << period << std::endl;

}

int lfsr()
{
	const std::bitset<4> start_state = 0b0000;
	std::bitset<4> reg = start_state;
	unsigned int period = 0;

	do {

		const std::bitset<1> bit_part1 = std::bitset<1>( ~(reg[2] ^ reg[3]) );
		const std::bitset<1> bit_part2a = std::bitset<1>( reg[2] & reg[1] & reg[0] );
		const std::bitset<1> bit_part2b = std::bitset<1>( reg[3] & reg[2] & ~reg[1] & reg[0] );

		std::bitset<1> bit = bit_part1 ^ (bit_part2a | bit_part2b);

		reg <<= 1;
		reg[0] = bit[0];

		std::cout << "Current value: " << reg << std::endl;
		++period;

	} while( (reg != start_state) && (period <=20) );

	std::cout << "Period: " << period << std::endl;

    return 0;
}

int main()
{
	prng();
	return 0;
}
