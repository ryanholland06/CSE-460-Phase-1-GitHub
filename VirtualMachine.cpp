#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>

#include "VirtualMachine.h"

using namespace std;

int VirtualMachine::get_clock()
{
	return clock;
}

void VirtualMachine::run(fstream& objectCode, fstream& in, fstream& out)
{
	const int debug = false;
	int opcode, rd, i, rs, constant, addr, j;

	base = 0;
	for (limit = 0; objectCode >> mem[limit]; limit++);

	sr = 2;
	sp = msize;
	pc = 0;
	while (pc < limit) {
		ir = mem[pc];
		pc++;
		opcode = (ir & 0xf800) >> 11;
		rd = (ir & 0x600) >> 9;
		i = (ir & 0x100) >> 8;
		rs = (ir & 0xc0) >> 6;
		addr = ir & 0xff;
		constant = addr;
		if (ir & 0x80) constant |= 0xffffff00; // if neg sign extend 

		clock++;

		if (opcode == 0) { /* load loadi */
			if (i) r[rd] = constant;
			else {
				r[rd] = mem[addr];
				clock += 3;
			}
		}
		else if (opcode == 1) { /* store */
			mem[addr] = r[rd];
			clock += 3;
		}
		else if (opcode == 2) { /* add addi  addc*/
			if (i)
			{
				r[rd] = r[rd] + constant;
				clock += 1;
			}
			else
			{
				r[rd] = r[rd] + r[rs];
				clock += 1;
			}
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
		}
		else if (opcode == 3) { /* addc addci*/
			if (i)
			{
				r[rd] = r[rd] + constant;
				clock += 1;
			}
			else
				r[rd] = r[rd] + r[rs];
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
			//add carry
			r[rd] += sr;
		}
		else if (opcode == 4) { /* sub subi*/
			if (i)
			{
				r[rd] = r[rd] - constant;
				clock += 1;
			}
			else
			{
				r[rd] = r[rd] - r[rs];
				clock += 1;
			}
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
		}
		else if (opcode == 5) { /* subc subci*/
			if (i)
			{
				r[rd] = r[rd] - constant;
				clock += 1;
			}
			else
			{
				r[rd] = r[rd] - r[rs];
				clock += 1;
			}
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// subtract carry
			r[rd] -= sr;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
		}
		else if (opcode == 6) { /* and andi*/
			if (i)
			{
				r[rd] = r[rd] & constant;
				clock += 1;
			}
			else
			{
				r[rd] = r[rd] & r[rs];
				clock += 1;
			}
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
		}
		else if (opcode == 7) { /* xor xori*/
			if (i)
			{
				r[rd] = r[rd] ^ constant;
				clock += 1;
			}
			else
			{
				r[rd] = r[rd] ^ r[rs];
				clock += 1;
			}
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
		}
		else if (opcode == 8) { /* compl*/
			r[rd] = ~r[rd];
			clock += 1;
		}
		else if (opcode == 9) { /* shl*/
			r[rd] = r[rd] << 1;
			// set CARRY
			if (r[rd] & 0x10000) sr |= 01;
			else sr &= 0xe;
			// sign extend
			if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
			else r[rd] &= 0xffff;
		}
		else if (opcode == 10) { /* shla*/
			r[rd] = r[rd] << 1;
			clock += 1;
		// set CARRY
		if (r[rd] & 0x10000) sr |= 01;
		else sr &= 0xe;
		// sign extend
		if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
		else r[rd] &= 0xffff;
		}
		else if (opcode == 11) { /* shr*/
		r[rd] = r[rd] >> 1;
		// set CARRY
		if (r[rd] & 0x10000) sr |= 01;
		else sr &= 0xe;
		// sign extend
		if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
		else r[rd] &= 0xffff;
		}

		else if (opcode == 12) { /* shra*/
		r[rd] = r[rd] >> 1;
		clock += 1;
		// set CARRY
		if (r[rd] & 0x10000) sr |= 01;
		else sr &= 0xe;
		// sign extend
		if (r[rd] & 0x8000) r[rd] |= 0xffff0000;
		else r[rd] &= 0xffff;
		}
		else if (opcode = 13) {
        		if (i) {
        			if (r[rd] < constant){			// if constant or r[rs] are <, =, or > then
        				sr ^= (-1 ^ sr) & (1UL << 3);	// bit 1 gets shifted to its apropriate place
        				sr ^= (-0 ^ sr) & (1UL << 2);	// and 0's are shifted  to reset either less,
        				sr ^= (-0 ^ sr) & (1UL << 1);	// equal, or greater than bits.
        			}
        			else if (r[rd] == constant){
        				sr ^= (-0 ^ sr) & (1UL << 3);
        				sr ^= (-1 ^ sr) & (1UL << 2);
        				sr ^= (-0 ^ sr) & (1UL << 1);
        			}
        			else if (r[rd] > constant){
        				sr ^= (-0 ^ sr) & (1UL << 3);
        				sr ^= (-0 ^ sr) & (1UL << 2);
        				sr ^= (-1 ^ sr) & (1UL << 1);
        			}
        		}
        		else {
        			if (r[rd] < r[rs]){			// if constant or r[rs] are <, =, or > then
        				sr ^= (-1 ^ sr) & (1UL << 3);	// bit 1 gets shifted to its apropriate place
        				sr ^= (-0 ^ sr) & (1UL << 2);	// and 0's are shifted  to reset either less,
        				sr ^= (-0 ^ sr) & (1UL << 1);	// equal, or greater than bits.
        			}
        			else if (r[rd] == r[rs]){
        				sr ^= (-0 ^ sr) & (1UL << 3);
        				sr ^= (-1 ^ sr) & (1UL << 2);
        				sr ^= (-0 ^ sr) & (1UL << 1);
        			}
        			else if (r[rd] > r[rs]){
        				sr ^= (-0 ^ sr) & (1UL << 3);
        				sr ^= (-0 ^ sr) & (1UL << 2);
        				sr ^= (-1 ^ sr) & (1UL << 1);
        			}
        		}
        	}
		else if (opcode == 14) {
        		r[rd] = sr;
        	}
       		 else if (opcode == 15) {
        		sr = r[rd];
       		}
       	 	else if (opcode == 16) {
        		pc = addr;
        	}
        	else if (opcode == 17) {
        		int LESS = (sr >> 3) & 1U;		// how do I keep the value of pc for the return?
        		if (LESS == 1){
				pc = addr;
        		}
        	}
        	else if (opcode == 18) {
        		int EQUAL = (sr >> 2) & 1U;
        		if (EQUAL == 1)
				pc = addr;
        	}
        	else if (opcode == 19) {
        		int GREATER = (sr >> 1) & 1U;
        		if (GREATER == 1)
				pc = addr;
       		 }
        	else if (opcode == 20) {
        		temp = pc;
        		pc = addr; 	
        	}
        	else if (opcode == 21) {
			pc = temp;
        	}
        	else if (opcode == 22) {
            		fstream inn;
            		inn.open ("read.in", fstream::in);
            
            		inn >> r[rd];
            
            		inn.close();
        	}
        	else if (opcode == 23) {
        		fstream facts;
        		facts.open ("write.out", fstream::out);
        	
        		facts << "r[" << rd << "] = " << r[rd];
        	
        		facts.close();
        	}
        	else if (opcode == 24) {
        	exit(0);
        	}
        	else if (opcode == 25) {	// empty braces mean do nothing, (noop) does nothing.
        	
        	}
		else {
			cout << "Bad opcode = " << opcode << endl;
			exit(3);
		}
		if (debug) {
			printf("ir=%d op=%d rd=%d i=%d rs=%d const=%d addr=%d\n", ir, opcode, rd, i, rs, constant, addr);
			printf("r[0]=%d r[1]=%d r[2]=%d r[3]=%d pc=%d sr=%d sp=%d clock=%d\n\n", r[0], r[1], r[2], r[3], pc, sr, sp, clock);
			//char c;
			//cin>>c;
		}

	}
	if (debug) {
		for (j = 0; j < limit; j++) {
			printf("%8d", mem[j]);
			if ((j % 8) == 7) printf("\n");
		}
		cout << endl;
		int x;
		cin >> x;
	}
} // run
