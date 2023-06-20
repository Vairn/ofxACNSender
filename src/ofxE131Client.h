//
//	ofxE131Client.h
//
//	Modified by Eduard Frigola on 24/05/21
//
//	ORIGINAL: https://github.com/sosolimited/Cinder-DMX/blob/master/src/sACN/E131Client.h
//  E131Client.h
//
//  Created by Alex Olivier on 04/05/16.
//
//	Some code from: https://github.com/tobiasebsen/ofxArtNode/blob/master/src/ofxArtNode.h

#pragma once

#include "ofMain.h"
#include "ofxUDPManager.h"

class ofxE131Client {
public:

	void setup(std::string addr, bool mCast = false);
	void update();

	void setUniverse(int universe);
	void setChannel(int channel, u_char value, int universe = 1);
	void setChannels(int startChannel, u_char* values, size_t size, int universe = 1);
	void setPriority(int priority);
	
private:
	void connectUDP();
	void sendDMX();
	void setLengthFlags();
	
	// Data structure to store payload and sequence num for each universe
	struct UniverseData {
		char universeSequenceNum;
		std::array<char, 512> payload;
	};
	
	// BIG ENDIAN
	std::array<char, 638> sac_packet = {{
		// ROOT LAYER (RLP)
		0x00, 0x10,                   // Define RLP Preamble Size
		0x00, 0x00,                   // RLP Post amble size
		0x41, 0x53, 0x43, 0x2d, 0x45, // E131 Packet identifier
		0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00, 0x00,
		0x00, // FILL OUT, low 12 bits = PDU length
		// High 4 bits = 0x7
		0x00, 0x00, 0x00, 0x04, // Identifies RLP Data as 1.31 Protocol PDU
		0x00, 0x00, 0x00, 0x00, // CID, Sender's unique CID
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		
		// FRAMING LAYER
		0x00, 0x00,             // Low 12 bits = PDU length, High 4 bits=07
		0x00, 0x00, 0x00, 0x02, // Identifies 1.31 data as DMP protocol PDU
		
		0x00, 0x00, 0x00, 0x00, // User assigned name of source
		0x00, 0x00, 0x00, 0x00, // UTF-8 string, null-terminated
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		
		0x64,       // DATA PRIORITY, 0-200, default of 100 (0x64)
		0x00, 0x00, // RESERVED, transmitters send 0, receivers ignore
		0x00,       // Sequence number to detect duplicate or out of order packets
		0x00,       // Options flag, bit 7 = preview data, bit 6 = stream terminated
		0x00, 0x00, // UNIVERSE number
		
		// DMP Layer
		0x00, 0x00, // Protocol flags + length, Low 12 bits = PDU length
		// High 4 bits = 0x7
		0x02,                    // Identifies DMP set property message PDU
		static_cast<char>(0xA1), // Identifies format of address and data
		0x00, 0x00, // Indicates DMX_start.  Code is at DMP address 0.
		0x00, 0x01, // Indicates each property is 1 octet / byte
		0x02, 0x01, // Indicates 1+the number of slots in packet
		0x00,       // DMX start code (0 is standard)
		char(512)   // DMX payload (all 512 channels)
	}};
	
	/*
		The range for multicast addresses is from
		224.0.0.0 to 239.255.255.255
	*/
	bool bMcast = false;
	bool loggedException = false;

	int packet_length = 638; // Length when all 512 DMX channels are sent
	int destPort = 5568;     // Default port for sACN protocol!
	int priority = 100;
	
	char* pAddr;
	std::string ipAddress;

	ofxUDPManager udp;

	std::map<int, UniverseData> universePackets;
};
