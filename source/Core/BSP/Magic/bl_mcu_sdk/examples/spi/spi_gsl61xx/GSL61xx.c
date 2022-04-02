#include "GSL61xx.h"
#include "GSL61xxCfg.h"
#include "hal_spi.h"

//#define DET_MEAN
#ifdef DET_MEAN
uint8_t DetMeanDown;
uint8_t DetMeanUp;
#endif

#ifdef TUNE_TEST
uint8_t DD[32];
uint8_t II;
#endif
GSL_FP_Data_T *GSL_FP;

GSL_FP_Data_T GSL_FP_6163 = {
    {
        Config_6163_NormalMode,
        sizeof(Config_6163_NormalMode) / sizeof(Chip_Config_T),
    },
    { 0xFF000020, 0xFF00002c, 0xFF08000c },
    { 120, 104, 120 * 104, { 10, 20, 40, 56, 70, 80, 112, 140 }, { 10, 20, 30, 40, 50, 60, 70, 80 } },
    { 0x07, 0xF0, 0x10 },
    { 80, 140, 0, 0, 0 },
    { 0, 0, 0 }
};

GSL_FP_Data_T GSL_FP_6163_00 = {
    {
        Config_6163_NormalMode_00,
        sizeof(Config_6163_NormalMode_00) / sizeof(Chip_Config_T),
    },
    { 0xFF000020, 0xFF00002c, 0xFF08000c },
    { 120, 104, 120 * 104, { 10, 20, 40, 56, 70, 80, 112, 140 }, { 10, 20, 30, 40, 50, 60, 70, 80 } },
    { 0x03, 0xF0, 0x10 },
    { 80, 140, 0, 0, 0 },
    { 0, 0, 0 }
};

GSL_FP_Data_T GSL_FP_6163_01 = {
    {
        Config_6163_NormalMode_01,
        sizeof(Config_6163_NormalMode_01) / sizeof(Chip_Config_T),
    },
    { 0xFF000020, 0xFF00002c, 0xFF08000c },
    { 120, 104, 120 * 104, { 10, 20, 40, 56, 70, 80, 112, 140 }, { 10, 20, 30, 40, 50, 60, 70, 80 } },
    { 0x07, 0xF0, 0x10 },
    { 80, 140, 0, 0, 0 },
    { 0, 0, 0 }
};

GSL_FP_Data_T GSL_FP_6163_02 = {
    {
        Config_6163_NormalMode_02,
        sizeof(Config_6163_NormalMode_02) / sizeof(Chip_Config_T),
    },
    { 0xFF000020, 0xFF00002c, 0xFF08000c },
    { 120, 104, 120 * 104, { 10, 20, 40, 56, 70, 80, 112, 140 }, { 10, 20, 30, 40, 50, 60, 70, 80 } },
    { 0x07, 0xF0, 0x10 },
    { 80, 140, 0, 0, 0 },
    { 0, 0, 0 }
};

GSL_FP_Data_T GSL_FP_6163_35 = {
    {
        Config_6163_NormalMode_35,
        sizeof(Config_6163_NormalMode_35) / sizeof(Chip_Config_T),
    },
    { 0xFF000020, 0xFF00002c, 0xFF08000c },
    { 120, 104, 120 * 104, { 10, 20, 40, 56, 70, 80, 112, 140 }, { 10, 20, 30, 40, 50, 60, 70, 80 } },
    { 0x03, 0xF0, 0x10 },
    { 60, 100, 0, 0, 0 },
    { 0, 0, 0 }
};

void GSL_FP_GetSetupData(uint32_t ChipID)
{
    if (0x35 == ((uint8_t)ChipID)) {
        GSL_FP = &GSL_FP_6163_35;
    } else if (0x02 == ((uint8_t)ChipID)) {
        GSL_FP = &GSL_FP_6163_02;
    } else if (0x01 == ((uint8_t)ChipID)) {
        GSL_FP = &GSL_FP_6163_01;
    } else if (0x00 == ((uint8_t)ChipID)) {
        GSL_FP = &GSL_FP_6163_00;
    } else {
        GSL_FP = &GSL_FP_6163;
    }
}

uint8_t FP_Spi_WriteRead(uint8_t Data)
{
    uint8_t Ret;
    struct device *spi = device_find("spi");

    spi = device_find("spi");

    spi_transmit_receive(spi, &Data, &Ret, 1, 0);

    return Ret;
}

