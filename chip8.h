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


class chip8 {
public:
		
	unsigned short opcode;
	unsigned char memory[4096];
	unsigned char V[16];

	unsigned short I;
	unsigned short pc;


	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16];
	unsigned short sp;

	bool key[16];


	bool gfx[32][64];
public:
		
	void initialize();

	void loadGame(char game[]);


	void emulateCycle();

	//Display functions
private:
	void gfxClear();
	void gfxSprite(unsigned short X, unsigned short Y, unsigned short N);

//	void drawFlag();
};
