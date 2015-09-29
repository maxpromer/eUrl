
#include <Arduino.h>
#include <Stream.h>
#include <ESP8266WiFi.h>
#include "String.h"

class eUrl {
public:
	typedef std::function<void(void)> THandlerFunction;
	void get(String url, String data, THandlerFunction fn);
	void get(String url, THandlerFunction fn);
	void post(String url, String data, THandlerFunction fn);
	void setTimeout(int time);
	int code();
	String content();
	
	String handle(String keyname);
	String handleKey(int index);
	String handleVal(int index);
	int handleCount();

protected:
  bool _ReqServer();
  bool _parseUrl(String url);
  
  String _host;
  int _port;
  String _url;
  String _method;
  String _data;
  String _content;
  int _httpCode;
  String _qKey[20];
  String _qVal[20];
  unsigned int _qCount = 0;

  int _timeOut = 30; // 30 Sec.

};