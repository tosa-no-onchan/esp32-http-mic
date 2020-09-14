/*
 * app_httpd_mic.c
 *
 *  Created on: 2020/09/08
 *      Author: nishi
 */

#include "app_httpd_mic.h"

#include "Wav.h"
//#include "audio_provider.h"
#include "i2s_acc.h"


/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
 */

static const char *TAG = "example";

static const char *index_html_s ="Mic WAV<br />\n\
<br />\n\
<br />\n\
<a href='/neko.wav'>/neko.wav</a><br />\n\
<br />\n\
<a href='/mic.wav'>esp-eye mic speach 2[sec]</a><br />\n\
<br />\n\
<a href='/mic_stream.html'>esp-eye mic stream</a><br />\n\
<br />\n\
";

static const char *mic_stream_html_s="\
<audio controls autoplay src='/mic_stream.wav' type='audio/wav'>\n\
Your browser does not support the\n\
<code>audio</code> element.\n\
</audio>\n\
";


static esp_err_t index_handler(httpd_req_t *req)
{
    //extern const unsigned char index_html_gz_start[] asm("_binary_index_html_gz_start");
    //extern const unsigned char index_html_gz_end[] asm("_binary_index_html_gz_end");
    //size_t index_html_gz_len = index_html_gz_end - index_html_gz_start;

    httpd_resp_set_type(req, "text/html");
    //httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

    return httpd_resp_send(req, (const char *)index_html_s, strlen(index_html_s));
}

static esp_err_t mic_stream_uri_handler(httpd_req_t *req){
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)mic_stream_html_s, strlen(mic_stream_html_s));
}

static esp_err_t neko_wav_handler(httpd_req_t *req)
{

    extern const unsigned char wav_dt_start[] asm("_binary_neko_wav_start");
    extern const unsigned char wav_dt_end[] asm("_binary_neko_wav_end");
    size_t wav_dt_len = wav_dt_end - wav_dt_start;

	printf("neko_wav_handler(): called\r\n");


    httpd_resp_set_type(req, "audio/wav");
    //httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

    esp_err_t rc = httpd_resp_send(req, (const char *)wav_dt_start, wav_dt_len);

	printf("neko_wav_handler(): #99\r\n");

    return rc;

}


static void stereo2mono_cp(uint8_t *dst,uint8_t *src,size_t buf_len){
	int lp=buf_len / 8;	// samples per i2s frame
	// I2S  32bit stereo data and 1 channel active ->  16bit mono data
    for (int i = 0; i < lp; ++i) {
      dst[0] = src[3];
      dst[1] = src[2];
      dst+=2;
      src+=8;
    }
}


static void stereo2mono_cp_16bit(uint8_t *dst,uint8_t *src,size_t buf_len){
	int lp=buf_len / 4;	// samples per i2s frame
	// I2S  32bit stereo data and 1 channel active ->  16bit mono data
    for (int i = 0; i < lp; ++i) {
      dst[0] = src[3];
      dst[1] = src[2];
      dst+=2;
      src+=4;
    }
}


static void stereo2mono_cp_16bit_0(uint16_t *dst,uint16_t *src,size_t buf_len){
	int lp=buf_len / 4;	// samples per i2s frame
	//int32_t c;
	// I2S  32bit stereo data and 1 channel active ->  16bit monoral data
	for (size_t i = 0; i < lp;i++){
		//c=(int32_t)src[1];
		//c-=32767;
		//dst[0] = (int16_t)c;
		dst[0]=src[1];
		dst++;
		src+=2;
	}
}

const int record_time = 2;  // second
const int headerSize = 44;
//const int waveDataSize = record_time * 88000;
const int waveDataSize = record_time * 16000 *2;
const int numCommunicationData = 8000;
const int numPartWavData = numCommunicationData/4;

