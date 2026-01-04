#include <fstream>
#include <vector>

#define SAVESTATE_VERSION 1

constexpr uint32_t SAVESTATE_MAGIC = { 'GBST' };

// Single POD type
template <typename T>
void GBWrite(std::ofstream& out, const T& data)
{
    out.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

// Read single POD type
template <typename T>
void GBRead(std::ifstream& in, T& data)
{
    in.read(reinterpret_cast<char*>(&data), sizeof(T));
}

// Write Vector/Array
template <typename T>
void GBWriteVec(std::ofstream& out, const std::vector<T>& vec)
{
    out.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(T));
}
template <typename T, size_t N>
void GBWriteArr(std::ofstream& out, const std::array<T, N>& vec)
{
    out.write(reinterpret_cast<const char*>(vec.data()), N * sizeof(T));
}

// Read Vector/Array
template <typename T>
void GBReadVec(std::ifstream& in, std::vector<T>& vec)
{
    in.read(reinterpret_cast<char*>(vec.data()), vec.size() * sizeof(T));
}
template <typename T, size_t N>
void GBReadArr(std::ifstream& in, std::array<T, N>& vec)
{
    in.read(reinterpret_cast<char*>(vec.data()), N * sizeof(T));
}

struct SaveStateHeader
{
    uint32_t magic = SAVESTATE_MAGIC; // "GameBoy STate"
    uint32_t version = SAVESTATE_VERSION;
    uint32_t romChecksum = 0;
};