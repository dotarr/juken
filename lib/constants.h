#ifndef JUKEN_CONSTANTS_H
#define JUKEN_CONSTANTS_H

// some maximums that exist in the protocol
const ushort MAX_PAYLOAD_LEN = 1024; // if this aint big enough we have to redo alot

// bytes used in protocol marshalling
const byte NUL = 0x00; // NULl
const byte SOH = 0x01; // Start of Header
const byte STX = 0x02; // Start of TeXt
const byte ETX = 0x03; // End of TeXt
const byte EOT = 0x04; // End Of Transmission
const byte ENQ = 0x05; // ENQuire
const byte ACK = 0x06; // ACKnowledge
const byte NAK = 0x15; // Not AcKnowledged
const byte ETB = 0x17; // End of Transmission Block

// flags for whether a request has a reply or not
const bool HAS_REPLIES = true;
const bool NO_REPLIES  = false;

// value sent in TextData to represent "No Data"
const char EMPTY_TEXT[] = { 0x01 };

// protocol event/request codes 
const byte HANDSHAKE            = 0x00;
const byte DATA_ACCESS          = 0x03;
const byte DISC_INFO            = 0x04;
const byte DISC_TOC             = 0x06;
const byte DISC_USERFILES       = 0x07;
const byte DISC_GENRE           = 0x08;
const byte READY_FOR_DATA       = 0x09;
const byte DO_ACTION            = 0x0A;
const byte CHANGE_DISC          = 0x0B;
const byte CHANGE_MODE          = 0x0C;
const byte CHAPTER_FRAMES       = 0x0C;
const byte DISC_LISTING         = 0x0D;
const byte DISC_VOLUME_ID       = 0x0E;
const byte DISC_TIME_STAMP      = 0x0F;
const byte INFO_EVENT           = 0x12;
const byte STATE_EVENT          = 0x13;
const byte DISC_EVENT           = 0x14;
const byte DOOR_EVENT           = 0x15;
const byte LONG_TEXT_DATA       = 0xFD;
const byte TEXT_DATA            = 0xFE;

// strings for player mode
const char* const MODE_NAMES[] = 
    { "track", "track (one random)", "track (all random)",
      "program", "best selection", 
      "musictype", "musictype (all random)", 
      "userfile", "userfile (one random)", "userfile (all random)" };

// strings for disc genre
const char* const GENRE_NAMES[] = 
{
    "?Unknown","?Unassigned",
    "Adult Comtemporary","Alternative Rock","Children's Music","Classical",
    "Contemporary Christian","Country","Dance","Easy Listening","Erotic",
    "Folk","Gospel","Hip Hop","Jazz","Latin","Musical","New Age","Opera",
    "Operetta","Pop Music","Rap","Reggae","Rock Music","Rhythm & Blues",
    "Sound Effects","Sound Track","Spoken Word","World Music"
};

#endif /* JUKEN_CONSTANTS_H */
