#ifndef __PAYLOAD_H__
#define __PAYLOAD_H__

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#include "constants.h"

enum mode { TrackMode, BestMode, OneRandomMode, AllRandomMode, RepeatMode };

enum state { Unknown, Stopped, Stopping, Changing, Playing, 
             Paused, SkipForward, SkipBackward };
 
enum genre
{
    UNKNOWN=0, UNASSIGNED,
    ADULT_CONTEMPORARY, ALTERNATIVE_ROCK, CHILDRENS_MUSIC, CLASSICAL,
    CONTEMPORARY_CHRISTIAN, COUNTRY, DANCE, EASY_LISTENING, EROTIC, 
    FOLK, GOSPEL, HIP_HOP, JAZZ, LATIN, MUSICAL, NEW_AGE, OPERA, 
    OPERETTA, POP_MUSIC, RAP, REGGAE, ROCK_MUSIC, RHYTHM_BLUES, 
    SOUND_EFFECTS, SOUND_TRACK, SPOKEN_WORD, WORLD_MUSIC
};


struct payload
{
    ushort len;
    byte data[MAX_PAYLOAD_LEN];
};

struct userfile_data
{
    byte   unknown1;
    byte   unknown2;
    byte   mask;
    byte   unknown3;
    byte   unknown4;
    byte   unknown5;
    byte   unknown6;
    char   title[MAX_USER_TITLE_LENGTH+1]; // +1 to hold null terminator
};

struct disc_data
{
    ushort slot;
    byte   unknown1;
    byte   userfiles;
    byte   unknown3;
    byte   genre;
    byte   unknown4;
    char   title[MAX_DISC_TITLE_LENGTH+1]; // +1 to hold null terminator
};

struct track_data
{
    byte unknown1;
    byte unknown2;
    byte index;
    byte unknown3;
    byte unknown4;
    byte unknown5;
    byte unknown6;
    char title[MAX_TRACK_TITLE_LENGTH+1]; // +1 to hold null terminator
};


#endif /* __PAYLOAD_H__ */
