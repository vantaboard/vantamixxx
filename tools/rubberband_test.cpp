#include <rubberband/RubberBandStretcher.h>

#include <fstream>
#include <iostream>
#include <limits>

#define RUBBERBANDV3 (RUBBERBAND_API_MAJOR_VERSION >= 2 && RUBBERBAND_API_MINOR_VERSION >= 7)

constexpr size_t buffer_size = 160000;
constexpr size_t channel_count = 8;
constexpr size_t channel_sample_count = buffer_size / channel_count;
constexpr size_t kAlignment = 32;

using RubberBand::RubberBandStretcher;

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "cannot open file " << argv[1] << std::endl;
        return -1;
    }

    float* interleaved_buffer = static_cast<float*>(
            std::aligned_alloc(kAlignment, sizeof(float) * buffer_size));

    float* deinterleaved_buffer[channel_count];

    for (int c = 0; c < channel_count; c++) {
        deinterleaved_buffer[c] = static_cast<float*>(std::aligned_alloc(
                kAlignment, sizeof(float) * channel_sample_count));
    }

    RubberBandStretcher::Options rubberbandOptions =
            RubberBandStretcher::OptionProcessRealTime;
#if RUBBERBANDV3
    rubberbandOptions |=
            RubberBandStretcher::OptionEngineFiner |
            // Process Channels Together. otherwise the result is not
            // mono-compatible. See #11361
            RubberBandStretcher::OptionChannelsTogether;
#endif
    RubberBandStretcher rubber_band(
            44100,
            channel_count,
            rubberbandOptions,
            1.0,
            1.0);

    // rubber_band.setTimeRatio(1.1);
    // rubber_band.setPitchScale(1.1);

#if RUBBERBANDV3
    int rubberband_version = rubber_band.getEngineVersion();
#else
    int rubberband_version = 2;
#endif
    std::cerr << "Using RubberBand " << rubberband_version << std::endl;

    while (!file.eof()) {
        size_t to_read = buffer_size * sizeof(float);
        do {
            file.read((char*)interleaved_buffer, to_read);
            to_read -= file.gcount();
        } while (to_read > 0 && !file.eof());

        for (int i = 0; i < channel_sample_count; i++) {
            for (int c = 0; c < channel_count; c++) {
                deinterleaved_buffer[c][i] =
                        interleaved_buffer[channel_count * i + c] /
                        std::numeric_limits<float>::max();
                // std::cerr<<std::fixed<<deinterleaved_buffer[c][i] << std::endl;
            }
        }
        rubber_band.process(deinterleaved_buffer, channel_sample_count, false);

        int frame_count = rubber_band.available();

        while (frame_count > 0) {
            int frame_to_fetch = std::min((int)channel_sample_count, frame_count);
            std::cerr << frame_count << " frame available. Fetching" << frame_to_fetch << std::endl;
            if (rubber_band.retrieve(
                        deinterleaved_buffer, frame_to_fetch) != frame_to_fetch) {
                std::cerr << "Problem with frame count" << std::endl;
                return 1;
            }
            std::fill(interleaved_buffer, interleaved_buffer + frame_to_fetch, .0f);
            for (int i = 0; i < frame_to_fetch; i++) {
                for (int c = 0; c < channel_count / 2; c++) {
                    interleaved_buffer[2 * i] +=
                            deinterleaved_buffer[2 * c][i] *
                            std::numeric_limits<float>::max();
                    interleaved_buffer[2 * i + 1] +=
                            deinterleaved_buffer[2 * c + 1][i] *
                            std::numeric_limits<float>::max();
                }
            }
            std::cout.write((char*)interleaved_buffer, sizeof(float) * frame_to_fetch * 2);
            frame_count -= frame_to_fetch;
        }
    }

    return 0;
}
