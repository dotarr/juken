#ifndef JUKEN_TYPES_H
#define JUKEN_TYPES_H

#include <common.h>

#include "constants.h"

typedef struct 
{
    byte minute;
    byte second;
    byte subsecond;
} TimeInfo;

typedef struct 
{
    short slot;
    byte track;
} DiscTrack;

typedef struct 
{
    short slot;
    byte title;
    short chapter;
} DiscTitleChapter;

typedef struct 
{
    byte b0;
    byte b1;
    byte b2;
} ChapterFrame;

typedef byte VolumeId[32];
typedef byte TimeStamp[17];


enum access { RetrieveData=0x00, SetDiscGenre=0x10, WriteProgram=0x20, 
              SetUserfiles=0x40, WriteText=0x80 };
enum data_type { Ready=0x00, Text=0x01, Info=0x02, TOC=0x04, 
                 Userfiles=0x08, Genre=0x10, Listing=0x20 };
enum cd_info_type { CDDiscNames=0x00, CDTrackNames=0x01, 
                    CDArtistName=0x02, CDUserfileNames=0x07 };
enum dvd_info_type { DVDDiscNames=0x01, DVDArtistNames=0x02, 
                     DVDDiscNamesInGenre=0x03, DVDDiscNamesInUserfile=0x04, 
                     DVDUserfileNames=0x05, DVDChapterNames=0x06, 
                     DVDDiscName=0x07, DVDTitleName=0x08, DVDChapterName=0x09, 
                     DVDArtistName=0x0A, DVDCDTOC=0x0B, DVDVolumeId=0x0C, 
                     DVDTimestamp=0x0D, DVDFrames=0x0E };
enum dvd_title_type { DVDDiscText=0x01, DVDTrackText=0x02, 
                      DVDArtistText=0x03, DVDGenreListText=0x04, 
                      DVDUserfileListText=0x05, DVDUserfileText=0x06 };


enum slots { NoSlots=-1, AllSlots = 0 };

enum mode { UnknownMode=-1, TrackMode=0, TrackModeRandomOne=1, 
            TrackModeRandomAll=2, ProgramMode=3, BestMode=4,
            MusicTypeMode=5, MusicTypeModeRandomAll=6,
            UserfileMode=7, UserfileModeRandomOne=8, UserfileModeRandomAll=9 };

enum state { UnknownState=-1, Stopped=0x40, Standby=0x41, Stopping=0x50, 
             Changing=0x60, Playing=0x70, Paused=0x80, 
             SkipForward=0x90, SkipBackward=0xA0 };
enum repeat { UnknownRepeat=-1, RepeatOff=0, RepeatOn=1 }; 
enum door { DoorUnknown=-1, DoorClosed=0, DoorOpen=1 }; 
 
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
