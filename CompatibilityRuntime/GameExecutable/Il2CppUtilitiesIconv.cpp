#include <Il2CppUtilities.h>

#include <iconv.h>

#include <stdexcept>
#include <system_error>

class IconvEncoding {
public:
    IconvEncoding(const char *from, const char *to) {
        m_handle = iconv_open(to, from);
        if(m_handle == reinterpret_cast<iconv_t>(-1)) {
            throw std::runtime_error("iconv_open failed");
        }
    }

    ~IconvEncoding() {
        iconv_close(m_handle);
    }

    IconvEncoding(const IconvEncoding &other) = delete;
    IconvEncoding &operator =(IconvEncoding &other) = delete;

    inline operator iconv_t() const {
        return m_handle;
    }

private:
    iconv_t m_handle;
};

template<typename InputCharacter>
static std::string convertEncoding(const IconvEncoding &encoding, const std::basic_string_view<InputCharacter> &src) {
    if(src.empty())
        return {};

    std::string output;

    output.resize(std::max<size_t>(1, src.size() * sizeof(InputCharacter)));

    auto inbuf = const_cast<char *>(reinterpret_cast<const char *>(src.data()));
    size_t inBytesLeft = src.size() * sizeof(InputCharacter);

    char *outbuf = output.data();
    size_t outBytesLeft = output.size();

    size_t result;
    do {
        result = iconv(encoding, &inbuf, &inBytesLeft, &outbuf, &outBytesLeft);
        if(result == static_cast<size_t>(-1)) {
            if(errno == E2BIG) {
                auto bytesWritten = outbuf - output.data();

                output.resize(output.size() * 2);

                outbuf = output.data() + bytesWritten;
                outBytesLeft = output.size() - bytesWritten;
            } else {
                throw std::system_error(errno, std::generic_category());
            }
        }
    } while(result == static_cast<size_t>(-1));

    output.resize(output.size() - outBytesLeft);
    output.shrink_to_fit();

    return output;
}


std::string utf16ToUtf8(const std::u16string_view &src) {
    static IconvEncoding encoding("UTF-16", "UTF-8");

    return convertEncoding(encoding, src);
}
