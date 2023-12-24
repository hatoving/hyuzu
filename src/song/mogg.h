#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

namespace Hyuzu {
    namespace MOGG {
        struct MOGG {
            uint32_t unk1, sample_rate, channels, samples, unk2, channels2, size;
            std::vector<uint8_t> data;
        };
    }
}
