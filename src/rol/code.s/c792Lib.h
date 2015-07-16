/******************************************************************************
*
*  c792Lib.h  -  Driver library header file for readout of a C.A.E.N. multiple
*                Model 792 QDCs using a VxWorks 5.2 or later based single board 
*                computer.
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          June 2000
*
*  SVN: $Rev$
*
*/
#ifndef __C792LIB__
#define __C792LIB__


#define C792_MAX_CHANNELS   32
#define C792_MAX_WORDS_PER_EVENT  34

/* Define a Structure for access to QDC*/
struct c792_struct {
  /* from Bryan
  volatile unsigned long  data[512];
  unsigned long blank1[512];
  volatile unsigned short rev;
  volatile unsigned short geoAddr;
  volatile unsigned short cbltAddr;
  volatile unsigned short bitSet1;
  volatile unsigned short bitClear1;
  volatile unsigned short intLevel;
  volatile unsigned short intVector;
  volatile unsigned short status1;
  volatile unsigned short control1;
  volatile unsigned short aderHigh;
  volatile unsigned short aderLow;
  volatile unsigned short ssReset;
  unsigned short blank2;
  volatile unsigned short cbltControl;
  unsigned short blank3[2];
  volatile unsigned short evTrigger;
  volatile unsigned short status2;
  volatile unsigned short evCountL;
  volatile unsigned short evCountH;
  volatile unsigned short incrEvent;
  volatile unsigned short incrOffset;
  volatile unsigned short loadTest;
  unsigned short blank4;
  volatile unsigned short fclrWindow;
  volatile unsigned short bitSet2;
  volatile unsigned short bitClear2;
  volatile unsigned short wMemTestAddr;
  volatile unsigned short memTestWordH;
  volatile unsigned short memTestWordL;
  volatile unsigned short crateSelect;
  volatile unsigned short testEvWrite;
  volatile unsigned short evCountReset;
  unsigned short blank5[15];
  volatile unsigned short iped;
  unsigned short blank6;
  volatile unsigned short rTestAddr;
  unsigned short blank7;
  volatile unsigned short swComm;
  volatile unsigned short slideConst;
  unsigned short blank8[2];
  volatile unsigned short AAD;
  volatile unsigned short BAD;
  unsigned short blank9[6];
  volatile unsigned short threshold[C792_MAX_CHANNELS];
  */
/*0x0000*/  volatile unsigned long  data[512];  /* Output Buffer */
/*0x0000*/  unsigned long blank1[512];
/*0x1000*/  volatile unsigned short rev;        /* Firmware Revision */
/*0x1002*/  volatile unsigned short geoAddr;    /* Geo Address */
/*0x1004*/  volatile unsigned short cbltAddr;   /* MCST/CBLT Address */
/*0x1006*/  volatile unsigned short bitSet1;    /* Bit Set 1 */
/*0x1008*/  volatile unsigned short bitClear1;  /* Bit Clear 1 */
/*0x100A*/  volatile unsigned short intLevel;   /* Interrupt Level */
/*0x100C*/  volatile unsigned short intVector;  /* Interrupt Vector */
/*0x100E*/  volatile unsigned short status1;
/*0x1010*/  volatile unsigned short control1;
/*0x1012*/  volatile unsigned short aderHigh;
/*0x1014*/  volatile unsigned short aderLow;
/*0x1016*/  volatile unsigned short ssReset;
/*0x1018*/  unsigned short blank2;
/*0x101A*/  volatile unsigned short cbltControl;
/*0x101C*/  unsigned short blank3[2];
/*0x1020*/  volatile unsigned short evTrigger;
/*0x1022*/  volatile unsigned short status2;
/*0x1024*/  volatile unsigned short evCountL;
/*0x1026*/  volatile unsigned short evCountH;
/*0x1028*/  volatile unsigned short incrEvent;
/*0x102A*/  volatile unsigned short incrOffset;
/*0x102C*/  volatile unsigned short loadTest;
/*0x102E*/  volatile unsigned short fclrWindow;
/*0x1030*/  unsigned short blank4;
/*0x1032*/  volatile unsigned short bitSet2;
/*0x1034*/  volatile unsigned short bitClear2;
/*0x1036*/  volatile unsigned short wMemTestAddr;
/*0x1038*/  volatile unsigned short memTestWordH;
/*0x103A*/  volatile unsigned short memTestWordL;
/*0x103C*/  volatile unsigned short crateSelect;
/*0x103E*/  volatile unsigned short testEvWrite;
/*0x1040*/  volatile unsigned short evCountReset;
/*0x1042*/  unsigned short blank5[15];
/*0x1060*/  volatile unsigned short iped;
/*0x1062*/  unsigned short blank6;
/*0x1064*/  volatile unsigned short rTestAddr;
/*0x1066*/  unsigned short blank7;
/*0x1068*/  volatile unsigned short swComm;
/*0x106A*/  unsigned short slideConstant; /* in manual, but not in main register table, only in MCST table */
/*0x106C*/  unsigned short blank8[2];
/*0x1070*/  volatile unsigned short AAD;
/*0x1072*/  volatile unsigned short BAD;
/*0x1074*/  unsigned short blank9[6];
/*0x1080*/  volatile unsigned short threshold[C792_MAX_CHANNELS];
};

