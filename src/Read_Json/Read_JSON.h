#ifndef READ_JSON_H
#define READ_JSON_H

#include <cstdint>
#include <string>
#include <curl/curl.h>

namespace LLTD {

struct LLTrackingData {
    int fID;
    float fIDtx;
    float fIDty;
    std::string neuralClass;
    float conf;
    float neuraltx;
    float neuralty;
};

class ReadJSON {
public:
    ReadJSON();
    ~ReadJSON();
    LLTrackingData getLLData(const std::string &LLIP);

private:
    static std::size_t callback(const char *in, std::size_t size, std::size_t num, std::string *out);
};

} // namespace LLTD

#endif // READ_JSON_H
