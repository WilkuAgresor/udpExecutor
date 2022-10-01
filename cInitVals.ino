
char separator = ',';
bool DEBUG_LED_ON = false;

// IP
byte mac[] = { 0xDA, 0xAD, 0xBE, 0x00, 0x00, 0x00}; 

IPAddress ip(192, 168, 100, 254);
unsigned int localPort = 8888;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
EthernetUDP Udp;

const int PWM_EXPANDERS_COUNT = 2;
const int PWM_PORTS_PER_EXPANDER = 16;

//PWM drivers
Adafruit_PWMServoDriver pwm[PWM_EXPANDERS_COUNT] = 
{
 Adafruit_PWMServoDriver(0x40),
 Adafruit_PWMServoDriver(0x41)
};

uint32_t sessionId = 0;
