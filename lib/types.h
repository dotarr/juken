#ifndef JUKEN_TYPES_H
#define JUKEN_TYPES_H

#include <common.h>

#include <list>

#include "constants.h"

// constants for Name.type
const byte DISC_NAME = 0;
const byte ARTIST_NAME = 1;
const byte DESCRIPTION_NAME = 1;
const byte TRACK_NAME = 2;
const byte TITLE_NAME = 2;
const byte USERFILE_NAME = 3;

class Name
{
    public:
        Name()
            : index(0), type(255), text(NULL) { }
        Name(const Name& name)
            : index(name.index), type(name.type), text(NULL)
            {
               if ( name.text != NULL )
                  text = ::strdup(name.text);
            }
        Name(short i, byte t, const char* txt)
            : index(i), type(t), text(NULL)
            {
               if ( txt != NULL )
                  text = ::strdup(txt);
            }
        ~Name() { delete text; text = NULL; }

        operator const char*() { return text; }

        short index;
        byte type;
        char* text;
};
typedef void (NameCallback)(void* context, Name& data);
typedef std::list<Name> NameList;

const byte DISC_CD_A   = 0; // CD Audio
const byte DISC_CD_MP3 = 1; // MP3 CD
const byte DISC_CD_V   = 2; // Video CD
const byte DISC_DVD_A  = 3; // DVD Audio
const byte DISC_DVD_V  = 4; // DVD Video

class Disc
{
    public:
        Disc()
            : id(NULL), index(0), type(255), title(NULL), artist(NULL), 
              tracks(), userfiles(0), genre(0) { }
        Disc(const Disc& disc)
            : id(NULL), index(disc.index), type(disc.type), title(NULL), artist(NULL),
              tracks(disc.tracks), userfiles(disc.userfiles), genre(disc.genre)
            {
                if ( disc.title != NULL )
                    title = ::strdup(disc.title);
                if ( disc.artist != NULL )
                  artist = ::strdup(disc.artist);
            }
        Disc(byte t)
            : id(NULL), index(0), type(t), title(NULL), artist(NULL), 
              tracks(), userfiles(0), genre(0) { }
        Disc(short i, byte t, const char* tit, const char* art, byte uf, byte g)
            : id(NULL), index(i), type(t), title(NULL), artist(NULL), 
              tracks(), userfiles(uf), genre(g)
            {
                if ( tit != NULL )
                    title = ::strdup(tit);
                if ( art != NULL )
                  artist = ::strdup(art);
            }
        ~Disc() { delete title; title = NULL; delete artist; artist = NULL; }

        operator const char*() { return title; }

        char* id;
        short index;
        byte type;
        char* title;
        char* artist;
        NameList tracks;
        byte userfiles;
        byte genre;
};
typedef void (DiscCallback)(void* context, Disc& data);
typedef std::list<Disc> DiscList;

class Info
{
    public:
        Info(const Info& info)
            : index(info.index), type(info.type), count(info.count) { }
        Info(short i, byte t, short c) 
            : index(i), type(t), count(c) { }
        ~Info() { }

        short index;
        byte type;
        short count;
};
typedef std::list<Info> InfoList;

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


enum access 
{ 
    RetrieveDataAccess=0x00, 
    WriteDiscGenreAccess=0x10, 
    WriteProgramAccess=0x20, 
    WriteUserfilesAccess=0x40, 
    WriteTextAccess=0x80 
};

enum data_type
{ 
    ReadyDataType=0x00, 
    TextDataType=0x01, 
    InfoDataType=0x02, 
    TOCDataType=0x04, 
    UserfilesDataType=0x08, 
    GenreDataType=0x10, 
    ListingDataType=0x20 
};

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
