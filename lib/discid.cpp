#include "discid.h"

unsigned int
cddb_sum(int n)
{
    unsigned int sum = 0;
    while ( n > 0 )
    {
        sum += (n % 10);
        n /= 10;
    }
    return sum;
}

unsigned int
hextoint(byte hex)
{
    char b[3];
    sprintf(b, "%02X", hex);
    return ((b[0]-'0')*10) + (b[1]-'0');
}

unsigned int
secs(const TimeInfo& time)
{
    return ((hextoint(time.minute)*60) + hextoint(time.second));
}

unsigned long
discid(byte num_tracks, TimeInfo* times)
{
    unsigned int n = 0;
    for (int i=0; i<num_tracks; i++)
        n = n + cddb_sum(secs(times[i]));

    //unsigned int t = secs(times[num_tracks]);
    unsigned int t = secs(times[num_tracks]) - secs(times[0]);

    return ((n % 0xff) << 24 | t << 8 | num_tracks);
}

