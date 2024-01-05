/*******************************************************************************************************
  --------------------------------------------------------------------------------------------------------
  Create date : DEC 2023
  Project     : YOUTUBE COUNTER
  Version     : 0.1
  Author      : Sotiris Bellos
  email       : sotirisbell.com@gmail.com
  --------------------------------------------------------------------------------------------------------
********************************************************************************************************/
/************************************************************************************** DEBUG       */

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif



/************************************************************************************** INCLUDES       */
#include "Arduino.h"
#include <WiFi.h>
//#include <WiFiClientSecure.h>
#include <EEPROM.h>
#include <WiFiClient.h>


/************************************************************************************** youtube   API      */
#include <YoutubeApi.h>
#include <ArduinoJson.h>

const char* ssid = "xxxxx";
const char* password = "xxxxx";
#define API_KEY "xxxxx"
#define CHANNEL_ID "xxxxx"
//WiFiClientSecure client;

WiFiClient client;
YoutubeApi api(API_KEY, client);
long  Mls = 0, Mcs = 0;
int Mmon = 0, Mms = 0;
#define EEPROM_SIZE 1024



/************************************************************************************** youtube   Analytics      */
#include <HTTPClient.h>
boolean WIFI_STATUS = false;
String   AllVideoList = "";                   //     
uint32_t AllVideoCount = 0;
uint32_t wC = 0;                              //  
uint32_t lC = 0;                              //          2,3
uint32_t dC = 0;                              //       4,6
uint32_t cC = 0;




/************************************************************************************** youtube   Sight      */


#include <YouTubeSight.h>

#define CHANNEL_GUID "25496ec1-2f5a-466d-998e-31120014103d" //ENTER YOUR CHANNEL GUID
YouTubeSight sight(CHANNEL_GUID, client);
/************************************************************************************** display         */
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>


// Defining size, and output pins
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   36
#define DATA_PIN  21
#define CS_PIN    34

//MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
MD_Parola PP = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

/************************************************************************************** DS18b20         */
#include <OneWire.h>
#include <DallasTemperature.h>
float TIN, TOUT, OTIN = 0, OTOUT = 0;
OneWire ds18in = (12);
OneWire ds18out = (11);
DallasTemperature sensor1(&ds18in);
DallasTemperature sensor2(&ds18out);

/************************************************************************************** RTC        */
#include "RTClib.h"

RTC_DS1307 rtc;
//int DS_yy, DS_MM, DS_dd, DS_hh, DS_mm, DS_ss;
int DD = 0;// , RTC = 0;
char DoW[7][12] = { "su", "mo", "tu", "we", "th", "fr", "sa" };
//char DoW[7][12] = { "KY", "DE", "TR", "TE", "PE", "PA", "SA" };
String NTP_tmp, RTC_tmp;
/************************************************************************************** NTP      */
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 2 * 60 * 60; //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 0;  //Replace with your daylight offset (seconds)

unsigned long previousMillis = 0;
unsigned long interval = 30000;
//int NT_yy, NT_MM, NT_dd, NT_hh, NT_mm, NT_ss;// , NTP = 0;
String TIME, DATE;
/************************************************************************************** BUZZER  -LED*/
#include "pitches.h"

#define LED 2



/************************************************************************************** STATITICS      */
typedef struct {
	int month;
	int day;
	long subs;
	long views;
	long likes;
	long comms;
} record_type0;

record_type0 D_SAVED;
record_type0 D_NOW;
record_type0 D_MONTLY;
record_type0 D_MTEMP;
/*******************************************************************************************************/
typedef struct {
	bool EXISTS;
	int DY;
	int DM;
	int DD;
	int TH;
	int TM;
	int TS;
	int DOW;
} record_type;

record_type RTC;
record_type NTP;
record_type TIME_NOW;

/*******************************************************************************************************/


uint8_t degCI[] = { 8, 0x00, 0x02, 0x7C, 0x44, 0x44, 0x00, 0x10, 0x28, }; // Deg C IN
uint8_t degCO[] = { 8, 0x00, 0x02, 0x7C, 0x44, 0x44, 0x00, 0x28, 0x10, }; // Deg C OUT
uint8_t arup[] = { 5, 0x10,0x08,0x34,0x08,0x10,0x00,0x00,0x00, }; // arrow up
uint8_t	ardo[] = { 5, 0x08,0x10,0x2C,0x10,0x08,0x00,0x00,0x00, }; // arrow down
//uint8_t es[] = { 3, 0x48, 0x54, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	s
//uint8_t ve[] = { 3, 0x3C, 0x40, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	v
//uint8_t el[] = { 3,0x7C, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	l
uint8_t es[] = { 4, 0x00,0x58,0x68,0x68,0x00,0x00,0x00,0x00, }; //	s
uint8_t ve[] = { 4, 0x00,0x38,0x40,0x38,0x00,0x00,0x00,0x00, }; //	v
uint8_t el[] = { 4,0x00,0x78,0x40,0x40,0x00,0x00,0x00,0x00, }; //	l

