#pragma once

/* number of symbols for weighing value */
constexpr auto WEIGHT_SIZE = 6;

/* parsing state */
enum ParsingState { status, polarity, weight, sync };

/* input format */
enum InputMode { brutto = 0, netto = 1 };
struct WeighingInputFormat {
    bool isWeightNotOnDisplay;
    InputMode mode;
    bool isZeroedAuto;
    bool isOutOfRange;
    bool isStabilized;
    bool isUnderMinRange;
    char sign;
    char weight[WEIGHT_SIZE];
};

/* output format, NOT CURRENTLY USED */
enum OutputMode { measure, count, other };
enum OutputStab { stable, unstable, outOfRange };
struct WeighingOutputFormat {
    OutputMode mode;
    OutputStab stability;
    uint8_t decCount;
    char sign;
    char weight[6];
    uint8_t checksum;
};
