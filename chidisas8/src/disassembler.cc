/*
 * Copyright 2021 Alwin Leerling <dna.leerling@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#include "disassembler.h"

std::string format_register( unsigned int reg )
{
	std::ostringstream oss;
	oss << "V" << std::uppercase << std::hex << reg;
	return oss.str();
}

std::string format_byte( unsigned int byte )
{
	std::ostringstream oss;
	oss << "0x" << std::setfill( '0') << std::setw(2) << std::uppercase << std::hex << byte;
	return oss.str();
}

std::string format_address( unsigned int address )
{
	std::ostringstream oss;
	oss << "0x" << std::setfill( '0') << std::setw(3) << std::uppercase << std::hex << address;
	return oss.str();
}


/*
 * Input section
 */

void Disassembler::add_raw_byte( uint16_t address, uint8_t value )
{
	data_set.insert( RawData( address, value, false ) );
}

void Disassembler::read_binary( std::istream& is )
{
	uint16_t address = origin;
	uint8_t ch = is.get();

	while( is.good() ) {

		add_raw_byte( address, ch );
		++address;

		ch = is.get();
	}
}


/*
 * Processing section
 */
/*
 * I don't like this function here.
 * It has two side effects: It formats a label for a target and adds an item to the jump target set.
 * Also this function is not at the same semantic level as the other functions of this class
 */
std::string Disassembler::add_target( uint16_t source_address, uint16_t target_address, Target::eTargetKind type )
{
	std::set<Target>::iterator it = jmp_targets.find( Target( target_address ) );

	if( it != jmp_targets.end() )
		return (*it).get_label();

	// not in target list, create a new entry
	std::string label;

	switch( type ) {
	case Target::eTargetKind::I_TARGET: label = "DATA" + std::to_string( data_sequence++ ); break;
	case Target::eTargetKind::SUBROUTINE: label = "FUNC" + std::to_string( funct_sequence++ ); break;
	case Target::eTargetKind::JUMP: label = "LAB" + std::to_string( label_sequence++ ); break;
	case Target::eTargetKind::INDEXED: label = "TABLE" + std::to_string( table_sequence++ ); break;
	default: label = "Target Error"; break;
	}

	jmp_targets.insert( Target( target_address, type, label ) );

	return label;
}

void Disassembler::mark_as_instruction( uint16_t address )
{
	std::set<RawData>::iterator it = data_set.find( RawData( address ) );

	if( it == data_set.end() )
		return;		// not allowed to happen, should throw a big wobbly here

	RawData new_key = RawData( (*it).get_location(), (*it).val(), true );

	data_set.erase( it );
	data_set.insert( new_key );
}

Instruction Disassembler::decode_SYS( uint16_t address, uint16_t opcode )
{
	std::string mnemonic;

	switch( opcode & 0xFFF ) {
	/* opcodes 0000 .. 00DF not defined */
	case 0x0E0:							// CLS : clear screen
		mnemonic = "CLS";
		address_stack.push( address + 2 );
		break;
	/* opcodes 00E1 .. 00ED not defined */
	case 0x0EE:							// RET : return from subroutine
		mnemonic = "RET";
		break;
	/* opcodes 00EF .. 0FFF not defined */
	default:
		mnemonic = format_byte( opcode >> 8 ) + " " + format_byte( opcode & 0xFF );
		address_stack.push( address + 2 );
		break;
	}

	return Instruction( address, mnemonic, "" );
}

Instruction Disassembler::decode_JP( uint16_t address, uint16_t opcode )
{
	uint16_t jmp_target = opcode & 0xFFF;

	std::string label = add_target( address, jmp_target, Target::eTargetKind::JUMP );

	address_stack.push( jmp_target );

	return Instruction( address, "JP", label );
}


Instruction Disassembler::decode_CALL( uint16_t address, uint16_t opcode )
{
	uint16_t jmp_target = opcode & 0xFFF;

	std::string label = add_target( address, jmp_target, Target::eTargetKind::SUBROUTINE );

	address_stack.push( jmp_target );
	address_stack.push( address + 2 );

	return Instruction( address, "JSR", label );
}

