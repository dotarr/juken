#ifndef JUKEN_EXPORTLISTENER_H
#define JUKEN_EXPORTLISTENER_H

#include <kenwoodlistener.h>

class ExportListener : public KenwoodListener
{
    public:
        ExportListener(const char* disc_id, const char* path);
        ~ExportListener();

        bool TextDataReply(short slot, byte track, byte userfiles, 
                           byte request_type, byte genre, 
                           byte formatting, char* title);

    protected:
        void OpenFile(const char* disc_id, const char* path);
        void CloseFile();

    private:
        FILE* m_file;
        short m_num_tracks;
};

#endif /* JUKEN_EXPORTLISTENER_H */
