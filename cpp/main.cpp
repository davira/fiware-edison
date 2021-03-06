//################# LIBRARIES ################
#include <stdio.h>
#include <curl/curl.h>
#include "mraa.hpp"
#include <map>
#include <unistd.h>
#include <iostream>
#include <sstream>

using namespace std;


int read() 
{
    static int i = 0;
    return ++i;
}

string int_to_string(int i) 
{
    stringstream ss;
    ss << i;
    return ss.str();
}

//################ FIWARE VARIABLES ################

string FIWARE_APIKEY = "lzSnQpEsC0lZVOVXaqZK";
string FIWARE_DEVICE = "myEdison";
string FIWARE_SERVER = "test.ttcloud.net";
string FIWARE_PORT = "8082";
string url="http://"+FIWARE_SERVER+":"+FIWARE_PORT+"/iot/d?i="+FIWARE_DEVICE+"&k="+FIWARE_APIKEY;

string body="";

void readsensors()
{
	// check that we are running on Galileo or Edison
	mraa_platform_t platform = mraa_get_platform_type();

	// create an analog input object for each sensor
	mraa::Aio* a0_pin = new mraa::Aio(0);
	mraa::Aio* a1_pin = new mraa::Aio(1);
	// Dictionary to persist Sensor values
	typedef map <string, string> Table;
	Table measures;
	//Reading Light sensor
	int lum = a0_pin->read();

	//Reading Button sensor
	int touch = a1_pin->read();
	string pulse = "false";
		if(touch>100){
		  pulse = "true";
		}
	// save lumininosity value in the dictionary
	measures["p"]=pulse;
	measures["l"]=int_to_string(lum);

	for (Table::const_iterator it = measures.begin(); it != measures.end(); ++it) {
		//add measures separator "#" to the body only when it is not the last measure
		if (body != "") {
			 body += "#";
		}
		//FIWARE IoT Stack body message
		body += it->first + "|" + it->second;
	}

}

void postmeasures(){

	CURL *curl;

	string url="http://"+FIWARE_SERVER+":"+FIWARE_PORT+"/iot/d?i="+FIWARE_DEVICE+"&k="+FIWARE_APIKEY;

	char bodyChar[body.length()];
	strcpy(bodyChar, body.c_str());

	char urlChar[url.length()];
	strcpy(urlChar, url.c_str());


	curl = curl_easy_init();
	
	//Send body to IoT Stack platform
	if(curl) {
		cout << "Sending -> " << body << endl;
	
		curl_easy_setopt(curl, CURLOPT_URL, urlChar);
		struct curl_slist *headers=NULL;
		headers = curl_slist_append(headers, "Content-Type:");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyChar);
	
		curl_easy_perform(curl);
	}
	curl_global_cleanup();
	// Clean body before the following iteration
	body="";

}

int main()
{
	// loop forever printing the input value every second
	for (;;) {

		readsensors();
		postmeasures();
		sleep(1);
	}
	return 0;
}