struct c792_ROM_struct {
  volatile unsigned short OUI_3;
  unsigned short blank1;
  volatile unsigned short OUI_2;
  unsigned short blank2;
  volatile unsigned short OUI_1;
  unsigned short blank3;
  volatile unsigned short version;
  unsigned short blank4;
  volatile unsigned short ID_3;
  unsigned short blank5;
  volatile unsigned short ID_2;
  unsigned short blank6;
  volatile unsigned short ID_1;
  unsigned short blank7[7];
  volatile unsigned short revision;
};


#define C792_BOARD_ID   0x00000318 /* 792 */
#define C775_BOARD_ID   0x00000307 /* 775 */

/* Define Address offset for 68K based A24/D32 VME addressing */
/* default VMEChip2 programming only supports A24/D16 */
#define C792_68K_A24D32_OFFSET   0xe0000000

/* Define default interrupt vector/level */
#define C792_INT_VEC      0xaa
#define C792_VME_INT_LEVEL   4

#define C792_ROM_OFFSET    0x8026

/* Register Bits */
#define C792_VME_BUS_ERROR 0x8
#define C792_SOFT_RESET    0x80
#define C792_DATA_RESET    0x4

#define C792_BUFFER_EMPTY  0x2
#define C792_BUFFER_FULL   0x4

#define C792_DATA_READY    0x1
#define C792_BUSY          0x4

#define C792_BLK_END       0x04
#define C792_BERR_ENABLE   0x20
#define C792_ALIGN64       0x40

#define C792_MEM_TEST            0x1
#define C792_OFFLINE             0x2
#define C792_OVERFLOW_SUP        0x8
#define C792_UNDERFLOW_SUP      0x10
#define C775_VALID_CONTROL      0x20 /* TDC v775 only */
#define C792_TEST_MODE          0x40
#define C792_SLIDE_ENABLE       0x80
#define C775_START_STOP        0x400 /* TDC v775 only */
#define C792_AUTO_INCR         0x800
#define C792_INC_HEADER       0x1000
#define C792_SLIDE_SUB_ENABLE 0x2000
#define C792_INCR_ALL_TRIG    0x4000


#define C792_DATA           0x00000000
#define C792_HEADER_DATA    0x02000000
#define C792_TRAILER_DATA   0x04000000
#define C792_INVALID_DATA   0x06000000


/* Register Masks */
#define C792_BITSET1_MASK   0x0098
#define C792_INTLEVEL_MASK  0x0007
#define C792_INTVECTOR_MASK 0x00ff
#define C792_STATUS1_MASK   0x01ff
#define C792_CONTROL1_MASK  0x0034
#define C792_STATUS2_MASK   0x00f6
#define C792_BITSET2_MASK   0x7fff
#define C792_EVTRIGGER_MASK 0x001f