static esp_err_t mic_wav_handler(httpd_req_t *req)
{
	//uint8_t header[headerSize];
	uint8_t communicationData[512];
	//uint8_t partWavData[numPartWavData];

	printf("mic_wav_handler(): called\r\n");
	int32_t dt_l;

	uint8_t *dtp = malloc(headerSize+waveDataSize);
	int dtp_off=0;
	if(dtp==NULL){
		 ESP_LOGE(TAG, "Error malloc");
		 printf("mic_wav_handler() :#1 malloc error\r\n");
		 return ESP_ERR_NO_MEM;
	}
    CreateWavHeader(dtp, waveDataSize);
    dtp_off+=headerSize;

    httpd_resp_set_type(req, "audio/wav");
    //httpd_resp_set_hdr(req, "Content-Encoding", "gzip");

    //for (int i=0; i < waveDataSize; i+=128) {	// 32bit -> increase 128 byte a i2s data[512]
    for (int i=0; i < waveDataSize; i+=256) {   //  16 bit -> increase 256 byte a i2s data[512]
      int16_t* audio_samples = NULL;
      int audio_samples_size = 0;
      dt_l = getSample(communicationData,sizeof(communicationData));
      if(dt_l != sizeof(communicationData)){
 		 ESP_LOGE(TAG, "i2s data length error");
 		 printf("mic_wav_handler() :#3 i2s data length error\r\n");
      }
      //stereo2mono_cp((uint8_t *)(dtp+i+dtp_off), (uint32_t *)communicationData,sizeof(communicationData));
      stereo2mono_cp_16bit((uint8_t *)(dtp+i+dtp_off), (uint32_t *)communicationData,sizeof(communicationData));
    }
    esp_err_t rc = httpd_resp_send(req, (const char *)dtp, headerSize+waveDataSize);
    free(dtp);

	printf("mic_wav_handler(): #99\r\n");

    return rc;
}

static esp_err_t mic_stream_wav_handler(httpd_req_t *req)
{
	//uint8_t header[headerSize];
	uint8_t communicationData[512];
	//uint8_t partWavData[numPartWavData];

	// streamming variables
    esp_err_t res = ESP_OK;
	char *part_buf[128];
    size_t _wav_buf_len = 0;
	int32_t dt_l;
	int first_f=1;
	int lp_ct=400;

	printf("mic_stream_wav_handler(): called\r\n");
	uint8_t *dtp = malloc(headerSize+waveDataSize);
	int dtp_off=0;
	if(dtp==NULL){
		 ESP_LOGE(TAG, "Error malloc");
		 printf("mic_stream_wav_handler() :#1 malloc error\r\n");
		 return ESP_ERR_NO_MEM;
	}
    CreateWavHeader(dtp, waveDataSize*lp_ct);
    dtp_off+=headerSize;

	httpd_resp_set_type(req, "audio/wav");
	httpd_resp_set_hdr(req, "Transfer-Encoding", "chunked");
    while(lp_ct>=0){
    	if(first_f){
    		_wav_buf_len=headerSize+waveDataSize;
    	}
    	else{
    		_wav_buf_len=waveDataSize;
    		dtp_off=0;
    	}
		//for (int i=0; i < waveDataSize; i+=128) {
		for (int i=0; i < waveDataSize; i+=256) {
		  int16_t* audio_samples = NULL;
		  int audio_samples_size = 0;
		  dt_l = getSample(communicationData,sizeof(communicationData));
		  if(dt_l != sizeof(communicationData)){
			 ESP_LOGE(TAG, "i2s data length error");
			 printf("mic_stream_wav_handler() :#3 i2s data length error\r\n");
		  }
		  //stereo2mono_cp((uint8_t *)(dtp+i+dtp_off), (uint32_t *)communicationData,512);
		  stereo2mono_cp_16bit((uint8_t *)(dtp+i+dtp_off), (uint32_t *)communicationData,sizeof(communicationData));
		}
		res = httpd_resp_send_chunk(req, (const char *)dtp, _wav_buf_len);
		if (res != ESP_OK)
		{
			 ESP_LOGE(TAG, "httpd_resp_send_chunk():#5 error");
			 break;
		}
		first_f=0;
		lp_ct--;
    }
    if(res == ESP_OK){
    	res = httpd_resp_send_chunk(req, (const char *)dtp, 0);
    }
    free(dtp);
	printf("mic_stream_wav_handler(): #99\r\n");
    return res;
}


static const httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL};


static const httpd_uri_t neko_wav = {
    .uri       = "/neko.wav",
    .method    = HTTP_GET,
    .handler   = neko_wav_handler,
    .user_ctx  = NULL
};


static const httpd_uri_t mic_wav = {
    .uri       = "/mic.wav",
    .method    = HTTP_GET,
    .handler   = mic_wav_handler,
    .user_ctx  = NULL
};


static const httpd_uri_t mic_stream_wav = {
    .uri       = "/mic_stream.wav",
    .method    = HTTP_GET,
    .handler   = mic_stream_wav_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t mic_stream_uri = {
    .uri       = "/mic_stream.html",
    .method    = HTTP_GET,
    .handler   = mic_stream_uri_handler,
    .user_ctx  = NULL
};


httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &neko_wav);
        httpd_register_uri_handler(server, &mic_wav);
        httpd_register_uri_handler(server, &mic_stream_uri);
        httpd_register_uri_handler(server, &mic_stream_wav);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}
