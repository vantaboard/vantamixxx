#include <fstream>
#include <iostream>
#include <vector>

constexpr uint8_t atomPreambleSize = 8;
constexpr uint8_t extendedSizeFieldSize = 8;

constexpr uint32_t be32(const char* bytes) {
    return ((uint8_t)bytes[0] << 24) | ((uint8_t)bytes[1] << 16) |
            ((uint8_t)bytes[2] << 8) | (uint8_t)bytes[3];
}

constexpr uint64_t be64(const char* bytes) {
    return ((uint64_t)be32(bytes) << 32) | be32(bytes + 4);
}

constexpr auto moovAtom = be32("moov");
constexpr auto udtaAtom = be32("udta");
constexpr auto stemAtom = be32("stem");

bool read(std::ifstream& stream, char* buffer, uint64_t size) {
    stream.read(buffer, size);
    return stream.gcount() == size;
}

uint32_t recursive_search(std::ifstream& file,
        const std::vector<uint32_t>& path,
        uint32_t box_size = 0,
        uint32_t pathIdx = 0) {
    if (path.size() <= pathIdx) {
        return box_size;
    }

    uint32_t byteRead = 0;
    char buffer[atomPreambleSize];
    while (!file.eof() || (box_size && byteRead >= box_size)) {
        file.read(buffer, atomPreambleSize);
        byteRead += box_size;
        if (be32(buffer + 4) == path[pathIdx]) {
            return recursive_search(file, path, be32(buffer) - atomPreambleSize, pathIdx + 1);
        }
        file.seekg(be32(buffer) - atomPreambleSize, std::ios::cur);
    }
    return -1;
}

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

    std::vector<uint32_t> path = {
            be32("moov"),
            be32("udta"),
            be32("stem"),
    };

    int manifest_size = 0;
    if ((manifest_size = recursive_search(file, path)) > 0) {
        std::cerr << "steam at " << file.tellg() << std::endl;
        std::string stemData(manifest_size, 0);
        if (!read(file, stemData.data(), manifest_size)) {
            std::cerr << "failed to read stem metadata" << std::endl;
            return 1;
        }
        std::cout << stemData << std::endl;
        return 0;
    }
    std::cerr << "not a stem file" << std::endl;
    return 1;
}