#define C792_DATA_ID_MASK    0x07000000
#define C792_WORDCOUNT_MASK  0x00003f00
#define C792_CHANNEL_MASK    0x003f0000
#define C792_CRATE_MASK      0x00ff0000
#define C792_EVENTCOUNT_MASK 0x00ffffff
#define C792_GEO_ADDR_MASK   0xf8000000
#define C792_ADC_DATA_MASK   0x00000fff

/* Macros */
#define C792_EXEC_SOFT_RESET(id) {					\
    c792Write(&c792p[id]->bitSet1, C792_SOFT_RESET);			\
    c792Write(&c792p[id]->bitClear1, C792_SOFT_RESET);}

#define C792_EXEC_DATA_RESET(id) {					\
    c792Write(&c792p[id]->bitSet2, C792_DATA_RESET);			\
    c792Write(&c792p[id]->bitClear2, C792_DATA_RESET);}

#define C792_EXEC_READ_EVENT_COUNT(id) {				\
    volatile unsigned short s1, s2;					\
    s1 = c792Read(&c792p[id]->evCountL);				\
    s2 = c792Read(&c792p[id]->evCountH);				\
    c792EventCount[id] = (c792EventCount[id]&0xff000000) +		\
      (s2<<16) +							\
      (s1);}
#define C792_EXEC_SET_EVTREADCNT(id,val) {			       \
    if(c792EvtReadCnt[id] < 0)					       \
      c792EvtReadCnt[id] = val;					       \
    else							       \
      c792EvtReadCnt[id] = (c792EvtReadCnt[id]&0x7f000000) + val;}

#define C792_EXEC_CLR_EVENT_COUNT(id) {		\
    c792Write(&c792p[id]->evCountReset, 1);	\
    c792EventCount[id] = 0;}
#define C792_EXEC_INCR_EVENT(id) {		\
    c792Write(&c792p[id]->incrEvent, 1);	\
    c792EvtReadCnt[id]++;}
#define C792_EXEC_INCR_WORD(id) {		\
    c792Write(&c792p[id]->incrOffset, 1);}
#define C792_EXEC_GATE(id) {			\
    c792Write(&c792p[id]->swComm, 1);}

/* Function Prototypes */
STATUS c792Init (UINT32 addr, UINT32 addr_inc, int nadc, UINT16 crateID);
void   c792Status( int id, int reg, int sflag);
int    c792PrintEvent(int id, int pflag);
int    c792ReadEvent(int id, UINT32 *data);
int    c792FlushEvent(int id, int fflag);
int    c792ReadBlock(int id, volatile UINT32 *data, int nwrds);
STATUS c792IntConnect (VOIDFUNCPTR routine, int arg, UINT16 level, UINT16 vector);
STATUS c792IntEnable (int id, UINT16 evCnt);
STATUS c792IntDisable (int iflag);
STATUS c792IntResume (void);
UINT16 c792Sparse(int id, int over, int under);
int    c792Dready(int id);
void   c792ClearThresh(int id);
short  c792SetThresh(int id, int chan, short val);
void   c792Gate(int id);
short  c792Control(int id, short val);
short  c792BitSet2(int id, short val);
void   c792BitClear2(int id, short val);
void   c792EnableBerr(int id);
void   c792DisableBerr(int id);
void   c792IncrEventBlk(int id, int count);
void   c792IncrEvent(int id);
void   c792IncrWord(int id);
void   c792Enable(int id);
void   c792Disable(int id);
void   c792Clear(int id);
void   c792Reset(int id);

int    c792Type(int id);
UINT16 c775Modes(int id, int valid, int start_stop);
int c792SetPedestal(int id, UINT16 value);
int c792GetPedestal(int id);
int c775SetResolution(int id, UINT16 value);
int c775GetResolution(int id);


#endif /* __C792LIB__ */
