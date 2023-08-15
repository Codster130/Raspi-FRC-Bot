#include "Read_JSON.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

namespace LLTD {

ReadJSON::ReadJSON() {
    //curl_global_init(CURL_GLOBAL_DEFAULT);
}

ReadJSON::~ReadJSON() {
    //curl_global_cleanup();
}

std::size_t ReadJSON::callback(const char *in, std::size_t size, std::size_t num, std::string *out) {
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

LLTrackingData ReadJSON::getLLData(const std::string &LLIP) {
    CURL *curl;
    CURLcode res;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();

    LLTrackingData Data;
    // Set remote URL.
    const std::string url(LLIP);

    // Response information.
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());

    // Don't bother trying IPv6, which would increase DNS resolution time.
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

    // Don't wait forever, time out after 5 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

    // Run our HTTP GET command, capture the HTTP response code, and clean up.
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);   
    
    if (httpCode == 200)
    {
        // Response looks good - done using Curl now.  Try to parse the results
        // and print them out.
        Json::Value jsonData;
        Json::Reader jsonReader;

        if (jsonReader.parse(*httpData.get(), jsonData))
        {
            Data.fID = jsonData["Results"]["Fiducial"][0]["fID"].asInt();
            Data.fIDtx = jsonData["Results"]["Fiducial"][0]["tx"].asFloat();
            Data.fIDty = jsonData["Results"]["Fiducial"][0]["ty"].asFloat();
            Data.neuralClass = jsonData["Results"]["Detector"][0]["class"].asString();
            Data.conf = jsonData["Results"]["Detector"][0]["conf"].asFloat();
            Data.neuraltx = jsonData["Results"]["Detector"][0]["tx"].asFloat();
            Data.neuralty = jsonData["Results"]["Detector"][0]["ty"].asFloat();

            return Data;
        }
        else
        {
            std::cout << "Could not parse HTTP data as JSON" << std::endl;
            std::cout << "HTTP data was:\n" << *httpData.get() << std::endl;
            return Data;
        }
    }
    else
    {
        std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
        curl_easy_cleanup(curl);
        Data.fID = 0;
        Data.fIDtx = 0;
        Data.fIDty = 0;
        Data.neuralClass = "0";
        Data.conf = 0;
        Data.neuraltx = 0;
        Data.neuralty = 0;
        return Data;
    }

    httpCode = 0;
    curl_easy_cleanup(curl);
    }
} // namespace LLTD