Instruction Disassembler::decode_SEI( uint16_t address, uint16_t opcode )		//SE Vx, byte : Skip next instruction if Vx = kk.
{
	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int byte = (opcode & 0xFF);

	address_stack.push( address + 4 );
	address_stack.push( address + 2 );

	return Instruction( address, "SKPEQ", format_register( reg_x ) + ", " + format_byte( byte ) );
}

Instruction Disassembler::decode_SNI( uint16_t address, uint16_t opcode )
{
	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int byte = (opcode & 0xFF);

	address_stack.push( address + 4 );
	address_stack.push( address + 2 );

	return Instruction( address, "SKPNEQ", format_register( reg_x ) + ", " + format_byte( byte ) );
}

Instruction Disassembler::decode_SER( uint16_t address, uint16_t opcode )
{
	std::string mnemonic;
	std::string argument;

	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int reg_y = (opcode >> 4) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0 :
		mnemonic = "SKPEQ";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 4 );
		address_stack.push( address + 2 );
		break;
	/* opcodes 5xy1 .. 5xyF not defined */
	default:
		mnemonic = format_byte( opcode >> 8 ) + " " + format_byte( opcode & 0xFF );
		address_stack.push( address + 2 );
		break;
	}

	return Instruction( address, mnemonic, argument );
}

Instruction Disassembler::decode_LD( uint16_t address, uint16_t opcode )
{
	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int byte = (opcode & 0xFF);

	address_stack.push( address + 2 );

	return Instruction( address, "LD", format_register( reg_x ) + ", " + format_byte( byte ) );
}

Instruction Disassembler::decode_ADD( uint16_t address, uint16_t opcode )
{
	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int byte = (opcode & 0xFF);

	address_stack.push( address + 2 );

	return Instruction( address, "ADD", format_register( reg_x ) + ", " + format_byte( byte ) );
}

Instruction Disassembler::decode_MathOp( uint16_t address, uint16_t opcode )
{
	std::string mnemonic;
	std::string argument;

	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int reg_y = (opcode >> 4) & 0xF;

	switch( opcode & 0xF ) {
	case 0x0: 							// LD Vx, Vy : Set Vx = Vy.
		mnemonic = "LD";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	case 0x1:		// OR Vx, Vy : Set Vx = Vx OR Vy
		mnemonic = "OR";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	case 0x2:				// AND Vx, Vy : Set Vx = Vx AND Vy
		mnemonic = "AND";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	case 0x3:				// XOR Vx, Vy : Set Vx = Vx XOR Vy
		mnemonic = "XOR";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	case 0x4:				// ADD Vx, Vy : Set Vx = Vx + Vy, set VF = carry
		mnemonic = "ADD";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	case 0x5:				// SUB Vx, Vy : Set Vx = Vx - Vy, set VF = NOT borrow.
		mnemonic = "SUB";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	/*
	 * Note from Wikipedia:
	 *
	 * 	CHIP-8's opcodes 8XY6 and 8XYE (the bit shift instructions), which were in fact undocumented opcodes in the original interpreter,
	 *	shifted the value in the register VY and stored the result in VX. The CHIP-48 and SCHIP implementations instead ignored VY, and simply shifted VX
	 */
	case 0x6: // SHR Vx {, Vy} : Set Vx = Vx SHR 1.	Error in documentation! should be Vx = Vy SHR 1
		mnemonic = "SHR";
		argument = format_register( reg_x ) + " {, " + format_register( reg_y ) + "}";
		address_stack.push( address + 2 );
		break;
	case 0x7: 										// SUBN Vx, Vy : Set Vx = Vy - Vx, set VF = NOT borrow.
		mnemonic = "SUBN";
		argument = format_register( reg_x ) + ", " + format_register( reg_y );
		address_stack.push( address + 2 );
		break;
	/* opcodes 8xy8 .. 8xyD not defined */
	case 0xE:										// SHL Vx {, Vy} : Set Vx = Vx SHL 1. Error in documentation, should be Vx = Vy SHL 1
		mnemonic = "SHL";
		argument = format_register( reg_x ) + " {, " + format_register( reg_y ) + "}";
		address_stack.push( address + 2 );
		break;
	/* opcode 8xyF not defined */
	default:
		mnemonic = format_byte( opcode >> 8 ) + " " + format_byte( opcode & 0xFF );
		address_stack.push( address + 2 );
		break;
	}

	return Instruction( address, mnemonic, argument );
}

