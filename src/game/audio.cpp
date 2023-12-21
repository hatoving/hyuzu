#include "audio.h"

void PlayAudio(Sound audio) {
    PlaySound(audio);
}

void UpdatePitchAndFormantByValue(int value) {
    pitch += value;
    formant -= value;

    
}
