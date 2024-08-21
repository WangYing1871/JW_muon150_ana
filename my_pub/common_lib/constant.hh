#ifndef constant_hh
#define constant_hh 1
#include <string>

using namespace std;

/******** For FEC data dec ********/
// Data Frame
const int cUSR_HEADER = 0x5A0F;
const int cUSER_TAIL = 0x0F5A;

const int cAGET_PROTOCOL_HEADER = 0xAC0F;
// Note that the length do not include the header(2 words) and the CRC (2 words)
const int cAGET_HEADER_LEN = 14;
const int cAGET_EVENT_HEADER = 0xFFFF & (0x4000 | (cAGET_HEADER_LEN * 2));

const int cAGET_DATA_LEN = 0x202;
const int cAGET_DATA_HEADER = 0xFFFF & (cAGET_DATA_LEN * 2);

const int cAGET_TAIL_LEN = 4;
const int cAGET_EVENT_TAIL = 0xFFFF & (0x2000 | (cAGET_TAIL_LEN * 2));

/********* For FEC data structure ********/
const int cFEC_NUM = 4;
const int cCHIP_NUM = 4;
const int cCHN_NUM = 68;
const int cSCA_NUM = 512;
// Fot the USB daq	this value is set to 64 or smaller
const int cSCA_NUM_USB = 256;
const int cMAX_HITS = cCHIP_NUM * cCHN_NUM;
const int cMAX_DEC_HITS = 2560;
const int cNOISE_CHN[4] = {11, 22, 45, 57};
const int cMAX_HITS_ALL = cFEC_NUM * cCHIP_NUM * 64;

const int cELINK_NUM = 32;
const string cELINK_NAME[cELINK_NUM] = {"0C1", "0C5", "0C9", "0CD", "0D1", "0D5", "0D9", "0DD"};

const int cAGET_VTH_CMD_HEADER = 0xa0;

const double cSTRIP_WIDTH = 0.4;

// These two value is the valid sca range. This range is determined by the sca waveform
const int cSCA_START = 95;
const int cSCA_END = 105;

const int cDET_CHN_BASE = 36;
const int cDET_CHN_MAX = 128;
const int cMAX_DET_LAYER = 20;
const int cDET_LAYER = 8;
const int cRADIOGRAPHY_LAYER = 4;
// const double cDET_Z[cDET_LAYER] = { 0., 52.5, 105., 157.5, 291.5, 344., 396.5, 448.7 };

const double cIMAGING_BOX_UPPER = 650.0;
const double cIMAGING_BOX_LOWER = 160.0;
const double cPI = 3.1415926535897932384626;

const int cMAX_CLUSTER = 5;

const double cDET_HEIGHT = 448.7;

const int cMAX_VOXEL_NUM = 300;

const int cMAX_PoCA_POINT = 100000;

#define cMAX_CLUSTER 25

// the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define cRESET "\033[0m"
#define cBLACK "\033[30m"              /* Black */
#define cRED "\033[31m"                /* Red */
#define cGREEN "\033[32m"              /* Green */
#define cYELLOW "\033[33m"             /* Yellow */
#define cBLUE "\033[34m"               /* Blue */
#define cMAGENTA "\033[35m"            /* Magenta */
#define cCYAN "\033[36m"               /* Cyan */
#define cWHITE "\033[37m"              /* White */
#define cBOLDBLACK "\033[1m\033[30m"   /* Bold Black */
#define cBOLDRED "\033[1m\033[31m"     /* Bold Red */
#define cBOLDGREEN "\033[1m\033[32m"   /* Bold Green */
#define cBOLDYELLOW "\033[1m\033[33m"  /* Bold Yellow */
#define cBOLDBLUE "\033[1m\033[34m"    /* Bold Blue */
#define cBOLDMAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define cBOLDCYAN "\033[1m\033[36m"    /* Bold Cyan */
#define cBOLDWHITE "\033[1m\033[37m"   /* Bold White */

#define cCLR_LINE "\x1B[2K\r"

#endif // !constant_hh