Instruction Disassembler::decode_SNE( uint16_t address, uint16_t opcode )
{
	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int reg_y = (opcode >> 4) & 0xF;

	address_stack.push( address + 4 );
	address_stack.push( address + 2 );

	return Instruction( address, "SKPNEQ", format_register( reg_x ) + ", " + format_register( reg_y ) );
}

Instruction Disassembler::decode_LDI( uint16_t address, uint16_t opcode )
{
	unsigned int load_address = opcode & 0xFFF;

	std::string label = add_target( address, load_address, Target::eTargetKind::I_TARGET );

	address_stack.push( address + 2 );

	return Instruction( address, "LDI", label );
}

Instruction Disassembler::decode_JMP( uint16_t address, uint16_t opcode )
{
	unsigned int target = (opcode >> 0) & 0xFFF;

	std::string label = add_target( address, target, Target::eTargetKind::INDEXED );
	//address_stack.push( target + V0 ); //this is a problem, we do not know the value of V0

	return Instruction( address, "JMP", std::string( "V0, ") + label );
}

Instruction Disassembler::decode_RND( uint16_t address, uint16_t opcode )
{
	unsigned int reg_x = (opcode >> 8) & 0xF;
	unsigned int byte  = (opcode >> 0) & 0xFF;

	address_stack.push( address + 2 );

	return Instruction( address, "RND", format_register( reg_x ) + ", " + format_byte( byte ) );
}

Instruction Disassembler::decode_DRW( uint16_t address, uint16_t opcode )
{
	unsigned int reg_x  = (opcode >> 8) & 0xF;
	unsigned int reg_y  = (opcode >> 4) & 0xF;
	unsigned int nibble = (opcode >> 0) & 0xF;

	address_stack.push( address + 2 );

	return Instruction( address, "DRW", format_register( reg_x ) + ", " + format_register( reg_y ) + ", " + std::to_string( nibble ) );
}

Instruction Disassembler::decode_Key( uint16_t address, uint16_t opcode )
{
	std::string mnemonic;
	std::string argument;
	unsigned int reg_x = (opcode >> 8) & 0xF;

	switch( opcode & 0xFF ) {
	/* opcodes Ex00 .. Ex9D not defined */
	case 0x9E:								// Ex9E - SKP Vx : Skip next instruction if key with the value of Vx is pressed
		mnemonic = "SKPK";
		argument = format_register( reg_x );
		address_stack.push( address + 4 );
		address_stack.push( address + 2 );
		break;
	/* opcodes Ex9F .. ExA0 not defined */
	case 0xA1:								// ExA1 - SKNP Vx : Skip next instruction if key with the value of Vx is not pressed.
		mnemonic = "SKPNK";
		argument = format_register( reg_x );
		address_stack.push( address + 4 );
		address_stack.push( address + 2 );
		break;
	/* opcodes ExA2 .. ExFF not defined */
	default:
		mnemonic = format_byte( opcode >> 8 ) + " " + format_byte( opcode & 0xFF );
		address_stack.push( address + 2 );
		break;
	}

	return Instruction( address, mnemonic, argument );
}

