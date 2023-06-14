#include <Arduino.h>

#ifndef DATA_PASIEN
#define DATA_PASIEN 1

enum
{
    LAKI_LAKI = true,
    PEREMPUAN = false
};
struct pasien_t
{
    uint16_t id;
    char nama[200];
    bool jenis_kelamin;
    uint8_t usia;
    float berat_badan;
};

#endif