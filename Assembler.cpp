// Assembler.cpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include "Assembler.h"

using namespace std;

Assembler::Assembler()
{
	jumpTable["load"] = &Assembler::load;
	jumpTable["loadi"] = &Assembler::loadi;
	jumpTable["store"] = &Assembler::store;
	jumpTable["add"] = &Assembler::add;
	jumpTable["addi"] = &Assembler::addi;
	jumpTable["addc"] = &Assembler::addc;
	jumpTable["addci"] = &Assembler::addci;
	jumpTable["sub"] = &Assembler::sub;
	jumpTable["subi"] = &Assembler::subi;
	jumpTable["subc"] = &Assembler::subc;
	jumpTable["subci"] = &Assembler::subci;
	jumpTable["and"] = &Assembler::_and;
	jumpTable["andi"] = &Assembler::andi;
	jumpTable["xor"] = &Assembler::_xor;
	jumpTable["xori"] = &Assembler::xori;
	jumpTable["compl"] = &Assembler::_compl;
	jumpTable["shl"] = &Assembler::shl;
	jumpTable["shla"] = &Assembler::shla;
	jumpTable["shr"] = &Assembler::shr;
	jumpTable["shra"] = &Assembler::shra;
	jumpTable["compr"] = &Assembler::compr;
	jumpTable["compri"] = &Assembler::compri;
	jumpTable["getstat"] = &Assembler::getstat;
	jumpTable["putstat"] = &Assembler::putstat;
	jumpTable["jump"] = &Assembler::jump;
	jumpTable["jumpl"] = &Assembler::jumpl;
	jumpTable["jumpe"] = &Assembler::jumpe;
	jumpTable["jumpg"] = &Assembler::jumpg;
	jumpTable["call"] = &Assembler::call;
	jumpTable["return"] = &Assembler::_return;
	jumpTable["read"] = &Assembler::read;
	jumpTable["write"] = &Assembler::write;
	jumpTable["halt"] = &Assembler::halt;
	jumpTable["noop"] = &Assembler::noop;

}	//Assembler

int Assembler::assemble(fstream& in, fstream& out)
{
	string line;
	string opcode;
	const int success = false;
	const int error = true;
	//const int debug = fasle;
	int instruction;

	getline(in, line);
	while (!in.eof()) {
		istringstream str(line.c_str());
		str >> opcode;
		if (opcode[0] == '!') {			// this is the part for commenting out code
			getline(in, line);			// MAYBE***
			continue;
		}

		try {
			if (not jumpTable[opcode])
				throw NullPointerException();
			else instruction = (this->*jumpTable[opcode])(str);

		} catch (NullPointerException e) {
			cerr << e.what() << endl;
			return error;
		}

		if (instruction == -1)
			return error;
		out << instruction << endl;
		getline(in, line);
	}
	return success;

}	//assemble

int Assembler::load(istringstream & str)
{
	int rd, addr;
	str >> rd >> addr;
	if (rd < 0 || rd > 3)
		return -1;
	if (addr < 0 || addr > 255)
		return -1;
	int inst = 0;
	inst = inst | rd<<9 | addr;
	return inst;
}

int Assembler::loadi(istringstream & str)
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 0;
	inst = inst | rd<<9 | 1<<8 | (0x000000ff & constant);
	return inst;
}

int Assembler::store(istringstream & str)
{
	int rd, addr;
	str >> rd >> addr;
	if (rd < 0 || rd > 3)
		return -1;
	if (addr < 0 ||addr > 255)
		return -1;
	int inst = 1;
	inst = inst << 11 | rd << 9 | addr;
	return inst;
}

int Assembler::add(istringstream & str)
{
	int rd, rs;
	str >> rd >> rs;
	if (rd < 0 || rd > 3)
		return -1;
	if (rs < 0 || rs > 3)
		return -1;
	int inst = 2;
	inst = inst << 11 | rd<<9 | rs<<6;
	return inst;
}

int Assembler::addi(istringstream & str)						// ask if done correctly?
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 2; 												
	inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);	// ask about how exactly it works?????
	return inst;
}

int Assembler::addc(istringstream & str)
{
	int rd, rs;						// how does the carry work?
	str >> rd >> rs;
	if (rd < 0 || rd > 3)
		return -1;
	if (rs < 0 || rs > 3)
		return -1;
	int inst = 3;
	inst = inst<<11 | rd<<9 | rs<<6;		//right now looks exactly like (add) need to worry about the carry
	return inst;
}

int Assembler::addci(istringstream & str)
{
	int rd, constant;
	str >> rd >> constant ;				// might have to involve carry in some way
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 3;
	inst = inst<<11 | rd<<9 | (0x000000ff & constant);	//might have to add a carry(maybe: carry << 0)
	return inst;
}

