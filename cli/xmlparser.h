#ifndef JUKEN_XML_PARSER_H
#define JUKEN_XML_PARSER_H

#include <expat.h>
#include <stack>

class ElementHandler
{
    public:
        ElementHandler() { };

        virtual ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes) = 0;
        virtual void CharHandler(const XML_Char* text, int len) = 0;
        virtual void EndHandler(const XML_Char* element) = 0;

        char* StrCat(const char* s, const XML_Char* text, int len);
        char* StrDup(const XML_Char* text, int len);
};

class XMLParser : public ElementHandler
{
    public:
        XMLParser();
        ~XMLParser();

        void ParseFile(const char* filename);

    protected:
        stack<ElementHandler*> m_handlers;

    private:

        static void start_handler(void* data, 
                                  const XML_Char* element, 
                                  const XML_Char** attrbutes);
        static void char_handler(void* data, 
                                 const XML_Char* text, 
                                 int len);
        static void end_handler(void* data, 
                                const XML_Char* element);
};

class StringHandler : public ElementHandler
{
    public:
        StringHandler(char** p) : m_data(p) { }
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes)
            { return NULL; }
        void CharHandler(const XML_Char* text, int len);
        void EndHandler(const XML_Char* element);
    protected:
        char** m_data;
};

class ShortHandler : public StringHandler
{
    public:
        ShortHandler(short* p) : StringHandler(&m_string), m_string(NULL), m_data(p) { }
        ~ShortHandler() { delete m_string; }
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes)
            { return NULL; }
        void EndHandler(const XML_Char* element);
    protected:
        char* m_string;
        short* m_data;
};

class IntHandler : public StringHandler
{
    public:
        IntHandler(int* p) : StringHandler(&m_string), m_string(NULL), m_data(p) { }
        ~IntHandler() { delete m_string; }
        ElementHandler* StartHandler(const XML_Char* element, const XML_Char** attrbutes)
            { return NULL; }
        void EndHandler(const XML_Char* element);
    protected:
        char* m_string;
        int* m_data;
};

#endif /* JUKEN_XML_PARSER_H */
