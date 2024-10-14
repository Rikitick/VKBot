#include <iostream>
#include <fstream>
#include <string>
#include<windows.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#define MAXNAME     501

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string request(std::string url) {
	CURL *curl;
  	//CURLcode res;
  	std::string readBuffer;

  	curl = curl_easy_init();
  	if(curl) {
    	curl_easy_setopt(curl, CURLOPT_URL, url.data());

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // ВАЖНО ДЛЯ HTTPS!!!
    	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);// ВАЖНО ДЛЯ HTTPS!!!

    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    	curl_easy_perform(curl);
    	curl_easy_cleanup(curl);
  	}
	return readBuffer;
}

void int_to_str(char* ar, int number) {

    for (int i = 0; i < 9 && number != 0; ++i) {
        ar[i] = number % 10 + 48;
        number /= 10;
    }

    strrev(ar);
}

int main(void)
{
	std::string group_name, group_domain, token;

	std::cout << "Enter group DOMAIN: ";
	std::cin >> group_domain;

	std::cout << "Enter TOKEN: ";
	std::cin >> token;

	std::string url = "https://api.vk.com/method/groups.getById?group_id=" + group_domain + "&access_token=" + token + "&v=5.199";
	std::string res = request(url);
	nlohmann::json resj = nlohmann::json::parse(res);
	std::string name = resj["response"]["groups"][0]["name"];

	for (char x : name) {
		if (x == ' ')
			group_name += "%20";
		else
			group_name += x;
	}
	std::cin.get();

	url = "https://api.vk.com/method/groups.getMembers?group_id=" + group_domain + "&access_token=" + token + "&v=5.199";
	res = request(url);
	resj = nlohmann::json::parse(res);
	int cnt = resj["response"]["count"];
	
	cnt = cnt / 1000 + 1;
	for (int i = 0; i < cnt/* 1 */; ++i) {
		int memcnt = i * 1000;
		char number[20]{48};
		int_to_str(number, memcnt);
		url = "https://api.vk.com/method/groups.getMembers?group_id=" + group_domain + "&fields=domain&offset=" + static_cast<std::string>(number) + "&access_token=" + token + "&v=5.199";
		res = request(url);
		resj = nlohmann::json::parse(res);
		nlohmann::json pers = resj["response"]["items"];

		for (int j = 0; j < 999; ++j) {
			bool flag = true;
			std::ifstream file("pers.txt");
			if (file.is_open()) {
				while (!file.eof()) {
					std::string x;
					file >> x;
					if (static_cast<std::string>(pers[j]["domain"]) == x) {
						flag = false;
						break;
					}
				}
				file.close();
			}
			
			if (flag) {
				std::ofstream file("pers.txt", std::ios::app);
				memset(number, 0, 20);
				int_to_str(number, static_cast<int>(static_cast<int>(pers[j]["id"]))); 

				url = "https://api.vk.com/method/messages.send?user_id=" + static_cast<std::string>(number) + "&random_id=100&message=" + static_cast<std::string>(pers[j]["first_name"]) + ",%20здравствуйте.%0A%0AНашёл%20Вас%20в%20сообществе%20\"" + group_name + "&access_token=" + token + "&v=5.199";
				//url = "https://api.vk.com/method/messages.send?user_id=" + static_cast<std::string>(number) + "&random_id=0&message=Нашёл%20Вас%20в%20сообществе%20\"" + group_name + "\".&access_token=" + token + "&v=5.199";
				res = request(url);
				std::cout << pers[j]["domain"]  << " --- " << res << "\n" <<  std::endl;
				resj = nlohmann::json::parse(res);
				auto sp = res.find_first_of(':');
    			res.erase(res.begin() + sp, res.end());
				if (res != "{\"response\"" && static_cast<std::string>(resj["error"]["error_msg"]) == "Captcha needed") {
					file.close();
					std::cout << "Wait" << std::endl;
					Sleep(60000);
					j--;
					continue;
				}
				file << static_cast<std::string>(pers[j]["domain"]) << " ";
			}
			file.close();
		}
	}

	std::cout << "END" << std::endl;

	std::cin.get();

  	return 0;
}
