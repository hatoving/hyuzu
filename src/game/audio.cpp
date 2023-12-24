#include "audio.h"

void PlayAudio(Sound audio) {
    PlaySound(audio);
}

void UpdatePitchAndFormantByValue(Sound audio, float value) {
    semitone += value;
    float pitch = pow(2, semitone / 12);
    formant = -pitch;

    printf("%f, %i\n", pitch, semitone);

    SetSoundPitch(audio, pitch);
}
