#ifndef JUKEN_DVD_CONSTANTS_H
#define JUKEN_DVD_CONSTANTS_H

// protocol parameter values
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
