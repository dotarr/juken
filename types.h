#ifndef __TYPES_H__
#define __TYPES_H__

#include <stddef.h>

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

#include "constants.h"

typedef void (*reply_handler) (byte reply_cmd, ushort reply_len, byte* reply_data);

enum mode { TrackMode, BestMode, UserfileMode, OneRandomMode, AllRandomMode, RepeatMode };

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

#endif /* __TYPES_H__ */
