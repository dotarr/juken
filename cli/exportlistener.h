#ifndef JUKEN_EXPORTLISTENER_H
#define JUKEN_EXPORTLISTENER_H

#include <kenwoodlistener.h>

class ExportListener : public KenwoodListener
{
    public:
        ExportListener(FILE* f);
        ~ExportListener();

        bool DiscDataReply(DiscData* info);
        bool CDTextDataReply(CDTextData* info);
        bool TrackTimesReply(TrackTimes* info);
        bool DiscTrackListReply(DiscTrackList* info);

    protected:

    private:
        FILE* m_file; // the file too export to
        uint  m_discid;
        short m_track_count;
};

#endif /* JUKEN_EXPORTLISTENER_H */