int Assembler::sub(istringstream & str)			// need to convert the value in rs into a negative value
{												// whould this be done in VM or in assembler?
	int rd, rs;								
	str >> rd >> rs;							// (>>)  shifts right and adds either 0s, if value is an unsigned type,
	if (rd < 0 || rd > 3)						// or extends the top bit (to preserve the sign) if its a signed type
		return -1;
	if (rs < 0 || rs > 3)						//The left shift and right shift operators should not be used for negative numbers. why?
		return -1;
	int inst = 4;								// how do you handle Negative operations?????
	inst = inst<<11 | rd<<9 | rs<<6;
	return inst;
}

int Assembler::subi(istringstream & str)						
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 4; 											
	inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);	// this seems like its meant to load, how do you sub?
	return inst;
}

int Assembler::subc(istringstream & str)
{
	int rd, rs;
	str >> rd >> rs;
	if (rd < 0 || rd > 3)
		return -1;
	if (rs < 0 || rs > 3)
		return -1;
	int inst = 5; 											
	inst = inst<<11 | rd<<9 | rs<<6; 
	return inst;
}

int Assembler::subci(istringstream & str)
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 5; 											
	inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
	return inst;
}

int Assembler::_and(istringstream & str)
{
	int rd, rs;
	str >> rd >> rs;
	if (rd < 0 || rd > 3)
		return -1;
	if (rs < 0 || rs > 3)
		return -1;
	int inst = 6;
	inst = inst<<11 | rd<<9 | rs<<6;
	return inst;
}

int Assembler::andi(istringstream & str)
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 6; 											
	inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
	return inst;
}

int Assembler::_xor(istringstream & str)
{
	int rd, rs;
	str >> rd >> rs;
	if (rd < 0 || rd > 3)
		return -1;
	if (rs < 0 || rs > 3)
		return -1;
	int inst = 7;
	inst = inst<<11 | rd<<9 | rs<<6;
	return inst;
}

int Assembler::xori(istringstream & str)
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 7; 											
	inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
	return inst;
}

int Assembler::_compl(istringstream & str)
{
	int rd;
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 8; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::shl(istringstream & str)
{
	int rd;				// only sets CARRY
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 9; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::shla(istringstream & str)
{
	int rd;			//same thing but this one both sets carry and sign extend
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 10; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::shr(istringstream & str)
{
	int rd;					// only sets CARRY
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 11; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::shra(istringstream & str)
{
	int rd;					//same thing but this one both sets carry and sign extend
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 12; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::compr(istringstream & str)
{
	int rd, rs;
	str >> rd >> rs;
	if (rd < 0 || rd > 3)
		return -1;
	if (rs < 0 || rs > 3)
		return -1;
	int inst = 13;
	inst = inst<<11 | rd<<9 | rs<<6;
	return inst;
}

int Assembler::compri(istringstream & str)
{
	int rd, constant;
	str >> rd >> constant;
	if (rd < 0 || rd > 3)
		return -1;
	if (constant < -128 || constant > 127)
		return -1;
	int inst = 13; 											
	inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
	return inst;
}

int Assembler::getstat(istringstream & str)
{
	int rd;
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 14; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::putstat(istringstream & str)
{
	int rd;
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 15; 											
	inst = inst<<11 | rd<<9;
	return inst;
}

int Assembler::jump(istringstream & str)
{
	int addr;
	str >> addr;
	if (addr < 0 ||addr > 255)
		return -1;
	int inst = 16;
	inst = inst<<11 | 1<<8 | addr;
	return inst;
}

int Assembler::jumpl(istringstream & str)
{
	int addr;
	str >> addr;
	if (addr < 0 ||addr > 255)
		return -1;
	int inst = 17;
	inst = inst<<11 | 1<<8 | addr;
	return inst;
}

int Assembler::jumpe(istringstream & str)
{
	int addr;
	str >> addr;
	if (addr < 0 ||addr > 255)
		return -1;
	int inst = 18;
	inst = inst<<11 | 1<<8 | addr;
	return inst;
}

int Assembler::jumpg(istringstream & str)
{
	int addr;
	str >> addr;
	if (addr < 0 ||addr > 255)
		return -1;
	int inst = 19;
	inst = inst<<11 | 1<<8 | addr;
	return inst;
}

int Assembler::call(istringstream & str)
{
	int addr;
	str >> addr;
	if (addr < 0 ||addr > 255)
		return -1;
	int inst = 20;
	inst = inst<<11 | addr;
	return inst;
}

int Assembler::_return(istringstream & str)
{
	int inst = 21;
	inst = inst<<11;
	return inst;
}

int Assembler::read(istringstream & str)
{
	int rd;
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 22;
	inst = inst<<11 | rd<<9; /* | 0<<5 */	
	return inst;		// (d) is that a dont care and is this done right
}

int Assembler::write(istringstream & str)
{
	int rd;
	str >> rd;
	if (rd < 0 || rd > 3)
		return -1;
	int inst = 23;
	inst = inst<<11 | rd<<9;	
	return inst;
}

int Assembler::halt(istringstream & str)
{
	int inst = 24;
	inst = inst<<11;	
	return inst;
}

int Assembler::noop(istringstream & str)
{
	int inst = 25;
	inst = inst<<11;
	return inst;
}
