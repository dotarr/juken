#ifndef JUKEN_EXPORTLISTENER_H
#define JUKEN_EXPORTLISTENER_H

#include <kenwoodlistener.h>

class ExportListener : public KenwoodListener
{
    public:
        ExportListener(const char* path);
        ~ExportListener();

        bool DiscDataReply(DiscData* info);
        bool CDTextDataReply(CDTextData* info);
        bool TrackTimesReply(TrackTimes* info);

    protected:
        void OpenFile(uint disc_id);
        void CloseFile(short num_tracks);

    private:
        const char* m_data_dir;

        FILE* m_file;
        short m_tracks_left;
};

#endif /* JUKEN_EXPORTLISTENER_H */
