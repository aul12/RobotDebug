#ifndef IDPRESETS_HPP
#define IDPRESETS_HPP
/*
 * A: Analog
 * B: Bus (I²C)
 * C: Calculated (Pos, Ballwinkel)
 * D: Digital (I/O)
 * E: Motor (Fahrtrichtung, Motor, Linie)
 */
enum ADDRESS{
    //ADC Channel-A
    ADCA0 = 0xA0,
    ADCA1 = 0xA1,
    ADCA2 = 0xA2,
    ADCA3 = 0xA3,
    ADCA4 = 0xA4,
    ADCA5 = 0xA5,
    ADCA6 = 0xA6,
    ADCA7 = 0xA7,

    //I²C
    BNO055_GRAD = 0xB0,
    BNO055_ROT = 0xB1,
    US0 = 0xB3,
    US1 = 0xB4,
    US2 = 0xB5,

    PIXIE_ERROR = 0xB7,
    PIXIEX1 = 0xB8,
    PIXIEY1 = 0xB9,
    PIXIEW1 = 0xBA,
    PIXIEH1 = 0xBB,

    //Berechnete Werte
    BALLWINKEL = 0xC0,
    POSX = 0xC2,
    POSY = 0xC3,
    SOLLPHI = 0xC4,
    DELTAPHI = 0xC5,
    SPIELPROGRAMM = 0xC6,
    KAMERA_WINKEL = 0xC7,
    BALLDISTANZ = 0xC8,
    BALLDA = 0xC9,

    //Digital
    MOTORSCHALTER = 0xD0,
    DISPLAYSCHALTER = 0xD1,
    LINIENSCHALTER = 0xD2,
    PROGRAMMLINKS = 0xD3,
    PROGRAMMRECHTS = 0xD4,
    EEPROMTASTER = 0xD5,
    LICHTSCHRANKE = 0xD6,
    AKTIVSCHALTER = 0xD7,

    //Motor
    MOTO0 = 0xE0,
    MOTO1 = 0xE1,
    MOTO2 = 0xE2,
    MOTO3 = 0xE3,
    DRIBBLER = 0xE4,
    SCHUSS = 0xE5,
    OUTSTATUS = 0xE6,
    OUTWINKEL = 0xE7
};
#endif // IDPRESETS_HPP