void FP_Spi_RegRead(uint8_t Addr, uint32_t *pData)
{
    uint8_t Buf[4];

    FP_CS_ENABLE();

    FP_Spi_WriteRead(Addr);
    FP_Spi_WriteRead(0x00);

    Buf[0] = FP_Spi_WriteRead(0x00);
    Buf[1] = FP_Spi_WriteRead(0x00);
    Buf[2] = FP_Spi_WriteRead(0x00);
    Buf[3] = FP_Spi_WriteRead(0x00);

    *pData = *((uint32_t *)Buf);

    FP_CS_DISABLE();
}

void FP_Spi_RegWrite(uint8_t Addr, uint32_t Data)
{
    FP_CS_ENABLE();

    FP_Spi_WriteRead(Addr);
    FP_Spi_WriteRead(0xFF);
    FP_Spi_WriteRead((uint8_t)((Data >> 0) & 0xFF));
    FP_Spi_WriteRead((uint8_t)((Data >> 8) & 0xFF));
    FP_Spi_WriteRead((uint8_t)((Data >> 16) & 0xFF));
    FP_Spi_WriteRead((uint8_t)((Data >> 24) & 0xFF));

    FP_CS_DISABLE();
}

void FP_Spi_Reg32Write(uint32_t Addr, uint32_t Data)
{
    uint32_t Page = Addr / 0x80;
    uint32_t Reg = Addr % 0x80;

    FP_Spi_RegWrite(0xF0, Page);
    FP_Spi_RegWrite((uint8_t)Reg, Data);
}

void GSL_FP_LoadConfig(Chip_Config_T *ChipConfig, uint32_t ConfigLen)
{
    uint32_t i;
    uint32_t Addr;
    uint32_t Data;
    Chip_Config_T *Config = ChipConfig;

    for (i = 0; i < ConfigLen; i++, Config++) {
        Addr = Config->Addr;
        Data = Config->Data;

        if (0 == (Addr & 0xff000000)) {
            FP_Spi_RegWrite((uint8_t)Addr, Data);
        } else {
            FP_Spi_Reg32Write(Addr, Data);
        }
    }
}

void GSL_FP_ReadImageData(uint32_t Page, uint8_t Reg, uint8_t *Buf, uint32_t Len)
{
    uint32_t i;

    FP_Spi_RegWrite(0xF0, Page);

    FP_CS_ENABLE();
    bflb_platform_delay_us(SPI_DELAY);

    FP_Spi_WriteRead(Reg);
    FP_Spi_WriteRead(0x00);
    FP_Spi_WriteRead(0x00);
    bflb_platform_delay_us(10000);

    for (i = 0; i < GSL_FP->Sensor.ImageW * 2; i++) {
        FP_Spi_WriteRead(0x00);
    }

    for (i = 0; i < Len; i++) {
        Buf[i] = FP_Spi_WriteRead(0x00);
    }

    FP_CS_DISABLE();
    bflb_platform_delay_us(SPI_DELAY);
}

void GSL_FP_SensorReset(void)
{
    FP_RST_LOW();
    bflb_platform_delay_ms(2);
    FP_RST_HIGH();
    bflb_platform_delay_ms(6);
}

void GSL_FP_CaptureStart(void)
{
    FP_Spi_RegWrite(0xBF, 0x00);
    FP_Spi_Reg32Write(0xFF080024, 0x2000FFFF);
}

uint32_t GSL_FP_GetCaptureStatus(void)
{
    uint32_t Status = 0;

    FP_Spi_RegRead(0xBF, &Status);

    return Status;
}

void GSL_FP_WaitCaptureEnd(uint32_t bflb_platform_delay_msMS, uint32_t TimeOut)
{
    uint32_t TryTime = 0;

    bflb_platform_delay_ms(SPI_DELAY);

    while (GSL_FP_GetCaptureStatus() != 1) {
        if (TryTime++ > TimeOut) {
            break;
        }

        bflb_platform_delay_ms(1);
    }
}

void GSL_FP_ReadChipID(uint32_t *ChipID)
{
    uint8_t i;
    uint32_t Buf;

    for (i = 0; i < 5; i++) {
        FP_Spi_RegRead(0xFC, &Buf);

        if (0x61 == (Buf >> 24)) {
            break;
        }

        bflb_platform_delay_ms(10);
    }

    *ChipID = Buf;
}

