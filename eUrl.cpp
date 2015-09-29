// #define DEBUG
#define DEBUG_OUTPUT Serial

#include "eUrl.h"
#include <Arduino.h>
#include <Stream.h>
#include <ESP8266WiFi.h>

void eUrl::setTimeout(int time) {
	_timeOut = time;
}

void eUrl::get(String url, String data, THandlerFunction fn) {
	get(url + "?" + data, fn);
}

void eUrl::get(String url, THandlerFunction fn) {
	_httpCode = 0;
	_method = "GET";
	_data = "";
	if (!_parseUrl(url)) return;
	_ReqServer();
	fn();
}

void eUrl::post(String url, String data, THandlerFunction fn) {
	_httpCode = 0;
	_method = "POST";
	_data = data;
	if (!_parseUrl(url)) return;
	_ReqServer();
	fn();
}

int eUrl::code() {
	return _httpCode;
}

String eUrl::content() {
	return _content;
}

String eUrl::handle(String keyname) {
	int i = _qCount-1;
	while(_qKey[i]!=keyname) i--;
	return _qVal[i];
}

String eUrl::handleKey(int index) {
	return _qKey[index];
}

String eUrl::handleVal(int index) {
	return _qVal[index];
}

int eUrl::handleCount() {
	return _qCount;
}

bool eUrl::_parseUrl(String url) {
	if (url.substring(0, 8)=="https://") {
		return false;
	} else if (url.substring(0, 7)=="http://") {
		int endHostPort = url.indexOf("/", 7);
		if (endHostPort<0)
			endHostPort = url.length();
		int startPort = url.indexOf(":", 7);
		if (startPort>0&&endHostPort>0) {
			_host = url.substring(7, startPort);
			_port = url.substring(startPort+1, endHostPort).toInt();
		} else if (startPort<0&&endHostPort>0) {
			_host = url.substring(7, endHostPort);
			_port = 80;
		} else {
			return false;
		}
		_url = url.substring(endHostPort);
		if (_url=="") _url = "/";
#ifdef DEBUG
    DEBUG_OUTPUT.println("Host: " + _host);
	DEBUG_OUTPUT.println("Port: " + String(_port));
	DEBUG_OUTPUT.println("Url: " + _url);
#endif
		return true;
	}
	return false;
}

bool eUrl::_ReqServer() {
	WiFiClient client;
	
	char host[255];
	_host.toCharArray(host, _host.length()+1);
	
	if (!client.connect(host, _port)) {
#ifdef DEBUG
    DEBUG_OUTPUT.println("Connection failed");
#endif
		return false;
	}
#ifdef DEBUG
    DEBUG_OUTPUT.println("Connection success");
#endif
	String req = "";
	req += _method + " " + _url + (_method=="GET"&&_data!="" ? "?" + _data : "") +" HTTP/1.1\r\n";
	req += "Host: " + _host + "\r\n";
    req += "Connection: close\r\n";
	req += "User-Agent: eUrl libraries on esp8266\r\n";

	if (_method == "POST") {
		req += "Content-Type: application/x-www-form-urlencoded\r\n";
		req += "Content-Length: " + String(_data.length()) + "\r\n";
	}
	req += "\r\n";
	if (_method == "POST") req += _data;
#ifdef DEBUG
    DEBUG_OUTPUT.println(req);
#endif
	client.print(req);
	delay(20);
	bool hEnd = false;
	_content = "";
	int _contentLen = 0;
	unsigned long Start = millis();
	while(client.connected()&&Start+(_timeOut*1000)>millis()) {
		if (client.available()) {
			if (!hEnd) {
				String line = client.readStringUntil('\r');
				client.readStringUntil('\n');
#ifdef DEBUG
    DEBUG_OUTPUT.println(line);
#endif
				if (line.indexOf("HTTP/1.1")>=0) {
					int StartCode = line.indexOf(" ");
					int endCode = line.indexOf(" ", StartCode+1);
					_httpCode = line.substring(StartCode+1, endCode).toInt();
				} else if (line=="") {
					hEnd = true;
				} else {
					int endKey = line.indexOf(":");
					if (endKey>0&&_qCount<20) {
						_qKey[_qCount] = line.substring(0, endKey);
						_qVal[_qCount] = line.substring(endKey+1);
						_qVal[_qCount].trim();
						_qCount++;
					}
				}
			} else {
				_content += (char)client.read();
			}
		}
	}
#ifdef DEBUG
    DEBUG_OUTPUT.println(_content);
#endif
	return true;
}