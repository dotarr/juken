#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// some maximums that exist in the protocol
const ushort MAX_PAYLOAD_LEN = 64;

const short MAX_USER_TITLE_LENGTH = 25;
const short MAX_DISC_TITLE_LENGTH = 25;
const short MAX_TRACK_TITLE_LENGTH = 25;

const short MAX_TRACK_COUNT = 20;

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
const bool NO_REPLIES = false;

// protocol event/request codes 
const byte HANDSHAKE_REQ = 0x00;
const byte QUERY_REQ = 0x03;
const byte STATE_REQ = 0x0A;

const byte INFO_EVT = 0x12;
const byte STATE_EVT = 0x13;
const byte DISC_EVT = 0x14;
const byte READY_EVT = 0x15;

// protocol parameter values
const byte NULL_PARAM = 0xFF;

const byte STATE_PARAM = 0xA0;
const byte MODE_PARAM = 0xA1;

const byte RANDOM_PARAM = 0xD4;

const byte REPEAT_PARAM = 0xCC;

const byte STOP_PARAM = 0xC9;
const byte PLAY_PAUSE_PARAM = 0xCB;
const byte PREV_PARAM = 0xCE;
const byte NEXT_PARAM = 0xCF;

const byte FASTBACK_PARAM = 0x06;
const byte FASTFORW_PARAM = 0x07;

// player state values
const byte STOPPED_STATE = 0x40;
const byte STOPPING_STATE = 0x50;
const byte CHANGING_STATE = 0x60;
const byte PLAYING_STATE = 0x70;
const byte PAUSED_STATE = 0x80;
const byte SKIPFORW_STATE = 0x90;
const byte SKIPBACK_STATE = 0xA0;

// strings for player mode
const char* const MODE_NAMES[] = 
    { "track", "best selection", "one random", "all random", "repeat" };

// strings for player state
const char* const STATE_NAMES[] = 
    { "UNKNOWN", "stopped", "stopping", "changing", "playing", 
      "paused", "skip forward", "skip backward" };

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

#endif