void GSL_FP_SensorSetup(void)
{
    int i;
    uint32_t ChipID;
    Chip_Config_T *Config;
    uint32_t ConfigLen;

    GSL_FP_ReadChipID(&ChipID);
    GSL_FP_GetSetupData(ChipID);

    Config = GSL_FP->Config.Config_NormalMode;
    ConfigLen = GSL_FP->Config.Config_NormalMode_Len;

    for (i = 0; i < ConfigLen; i++, Config++) {
        if (Config->Addr == GSL_FP->Reg.RegGain12) {
            GSL_FP->Detect.DetGain12 = Config->Data;
            GSL_FP->Capture.CapGain12 = Config->Data;
        } else if (Config->Addr == GSL_FP->Reg.RegGain34) {
            GSL_FP->Detect.DetGain34 = Config->Data;
            GSL_FP->Capture.CapGain34 = Config->Data;
        } else if (Config->Addr == GSL_FP->Reg.RegDac) {
            GSL_FP->Detect.DetDac = Config->Data;
            GSL_FP->Capture.CapDac = Config->Data;
        }
    }
}

uint8_t GSL_FP_GetDacShift(uint32_t Gain12, uint32_t Gain34, uint8_t *Gain12R, uint8_t *Gain34R, uint32_t Ref)
{
    uint8_t Gain2 = (uint8_t)((Gain12 >> 16) & 0x7);
    uint8_t Gain3 = (uint8_t)(Gain34 & 0x7);
    uint8_t Gain4 = (uint8_t)((Gain34 >> 8) & 0x7);
    uint8_t Ref2 = (uint8_t)((Ref >> 4) & 0x7);
    uint8_t Ref3 = (uint8_t)((Ref >> 8) & 0x7);
    uint8_t Ref4 = (uint8_t)((Ref >> 12) & 0x7);
    uint32_t DacShift;

    DacShift = 374 * (Gain12R[Gain2] * Gain34R[Gain3] * Gain34R[Gain4]) / (Gain12R[Ref2] * Gain34R[Ref3] * Gain34R[Ref4]) / 100;

    if (DacShift < 1) {
        DacShift = 1;
    }

    return (uint8_t)DacShift;
}

void GSL_FP_SensorTune(void)
{
    uint32_t DacBase, DacDetect;
    uint32_t Temp;

#if 0
    GSL_FP->Tune.DacShift = GSL_FP_GetDacShift(GSL_FP->Capture.CapGain12,
                            GSL_FP->Capture.CapGain34,
                            GSL_FP->Sensor.Gain12Ratio,
                            GSL_FP->Sensor.Gain34Ratio,
                            0x1227);
#endif

    DacBase = (GSL_FP->Capture.CapDac << 24) >> 24;

    if (DacBase > DAC_TUNE_OFFSET_MIN) {
        GSL_FP->Tune.DacMin = DacBase - DAC_TUNE_OFFSET_MIN;
    } else {
        GSL_FP->Tune.DacMin = 0x00;
    }

    if (DacBase < 0xFF - DAC_TUNE_OFFSET_MAX) {
        GSL_FP->Tune.DacMax = DacBase + DAC_TUNE_OFFSET_MAX;
    } else {
        GSL_FP->Tune.DacMax = 0xFF;
    }

    Temp = 200 / GSL_FP->Tune.DacShift;

    if (DacBase > Temp) {
        DacDetect = DacBase - Temp;
    } else {
        DacDetect = 0;
    }

    Temp = (GSL_FP->Detect.DetDac >> 8) << 8;
    GSL_FP->Detect.DetDac = Temp + DacDetect;
}

uint8_t GSL_FP_DetectFingerDown(void)
{
    uint8_t i, j;
    uint8_t Buf[160];
    uint8_t Mean;
    uint32_t Temp;
    uint32_t Page;
    uint32_t Reg;
    uint32_t ImageW = GSL_FP->Sensor.ImageW;
    uint32_t ImageH = GSL_FP->Sensor.ImageH;
    uint8_t DownCnt = 0;

    FP_Spi_Reg32Write(GSL_FP->Reg.RegDac, GSL_FP->Detect.DetDac);
    GSL_FP_CaptureStart();
    GSL_FP_WaitCaptureEnd(5, 50);

    for (i = 0; i < 4; i++) {
        Temp = (ImageH / 16 + i * (ImageH - ImageH / 8) / 3) * ImageW;
        Page = Temp / 0x80;
        Reg = Temp % 0x80;

        GSL_FP_ReadImageData(Page, (uint8_t)Reg, Buf, ImageW);

        for (Temp = 0, j = 0; j < ImageW; j++) {
            Temp += Buf[j];
        }

        Mean = (uint8_t)(Temp / ImageW);

#ifdef DET_MEAN

        if (Mean < DetMeanDown) {
            DetMeanDown = Mean;
        }

#endif

        if (Mean < GSL_FP->Detect.DownThreshold) {
            DownCnt++;
        }

        if (DownCnt >= 3) {
            return FINGER_DOWN;
        }
    }

    return FINGER_UP;
}

