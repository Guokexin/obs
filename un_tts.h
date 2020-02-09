#ifndef _UN_TTS_H_
#define _UN_TTS_H_

#include <string>
#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "nlsClient.h"
#include "nlsEvent.h"
#include "speechSynthesizerRequest.h"

using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ios;
using namespace std;
#define SAMPLE_RATE 16000

using AlibabaNls::NlsClient;
using AlibabaNls::NlsEvent;
using AlibabaNls::LogDebug;
using AlibabaNls::LogInfo;
using AlibabaNls::SpeechSynthesizerCallback;
using AlibabaNls::SpeechSynthesizerRequest;




// 自定义线程参数
struct ParamStruct {
	string text;
	string token;
	string appkey;
    string audioFile;
};

// 自定义事件回调参数
struct ParamCallBack {
    string binAudioFile;
	ofstream audioFile;
};

class AliTTS {
  public:
    std::string accessKeySecret;
    std::string accessKeyId;
    std::string domain;
    std::string serverVersion;
    std::string serverResourcePath;
    std::string app_key;
  public:
    std::string get_token();
    int gen_voice(std::string& msg, std::string& voice);
/*
    void OnSynthesisStarted(NlsEvent* cbEvent, void* cbParam); 
    void OnSynthesisCompleted(NlsEvent* cbEvent, void* cbParam);
    void OnSynthesisTaskFailed(NlsEvent* cbEvent, void* cbParam);
    void OnSynthesisChannelClosed(NlsEvent* cbEvent, void* cbParam);
    void OnBinaryDataRecved(NlsEvent* cbEvent, void* cbParam);
    void* pthreadFunc(void* arg);
    int speechSynthesizerFile(const char* token, const char* appkey);
    int speechSynthesizerMultFile(const char* token, const char* appkey);
*/
};
#endif
