#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>

class Controller{
	public:
		Controller(bool isPlayer1);
		unsigned char read();
		void write(bool value);
		void setA(bool value);
		void setB(bool value);
		void setSelect(bool value);
		void setStart(bool value);
		void setUp(bool value);
		void setDown(bool value);
		void setLeft(bool value);
		void setRight(bool value);
	private:
		bool a;
		bool b;
		bool select;
		bool start;
		bool up;
		bool down;
		bool left;
		bool right;

		bool reset;
		unsigned char readCounter;
		bool player1;
};

#endif