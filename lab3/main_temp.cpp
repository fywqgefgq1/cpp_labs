#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <cstring>

typedef unsigned int uint32;
typedef unsigned short uint16;
using namespace std;

struct WavHeader
{
    char chunkID[4];
    uint32 chunkSize;
    char format[4];

    char fmt[4];
    uint32 fmtSize;
    unsigned short audioFormat;
    unsigned short numChannels;
    uint32 sampleRate;
    uint32 byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;

    char data[4];
    uint32 dataSize;
};

uint32 TimeToBytePosition(uint32 time, WavHeader* Header)
{
    uint32 sampleNumber = (time * Header->sampleRate * Header->numChannels);
    uint32 bitPos = sampleNumber * Header->bitsPerSample;
    return bitPos / 8;
}

class SoundProcessor
{
    protected:
        WavHeader Header;
        std::vector<char> audioData;
    public:

        SoundProcessor()
        {
            audioData = std::vector<char>();
        }
        SoundProcessor(std::string dir)
        {
            audioData = std::vector<char>();
            if (!Read(dir))
            {
                std::cerr<<"Input file error"<<std::endl;
            }
        }

        uint32 audioLength()
        {
            return Header.dataSize * sizeof(char) * 8 / 
            (Header.bitsPerSample * Header.sampleRate * Header.numChannels);
        }

        std::vector<char>* getAudiodata()
        {
            return &audioData;
        }
        WavHeader* getHeader()
        {
            return &Header;
        }

        bool Read(std::string dir)
        {
            std::ifstream inputFile(dir, std::ios::binary);
            if (inputFile.fail() || !inputFile.is_open())
            {
                return false;
            }
            char buffer[sizeof(Header)];
            inputFile.read(buffer, sizeof(buffer));
            memcpy(&Header, buffer, sizeof(Header));

            audioData.resize(Header.dataSize);
            inputFile.read(audioData.data(), Header.dataSize);
            inputFile.close();

            return true;
        }
        bool Write(std::string Name = "Output.wav")
        {
            std::ofstream Output(Name);
            char buffer[sizeof(Header)];
            mempcpy(buffer, &Header, sizeof(Header));
            Output.write(buffer, sizeof(buffer));
            Output.write(audioData.data(), Header.dataSize);
            return true;
        }
        void Process();
};

class MuteProcessor : public SoundProcessor
{
    public:
        MuteProcessor(std::string dir) : SoundProcessor(dir) {};
    void Process(uint32 start, uint32 end)
    {
        uint32 startpos = TimeToBytePosition(start, &Header);
        uint32 endpos  = TimeToBytePosition(end, &Header);
        memset(&audioData[startpos], 0, (endpos - startpos));
    }
};

class MixerProcessor : public SoundProcessor
{
    private:
        SoundProcessor* audio2;
    public:
        MixerProcessor(std::string dir1, std::string dir2) : SoundProcessor(dir1)
        {
            audio2 = &SoundProcessor(dir2);
        }
        void Process(uint32 start_time, uint32 end_time, uint32 start_time1 = 0)
        {
            uint32 startpos = TimeToBytePosition(start_time, &Header);
            uint32 endpos  = TimeToBytePosition(end_time, &Header);

            WavHeader* audio2head = audio2->getHeader();
            uint32 startpos1 = TimeToBytePosition(start_time1, audio2head);
            
            std::vector<char>* audio2data = audio2->getAudiodata();
            memcpy(&audioData[startpos], &(*audio2data)[startpos1], endpos - startpos);
        }
};

class PitchProcessor : public SoundProcessor
{
    public:
        PitchProcessor(std::string dir) : SoundProcessor(dir) {};
        void Process(double multiplier)
        {
            Header.sampleRate*=multiplier;
        }
};

int main(int argc, char *argv[])
{
    MuteProcessor msp("BALLOUT.wav");
    uint32 len = msp.audioLength();
    msp.Process(15, 20);
    msp.Write("Output.wav");

    MixerProcessor mixsp("Output.wav", "MixExample.wav");
    mixsp.Process(12, 15, 1);
    mixsp.Write("Output.wav");

    PitchProcessor psp("Output.wav");
    psp.Process(0.85);
    psp.Write("Output.wav");
    return 0;
}
