#pragma once

#ifndef MYSERIALIZATION_H
#define MYSERIALIZATION_H

#include <string>

class MySerialization
{
public:
	std::string plantNAME;
	std::string zombieNAME;
	int plantHP;
	int plantDAMAGE;
	int zombieHP;
	int zombieDAMAGE;
	MySerialization() {
		plantNAME = "Plant"; 
		plantHP = 30; 
		plantDAMAGE = 2;
		zombieNAME = "Zombie";
		zombieHP = 30;
		zombieDAMAGE = 4;
	}
	~MySerialization() {}
};

#endif