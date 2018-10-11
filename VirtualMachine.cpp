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