Instruction Disassembler::decode_Misc( uint16_t address, uint16_t opcode )
{
	std::string mnemonic;
	std::string argument;
	unsigned int reg_x = (opcode >> 8) & 0xF;

	switch( opcode & 0xFF ) {
	/* opcodes Fx00 .. Fx06 not defined */
	case 0x07:				// Fx07 - LD Vx, DT : Set Vx = delay timer value.
		mnemonic = "LD";
		argument = format_register( reg_x ) + ", DT";
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx08 .. Fx09 not defined */
	case 0x0A:												// Fx0A - LD Vx, K : Wait for a key press, store the value of the key in Vx. Stops execution
		mnemonic = "LD";
		argument = format_register( reg_x ) + ", K";
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx0B .. Fx14 not defined */
	case 0x15: 												// Fx15 - LD DT, Vx : Set delay timer = Vx.
		mnemonic = "LD";
		argument = std::string( "DT, " ) + format_register( reg_x );
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx16 and Fx17 not defined */
	case 0x18: 												// Fx18 - LD ST, Vx : Set sound timer = Vx.
		mnemonic = "LD";
		argument = std::string( "ST, " ) + format_register( reg_x );
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx19 .. Fx1D not defined */
	case 0x1E:												// Fx1E - ADD I, Vx : Set I = I + Vx
		mnemonic = "ADD";
		argument = std::string( "I, " ) + format_register( reg_x );
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx1F .. Fx28 not defined */
	case 0x29:												// Fx29 - LD F, Vx : Set I = location of sprite for digit Vx.
		mnemonic = "LD";
		argument = std::string( "I, CHR[" ) + format_register( reg_x ) + "]";
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx2A .. Fx32 not defined */
	case 0x33 : 											// Fx33 - LD B, Vx : Store BCD representation of Vx in memory locations I, I+1, and I+2
		mnemonic = "LD";
		argument = std::string( "I, BCD[" ) + format_register( reg_x ) + "]";
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx34 .. Fx54 not defined */
	case 0x55:												// Fx55 - LD [I], Vx : Store registers V0 through Vx in memory starting at location I
		mnemonic = "LD";
		argument = std::string( "[I], " ) + format_register( reg_x );
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx56 .. Fx64 not defined */
	case 0x65:												// Fx65 - LD Vx, [I] : Read registers V0 through Vx from memory starting at location I
		mnemonic = "LD";
		argument = format_register( reg_x ) + ", [I]";
		address_stack.push( address + 2 );
		break;
	/* opcodes Fx66 .. FxFF not defined */
	default:
		mnemonic = format_byte( opcode >> 8 ) + " " + format_byte( opcode & 0xFF );
		address_stack.push( address + 2 );
		break;
	}

	return Instruction( address, mnemonic, argument );
}

void Disassembler::disassemble_instruction( uint16_t address )
{
	if( instructions.find( Instruction( address ) ) != instructions.end() )
		return;	// the instruction has already been pushed so all is good

	// find this address (and the next one) in our raw data set
	std::set<RawData>::iterator first_byte = data_set.find( RawData(address) );
	std::set<RawData>::iterator second_byte = data_set.find( RawData(address + 1) );

	if( (first_byte == data_set.end()) || (second_byte == data_set.end()) ) {
		// not in raw bytes and not in instruction set. Worthy of an exception
		return;
	}

	// Create instruction here, dissemble and add to the instruction set.
	unsigned int opcode = ((*first_byte).val() << 8) | (*second_byte).val();

	switch( opcode >> 12 ) {
	case 0x0: instructions.insert( decode_SYS( address, opcode ) ); break;
	case 0x1: instructions.insert( decode_JP( address, opcode ) ); break;
	case 0x2: instructions.insert( decode_CALL( address, opcode ) ); break;
	case 0x3: instructions.insert( decode_SEI( address, opcode ) ); break;
	case 0x4: instructions.insert( decode_SNI( address, opcode ) ); break;
	case 0x5: instructions.insert( decode_SER( address, opcode ) ); break;
	case 0x6: instructions.insert( decode_LD( address, opcode ) ); break;
	case 0x7: instructions.insert( decode_ADD( address, opcode ) ); break;
	case 0x8: instructions.insert( decode_MathOp( address, opcode ) ); break;
	case 0x9: instructions.insert( decode_SNE( address, opcode ) ); break;
	case 0xA: instructions.insert( decode_LDI( address, opcode ) ); break;
	case 0xB: instructions.insert( decode_JMP( address, opcode ) ); break;
	case 0xC: instructions.insert( decode_RND( address, opcode ) ); break;
	case 0xD: instructions.insert( decode_DRW( address, opcode ) ); break;
	case 0xE: instructions.insert( decode_Key( address, opcode ) ); break;
	case 0xF: instructions.insert( decode_Misc( address, opcode ) ); break;
	}

	//  Remove from raw bytes
	mark_as_instruction( address );
	mark_as_instruction( address + 1 );
}

