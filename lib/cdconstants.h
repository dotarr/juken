#ifndef JUKEN_CD_CONSTANTS_H
#define JUKEN_CD_CONSTANTS_H

// protocol parameter values
const short NULL_PARAM          = 0xFFFF;

const short STATE_PARAM         = 0x00A0;
const short MODE_PARAM          = 0x00A1;

const short NULL_CMD            = 0xC900;

const short STOP_CMD            = 0xC900;
const short PLAY_PAUSE_CMD      = 0xCB00;
const short PREV_CMD            = 0xCE00;
const short NEXT_CMD            = 0xCF00;
const short FASTBACK_CMD        = 0x0600;
const short FASTFORW_CMD        = 0x0700;

const short RANDOM_CMD          = 0xD400;
const short REPEAT_CMD          = 0xCC00;


#endif /* JUKEN_CD_CONSTANTS_H */
