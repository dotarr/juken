#include <common.h>

#include "xmlparser.h"

const int BUFF_SIZE = 8192;

char*
ElementHandler::StrCat(const char* s, const XML_Char* text, int len)
{
    int s_len = ::strlen(s);
    char* str = (char*) ::malloc(s_len+len+1);
    ::strcpy(str, s);
    if ( len > 0 )
        ::strncpy(str+s_len, text, len);
    str[s_len+len] = '\0';
    return str;
}

char*
ElementHandler::StrDup(const XML_Char* text, int len)
{
    char* str = (char*) ::malloc(len+1);
    if ( len > 0 )
        ::strncpy(str, text, len);
    str[len] = '\0';
    return str;
}

XMLParser::XMLParser()
{
}

XMLParser::~XMLParser()
{
}

void
XMLParser::ParseFile(const char* filename)
{
    FILE* file;
    XML_Parser parser;

    try
    {
        if ( ::strcmp(filename, "-") == 0 )
            file = stdin;
        else
            file = fopen(filename, "r");
        ThrowIfNull(file, "unable to open %s: %s", filename, strerror(errno));

        parser = XML_ParserCreate(NULL);
        ThrowIfNull(parser, "unable to create parser: %s", filename, strerror(errno));

        XML_SetUserData(parser, this);
        XML_SetElementHandler(parser, &start_handler, &end_handler);
        XML_SetCharacterDataHandler(parser, &char_handler);

        m_handlers.push(this);

        bool done = false;
        while ( !done )
        {
            void* buffer = XML_GetBuffer(parser, BUFF_SIZE);
            ThrowIfNull(buffer, "unable to create parse buffer: %s", filename, strerror(errno));

            int bytes_read = ::fread(buffer, sizeof(byte), BUFF_SIZE, file);
            ThrowIfNeg(bytes_read, "error reading file %s: %s", filename, strerror(errno));
            done = (bytes_read==0);

            if ( XML_ParseBuffer(parser, bytes_read, done) )
            {
                ThrowIfNeg(bytes_read, "parse error %s:%d[%d]: %s", 
                        filename, 
                        XML_GetCurrentLineNumber(parser), 
                        XML_GetCurrentColumnNumber(parser),
                        XML_ErrorString(XML_GetErrorCode(parser)));
            }
        }

        XML_ParserFree(parser);
        parser = NULL;

        ::fclose(file);
        file = NULL;    }
    catch(...)
    {
        if ( parser != NULL )
            XML_ParserFree(parser);
        parser = NULL;

        if ( file != NULL )
            ::fclose(file);
        file = NULL;
        throw;
    }

}

void 
XMLParser::start_handler(void* data, const XML_Char* element, const XML_Char** attributes)
{
    XMLParser* _this = (XMLParser*) data;
    ElementHandler* dispatch = _this->m_handlers.top();
    ElementHandler* new_handler = NULL;
    if ( dispatch != NULL )
        new_handler = dispatch->StartHandler(element, attributes);
    _this->m_handlers.push(new_handler);
}

void 
XMLParser::char_handler(void* data, const XML_Char* text, int len)
{
    XMLParser* _this = (XMLParser*) data;
    ElementHandler* dispatch = _this->m_handlers.top();
    if ( dispatch != NULL )
        dispatch->CharHandler(text, len);
}

void 
XMLParser::end_handler(void* data, const XML_Char* element)
{
    XMLParser* _this = (XMLParser*) data;
    ElementHandler* dispatch = _this->m_handlers.top();
    if ( dispatch != NULL )
        dispatch->EndHandler(element);
    _this->m_handlers.pop();
}


void 
StringHandler::CharHandler(const XML_Char* text, int len)
{ 
    if ( (*m_data) == NULL )
        (*m_data) = StrDup(text, len);
    else
    {
        char* tmp = StrCat((*m_data), text, len);
        delete (*m_data);
        (*m_data) = tmp;
    }
}

void 
StringHandler::EndHandler(const XML_Char* element)
{ 
    //printf("String %s\n", (*m_data));
}

void 
ShortHandler::EndHandler(const XML_Char* element)
{ 
    if ( m_string != NULL )
        (*m_data) = (short) atoi(m_string); 
}
 void 
IntHandler::EndHandler(const XML_Char* element)
{ 
    if ( m_string != NULL )
        (*m_data) = atoi(m_string); 
}
            