void Disassembler::disassemble( )
{
	address_stack.push( origin );		// push the start address

	while( !address_stack.empty() ) {
		uint16_t address = address_stack.top();
		address_stack.pop();

		disassemble_instruction( address ); /* this call can push between 0 and 2 new addresses on the address stack */
	}

	// The raw bytes that have not been marked are thus data bytes
	// Chuck them together into DataBytes objects

	std::vector<uint8_t> datarun;
	uint16_t address = origin;

	for( std::set<RawData>::iterator it = data_set.begin(); it != data_set.end(); ++it ) {

		if( ! (*it).instruction() ) {
			// if we are not collecting data, start now
			if( datarun.empty() )
				address = (*it).get_location();
			// if the location has a data label associated with it, we need to break the run
			else if( jmp_targets.find( Target( (*it).get_location() ) ) != jmp_targets.end() ) {
				databytes.insert( DataBytes( address, datarun ) );
				datarun.clear();
				address = (*it).get_location();
			}

			datarun.push_back( (*it).val() );

			// collect a maximum of 16 bytes
			if( datarun.size() == 16 ) {
				databytes.insert( DataBytes( address, datarun ) );
				datarun.clear();
			}

		} else {
			// this byte belongs to an instruction. End the data run if we are collecting and skip this byte
			if( !datarun.empty() ) {
				databytes.insert( DataBytes( address, datarun ) );
				datarun.clear();
			}
		}

	}
}


/*
 * Output section
 */
void Disassembler::write_listing( std::ostream& os ) const
{
    configure_stream( os );
    write_header( os );

	std::set<Instruction>::iterator instruction_iter = instructions.begin();
	std::set<DataBytes>::iterator datarun_iter = databytes.begin();

	while( instruction_iter != instructions.end() && datarun_iter != databytes.end() ) {

		if( (*instruction_iter).get_address() < (*datarun_iter).get_address() ) {
			output_instruction( os, *instruction_iter );
			++instruction_iter;
		} else {
			output_datarun( os, *datarun_iter );
			++datarun_iter;
		}
	}

	// what have we remaining
	while( instruction_iter != instructions.end() ) {
		output_instruction( os, *instruction_iter );
		++instruction_iter;
	}

	while( datarun_iter != databytes.end() ) {
		output_datarun( os, *datarun_iter );
		++datarun_iter;
	}
}

void Disassembler::configure_stream( std::ostream& os ) const
{
	os << std::setfill( '0');
	os << std::setw( 3 );
	os << std::hex << std::uppercase;
}

void Disassembler::write_header( std::ostream& os ) const
{
	os << "; Disasembly of " << bin_name << "\n";
	os << "; Generated by chidisas8\n";
	os << ";\n\n";
	os << "\t.ORG " << format_address(origin) << "\n\n";
}

void Disassembler::output_instruction( std::ostream& os, Instruction inst ) const
{
	std::set<Target>::iterator target_iter;

	if( label_present( target_iter, inst.get_address()) )
		(*target_iter).print( os );

	inst.print( os );
}

void Disassembler::output_datarun( std::ostream& os, DataBytes datarun ) const
{
	std::set<Target>::iterator target_iter;

	if( label_present( target_iter, datarun.get_address()) )
		(*target_iter).print( os );

	datarun.print( os );
}

bool Disassembler::label_present( std::set<Target>::iterator& it, uint16_t address ) const
{
	it = jmp_targets.find( Target( address ) );

	return it != jmp_targets.end();
}



void Target::print( std::ostream& os ) const
{
	if( type == eTargetKind::SUBROUTINE )
		os << '\n';

	os << '\t' << label << ":\n";
}

void Instruction::print( std::ostream& os ) const
{
	os << address << "\t\t" << mnemonic << "\t" << argument << "\n";
}

void DataBytes::print( std::ostream& os ) const
{
	os << address << "\t\t" << ".DB\t";

	for( uint8_t byte : byte_run )
		os << format_byte( byte ) << " ";

	os << '\n';
}
