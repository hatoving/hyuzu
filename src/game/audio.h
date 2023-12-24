#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <vector>
#include <cmath>

#include <raylib.h>

inline float semitone = 0;
inline float formant = 0;

void PlayAudio(Sound audio);
void UpdatePitchAndFormantByValue(Sound audio, float value);