uint8_t ce[] = { 4,0x00,0x78,0x48,0x48,0x00,0x00,0x00,0x00, };// c
uint8_t dia[] = { 1, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }; // :
uint8_t pavla[] = { 2, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }; //pavla
uint8_t syn[] = { 4, 0x10, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	+
uint8_t dot[] = { 1,0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	.

uint8_t d1[] = { 3, 0x42, 0x7E, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, };//1
uint8_t d2[] = { 3, 0x7A, 0x4A, 0x4E, 0x00, 0x00, 0x00, 0x00, 0x00, };//2
uint8_t d3[] = { 3, 0x4A, 0x4A, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, };//3
uint8_t d4[] = { 3, 0x0E, 0x08, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, };//4
uint8_t d5[] = { 3, 0x4E, 0x4A, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, };//5
uint8_t d6[] = { 3, 0x7E, 0x4A, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, };//6
uint8_t d7[] = { 3, 0x02, 0x02, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, };//7
uint8_t d8[] = { 3, 0x7E, 0x4A, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, };//8
uint8_t d9[] = { 3, 0x4E, 0x4A, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, };//9
uint8_t d0[] = { 3, 0x7E, 0x42, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, };//0
// GREEK CHARS
uint8_t CK[] = { 3, 0x7E, 0x18, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	K
uint8_t CY[] = { 3, 0x1E, 0x70, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	Y
uint8_t CD[] = { 3, 0x7C, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	D
uint8_t CE[] = { 3, 0x7E, 0x4A, 0x4A, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	E
uint8_t CT[] = { 3, 0x02, 0x7E, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	T
uint8_t CR[] = { 3, 0x7E, 0x0A, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	R
uint8_t CP[] = { 3, 0x7E, 0x02, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	P
uint8_t CA[] = { 3, 0x7E, 0x0A, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	A
uint8_t CS[] = { 3, 0x66, 0x5A, 0x4A, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	S
// ENGLISH CHARS
uint8_t ES[] = { 3, 0x4E, 0x4A, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	s
uint8_t EU[] = { 3, 0x7E, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	u
uint8_t EM[] = { 3, 0x7E,0x0C,0x7E,0x00,0x00,0x00,0x00,0x00, }; //	m
uint8_t EO[] = { 3, 0x7E, 0x42, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	o
uint8_t ET[] = { 3, 0x02, 0x7E, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	t
uint8_t EW[] = { 3, 0x7E,0x30,0x7E,0x00,0x00,0x00,0x00,0x00, }; //	w
uint8_t EF[] = { 3, 0x7E, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	f
uint8_t ER[] = { 3, 0x7E, 0x1A, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //	r
uint8_t EH[] = { 3, 0x7E,0x08,0x7E,0x00,0x00,0x00,0x00,0x00, }; //	h
uint8_t EA[] = { 3, 0x7E, 0x0A, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, }; //   a
uint8_t bb[] = { 7, 0xFF,0xFF,0x91,0x9F,0x9F,0xF0,0xF0,0x00, };//	b
uint8_t be[] = { 6, 0xFC,0xFC,0xA4,0xA4,0xBC,0xBC,0x00,0x00, };//	e
uint8_t bl[] = { 4,0x80,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00, };//	l











/************************************************************************************** WIFI INIT         */

void initWiFi() {
	WiFi.mode(WIFI_STA);
	//WiFi.begin("Wokwi-GUEST", "", 6);

	WiFi.begin(ssid, password);
	debugln("Connecting to WiFi ..");
	while (WiFi.status() != WL_CONNECTED) {
		debug('.');
		delay(1000);
	}
	debugln(WiFi.localIP());
	debug("RSSI: ");
	debugln(WiFi.RSSI());

}

/************************************************************************************** TIMER         */

void TDISPLAY();
#include <Ticker.h>

Ticker Tdisplay;




/************************************************************************************** SETUP          */
void setup() {
	Serial.begin(115200);
	pinMode(0, INPUT_PULLUP);
	pinMode(2, OUTPUT);
	EEPROM.begin(EEPROM_SIZE);

	PP.begin();

	PP.addChar('$', degCI);
	PP.addChar('^', degCO);
	PP.addChar('&', arup);
	PP.addChar('~', ardo);
	PP.addChar('z', es);
	PP.addChar('v', ve);
	PP.addChar('!', el);
	PP.addChar('@', ce);
	PP.addChar('+', syn);
	PP.addChar(':', dia);
	PP.addChar('-', pavla);
	PP.addChar('.', dot);
	PP.addChar('1', d1);
	PP.addChar('2', d2);
	PP.addChar('3', d3);
	PP.addChar('4', d4);
	PP.addChar('5', d5);
	PP.addChar('6', d6);
	PP.addChar('7', d7);
	PP.addChar('8', d8);
	PP.addChar('9', d9);
	PP.addChar('0', d0);


	PP.addChar('K', CK);
	PP.addChar('Y', CY);
	PP.addChar('D', CD);
	PP.addChar('E', CE);
	PP.addChar('T', CT);
	PP.addChar('R', CR);
	PP.addChar('P', CP);
	PP.addChar('A', CA);
	PP.addChar('S', CS);


	PP.addChar('s', ES);
	PP.addChar('u', EU);
	PP.addChar('m', EM);
	PP.addChar('o', EO);
	PP.addChar('t', ET);
	PP.addChar('w', EW);
	PP.addChar('f', EF);
	PP.addChar('r', ER);
	PP.addChar('a', EA);
	PP.addChar('h', EH);
	PP.addChar('b', bb);
	PP.addChar('e', be);
	PP.addChar('l', bl);







	PP.setIntensity(0);
	PP.setCharSpacing(1);
	PP.displayClear();
	PP.setTextAlignment(PA_CENTER);
	PP.print("bell");
	PLAY_MUSIC0();
	delay(3000);
	debugln("GET button...");
	if (digitalRead(0) == 0) RESET_EEPROM();



	delay(3000);
	GET_EEPROM();
	initWiFi();
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

	//GET_NTP();


	sensor1.begin();
	sensor2.begin();

	if (!rtc.begin()) {
		debugln("RTC....Couldn't find");

		RTC.EXISTS = 0;
	}
	else
	{

		RTC.EXISTS = 1;
		debugln("RTC....OK");
	}
	GET_TIME();



	TDISPLAY();



	Tdisplay.attach_ms(5000, TDISPLAY);
	//client.setInsecure();




}
/*_____________________________________________________________________________________________________*/

void loop() {


	unsigned long currentMillis = millis();
	// if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
	if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
		debug(millis());
		debugln("Reconnecting to WiFi...");
		WiFi.disconnect();
		WiFi.reconnect();
		previousMillis = currentMillis;

	}
	//GET_YOUTUBE();
	GET_ANALITICS();
	delay(60000);
}
/*******************************************************************************************************/


/*_____________________________________________________________________________________________________*/
void GET_YOUTUBE()
{
	goto sight;
	if (api.getChannelStatistics(CHANNEL_ID))
	{
		D_NOW.subs = api.channelStats.subscriberCount;
		D_NOW.views = api.channelStats.viewCount;
		debug("D_NOW.subsCRIBERS = "); debugln(D_NOW.subs);
		debug("D_NOW.views = ");		debugln(D_NOW.views);


	}
	else
	{
		debugln("youtube error");
	}
sight:
	if (sight.getData()) {
		debugln("views: " + sight.channelStats.views);
		debugln("subscribers_gained: " + sight.channelStats.subscribers_gained);
		debugln("subscribers_lost: " + sight.channelStats.subscribers_lost);
		debugln("subscribers_count: " + sight.channelStats.subscribers_count);
		debugln("estimated_minutes_watched: " + sight.channelStats.estimated_minutes_watched);
		debugln("average_view_duration: " + sight.channelStats.average_view_duration);
		D_NOW.subs = atol(sight.channelStats.subscribers_count.c_str());
		D_NOW.views = atol(sight.channelStats.views.c_str());
	}
	else {
		//use sight._debug = true; at the begining to debug the library output
		debugln("There was an error getting the data.");
		return;
	}


	D_MONTLY.subs = D_NOW.subs - D_SAVED.subs;
	D_MONTLY.views = D_NOW.views - D_SAVED.views;
	//	D_MONTLY.likes = D_NOW.likes - Mls;

}
/*_____________________________________________________________________________________________________*/
void GET_EEPROM()
{
	debugln("GET eeprom");
	D_SAVED.month = EEPROM.readLong(0);
	D_SAVED.day = EEPROM.readLong(20);
	D_SAVED.subs = EEPROM.readLong(30);
	D_SAVED.views = EEPROM.readLong(40);
	D_SAVED.likes = EEPROM.readLong(50);
	D_SAVED.comms = EEPROM.readLong(60);



	debug("D_SAVED.month = ");	debugln(D_SAVED.month);
	debug("D_SAVED.day = ");	debugln(D_SAVED.day);
	debug("D_SAVED.subs = ");	debugln(D_SAVED.subs);
	debug("D_SAVED.views = ");	debugln(D_SAVED.views);
	debug("D_SAVED.likes = ");	debugln(D_SAVED.likes);
	debug("D_SAVED.comms = ");	debugln(D_SAVED.comms);

}

/*_____________________________________________________________________________________________________*/
void SET_EEPROM()
{
	debugln("SET eeprom");
	EEPROM.writeLong(0, D_NOW.month);
	EEPROM.writeLong(20, D_NOW.day);
	EEPROM.writeLong(30, D_NOW.subs);
	EEPROM.writeLong(40, D_NOW.views);
	EEPROM.writeLong(50, D_NOW.likes);
	EEPROM.writeLong(60, D_NOW.comms);
	EEPROM.commit();
	debug("D_NOW.month = ");	debugln(D_NOW.month);
	debug("D_NOW.day = ");	debugln(D_NOW.day);
	debug("D_NOW.subs = ");	debugln(D_NOW.subs);
	debug("D_NOW.views = ");	debugln(D_NOW.views);
	debug("D_NOW.likes = ");	debugln(D_NOW.likes);
	debug("D_NOW.comms = ");	debugln(D_NOW.comms);

}
/*_____________________________________________________________________________________________________*/
void RESET_EEPROM()
{
	EEPROM.writeLong(0, 0);
	EEPROM.writeLong(20, 0);
	EEPROM.writeLong(30, 0);
	EEPROM.writeLong(40, 0);
	EEPROM.writeLong(50, 0);
	EEPROM.writeLong(60, 0);
	EEPROM.commit();
	PP.print("-----");
	GET_ANALITICS();
	SET_EEPROM();



}
/*_____________________________________________________________________________________________________*/

/*_____________________________________________________________________________________________________*/










/*_____________________________________________________________________________________________________*/
void TDISPLAY()
{

again:
	DD += 1;
	if (DD > 12) DD = 1;
	debug("update display   dd= ");
	debugln(DD);
	switch (DD)
	{

	case 1:
		debug("display temp IN  ");
		GET_DS18B20(1);
		if (TIN == -127)
		{
			debugln("sensor IN error");
			DD += 1;
			goto next2;
		}


		if (TIN == OTIN)
		{
			PP.print(String(TIN, 1) + "$");
		}
		else if (TIN > OTIN)
		{
			PP.print("&" + String(TIN, 1) + "$");
		}
		else if (TIN < OTIN)
		{
			PP.print("~" + String(TIN, 1) + "$");
		}
		OTIN = TIN;
		//debugln(TIN);
		break;
	case 2:
	next2:
		debug("display temp OUT  ");
		GET_DS18B20(2);
		if (TOUT == -127)
		{
			debugln("sensor OUT error");
			//DD += 1;
			return;
			//goto next3;
		}
		if (TOUT == OTOUT)
		{
			PP.print(String(TOUT, 1) + "^");
		}
		else if (TOUT > OTOUT)
		{
			PP.print("&" + String(TOUT, 1) + "^");
		}
		else if (TOUT < OTOUT)
		{
			PP.print("~" + String(TOUT, 1) + "^");
		}
		OTOUT = TOUT;
		//debugln(TOUT);
		break;
	case 3:
	next3:

		GET_TIME();
		debug("display date  ");

		PP.print(DATE);
		debugln(DATE);
		break;
	case 4:

		debug("display time   ");
		PP.print(TIME);
		debugln(TIME);
		break;

	case 5:
		debug("display subs   ");
		if (D_NOW.subs == 0)
		{
			debugln("NO subscribers");
			DD += 1;
			goto next6;
		}
		PP.print(String(D_NOW.subs) + "z");
		if (D_MTEMP.subs ==0) D_MTEMP.subs = D_MONTLY.subs;
		if (D_MTEMP.subs < D_MONTLY.subs)
		{
			PLAY_MUSIC0();
			D_MTEMP.subs = D_MONTLY.subs;
		}

		debugln(D_NOW.subs);
		break;
	case 6:
	next6:
		debug("dispaly monthly subs   ");
		if (D_NOW.subs == 0)
		{
			debugln("NO montly subs");
			DD += 1;
			goto next7;
		}

		PP.print("+" + String(D_MONTLY.subs) + "z");
		debugln(D_MONTLY.subs);
		break;
	case 7:
	next7:
		debug("display views   ");
		if (D_NOW.views == 0)
		{
			debugln("NO views");
			DD += 1;
			goto next8;
		}
		PP.print(String(D_NOW.views) + "v");
		if (D_MTEMP.views == 0) D_MTEMP.views = D_MONTLY.views;
		if (D_MTEMP.views < D_MONTLY.views)
		{
			PLAY_MUSIC0();
			D_MTEMP.views = D_MONTLY.views;
		}

		debugln(D_NOW.views);
		break;
	case 8:
	next8:
		debug("display monthly views   ");
		if (D_NOW.views == 0)
		{
			debugln("NO montly views");
			DD += 1;
			goto next9;
		}
		PP.print("+" + String(D_MONTLY.views) + "v");
		debugln(D_MONTLY.views);
		break;
	case 9:
	next9:
		debug("display likes   ");
		if (D_NOW.likes == 0)
		{
			debugln("NO likes");
			DD += 1;
			goto next10;
		}
		PP.print(String(D_NOW.likes) + "!");
		if (D_MTEMP.likes == 0) D_MTEMP.likes = D_MONTLY.likes;
		if (D_MTEMP.likes < D_MONTLY.likes)
		{
			PLAY_MUSIC0();
			D_MTEMP.likes = D_MONTLY.likes;
		}

		debugln(D_NOW.likes);

		break;
	case 10:
	next10:
		debug("display montly likes   ");
		if (D_NOW.likes == 0)
		{
			goto next11;
			debugln("NO montly likes");
		}
		PP.print("+" + String(D_MONTLY.likes) + "!");
		debugln(D_MONTLY.likes);
		break;


	case 11:
	next11:
		debug("display comments   ");
		if (D_NOW.comms == 0)
		{
			debugln("NO comments");
			DD += 1;
			goto next12;
		}
		PP.print(String(D_NOW.comms) + "@");
		if (D_MTEMP.comms == 0) D_MTEMP.comms = D_MONTLY.comms;
		if (D_MTEMP.comms < D_MONTLY.comms)
		{
			PLAY_MUSIC0();
			D_MTEMP.comms = D_MONTLY.comms;
		}

		debugln(D_NOW.comms);
		break;
	case 12:
	next12:
		debug("display montly comments   ");
		if (D_NOW.comms == 0)
		{

			debugln("NO montly comments");
			return;
		}
		PP.print("+" + String(D_MONTLY.comms) + "@");
		debugln(D_MONTLY.comms);
		break;



	default:
		break;
	}




}



/*_____________________________________________________________________________________________________*/

/*_____________________________________________________________________________________________________*/


/*_____________________________________________________________________________________________________*/
void GET_DS18B20(int i)
{
	switch (i)
	{
	case 1:
		sensor1.requestTemperatures();
		debug("IN: ");
		delay(10);
		TIN = sensor1.getTempCByIndex(0);
		debugln(TIN);
		break;
	case 2:
		sensor2.requestTemperatures();
		debug("OUT: ");
		delay(10);
		TOUT = sensor2.getTempCByIndex(0);
		debugln(TOUT);
		break;
	}

}
/*_____________________________________________________________________________________________________*/





/*_____________________________________________________________________________________________________*/

/*_____________________________________________________________________________________________________*/
void GET_TIME()
{

	GET_NTP();
	GET_RTC();
	if (RTC.EXISTS == 1 && NTP.EXISTS == 1)
	{
		if (NTP_tmp != RTC_tmp)
		{
			SET_RTC(NTP.DY, NTP.DM, NTP.DD, NTP.TH, NTP.TM, NTP.TS);
			GET_RTC();
		}
	}


	/*if (RTC.EXISTS == 1)
	{
		GET_RTC();
	}
	else if (RTC.EXISTS == 0 && NTP.EXISTS == 1)
	{
		GET_NTP();

	}
	else if (NTP.EXISTS == 0 && WiFi.status() == WL_CONNECTED)
	{
		GET_NTP();
	}*/


	debug(TIME_NOW.DM); debug(D_SAVED.subs); debug(D_SAVED.views); debug(D_SAVED.likes); debugln(D_NOW.month);

	/*if (Mms != TIME_NOW.DM || D_SAVED.subs==0 || D_SAVED.views==0)
	{
		D_SAVED.subs = D_NOW.subs;
		D_SAVED.views = D_NOW.views;
		Mls = D_NOW.likes;
		Mcs = D_NOW.comms;
		Mms = RTC.DM;
		Mmon = RTC.DM;
		D_MONTLY.subs = 0;
		D_MONTLY.views = 0;
		D_MONTLY.likes = 0;
		D_MONTLY.comms = 0;
		debug(TIME_NOW.DM); debug(D_SAVED.subs); debug(D_SAVED.views); debug(Mls);  debugln(Mcs);  debugln(Mmon);

		SET_EEPROM();
	}
	else
	{
		D_MONTLY.subs = D_NOW.subs - D_SAVED.subs;
		D_MONTLY.views = D_NOW.views - D_SAVED.views;
		D_MONTLY.likes = D_NOW.likes - Mls;
		D_MONTLY.comms = D_NOW.comms - Mcs;
	}*/


}
/*_____________________________________________________________________________________________________*/
void GET_NTP()
{
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		debugln("NTP.....Failed to obtain time");
		NTP.EXISTS = 0;
		return;
	}
	debugln("NTP.....OK");

	NTP.EXISTS = 1;
	char tt[3];


	char buf[64];
	strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo); TIME = buf;
	//debugln("The current NTP time iS: " + TIME);



	strftime(buf, sizeof(buf), "%d-%m-%y", &timeinfo); DATE = buf;
	//debugln("The current NTP date iS: " + DATE);
	NTP.DY = timeinfo.tm_year + 1900;
	NTP.DM = timeinfo.tm_mon + 1;
	NTP.DD = timeinfo.tm_mday;
	NTP.TH = timeinfo.tm_hour;
	NTP.TM = timeinfo.tm_min;
	NTP.TS = timeinfo.tm_sec;
	NTP.DOW = timeinfo.tm_wday - 1;
	if (NTP.TH < 10)
	{
		TIME = "0" + String(NTP.TH);
	}
	else
	{
		TIME = String(NTP.TH);
	}
	if (NTP.TM < 10)
	{
		TIME = TIME + ":0" + String(NTP.TM);
	}
	else
	{
		TIME = TIME + ":" + String(NTP.TM);
	}
	DATE = String(DoW[NTP.DOW]) + " " + String(NTP.DOW) + " " + String(NTP.DD) + "-" + String(NTP.DM);
	debugln("NTP time = " + TIME);
	debugln("NTP date = " + String(NTP.DOW) + " " + DATE);
	TIME_NOW.DM = NTP.DM;
	TIME_NOW.DD = NTP.DD;
	NTP_tmp = String(NTP.DD) + "-" + String(NTP.DM);
}
/*_____________________________________________________________________________________________________*/
void GET_RTC()
{

	DateTime now = rtc.now();

	RTC.DY = now.year();
	RTC.DM = now.month();
	RTC.DD = now.day();
	RTC.TH = now.hour();
	RTC.TM = now.minute();
	RTC.TS = now.second();
	RTC.DOW = now.dayOfTheWeek() - 1;
	if (RTC.TH < 10)
	{
		TIME = "0" + String(RTC.TH);
	}
	else
	{
		TIME = String(RTC.TH);
	}
	if (RTC.TM < 10)
	{
		TIME = TIME + ":0" + String(RTC.TM);
	}
	else
	{
		TIME = TIME + ":" + String(RTC.TM);
	}



	DATE = String(DoW[RTC.DOW]) + " " + String(RTC.DD) + "-" + String(RTC.DM);
	debugln("rtc time = " + TIME);
	debugln("rtc date = " + String(RTC.DOW) + " " + DATE);
	TIME_NOW.DM = RTC.DM;
	TIME_NOW.DD = NTP.DD;
	RTC_tmp = String(RTC.DD) + "-" + String(RTC.DM);
}



/*_____________________________________________________________________________________________________*/
void SET_RTC(int yy, int Mm, int dd, int hh, int mm, int ss)
{
	if (rtc.isrunning()) {
		debugln("RTC..... SET time!");
		rtc.adjust(DateTime(yy, Mm, dd, hh, mm, ss));
		delay(500);
		GET_RTC();
	}
	else
	{
		debugln("RTC.... NOT running");
	}

}
/*_____________________________________________________________________________________________________*/

/*_____________________________________________________________________________________________________*/
void GET_subs() {
	long ss;
	if (WiFi.status() == WL_CONNECTED) {
		WIFI_STATUS = true;

		String URL = "https://www.googleapis.com/youtube/v3/channels?part=statistics&id=" + String(CHANNEL_ID) + "&key=" + API_KEY;
		String S = URL_GET(URL.c_str());
		String s; int32_t findStart = 0; int32_t findNext = 0; int32_t findEnd = 0;
		findStart = S.indexOf("\"subscriberCount", findNext);
		findNext = findStart + 20;
		findEnd = S.indexOf("\"", findNext);
		s = S.substring(findNext, findEnd);
		ss = s.toInt();
		//if (D_NOW.subs != ss)PLAY_MUSIC0();

		D_NOW.subs = ss;
		delay(25);

		//  Serial.println("D_NOW.subs = "+String(D_NOW.subs));
	}
	else {

		WIFI_STATUS = false;
	}
}
String URL_GET(const char* ulr) {
	HTTPClient http;
	http.begin(ulr);
	int httpResponseCode = http.GET();
	String payload = "{}";
	if (httpResponseCode > 0) {
		//Serial.print("HTTP Response code: ");
		//Serial.println(httpResponseCode);
		payload = http.getString();
	}
	else {
		//Serial.print("Error code: ");
		//Serial.println(httpResponseCode);
	}
	// Free resources
	http.end();
	return payload;
}


void makeVideoList() {

	if (WiFi.status() == WL_CONNECTED) {
		WIFI_STATUS = true;
		AllVideoList = "";  //    
		AllVideoCount = 0;  //        
		int32_t findStart = 0; int32_t findNext = 0; int32_t findEnd = 0;

		//     ""
		String URL = "https://www.googleapis.com/youtube/v3/channels?part=contentDetails&id=" + String(CHANNEL_ID) + "&key=" + API_KEY;
		String S = URL_GET(URL.c_str());
		String uploads = "";
		findStart = S.indexOf("\"uploads", findNext);
		findNext = findStart + 12;
		findEnd = S.indexOf("\"", findNext);
		uploads = S.substring(findNext, findEnd);
		delay(25);

		//    ID  30       30
		URL = "https://www.googleapis.com/youtube/v3/playlistItems?playlistId=" + uploads + "&maxResults=30&key=" + API_KEY + "&part=contentDetails";
		S = URL_GET(URL.c_str());
		// Serial.println(S);
		 //       11  + 
		findStart = 0; findNext = 0; findEnd = 0;
		findStart = S.indexOf("\"videoId", findNext);
		while (findStart != -1) {
			findNext = findStart + 12;
			findEnd = S.indexOf("\"", findNext);
			AllVideoList = AllVideoList + S.substring(findNext, findEnd);
			AllVideoList = AllVideoList + ",";
			AllVideoCount++;
			//D_NOW.views = AllVideoCount;                       //  
			findStart = S.indexOf("\"videoId", findEnd);

		}

		//     
		String nextPageToken = "";
		findStart = 0; findNext = 0; findEnd = 0;
		findStart = S.indexOf("\"nextPageToken", findNext);
		if (findStart == -1) return; //   30  -  
		findNext = findStart + 18;
		findEnd = S.indexOf("\"", findNext);
		nextPageToken = S.substring(findNext, findEnd);

	label:
		URL = "https://www.googleapis.com/youtube/v3/playlistItems?playlistId=" + uploads + "&maxResults=30&key=" + API_KEY + "&part=contentDetails&pageToken=" + nextPageToken;
		S = URL_GET(URL.c_str());
		//Serial.println(S);
		//       11  + 
		findStart = 0; findNext = 0; findEnd = 0;
		findStart = S.indexOf("\"videoId", findNext);
		while (findStart != -1) {
			findNext = findStart + 12;
			findEnd = S.indexOf("\"", findNext);
			AllVideoList = AllVideoList + S.substring(findNext, findEnd);
			AllVideoList = AllVideoList + ",";
			AllVideoCount++;
			//	D_NOW.views = AllVideoCount;                       //  
			findStart = S.indexOf("\"videoId", findEnd);

		}

		//     
		nextPageToken = "";
		findStart = 0; findNext = 0; findEnd = 0;
		findStart = S.indexOf("\"nextPageToken", findNext);
		if (findStart == -1) return; // nextPageToken   -    
		findNext = findStart + 18;
		findEnd = S.indexOf("\"", findNext);
		nextPageToken = S.substring(findNext, findEnd);
		delay(25);
		goto label;

	}
	else {
		//  
		WIFI_STATUS = false;
	}

}




void getYoutubeVideoS(String listVideo) {
	wC = 0; lC = 0;	dC = 0;	cC = 0;
	if (WiFi.status() == WL_CONNECTED) {

		WIFI_STATUS = true;

		String URL = "https://www.googleapis.com/youtube/v3/videos?id=" + listVideo + "&key=" + API_KEY + "&part=statistics";
		String S = URL_GET(URL.c_str());

		String s = ""; int32_t findStart = 0; int32_t findNext = 0; int32_t findEnd = 0;
		uint32_t viewCount = 0;                          //  
		findStart = S.indexOf("\"viewCount", findNext);
		while (findStart != -1) {
			findNext = findStart + 14;
			findEnd = S.indexOf("\"", findNext);
			s = S.substring(findNext, findEnd);
			viewCount = viewCount + s.toInt();
			//Serial.println(s.toInt());
			findStart = S.indexOf("\"viewCount", findEnd);
		}

		s = ""; findStart = 0; findNext = 0; findEnd = 0;
		uint32_t likeCount = 0;                          //  
		findStart = S.indexOf("\"likeCount", findNext);
		while (findStart != -1) {
			findNext = findStart + 14;
			findEnd = S.indexOf("\"", findNext);
			s = S.substring(findNext, findEnd);
			likeCount = likeCount + s.toInt();
			//Serial.print(s.toInt()); Serial.print(" - ");
			findStart = S.indexOf("\"likeCount", findNext);
		}

		s = ""; findStart = 0; findNext = 0; findEnd = 0;
		uint32_t dislikeCount = 0;                        //  
		findStart = S.indexOf("\"dislikeCount", findNext);
		while (findStart != -1) {
			findNext = findStart + 17;
			findEnd = S.indexOf("\"", findNext);
			s = S.substring(findNext, findEnd);
			dislikeCount = dislikeCount + s.toInt();
			//Serial.print(s.toInt()); Serial.print(" - ");
			findStart = S.indexOf("\"dislikeCount", findNext);
		}

		s = ""; findStart = 0; findNext = 0; findEnd = 0;
		uint32_t commentCount = 0;                        //  
		findStart = S.indexOf("\"commentCount", findNext);
		while (findStart != -1) {
			findNext = findStart + 17;
			findEnd = S.indexOf("\"", findNext);
			s = S.substring(findNext, findEnd);
			commentCount = commentCount + s.toInt();
			//Serial.print(s.toInt()); Serial.println(" - ");
			findStart = S.indexOf("\"commentCount", findNext);
		}

		wC = wC + viewCount;                     //  
		lC = lC + likeCount;                     //  
		dC = dC + dislikeCount;                  //  
		cC = cC + commentCount;                  //  

	}
	else {
		//   -   
		WIFI_STATUS = false;
	}


}




void refreshA() {

	uint32_t remainVideo = AllVideoCount;
	String S = AllVideoList;
	String s = "";

label:

	if (remainVideo <= 30) {                   //  
		S.remove(S.lastIndexOf(','));            //   
		getYoutubeVideoS(S);
		return;
	}

	do {                                       //    30 
		s = S.substring(0, 360);
		s.remove(s.lastIndexOf(','));
		S.remove(0, 360);
		getYoutubeVideoS(s);
		remainVideo = remainVideo - 30;
		delay(25);
	} while (remainVideo > 30);

	goto label;
}

void GET_ANALITICS()
{
	long s, v, l, c;
	GET_subs();
	makeVideoList();
	refreshA();
	//if (D_NOW.views != wC || D_NOW.likes != lC || D_NOW.comms != cC) PLAY_MUSIC0();

	D_NOW.views = wC;
	D_NOW.likes = lC;
	D_NOW.comms = cC;

	debugln("likes  " + String(D_NOW.likes));
	// Serial.println("dislikecount  " + String(dC));
	debugln("comments  " + String(D_NOW.comms));
	debugln("D_NOW.views =" + String(D_NOW.views));
	debugln("D_NOW.subs = " + String(D_NOW.subs));
	if (D_SAVED.month != TIME_NOW.DM || D_SAVED.subs == 0 || D_SAVED.views == 0)
	{
		D_SAVED.subs = D_NOW.subs;
		D_SAVED.views = D_NOW.views;
		D_SAVED.likes = D_NOW.likes;
		D_SAVED.comms = D_NOW.comms;
		D_NOW.month = TIME_NOW.DM;
		D_NOW.day = TIME_NOW.DD;
		D_MONTLY.subs = 0;
		D_MONTLY.views = 0;
		D_MONTLY.likes = 0;
		D_MONTLY.comms = 0;
		D_MTEMP = D_MONTLY;


		//debug(TIME_NOW.DM); debug(D_SAVED.subs); debug(D_SAVED.views); debug(D_SAVED.likes);  debugln(D_SAVED.comms);  debugln(D_NOW.month);

		SET_EEPROM();
		delay(500);
		GET_EEPROM();
	}
	else
	{
		D_MONTLY.subs = D_NOW.subs - D_SAVED.subs;
		D_MONTLY.views = D_NOW.views - D_SAVED.views;
		D_MONTLY.likes = D_NOW.likes - D_SAVED.likes;
		D_MONTLY.comms = D_NOW.comms - D_SAVED.comms;
	}


}


void PLAY_MUSIC0()
{
	// notes in the melody:
	int melody[] = {
	  NOTE_C5, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, 0, NOTE_B4, NOTE_C5
	};

	// note durations: 4 = quarter note, 8 = eighth note, etc.:
	int noteDurations[] = {
	  4, 8, 8, 4, 4, 4, 4, 4
	};

	for (int thisNote = 0; thisNote < 8; thisNote++) {

		// to calculate the note duration, take one second divided by the note type.
		//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
		int noteDuration = 1000 / noteDurations[thisNote];
		tone(3, melody[thisNote], noteDuration);
		digitalWrite(LED, HIGH);
		// to distinguish the notes, set a minimum time between them.
		// the note's duration + 30% seems to work well:
		int pauseBetweenNotes = noteDuration * 1.30;
		delay(pauseBetweenNotes);
		// stop the tone playing:
		noTone(3);
		digitalWrite(LED, LOW);
	}
}
