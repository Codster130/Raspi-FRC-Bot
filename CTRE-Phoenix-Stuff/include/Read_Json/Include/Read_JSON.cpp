//g++ Read_JSON.cpp -ljsoncpp -lcurl -o main.out
#include "Read_JSON.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include <curl/curl.h>
#include <json/json.h>

using namespace TrackData;

namespace
{
    std::size_t callback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
}

LLTrackingData LLTD::getLLData(std::string LLIP){
    LLTrackingData Data;
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    while(1)
    {
        curl = curl_easy_init();
        // Set remote URL.
        const std::string url(LLIP);

        // Response information.
        long httpCode(0);
        std::unique_ptr<std::string> httpData(new std::string());

        // Don't bother trying IPv6, which would increase DNS resolution time.
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

        // Don't wait forever, time out after 10 seconds.
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
            //std::cout << "\nGot successful response from " << url << std::endl;

            // Response looks good - done using Curl now.  Try to parse the results
            // and print them out.
            Json::Value jsonData;
            Json::Reader jsonReader;

            if (jsonReader.parse(*httpData.get(), jsonData))
            {
                //std::cout << "Successfully parsed JSON data" << std::endl;
                //std::cout << "\nJSON data received:" << std::endl;
                //std::cout << jsonData.toStyledString() << std::endl;

                Data.fID = jsonData["Results"]["Fiducial"][0]["fID"].asInt();
                // const std::string Results(jsonData["Results"].toStyledString());
                // const std::string Bardcode(jsonData["Bardcode"].toStyledString());
                // const std::string Classifier(jsonData["Classifier"].toStyledString());
                // const std::string Detector(jsonData["Detector"].toStyledString());
                //const std::string fID(jsonData["Fiducial"].asString());
                Data.tx = jsonData["Results"]["Fiducial"][0]["tx"].asFloat();
                Data.ty = jsonData["Results"]["Fiducial"][0]["ty"].asFloat();

                const Json::Value& Fiducial = jsonData["Fiducial"]; // array of characters

                std::cout << "Natively parsed:" << std::endl;
                //std::cout << "\tResults: " << Results << std::endl;
                std::cout << "\tfID: " << Data.fID << std::endl;
                std::cout << "\ttx: " << Data.tx << std::endl;
                std::cout << "\tty: " << Data.ty << std::endl;
                std::cout << std::endl;
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
            return Data;
        }

    httpCode = 0;
    curl_easy_cleanup(curl);
    //return 0;
    }
}