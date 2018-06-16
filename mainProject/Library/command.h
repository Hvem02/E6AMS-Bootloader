//
// Created by munk on 5/27/18.
//

#ifndef E6AMS_COMMAND_H
#define E6AMS_COMMAND_H


typedef enum {
    AckNack = 0,
    Control = 1,
    FWReset = 2,
    FWReady = 3,
    FWSegCount = 4,
    FWSeg = 5,
    MaxProfiles = 6,
} Command;

#endif //E6AMS_COMMAND_H
