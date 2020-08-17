#include "KX134.h"

// register map
#define KX134_MAN_ID 0x00
#define KX134_PART_ID 0x01
#define KX134_XADP_L 0x02
#define KX134_XADP_H 0x03
#define KX134_YADP_L 0x04
#define KX134_YADP_H 0x05
#define KX134_ZADP_L 0x06
#define KX134_ZADP_H 0x07
#define KX134_XOUT_L 0x08
#define KX134_XOUT_H 0x09
#define KX134_YOUT_L 0x0A
#define KX134_YOUT_H 0x0B
#define KX134_ZOUT_L 0x0C
#define KX134_ZOUT_H 0x0D
#define KX134_COTR 0x12
#define KX134_WHO_AM_I 0x13
#define KX134_TSCP 0x14
#define KX134_TSPP 0x15
#define KX134_INS1 0x16
#define KX134_INS2 0x17
#define KX134_INS3 0x18
#define KX134_STATUS_REG 0x19
#define KX134_INT_REL 0x1A
#define KX134_CNTL1 0x1B
#define KX134_CNTL2 0x1C
#define KX134_CNTL3 0x1D
#define KX134_CNTL4 0x1E
#define KX134_CNTL5 0x1F
#define KX134_CNTL6 0x20
#define KX134_ODCNTL 0x21
#define KX134_INC1 0x22
#define KX134_INC2 0x23
#define KX134_INC3 0x24
#define KX134_INC4 0x25
#define KX134_INC5 0x26
#define KX134_INC6 0x27
#define KX134_TILT_TIMER 0x29
#define KX134_TDTRC 0x2A
#define KX134_TDTC 0x2B
#define KX134_TTH 0x2C
#define KX134_TTL 0x2D
#define KX134_FTD 0x2E
#define KX134_STD 0x2F
#define KX134_TLT 0x30
#define KX134_TWS 0x31
#define KX134_FFTH 0x32
#define KX134_FFC 0x33
#define KX134_FFCNTL 0x34
#define KX134_TILT_ANGLE_LL 0x37
#define KX134_TILT_ANGLE_HL 0x38
#define KX134_HYST_SET 0x39
#define KX134_LP_CNTL1 0x3A
#define KX134_LP_CNTL2 0x3B
#define KX134_WUFTH 0x49
#define KX134_BTSWUFTH 0x4A
#define KX134_BTSTH 0x4B
#define KX134_BTSC 0x4C
#define KX134_WUFC 0x4D
#define KX134_SELF_TEST 0x5D
#define KX134_BUF_CNTL1 0x5E
#define KX134_BUF_CNTL2 0x5F
#define KX134_BUF_STATUS_1 0x60
#define KX134_BUF_STATUS_2 0x61
#define KX134_BUF_CLEAR 0x62
#define KX134_BUF_READ 0x63
#define KX134_ADP_CNTL1 0x64
#define KX134_ADP_CNTL2 0x65
#define KX134_ADP_CNTL3 0x66
#define KX134_ADP_CNTL4 0x67
#define KX134_ADP_CNTL5 0x68
#define KX134_ADP_CNTL6 0x69
#define KX134_ADP_CNTL7 0x6A
#define KX134_ADP_CNTL8 0x6B
#define KX134_ADP_CNTL9 0x6C
#define KX134_ADP_CNTL10 0x6D
#define KX134_ADP_CNTL11 0x6E
#define KX134_ADP_CNTL12 0x6F
#define KX134_ADP_CNTL13 0x70
#define KX134_ADP_CNTL14 0x71
#define KX134_ADP_CNTL15 0x72
#define KX134_ADP_CNTL16 0x73
#define KX134_ADP_CNTL17 0x74
#define KX134_ADP_CNTL18 0x75
#define KX134_ADP_CNTL19 0x76

KX134::KX134(PinName mosi, PinName miso, PinName sclk, PinName cs, PinName int1,
             PinName int2, PinName rst)
    : _spi(mosi, miso, sclk, cs), _int1(int1), _int2(int2), _rst(rst)
{
    printf("Creating KX134-1211\r\n");
}

bool KX134::init()
{
    printf("Initing KX134-1211\r\n");

    return init_asynch_reading();
}

bool KX134::init_asynch_reading()
{
    printf("Initing asynch\r\n");
    return false;
}
