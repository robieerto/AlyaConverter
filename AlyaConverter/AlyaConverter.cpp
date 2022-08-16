// AlyaConverter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <cstring>
#include <iostream>
#include "AlyaConverter.h"

// for testing purposes
char input[12] = { 'a', 'b', '\x0d', (char)0b11111111, '+', '0', '0', '0', '5', '.', '5', '\x0d' };
// new output format
char output[14];

ParsingState state = sync;
WeighingInputFormat inputFormat;
uint8_t numCount = 0; // for counting symbols for weighing value

/* parse input and return true if the whole input is read */
bool parseInput(char inputChar) {
    bool isParsed = false;
    switch (state) {
    case sync:
        if (inputChar == '\x0d') {
            state = status;
        }
        break;
    case status:
        inputFormat.isWeightNotOnDisplay = inputChar & 1u;
        inputFormat.mode = (InputMode)(bool)(inputChar & (1u << 1));
        inputFormat.isZeroedAuto = inputChar & (1u << 2);
        inputFormat.isOutOfRange = inputChar & (1u << 3);
        inputFormat.isStabilized = inputChar & (1u << 4);
        inputFormat.isUnderMinRange = inputChar & (1u << 5);
        state = polarity;
        break;
    case polarity:
        inputFormat.sign = inputChar;
        state = weight;
        break;
    case weight:
        inputFormat.weight[numCount] = inputChar;
        numCount++;
        if (numCount == WEIGHT_SIZE) {
            numCount = 0;
            state = sync;
            isParsed = true;
        }
        break;
    default:
        break;
    }

    return isParsed;
}

// find index of decimal dot in input
int8_t findDecimalPoint() {
    for (int8_t i = 0; i < WEIGHT_SIZE; i++) {
        if (inputFormat.weight[i] == '.') {
            return i;
            break;
        }
    }
    return -1;
}

void convertOutput() {
    // STX: starting symbol
    output[0] = '\x02';
    // MODE: M - measuring, P - counting, O - other
    output[1] = 'M';
    // STAB: if weighing is stabilized
    if (inputFormat.isOutOfRange) {
        output[2] = 'F';
    }
    else {
        output[2] = (inputFormat.isStabilized) ? 'S' : 'U';
    }
    // NDC: number of decimal digits
    int8_t decPoint = findDecimalPoint(); // if it isn't found (-1), number is 0
    output[3] = '0' + ((decPoint >= 0) ? (WEIGHT_SIZE - 1 - decPoint) : 0);
    // SGN: sign
    output[4] = (inputFormat.sign == '-') ? '-' : ' ';
    // W1 - W6: measuring value
    memset(&output[5], '0', sizeof(char) * 6); // default number is '0'
    // copy values from the end; indexes in output[] are from 5 to 10
    for (int8_t i = WEIGHT_SIZE - 1, outputIdx = 10; i >= 0; i--) {
        // if decimal point is found, skip it in the source array
        if (i == decPoint) {
            i--;
        }
        output[outputIdx] = inputFormat.weight[i];
        outputIdx--;
    }
    // BCC: checksum, XOR values from indexes 1 to 10
    output[11] = output[1];
    for (int8_t i = 2; i <= 10; i++) {
        output[11] ^= output[i];
    }
    // ETX: ending symbol
    output[12] = '\x03';
}

int main()
{
    // for testing purposes
    for (int8_t i = 0; i < sizeof(input); i++) {
        char actChar = input[i];
        // parseInput accepts 1 character
        if (parseInput(actChar) == true) {
            convertOutput();
        }
    }
}
