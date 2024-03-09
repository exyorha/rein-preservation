#include "DatabaseViewer/TextXMLWriter.h"
#include "DatabaseViewer/UTF8Helpers.h"
#include "DatabaseViewer/XMLFile.h"

#include <stdexcept>

TextXMLWriter::TextXMLWriter(std::string &output) : m_output(output), m_indentLevel(0) {

}

TextXMLWriter::~TextXMLWriter() = default;

void TextXMLWriter::write(const XMLFile &file) {
    if(file.hasRoot())
        write(file.root());
}

void TextXMLWriter::writeText(const std::string_view &string) {
    m_output.append(string);
}

void TextXMLWriter::write(const XMLNode &node) {
    if(!node.hasName()) {
        if(node.hasContent()) {

            writeIndent();
            writeCharacterContent(node.content());
            writeText("\n");
        }

    } else {
        writeIndent();

        writeText("<");
        writeText(node.name());

        if(!node.attributes().empty()) {
            for(const auto &attribute: node.attributes()) {
                writeText(" ");
                writeText(attribute.name());
                writeText("=\"");

                if(attribute.hasValue())
                    writeCharacterContent(attribute.value(), true);

                writeText("\"");
            }
        }

        if(node.children().empty() && !node.hasContent())
            writeText("/>\n");
        else {
            writeText(">");

            if(node.hasContent() && node.children().empty()) {
                writeCharacterContent(node.content());
                writeText("</");
                writeText(node.name());
                writeText(">\n");
            } else {
                writeText("\n");
                m_indentLevel++;

                if(node.hasContent()) {
                    writeIndent();
                    writeCharacterContent(node.content());
                    writeText("\n");
                }

                for(const auto &child: node.children()) {
                    write(child);
                }

                m_indentLevel--;

                writeIndent();

                writeText("</");
                writeText(node.name());
                writeText(">\n");
            }
        }
    }
}

void TextXMLWriter::writeCharacterContent(const std::string_view &string, bool attribute) {
    auto runBegin = string.begin();

    // We escape following characters: '<', '>', '&' (always), '"', ''' (in attributes)
    // These are all ASCII characters, so we don't decode UTF-8 data, only follow it.

    unsigned int continuationBytes = 0;

    for(auto it = runBegin, end = string.end(); it != end; it++) {
        char ch = *it;

        auto classification = classifyUTF8Byte(ch);
        if(!classification)
            throw std::logic_error("malformed UTF-8 stream");

        if(classification < 0) {
            if(continuationBytes == 0)
                throw std::logic_error("malformed UTF-8 stream");
            else
                continuationBytes -= 1;
        } else {
            if(continuationBytes != 0) {
                throw std::logic_error("malformed UTF-8 stream");
            }

            if (classification == 0) {
                // ASCII character

                if(ch == '<' || ch == '>' || ch == '&' || (attribute && (ch == '"' || ch == '\''))) {
                    if(it != runBegin) {
                        writeText(std::string_view(runBegin, it));
                    }

                    switch(ch) {
                        case '<':
                            writeText("&lt;");
                            break;

                        case '>':
                            writeText("&gt;");
                            break;

                        case '&':
                            writeText("&amp;");
                            break;

                        case '"':
                            writeText("&quot;");
                            break;

                        case '\'':
                            writeText("&apos;");
                            break;

                        default:
                            throw std::logic_error("unknown character to be escaped");
                            break;
                    }

                    runBegin = it;
                    ++runBegin;
                }
            } else {
                    continuationBytes = *classification;
            }
        }
    }

    if(runBegin != string.end()) {
        writeText(std::string_view(runBegin, string.end()));
    }
}

void TextXMLWriter::writeIndent() {
    for(int indent = m_indentLevel; indent > 0; indent--)
        writeText("  ");
}

