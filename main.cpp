#include <stdlib.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>

#include "unixdomainsock.h"
#include "jukebox.h"

const char* handshake_cmd   = "handshake";

const char* play_cmd        = "play";
const char* pause_cmd       = "pause";
const char* stop_cmd        = "stop";
const char* next_track_cmd  = "nexttrack";
const char* prev_track_cmd  = "prevtrack";

const char* best_tracks_cmd = "besttracks";
const char* disc_titles_cmd = "disctitles";
const char* disc_tracks_cmd = "disctracks";
const char* track_times_cmd = "tracktimes";
const char* userfiles_cmd  = "userfiles";

char* command = NULL;
bool print_version = false;
bool print_help = false;
short disc = 0;
char* messaging_socket = "/tmp/juken";

void
parse_env()
{
    char* sock = getenv("JUKEN_SOCK");
    if ( sock != NULL ) messaging_socket = sock;
}

char*
strip_path(const char* p)
{
    int len = ::strlen(p);
    int i = len;

    for (i=len; i>0; i--)
        if ( p[i-1] == '/' ) break;

    char* s = new char[len-i+1];
    ::strncpy(s, &p[i], len-i);
    s[len-i+1] = '\0';

    return s;
}

void
parse_args(int argc, char* argv[])
{
    char* arg0 = strip_path(argv[0]);
    if ( ::strcmp(arg0, "main") )
        command = arg0;

    char* short_opts = "vhd:m:";
    struct option long_opts[] = {
        { "version", no_argument, NULL, 'v' },
        { "help", no_argument, NULL, 'h' },
        { "disc", required_argument, NULL, 'd' },
        { "msg_sock", required_argument, NULL, 'm' },
        { NULL, no_argument, NULL, 0 }
    };

    int c = EOF;
    while( (c=::getopt_long(argc, argv, short_opts, long_opts, NULL)) != EOF )
    {
        switch( c )
        {
            case 'v': print_version = true;                     break;
            case 'h': print_help = true;                        break;
            case 'd': disc = ::atoi(optarg);                    break;
            case 'm': messaging_socket = optarg;                break;
            case ':': ::fprintf(stderr, "missing parameter\n"); break;
            case '?': ::fprintf(stderr, "unknown option\n");    break;
        }
    }

    if ( command == NULL )
    {
        if ( optind < argc )
            command = strip_path(argv[optind]);
        else
            ::fprintf(stderr, "missing command\n");
    }
}

bool
is_cmd(const char* c)
{
    return (::strcmp(command, c) == 0);
}

