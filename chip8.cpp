/*
Copyright (C) 2013, Dmitry Ivanov (ddvanov@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"


void chip8::initialize(){
	unsigned char fonts[80] = 
	{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //"0"
	0x20, 0x60, 0x20, 0x20, 0x70, //"1"
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //"2"
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //"3"
	0x90, 0x90, 0xF0, 0x10, 0x10, //"4"
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //"5"
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //"6"
	0xF0, 0x10, 0x20, 0x40, 0x40, //"7"
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //"8"
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //"9"
	0xF0, 0x90, 0xF0, 0x90, 0x90, //"A"
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //"B"
	0xF0, 0x80, 0x80, 0x80, 0xF0, //"C"
	0xE0, 0x90, 0x90, 0x90, 0xE0, //"D"
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //"E"
	0xF0, 0x80, 0xF0, 0x80, 0x80 //"F"
	};

	sp = 0;
	pc = 512;
	I = 0;
	opcode = 0;
	
	delay_timer = 0;
	sound_timer = 0;


	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 64; j++){
			gfx[i][j] = false;
		}
	}
	
	for(int i = 0; i < 16; i++){
		stack[i] = 0;
		key[i] = false;

	}
	
	for(int i = 0; i < 4096; i++){
		memory[i] = 0;
	}

	//Initialize the sprites
	for(int i = 0; i < 80; i++){
		memory[i] = fonts[i];
	}

		
}

void chip8::loadGame(char game[]){
	FILE *file;	
	
	file = fopen(game, "rb");
	if(file == NULL){
		printf("Error: not a valid rom");
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	
	long size;
	size = ftell(file);

	if(size > (4096 -512)){
		printf("Error: rom is to large");
		exit(1);
	}
	
	rewind(file);

	char *buffer = (char*)malloc(sizeof(char) * size);
	fread(buffer, 1, size, file);

	for(int i = 0; i < size; i++){
		memory[i + 512] = buffer[i];	
	}

	free(buffer);
	fclose(file);
}

void chip8::emulateCycle(){
	opcode = ( memory[pc] << 8 ) | memory[pc + 1];
	bool keyP = false;

	unsigned short NNN = opcode & 0x0FFF;
	unsigned short NN = opcode & 0x00FF;
	unsigned short N = opcode & 0x000F;

	unsigned short X = (opcode & 0x0F00) >> 8;
	unsigned short Y = (opcode & 0x00F0) >> 4; 	
	
//	printf("opcode 0x%x, NNN 0x%x NN 0x%x N 0x%x X 0x%x Y 0x%x \n", opcode, NNN, NN, N, X, Y);
	pc = pc + 2;

	//Different Masks
	switch(opcode & 0xF000){
		case 0x0000:
			switch(opcode){
				case 0x00E0:
					gfxClear();	
				break;
				case 0x00EE:
					sp --;
					pc = stack[sp];
				break;
				default:
					pc = NNN;	
			}
		break;
		case 0x1000:
			pc = NNN;	
		break;
		case 0x2000:
			stack[sp] = pc;
			sp++;
			pc = NNN;
		break;
		case 0x3000:
			if(V[X] == NN)
				pc = pc + 2;
		break;
		case 0x4000:
			if(V[X] != NN)
				pc = pc + 2;
		break;
		case 0x5000:
			if(V[X] == V[Y])
				pc = pc + 2;
		break;
		case 0x6000:
			V[X] = NN;
		break;
		case 0x7000:
			V[X] = V[X] + NN;
		break;
		case 0x8000:
			switch(opcode & 0xF00F){
				case 0x8000:
					V[X] = V[Y];
				break;
				case 0x8001:
					V[X] = V[X] | V[Y];
				break;
				case 0x8002:
					V[X] = V[X] & V[Y];
				break;
				case 0x8003:
					V[X] = V[X] ^ V[Y];
				break;
				case 0x8004:
					if(V[X] > 0xFF - V[Y]){
						V[15] = 1;
					}else {
						V[15] = 0;
					}
					V[X] = V[X] + V[Y];
				break;
				case 0x8005:
					if(V[X] < V[Y]){
						V[15] = 0;
					}else {
						V[15] = 1;
					}
					V[X] = V[X] - V[Y];
				break;
				case 0x8006:
					V[15] = V[X] & 1;
					V[X] = V[X] >> 1;
				break;
				case 0x8007:
					if(V[X] > V[Y]){
						V[15] = 0;
					}else {
						V[15] = 1;
					}
					V[X] = V[Y] - V[X];
				break;
				case 0x800E:
					V[15] = V[X] >> 7; 	
					V[X] = V[X] << 1;
				break;
				default:
					printf("Warning: Unknown opcode 0x%x", opcode);
			}
		break;
		case 0x9000:
			if(V[X] != V[Y]){
				pc = pc + 2;
			}
		break;
		case 0xA000:
			I = NNN;	
		break;
		case 0xB000:
			pc = NNN + V[0];
		break;
		case 0xC000:
			V[X] = NN & (rand() % 0xFF); 
		break;
		case 0xD000:
			gfxSprite(X, Y, N);			
		break;
		case 0xE000:
			switch(opcode & 0xF0FF){
				case 0xE09E:
					if(key[V[X]]){
						pc = pc + 2;
					}
				break;
				case 0xE0A1:
					if(!key[V[X]]){
						pc = pc + 2;
					}
				break;
				default:
					printf("Warning: Unknown opcode 0x%x", opcode);
			}
		break;
		case 0xF000:
			switch(opcode & 0xF0FF){
				case 0xF007:
					V[X] = delay_timer;
				break;
				case 0xF00A:
					for(int i = 0; i < 16; i++){
						if(key[i]){
							V[X] = i;
							keyP = true;
						}
					}
					if(!keyP){
						pc =  pc - 2;
					}
				break;
				case 0xF015:
					delay_timer = V[X];	
				break;
				case 0xF018:
					sound_timer = V[X];	
				break;
				case 0xF01E:
					I = I + V[X];
				break;
				case 0xF029:
					I = V[X] * 5;	
				break;
				case 0xF033:
					memory[I] = V[X] /100;
					memory[I + 1] = (V[X] % 100)/10;
					memory[I + 2] = V[X] % 10;
				break;
				case 0xF055:
					for(int i = 0; i < 16; i++){
						memory[I + i] = V[i];	
					}
					
					I = I + X + 1;	
				break;
				case 0xF065:
					for(int i = 0; i < 16; i++){
						V[i] = memory[I + i];
					}
					
					I = I + X + 1;	
				break;
				default:
					printf("Warning: Unknown opcode 0x%x", opcode);
			}
		break;
		default:
			printf("Warning: Unknown opcode 0x%x", opcode);
	}

	if(sound_timer > 0)
		sound_timer--;
	if(delay_timer > 0)
		delay_timer--;

}


void chip8::gfxClear(){
	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 64; j++){
			gfx[i][j] = false;
		}
	}

}

void chip8::gfxSprite(unsigned short X, unsigned short Y, unsigned short N){
	V[15] = 0;

	for(int i = 0; i < N; i++){
		for(int j = 0; j < 8; j++){
			if(((memory[I + i] >> (7 - j)) & 0x1) != 0){
				if(gfx[V[Y] + i][V[X] + j] == true)
					V[15] = 1;
				
			gfx[V[Y] + i][V[X] + j] = gfx[V[Y] + i][V[X] + j] ^ true;	
			}
		}
	}
}

