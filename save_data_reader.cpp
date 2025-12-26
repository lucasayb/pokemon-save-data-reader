#include <cstdint>
#include <iostream>
#include <fstream>
#include "pokemon_list.h"

struct PlayerData {
	uint64_t options;
	uint8_t trainerId[3];
	char playerName[11];
	char _[11];
	char rivalName[11];
	uint8_t gender;
	
};

struct PartyData {
	uint8_t count;
};

struct SaveData {
	PlayerData playerData;
	PartyData partyData;
	std::string playerName;
	std::string rivalName;
	std::string gender;
	
	SaveData(const char *fname) {
		printf("Reading %s...\n", fname);
		read(fname);
	}
	
	void read(const char *fname) {
		std::ifstream inp(fname, std::ios_base::binary);
		if (!inp) {
			printf("Cannot read file");
			return;
		}
		
		// Move o cursor até a tabela de dados
		inp.seekg(0x2000, std::ios::beg);
		
		inp.read(reinterpret_cast<char*>(&playerData.options), sizeof(playerData.options));
		
		inp.read(reinterpret_cast<char*>(&playerData.trainerId), sizeof(playerData.trainerId));
		
		inp.read(reinterpret_cast<char*>(&playerData.playerName), sizeof(playerData.playerName));
		
		inp.read(reinterpret_cast<char*>(&playerData._), sizeof(playerData._));
		
		inp.read(reinterpret_cast<char*>(&playerData.rivalName), sizeof(playerData.rivalName));

		playerName = decodeGen2Name((const uint8_t*) playerData.playerName, sizeof(playerData.playerName));
				
		rivalName = decodeGen2Name((const uint8_t*) playerData.rivalName, sizeof(playerData.rivalName));
		
		// Move o cursor até o gênero
		inp.seekg(0x3E3D, std::ios::beg);		
		
		inp.read(reinterpret_cast<char*>(&playerData.gender), sizeof(playerData.gender));
		
		// Start of party Pokémon
		inp.seekg(0x2865, std::ios::beg);
		
		inp.read(reinterpret_cast<char*>(&partyData), sizeof(partyData));
		
		gender = playerData.gender == 0x00 ? "BOY" : "GIRL";
		
	}
	
private:
	std::string decodeGen2Name(const uint8_t* data, size_t n) {
		std::string out;
		
		out.reserve(n);
		
		for (size_t i = 0; i < n; i++) {
			uint8_t b = data[i];
			
			if (b == 0x50) break; // terminador "END"
			if ((b>= 0x80) && (b <= 0x99)) { // A..Z
				out.push_back(char('A' + (b - 0x80)));
			} else {
				out.push_back('?');
			}
		}
		return out;
	}
};


int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s <sav.dat>\n", argv[0]);
		return 1;
	}
	
	SaveData sav(argv[1]);
	
	std::cout << "Player name = " << sav.playerName << "\n";
	std::cout << "Rival name = " << sav.rivalName << "\n";
	std::cout << "Player gender = " << sav.gender << "\n";
	
	printf("%d",sav.partyData.count);
	
	
	return 0;
}