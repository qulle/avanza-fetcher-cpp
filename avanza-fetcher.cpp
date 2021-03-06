// Author: Qulle 2021-03-06
// Github: github.com/qulle/avanza-fetcher-cpp
// Editor: vscode

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <regex>
#include <chrono>
#include <thread>
#include <exception>

// JSON-parser from @ https://github.com/nlohmann/json
#include "hpp/json.hpp"
using json = nlohmann::json;

// HTTPLib from @ https://github.com/yhirose/cpp-httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "hpp/httplib.hpp"

namespace Config
{
    int const LAST_GROUP{3};    // Regex capture group
    int const SLEEP_TIME{1000}; // Delay between each request, milliseconds
}

namespace HTTP_STATUS
{
    int const OK{200};
}

// Don't touch if you don't know what you are doing, you have been warned // Qulle
namespace RegexObjects
{
    std::regex const CHANGE_PROCENT{"(changePercent)(.*?)>([\\+|-]?\\d+,\\d+\\s+%)"};
    std::regex const CHANGE_SEK{"(change)(.*?)>([\\+|-]?\\d+,\\d+\\s+SEK)"};
    std::regex const BUY{"(buyPrice)(.*?)>(\\d+,?\\d+|-)"};
    std::regex const SELL{"(sellPrice)(.*?)>(\\d+,?\\d+|-)"};
    std::regex const LATEST{"(data-e2e=\"quoteLastPrice\")(.*?)>(\\d+,?\\d+|-)"};
    std::regex const HIGHEST{"(highestPrice)(.*?)>(\\d+,?\\d+|-)"};
    std::regex const LOWEST{"(lowestPrice)(.*?)>(\\d+,?\\d+|-)"};
    std::regex const AMOUNT{"(totalVolumeTraded)(.*?)>([\\d|\\s|]+)"};
}

template <typename T>
struct Testable
{
    T data;
    bool error{};
    std::string errorMessage{};
};

Testable<json> getJsonDataFromFile(std::string const&);
Testable<std::string> getWebPage(std::string const&);
std::string getRegexValue(std::string const&, std::regex const&);

int main(int argc, char* argv[]) 
{
    if(argc <= 1)
    {
        std::cout << "Run using: " << argv[0] << " json-file" << std::endl;
        return 0;
    }

    Testable<json> jsonObject = getJsonDataFromFile(argv[1]);

    if(jsonObject.error)
    {
        std::cout << jsonObject.errorMessage << std::endl;
        return 0;
    }

    Testable<std::string> webRequestObject{};
    for(auto const& group : jsonObject.data.items()) 
    {
        std::cout << std::endl
            << group.key() << std::endl 
            << std::setfill('-') << std::setw(142) << '-' << std::setfill(' ') << std::endl
            << "Name" 
            << std::setw(27) << "Today %"
            << std::setw(19) << "Today SEK"
            << std::setw(13) << "Buy"
            << std::setw(14) << "Sell"
            << std::setw(16) << "Latest"
            << std::setw(17) << "Highest"
            << std::setw(16) << "Lowest"
            << std::setw(16) << "Amount" << std::endl;

        for(auto const& item : group.value())
        {
            webRequestObject = getWebPage(item["url"]);

            if(webRequestObject.error)
            {
                std::cout << webRequestObject.errorMessage << std::endl;
            }
            else 
            {
                std::string HTMLBuffer{webRequestObject.data};
            
                std::string name{item["name"]};
                std::cout << name
                    << std::setw(31 - name.length()) << getRegexValue(HTMLBuffer, RegexObjects::CHANGE_PROCENT)
                    << std::setw(19) << getRegexValue(HTMLBuffer, RegexObjects::CHANGE_SEK)
                    << std::setw(13) << getRegexValue(HTMLBuffer, RegexObjects::BUY)
                    << std::setw(14) << getRegexValue(HTMLBuffer, RegexObjects::SELL)
                    << std::setw(16) << getRegexValue(HTMLBuffer, RegexObjects::LATEST)
                    << std::setw(17) << getRegexValue(HTMLBuffer, RegexObjects::HIGHEST)
                    << std::setw(16) << getRegexValue(HTMLBuffer, RegexObjects::LOWEST) 
                    << std::setw(16) << getRegexValue(HTMLBuffer, RegexObjects::AMOUNT) << std::endl;

                // Sleep so that server not block two many requests
                std::this_thread::sleep_for(std::chrono::milliseconds(Config::SLEEP_TIME));
            }
        }

        std::cout << std::endl;
    }

    return 0;
}

Testable<json> getJsonDataFromFile(std::string const& filepath)
{
    std::ifstream ifs{filepath};
    if(!ifs)
    {
        return {NULL, true, "The file '" + filepath + "' was not found"};
    }

    json jsonFromFile{}; 

    try
    {
        ifs >> jsonFromFile;
    }
    catch(std::exception const& e)
    {
        return {NULL, true, "Error decoding the json-file. Check the syntax"};
    }

    return {jsonFromFile, false, {}};
}

Testable<std::string> getWebPage(std::string const& url)
{
    // Prepair the url for the httpClient
    std::string const delimiter{"/aktier"};
    std::string const domain{url.substr(0, url.find(delimiter))};
    std::string const endpoint{url.substr(url.find(delimiter), url.length())};

    // Make the http(s) request
    httplib::Client client(domain.c_str());
    auto response = client.Get(endpoint.c_str());

    if(response->status != HTTP_STATUS::OK) 
    {
        return {{}, true, "Error fetching '" + url + "' status " + std::to_string(response->status)};
    }

    // Clean the response, containts hard-coded-spaces
    // The hard-coded-spaces are not found by the \s in the regex expressions used later to extract the stock informations
    std::string const nonBreakingSpace{"\xc2\xa0"};
    std::string const normalSpace{" "};

    std::string result{std::regex_replace(
        response->body, 
        std::regex(nonBreakingSpace), 
        normalSpace
    )};

    return {result, false, {}};
}

std::string getRegexValue(std::string const& HTMLBuffer, std::regex const& regex)
{
    std::string result{"***"};
    std::smatch match{};

    if(std::regex_search(HTMLBuffer, match, regex))
    {
        result = match.str(Config::LAST_GROUP);
    }

    return result;
}