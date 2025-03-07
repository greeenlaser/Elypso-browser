//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#include <iostream>
#include <regex>
#include <filesystem>
#include <vector>

//external
#include "curl.h"

//browser
#include "networkmanager.hpp"
#include "httprequest.hpp"
#include "httpclient.hpp"
#include "httpresponse.hpp"
#include "core.hpp"
#include "htmlparser.hpp"
#include "content.hpp"

using Core::Browser;
using Graphics::Content;

using std::cout;
using std::regex;
using std::smatch;
using std::regex_match;
using std::filesystem::path;
using std::filesystem::exists;
using std::vector;

namespace Networking
{
	bool NetworkManager::HasInternet()
	{
		if (localFiles.empty()) FillLocalFilePaths();

		CURL* curl = curl_easy_init();
		if (!curl) 
		{
			cout << "Error: Failed to initialize LibCurl!\n";
			return false;
		}

		string curlCertPath = (path(Browser::filesPath) / "certifications" / "cacert.pem").string();
		if (!exists(curlCertPath))
		{
			cout << "Error: Failed to find LibCurl certification file path!\n";
			curl_easy_cleanup(curl);
			return false;
		}

		//dont download anything
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
		//wait 5 seconds
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
		//basic user agent
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 11.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36 Edg/122.0.0.0");

		//assign curl certification path
		curl_easy_setopt(curl, CURLOPT_CAINFO, curlCertPath.c_str());

		//let OpenSSL auto-negotiate TLS version
		curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_MAX_DEFAULT);

		vector<string> testUrls = { 
			"https://www.google.com", 
			"https://www.cloudflare.com", 
			"https://example.com" };

		bool hasInternet = false;
		for (const auto& url : testUrls)
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			CURLcode res = curl_easy_perform(curl);

			if (res == CURLE_OK)
			{
				hasInternet = true;
				break;
			}
			else
			{
				cout << "Failed to connect to " << url << " - Error: " << curl_easy_strerror(res) << "\n";
			}
		}

		curl_easy_cleanup(curl);
		return hasInternet;
	}

	void NetworkManager::ParseURL(const string& URL)
	{
		if (URL == "")
		{
			cout << "Error: Website link cannot be empty!";
			return;
		}

		if (URL[0] == '/') ParseLocalFileURL(URL);
		else ParseWebsiteURL(URL);
	}

	void NetworkManager::FillLocalFilePaths()
	{
		localFiles = unordered_map<string, string>
		{
			{ "/home",       "home.html" },
			{ "/jsRequired", "jsRequired.html" },
			{ "/about",      "about.html" }
		};
	}

	void NetworkManager::ParseLocalFileURL(const string& URL)
	{
		auto it = localFiles.find(URL);
		if (it == localFiles.end())
		{
			cout << "Error: Failed to find local URL '" << URL << "'!\n";
			return;
		}
		else
		{
			string fileName = it->second;
			string fullURL = (path(Browser::filesPath) / "pages" / fileName).string();
			Content::LoadHTMLFromFile(fullURL);
		}
	}

	void NetworkManager::ParseWebsiteURL(const string& URL)
	{
		regex urlRegex(R"(^(https?)://([^/]+)(/?.*)$)");
		smatch match;

		if (!regex_match(URL, match, urlRegex))
		{
			cout << "Error: Invalid url '" + URL + "'!\n";
			return;
		}

		string protocol = match[1];
		string host = match[2];
		string path = match[3];

		if (path.empty()) path = "/";

		cout << "Parsed URL - Protocol: " << protocol
			<< ", Host: " << host
			<< ", Path: " << path << "\n";

		//create a new request
		HTTPRequest request(protocol, host, path);

		//send the new request to httpclient
		HTTPClient client;
		HTTPResponse response = client.SendRequest(request);

		cout << "HTTP Status: " << response.GetStatusCode() << "\n";

		HTMLParser::ParseHTML(response.GetBody());
	}
}