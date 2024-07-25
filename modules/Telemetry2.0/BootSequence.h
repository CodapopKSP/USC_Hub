#include <avr/pgmspace.h>

const char BootSequenceString_00[] PROGMEM = "Loading Kerbnal";
const char BootSequenceString_01[] PROGMEM = "Loading ENV_VARS";
const char BootSequenceString_02[] PROGMEM = "Begin System Check";
const char BootSequenceString_03[] PROGMEM = "Mount File System";
const char BootSequenceString_04[] PROGMEM = "Mounting /boot";
const char BootSequenceString_05[] PROGMEM = "Mounted /boot";
const char BootSequenceString_06[] PROGMEM = "Starting Restore";
const char BootSequenceString_07[] PROGMEM = "Checking Network";
const char BootSequenceString_08[] PROGMEM = "Begin File Audit";
const char BootSequenceString_09[] PROGMEM = "/etc";
const char BootSequenceString_10[] PROGMEM = "/etc/simpit";
const char BootSequenceString_11[] PROGMEM = "/etc/simpit/ksp";
const char BootSequenceString_12[] PROGMEM = "/etc/coda_hub";
const char BootSequenceString_13[] PROGMEM = "/etc/coda_hub/telemetry";
const char BootSequenceString_14[] PROGMEM = "Corruption detected: 90%";
const char BootSequenceString_15[] PROGMEM = "Ignoring.........OK";
const char BootSequenceString_16[] PROGMEM = "Loading Users...";
const char BootSequenceString_17[] PROGMEM = "/usr/rettoph";
const char BootSequenceString_18[] PROGMEM = "/usr/CodapopKSP";
const char BootSequenceString_19[] PROGMEM = "Load UntitledSpaceCraft";
const char BootSequenceString_20[] PROGMEM = "Done.";

const char *const BootSequenceStringTable[] PROGMEM = {
    BootSequenceString_00,
    BootSequenceString_01,
    BootSequenceString_02,
    BootSequenceString_03,
    BootSequenceString_04,
    BootSequenceString_05,
    BootSequenceString_06,
    BootSequenceString_07,
    BootSequenceString_08,
    BootSequenceString_09,
    BootSequenceString_10,
    BootSequenceString_11,
    BootSequenceString_12,
    BootSequenceString_13,
    BootSequenceString_14,
    BootSequenceString_15,
    BootSequenceString_16,
    BootSequenceString_17,
    BootSequenceString_18,
    BootSequenceString_19,
    BootSequenceString_20,
};

const char SplashSequenceString_00[] PROGMEM = "Ordering snacks...";
const char SplashSequenceString_01[] PROGMEM = "Checking funds...";
const char SplashSequenceString_02[] PROGMEM = "Doing science...";
const char SplashSequenceString_03[] PROGMEM = "Finding Jeb...";
const char SplashSequenceString_04[] PROGMEM = "Ready!";

const char *const SpashSequenceStringTable[] PROGMEM = {
    SplashSequenceString_00,
    SplashSequenceString_01,
    SplashSequenceString_02,
    SplashSequenceString_03,
    SplashSequenceString_04
};