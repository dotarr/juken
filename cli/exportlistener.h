#ifndef JUKEN_EXPORTLISTENER_H
#define JUKEN_EXPORTLISTENER_H

#include <kenwoodlistener.h>

class ExportListener : public KenwoodListener
{
    public:
        ExportListener(uint disc_id, const char* path);
        ~ExportListener();

        bool DiscDataReply(DiscData* info);
        bool CDTextDataReply(CDTextData* info);

    protected:
        void OpenFile(uint disc_id, const char* path);
        void CloseFile();

    private:
        FILE* m_file;
        short m_num_tracks;
};

#endif /* JUKEN_EXPORTLISTENER_H */
