#ifndef JUKEN_DVD_CONSTANTS_H
#define JUKEN_DVD_CONSTANTS_H

//  DataAccess info_types when access=RetrieveData and type=Text
const byte DiscNames=0x01;
const byte ArtistNames=0x02;
const byte DiscNamesInGenre=0x03;
const byte DiscNamesInUserfile=0x04;
const byte UserfileNames=0x05;
const byte DiscArtistTrackNames=0x06;
const byte DiscName=0x07;
const byte TitleName=0x08;
const byte ChapterName=0x09;
const byte ArtistName=0x0A;

//  DataAccess info_types when access=RetrieveData and type=TOC
const byte TOCId=0x0B;
const byte DiscVolumeId=0x0C;
const byte DiscTimestamp=0x0D;
const byte ChapterFrames=0x0E;

//  DataAccess info_types when access=Write* 
const byte AllUserfileNames=0x03;
const byte DiscArtistNames=0x06;

// type of text returned in TextData/LongTextData
const byte DiscText=0x01;
const byte TrackText=0x02;
const byte ArtistText=0x03;
const byte GenreListText=0x04;
const byte UserfileListText=0x05;
const byte UserfileText=0x06;

// action values
const short NULL_PARAM          = 0xFFFF;

const short STATE_PARAM         = 0x000C;

const short NULL_CMD            = 0xC900;

const short STOP_CMD            = 0x1100;
const short PLAY_CMD            = 0x1300;
const short PAUSE_CMD           = 0x1300;
const short PREV_CMD            = 0x1700;
const short NEXT_CMD            = 0x1600;
const short FASTBACK_CMD        = 0x1400;
const short FASTFORW_CMD        = 0x1500;

const short RANDOM_CMD          = 0x4F00;


#endif /* JUKEN_DVD_CONSTANTS_H */
