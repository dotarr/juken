#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// some maximums that exist in the protocol
const ushort MAX_PAYLOAD_LEN = 1024; // if this aint big enough we have to redo alot

const short MAX_TITLE_LENGTH = 25;

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
const byte SELECT_DISC_REQ = 0x0B;

const byte INFO_EVT = 0x12;
const byte STATE_EVT = 0x13;
const byte DISC_EVT = 0x14;
const byte DOOR_EVT = 0x15;

// protocol parameter values
const short NULL_PARAM = 0x00FF;

const short STATE_PARAM = 0x00A0;
const short MODE_PARAM = 0x00A1;

const short NULL_CMD = 0xC900;

const short STOP_CMD = 0xC900;
const short PLAY_PAUSE_CMD = 0xCB00;
const short PREV_CMD = 0xCE00;
const short NEXT_CMD = 0xCF00;
const short FASTBACK_CMD = 0x0600;
const short FASTFORW_CMD = 0x0700;

const short RANDOM_CMD = 0xD400;
const short REPEAT_CMD = 0xCC00;


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
    { "track", "best selection", "userfile", "one random", "all random", "repeat" };

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