uint8_t GSL_FP_DetectFingerUp(void)
{
    uint8_t i, j;
    uint8_t Buf[160];
    uint8_t Mean;
    uint32_t Temp;
    uint32_t Page;
    uint32_t Reg;
    uint32_t ImageW = GSL_FP->Sensor.ImageW;
    uint32_t ImageH = GSL_FP->Sensor.ImageH;
    uint8_t UpCnt = 0;

    FP_Spi_Reg32Write(GSL_FP->Reg.RegDac, GSL_FP->Detect.DetDac);
    GSL_FP_CaptureStart();
    GSL_FP_WaitCaptureEnd(5, 50);

    for (i = 0; i < 4; i++) {
        Temp = (ImageH / 16 + i * (ImageH - ImageH / 8) / 3) * ImageW;
        Page = Temp / 0x80;
        Reg = Temp % 0x80;

        GSL_FP_ReadImageData(Page, (uint8_t)Reg, Buf, ImageW);

        for (Temp = 0, j = 0; j < ImageW; j++) {
            Temp += Buf[j];
        }

        Mean = (uint8_t)(Temp / ImageW);
#ifdef DET_MEAN

        if (Mean > DetMeanUp) {
            DetMeanUp = Mean;
        }

#endif

        if (Mean > GSL_FP->Detect.UpThreshold) {
            UpCnt++;
        }

        if (UpCnt >= 4) {
            return FINGER_UP;
        }
    }

    return FINGER_DOWN;
}

int GSL_FP_DacTune(uint8_t *Image, uint32_t Width, uint32_t Height, uint32_t *TuneDac)
{
    int i, j, n;
    int Ret = 1;
    uint8_t Dac = (uint8_t)((*TuneDac) & 0xFF);
    uint8_t DacShift = GSL_FP->Tune.DacShift;
    uint8_t DacMax = GSL_FP->Tune.DacMax;
    uint8_t DacMin = GSL_FP->Tune.DacMin;
    uint8_t DacOffset;
    uint32_t StartX, StartY;
    uint32_t Temp;
    uint32_t Mean = 255;

    for (n = 0; n < 5; n++) {
        if (0 == n) {
            StartX = (Width - TUNE_IMAGE_W) / 2;
            StartY = (Height - TUNE_IMAGE_H) / 2;
        } else if (1 == n) {
            StartX = (Width - TUNE_IMAGE_W) / 4;
            StartY = (Height - TUNE_IMAGE_H) / 4;
        } else if (2 == n) {
            StartX = (Width - TUNE_IMAGE_W) * 3 / 4;
            StartY = (Height - TUNE_IMAGE_H) / 4;
        } else if (3 == n) {
            StartX = (Width - TUNE_IMAGE_W) / 4;
            StartY = (Height - TUNE_IMAGE_H) * 3 / 4;
        } else if (4 == n) {
            StartX = (Width - TUNE_IMAGE_W) * 3 / 4;
            StartY = (Height - TUNE_IMAGE_H) * 3 / 4;
        }

        Temp = 0;

        for (i = 0; i < TUNE_IMAGE_H; i++) {
            for (j = 0; j < TUNE_IMAGE_W; j++) {
                Temp += Image[(StartY + i) * Width + StartX + j];
            }
        }

        Temp /= TUNE_IMAGE_W * TUNE_IMAGE_H;

        if (Temp < Mean) {
            Mean = Temp;
        }
    }

#ifdef TUNE_TEST
    DD[II++] = Dac;
    DD[II++] = Mean;
#endif

    if (Mean < 100) {
        DacOffset = (110 - Mean) / DacShift;

        if (DacOffset > DAC_TUNE_STEP_MAX) {
            DacOffset = DAC_TUNE_STEP_MAX;
        } else if (DacOffset < 1) {
            DacOffset = 1;
        }

        if (Dac < 0xFF - DacOffset) {
            Dac += DacOffset;
        } else {
            Dac = 0xFF;
        }
    } else if (Mean > 120) {
        DacOffset = (Mean - 110) / DacShift;

        if (DacOffset > DAC_TUNE_STEP_MAX) {
            DacOffset = DAC_TUNE_STEP_MAX;
        } else if (DacOffset < 1) {
            DacOffset = 1;
        }

        if (Dac > DacOffset) {
            Dac -= DacOffset;
        } else {
            Dac = 0x00;
        }
    } else {
        Ret = 0;
    }

    if (Dac < DacMin) {
        Dac = DacMin;
    }

    if (Dac > DacMax) {
        Dac = DacMax;
    }

    *TuneDac = ((*TuneDac) & 0xFFFFFF00) + Dac;

    return Ret;
}