int
main (int argc, char* argv[])
{
    // parse parameters
    parse_env();
    parse_args(argc, argv);

    if ( print_version )
    {
        // print the toolbox version
    }
    else if ( print_help )
    {
        // print the toolbox help
    }
    else if ( command != NULL )
    {
        ::fprintf(stderr, "%s\n", command);
        try
        {
            // create the protocol object and messaging socket
            Jukebox protocol;
            UnixDomainSock sock;
            int fd = sock.OpenSock(messaging_socket);

            if ( is_cmd(handshake_cmd) )
            {
                // prepare the handshake request
                const char* NAME = "I'm PC";
                struct payload msg;
                msg.len = strlen(NAME);
                ::memcpy(msg.data, (byte*) NAME, msg.len);

                // issue the request
                protocol.IssueRequest(fd, HANDSHAKE_REQ, msg, HAS_REPLIES); 

                // process the reply
                struct payload reply;
                while ( protocol.GetReply(fd, reply) )
                {
                    // copy the reply into an allocated string
                    ushort name_len = reply.len-4;
                    char* name = new char[name_len+1];
                    ::strncpy(name, (char*) &reply.data[4], name_len);
                    name[name_len] = '\0';

                    // output the reply
                    DebugMsg("connected to %s\n", name);

                    // free the string
                    delete[] name;
                }
            }
            else if ( is_cmd(play_cmd) || is_cmd(pause_cmd) )
            {
                protocol.IssueChangeState(fd, PLAY_PAUSE_PARAM);
            }
            else if ( is_cmd(stop_cmd) )
            {
                protocol.IssueChangeState(fd, STOP_PARAM);
            }
            else if ( is_cmd(next_track_cmd) )
            {
                protocol.IssueChangeState(fd, NEXT_PARAM);
                protocol.IssueChangeState(fd, NULL_PARAM);
            }
            else if ( is_cmd(prev_track_cmd) )
            {
                protocol.IssueChangeState(fd, PREV_PARAM);
                protocol.IssueChangeState(fd, NULL_PARAM);
            }
            else if ( is_cmd(best_tracks_cmd) )
            {
                 // issue the query request
                protocol.IssueQueryDevice(fd, 0x00, 0x20, 0, 0x00, 0x01, 0x00);

                // process the reply
                struct payload reply;
                while ( protocol.GetReply(fd, reply) )
                {
                    byte num_tracks = reply.data[0];
                    byte* p = &reply.data[1];
                    for (int i=0; i<num_tracks; i++)
                    {
                        short disc = *((short*) p);
                        p += sizeof(short);
                        byte track = *p;
                        p++;
                        DebugMsg("slot:%3d track:%3d\n", disc, track);
                    }
                }
            }
            else if ( is_cmd(disc_titles_cmd) )
            {
                 // issue the query request
                protocol.IssueQueryDevice(fd, 0x00, 0x01, disc, 0x00, 0x00, 0x00);

                // process the reply
                struct payload reply;
                while ( protocol.GetReply(fd, reply) )
                {
                    // cast the reply into a disc_data
                    struct disc_data* info = (struct disc_data*) reply.data;

                    // add null terminator to title
                    byte data_len = sizeof(struct disc_data)-MAX_TITLE_LENGTH-1;
                    byte title_len = 0;
                    if ( info->title[0] != 0x01 )
                        title_len = reply.len-data_len;
                    info->title[title_len] = '\0';

                    // output the reply
                    DebugMsg("slot:%3d unknown:0x%02x title:%s genre:%s userfiles:0x%02X formatting:%s\n",
                            info->slot, info->unknown1, info->title, GENRE_NAMES[info->genre], info->userfiles, (info->format==0x13)?"cd-text":"none");
                }
            }
            else if ( is_cmd(disc_tracks_cmd) )
            {
                 // issue the query request
                protocol.IssueQueryDevice(fd, 0x00, 0x01, disc, 0x00, 0x01, 0x00);

                // process the reply
                struct payload reply;
                while ( protocol.GetReply(fd, reply) )
                {
                    // cast the reply into a track_data
                    struct disc_data* info = (struct disc_data*) reply.data;

                    // add null terminator to title
                    byte data_len = sizeof(struct disc_data)-MAX_TITLE_LENGTH-1;
                    byte title_len = 0;
                    if ( info->title[0] != 0x01 )
                        title_len = reply.len-data_len;
                    info->title[title_len] = '\0';

                    // output the reply
                    DebugMsg("slot:%3d index:%3d unknown:0x%02x title:%s genre:%s userfiles:0x%02X formatting:%s\n",
                            info->slot, info->index, info->unknown1, info->title, GENRE_NAMES[info->genre], info->userfiles, (info->format==0x13)?"cd-text":"none");
                }
            }
            else if ( is_cmd(track_times_cmd) )
            {
                 // issue the query request
                protocol.IssueQueryDevice(fd, 0x00, 0x04, disc, 0x00, 0x00, 0x00);

                // process the reply
                struct payload reply;
                while ( protocol.GetReply(fd, reply) )
                {
                    short slot = *((short*) reply.data);
                    byte unknown1 = reply.data[1];
                    byte unknown2 = reply.data[2];
                    byte unknown3 = reply.data[3];
                    byte unknown4 = reply.data[4];
                    byte num_tracks = reply.data[5];
                    DebugMsg("slot:%3d unknown:0x%02X,0x%02X,0x%02X,0x%02X num_tracks:%3d\n",
                            slot, unknown1, unknown2, unknown3, unknown4, num_tracks);
                    DebugMsg("track\tstart\tunknown\n");
                    DebugMsg("-----------------------------\n");
                    byte* p = &reply.data[6];
                    for (int i=0; i<num_tracks+1; i++)
                    {
                        byte start_min = *(p++);
                        byte start_sec = *(p++);
                        byte unknown = *(p++);
                        DebugMsg("%5d\t%02x:%02x\t0x%02X\n", i+1, start_min, start_sec, unknown);
                    }
                }
            }
            else if ( is_cmd(userfiles_cmd) )
            {
                 // issue the query request
                protocol.IssueQueryDevice(fd, 0x00, 1, 0, 0x00, 0x07, 0x00);

                // process the reply
                struct payload reply;
                while ( protocol.GetReply(fd, reply) )
                {
                    // cast the reply into a userfile_data
                    struct userfile_data* info = (struct userfile_data*) reply.data;
                    byte mask = info->mask;

                    // add null terminator to title
                    byte data_len = sizeof(struct userfile_data)-MAX_TITLE_LENGTH-1;
                    byte title_len = reply.len-data_len;
                    info->title[title_len] = '\0';

                    // output the reply
                    DebugMsg("mask:%02X unknown:%02X %02X %02X %02X %02X %02X title:%s\n",
                             mask, info->unknown1, info->unknown2, info->unknown3, 
                             info->unknown4, info->unknown5, info->unknown6, info->title);
                }
            }
        }
        catch (char* e)
        {
            // output the error
            ::fprintf(stderr, "%s\n", e);

            // exit with failure
            return EXIT_FAILURE;
        }
    }

    // return with success
    return EXIT_SUCCESS;
}

