#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <vector>

#include <raylib.h>

inline int pitch = 0;
inline int formant = 0;

void PlayAudio(Sound audio);
void UpdatePitchAndFormantByValue(int value);