void GSL_FP_CaptureImage(BYTE *pImageBmp)
{
    int Ret;
    uint8_t CaptureCnt;
    uint32_t TuneDac;

    TuneDac = GSL_FP->Capture.CapDac;

    for (CaptureCnt = 0; CaptureCnt < DAC_TUNE_CNT_MAX; CaptureCnt++) {
        FP_Spi_Reg32Write(GSL_FP->Reg.RegDac, TuneDac);
        GSL_FP_CaptureStart();
        GSL_FP_WaitCaptureEnd(5, 50);
        GSL_FP_ReadImageData(0, 0, pImageBmp, GSL_FP->Sensor.ImageSize);
        Ret = GSL_FP_DacTune(pImageBmp, GSL_FP->Sensor.ImageW, GSL_FP->Sensor.ImageH, &TuneDac);

        if (0 == Ret) {
            break;
        }
    }

    GSL_FP->Capture.CapDac = TuneDac;
}

void FP_GPIO_Configuration(void)
{
    gpio_set_mode(FP_CS_PIN, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(FP_RESET_PIN, GPIO_OUTPUT_PP_MODE);
}

uint8_t FP_Spi_Init()
{
    spi_register(SPI0_INDEX, "spi");

    struct device *spi = device_find("spi");

    spi = device_find("spi");

    if (spi) {
        device_open(spi, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    }

    return 0;
}

uint8_t GSL61xx_init(void)
{
    FP_GPIO_Configuration();

    FP_Spi_Init();

    FP_CS_DISABLE();

    bflb_platform_delay_ms(1);

    GSL_FP_SensorReset();
    GSL_FP_SensorSetup();
    GSL_FP_SensorTune();

    FP_RST_LOW();

    return 0;
}

BYTE CaptureGSL61xx(BYTE *pImageBmp)
{
    uint8_t WaitDownCnt;

    /* 6C 63 62 10 D5 6C 63 62 D0 95 */
    //memset(pImageBmp, 0xFF, GSL_FP->Capture.ImageSize);
    GSL_FP_SensorReset();
    GSL_FP_LoadConfig(GSL_FP->Config.Config_NormalMode, GSL_FP->Config.Config_NormalMode_Len);

    for (WaitDownCnt = 0; WaitDownCnt < 3; WaitDownCnt++) {
        if (GSL_FP_DetectFingerDown() != 0) {
            break;
        }
    }

#ifdef TUNE_TEST

    for (II = 0; II < 10; II++) {
        DD[II] = 0;
    }

    II = 0;
#endif

    GSL_FP_CaptureImage(pImageBmp);

#if 0
    {
        uint32_t RR;
        GSL_FP_ReadChipID(&RR);
        pImageBmp[0] = (uint8_t)(RR >> 24);
        pImageBmp[1] = (uint8_t)(RR >> 16);
        pImageBmp[2] = (uint8_t)(RR >> 8);
        pImageBmp[3] = (uint8_t)(RR >> 0);
    }
#endif

    FP_RST_LOW();

#ifdef TUNE_TEST
    pImageBmp[0] = GSL_FP->Tune.DacShift;

    for (II = 0; II < 10; II++) {
        pImageBmp[1 + II] = DD[II];
    }

#endif

#ifdef DET_MEAN
    pImageBmp[0] = DetMeanDown;
    pImageBmp[1] = DetMeanUp;
    pImageBmp[2] = 0xFF;
#endif

    return 0;
}
