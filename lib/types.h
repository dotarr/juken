#ifndef JUKEN_TYPES_H
#define JUKEN_TYPES_H

#include <common.h>

#include "constants.h"

enum mode { TrackMode, ProgramMode, BestMode, MusicTypeMode, UserfileMode };
enum random { RandomOff, RandomSingle, RandomAll };

enum state { Unknown, Stopped, Stopping, Changing, Playing, 
             Paused, SkipForward, SkipBackward };
 
enum genre
{
    UNKNOWN                     =  0, // 0x00
    UNASSIGNED                  =  1, // 0x01
    ADULT_CONTEMPORARY          =  2, // 0x02
    ALTERNATIVE_ROCK            =  3, // 0x03
    CHILDRENS_MUSIC             =  4, // 0x04
    CLASSICAL                   =  5, // 0x05
    CONTEMPORARY_CHRISTIAN      =  6, // 0x06
    COUNTRY                     =  7, // 0x07
    DANCE                       =  8, // 0x08
    EASY_LISTENING              =  9, // 0x09
    EROTIC                      = 10, // 0x0A
    FOLK                        = 11, // 0x0B
    GOSPEL                      = 12, // 0x0C
    HIP_HOP                     = 13, // 0x0D
    JAZZ                        = 14, // 0x0E
    LATIN                       = 15, // 0x0F
    MUSICAL                     = 16, // 0x10
    NEW_AGE                     = 17, // 0x11
    OPERA                       = 18, // 0x12
    OPERETTA                    = 19, // 0x13
    POP_MUSIC                   = 20, // 0x14
    RAP                         = 21, // 0x15
    REGGAE                      = 22, // 0x16
    ROCK_MUSIC                  = 23, // 0x17
    RHYTHM_BLUES                = 24, // 0x18
    SOUND_EFFECTS               = 25, // 0x19
    SOUND_TRACK                 = 26, // 0x1A
    SPOKEN_WORD                 = 27, // 0x1B
    WORLD_MUSIC                 = 28  // 0x1C
};

#endif /* JUKEN_TYPES_H */
