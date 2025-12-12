#include <iostream>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <cstring>
#include <math.h>
#include <map>
#include <limits.h>

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
    uint16 audioFormat;
    uint16 numChannels;
    uint32 sampleRate;
    uint32 byteRate;
    uint16 blockAlign;
    uint16 bitsPerSample;

    char data[4];
    uint32 dataSize;
};

struct CommandInfo 
{
    std::string name;
    std::string description;
    std::vector<std::string> parameters;
    std::string example;
};

uint32 TimeToBytePosition(uint32 time, WavHeader* Header)
{
    uint32 sampleNumber = (time * Header->sampleRate * Header->numChannels);
    
    uint32 bitPos = sampleNumber * Header->bitsPerSample;
    return bitPos / 8;
}

class SoundFile
{
    protected:
        WavHeader Header;
        std::vector<char> audioData;
    public:

        SoundFile()
        {
            audioData = std::vector<char>();
        }
        SoundFile(std::string dir)
        {
            audioData = std::vector<char>();
            if (!Read(dir))
            {
                std::cerr<<"Can't read input"<<std::endl;
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

        bool isValid()
        {
            //cout<<Header.audioFormat<<" "<<Header.bitsPerSample<<" "<<Header.sampleRate<<endl;
            return (Header.audioFormat == 1 && Header.bitsPerSample == 16);
        }
        bool Read(std::string dir)
        {
            std::ifstream inputFile(dir, std::ios::binary);
            if (inputFile.fail() || !inputFile.is_open())
            {
                cerr<<"Input file error"<<endl;
                return false;
            }
            char buffer[sizeof(Header)];
            inputFile.read(buffer, sizeof(buffer));
            memcpy(&Header, buffer, sizeof(Header));

            audioData.resize(Header.dataSize);
            inputFile.read(audioData.data(), Header.dataSize);
            inputFile.close();

            //cout<<"A"<<endl;
            if (!isValid())
            {
                cerr<<"Wrong format"<<endl; 
                return false;
            } 

            return true;
        }
        bool Write(std::string Name = "Output.wav")
        {
            //cout<<Name<<endl;
            std::ofstream Output(Name, ios::binary);
            if (Output.fail() || !Output.is_open() || !isValid())
            {
                cerr<<"Output file error"<<endl;
                return false;
            }
            char buffer[sizeof(Header)];
            memcpy(buffer, &Header, sizeof(Header));
            Output.write(buffer, sizeof(buffer));
            Output.write(audioData.data(), Header.dataSize);
            Output.close();
            return true;
        }
        void Write16Bits(short bits, uint32 position)
        {
            if (bits > 32767) bits = 32767;
            if (bits < -32768) bits = -32768;

            (audioData)[position] = bits & 0xFF;
            (audioData)[position + 1] = (bits >> 8) & 0xFF;
        }
        short Read16Bits(uint32 position)
        {
            short sample = ((short)(audioData[position] & 0xFF)) |
                            ((short)(audioData[position + 1] & 0xFF) << 8);
            return sample;
        }
};

class SoundProcessor
{
    protected:
        SoundFile* sound;
        CommandInfo Info;
    public:
        SoundProcessor() = default;
        SoundProcessor(SoundFile* soundfile)
        {
            SetSF(soundfile);
        }
        void SetSF(SoundFile* stoset)
        {
            sound = stoset;
        }
        bool ParseCommandLine(string& token_str, vector<SoundFile>* s_files);
        void WriteInfo()
        {
            cout<<endl;
            cout<<Info.name<<endl;
            cout<<Info.description<<endl;
            cout<< "params: ";
            for (int i = 0; i < Info.parameters.size(); ++i) 
            {
                if (i > 0) cout<<", ";
                cout<<Info.parameters[i];
            }
            cout<<endl;
            cout<<"example: "<<Info.example<<endl;
        }
        
};

bool CheckifFileNumberValid(int number, vector<SoundFile>* s_files)
{
    if(number > s_files->size())
    {
        cerr<<"FILE NUMBER OUT OF RANGE"<<endl;
        return false;
    }
    return true;
}

bool ExtractFileNumber(string& command, int& number, vector<SoundFile>* s_files) 
{
    if (command[0] != '$') return false;
    
    try 
    {
        command.erase(0, 1);
        number = stoi(command) - 1;
        return CheckifFileNumberValid(number, s_files);
    } 
    catch (const exception& e) 
    {
        cerr<<"INVALID FILE"<<command<<endl;
        return false;
    }
}

bool ExtractTimeParam(string& token, uint32* time_value) 
{
    if (token.empty()) 
    {
        cerr<<"MISSING PARAM"<<endl;
        return false;
    }
    
    try 
    {
        *time_value = stoi(token);
        return true;
    } 
    catch (const exception& e) 
    {
        cerr<<"INVALID TIME VALUE"<<endl;
        return false;
    }
}

class MuteProcessor : public SoundProcessor
{
    public:
        MuteProcessor()
        {
            Info = {
            "mute",
            "mute time interval",
            {"[$file]", "start_time", "end_time"},
            "mute $1 0 5"
            };
        }
        MuteProcessor(SoundFile* soundfile) : SoundProcessor(soundfile) {}
        void Process(uint32 start, uint32 end)
        {
            WavHeader* Header = sound->getHeader();
            std::vector<char>* audioData = sound->getAudiodata();

            uint32 startpos = TimeToBytePosition(start, Header);
            uint32 endpos  = TimeToBytePosition(end, Header);
            memset(&(*audioData)[startpos], 0, (endpos - startpos));
        }
        bool ParseCommandLine(string& command, vector<SoundFile>* s_files)
        {
            SoundFile* sf;
            int number = 0;
            uint32 start_time = 0;
            uint32 end_time = 0;
            string token_str = command;

            if (command[0] == '$')
            {
                if (!ExtractFileNumber(token_str, number, s_files)) return false;
                command = strtok(NULL, " "); 
            }
            if (!ExtractTimeParam(token_str, &start_time)) return false;
            command = strtok(NULL, " ");
            token_str = command;
            if (!ExtractTimeParam(token_str, &end_time)) return false;
            sf = &(*s_files)[number];

            if (start_time >= end_time || !sf->isValid())
            {
                cerr<<"INVALID MUTEPROCESSOR PARAMS"<<endl;
                return false;
            }

            SetSF(sf);
            Process(start_time, end_time);
            return true;
        }
};

class MixerProcessor : public SoundProcessor
{
    private:
        SoundFile* sound2;
    public:
        MixerProcessor()
        {
            Info = {
            "mix",
            "mix 2 sound files",
            {"$target_file", "[$source_file]", "target_start", "target_end", "source_start"},
            "mix $1 $2 0 10 5"
            };
        }
        MixerProcessor(SoundFile* soundfile1, SoundFile* soundfile2) : SoundProcessor(soundfile1) 
        {
            sound2 = soundfile2;
        };
        void SetSF2(SoundFile* stoset)
        {
            sound2 = stoset;
        }
        void Process(uint32 start_time, uint32 end_time, uint32 start_time1 = 0)
        {
            WavHeader* Header = sound->getHeader();
            std::vector<char>* audioData = sound->getAudiodata();

            uint32 startpos = TimeToBytePosition(start_time, Header);
            uint32 endpos  = TimeToBytePosition(end_time, Header);

            if (endpos - startpos < 0) return;

            WavHeader* sound2head = sound2->getHeader();
            uint32 startpos1 = TimeToBytePosition(start_time1, sound2head);
            
            std::vector<char>* sound2data = sound2->getAudiodata();
            uint32 avaliable_bytes = sound2data->size() - startpos1;
            uint32 mixlen = endpos - startpos;
            uint32 bytes_to_mix = (avaliable_bytes < mixlen) ? avaliable_bytes : mixlen;

            for (uint32 i = 0; i < bytes_to_mix; i+=2)
            {
                short sample1 = sound->Read16Bits(startpos + i);
                short sample2 = sound2->Read16Bits(startpos1 + i);
                short mixed = sample1 + sample2;
                sound->Write16Bits(mixed, startpos + i);
            }

            // if (mixlen > bytes_to_mix)
            // {
            //     memset(&(*audioData)[startpos + bytes_to_mix], 0, mixlen - bytes_to_mix);
            // }

            // memcpy(&(*audioData)[startpos], &(*audio2data)[startpos1], endpos - startpos);
            // if (endpos - startpos > avaliable_bytes)
            // {
            //     memset(&(*audioData)[startpos + avaliable_bytes], 0, endpos - startpos - avaliable_bytes);
            // }
        }
        bool ParseCommandLine(string& command, vector<SoundFile>* s_files)
        {
            SoundFile* sf1;
            SoundFile* sf2;
            int number = 0;
            uint32 start_time = 0;
            uint32 end_time = 0;
            uint32 start_time1 = 0;
            string token_str = command;

            if (!ExtractFileNumber(token_str, number, s_files)) return false;
            sf1 = &(*s_files)[0];
            sf2 = &(*s_files)[number];
            command = strtok(NULL, " ");
            token_str = command;
            if (command[0] == '$')
            {
                if (!ExtractFileNumber(token_str, number, s_files)) return false;
                sf1 = sf2;
                sf2 = &(*s_files)[number];
                command = strtok(NULL, " ");
            }
            // cout<<sf1->audioLength()<<" "<<sf2->audioLength()<<endl;
            //cout<<number<<"A"<<command<<endl;
            if (!ExtractTimeParam(token_str, &start_time)) return false;
            command = strtok(NULL, " ");
            token_str = command;

            if (!ExtractTimeParam(token_str, &end_time)) return false;
            command = strtok(NULL, " ");
            token_str = command;

            start_time1 = stoi(command);
            //cout<<sf1->audioLength()<<" "<<sf2->audioLength()<<" "<<par1<<" "<<par2<<" "<<par3<<endl;
            //cout<<par1<<" "<<par2<<" "<<par3<<endl;

            if (start_time > end_time || start_time1 > sf2->audioLength() || !sf1->isValid() || !sf2->isValid())
            {
                cerr<<"INVALID MIXERPROCESSOR PARAMS"<<endl;
                return false;
            }

            SetSF(sf1);
            SetSF2(sf2);
            Process(start_time, end_time, start_time1);
            return true;
        }
};

class VolumeProcessor : public SoundProcessor
{
    public:
        VolumeProcessor()
        {
            Info = {"volume",
        "change volume of a time interval",
        {"[$file]", "multiplier", "start_time", "end_time"},
        "volume $1 2.0 10 20"};
        }
        VolumeProcessor(SoundFile* soundfile) : SoundProcessor(soundfile) {};
        void Process(double multiplier, uint32 start = 0, uint32 end = 0)
        {
            if (multiplier == 1) return;
            if (end == 0)
            {
                end = sound->audioLength();
            }
            WavHeader* Header = sound->getHeader();
            vector<char>* audioData = sound->getAudiodata();

            uint32 startpos = TimeToBytePosition(start, Header);
            uint32 endpos  = TimeToBytePosition(end, Header);

            for (uint32 i = startpos; i < endpos; i+=2)
            {
                short sample = sound->Read16Bits(i);
                short res_sample = sample * multiplier;
                sound->Write16Bits(res_sample, i);
            }
        }
        bool ParseCommandLine(string& command, vector<SoundFile>* s_files)
        {
            string token_str = command;

            SoundFile* sf;
            int number = 0;
            double multiplier = 0;
            uint32 start_time = 0;
            uint32 end_time = 0;

            if (command[0] == '$')
            {
                if (!ExtractFileNumber(token_str, number, s_files)) return false;
                command = strtok(NULL, " "); 
            }
            multiplier = stod(command);
            command = strtok(NULL, " ");
            token_str = command;
            if (!ExtractTimeParam(token_str, &start_time)) return false;

            command = strtok(NULL, " ");
            token_str = command;
            if (!ExtractTimeParam(token_str, &end_time)) return false;

            sf = &(*s_files)[number];
            if (start_time >= end_time || multiplier < 0 || !sf->isValid())
            {
                cerr<<"INVALID VOLUMEPROCESSOR PARAMS"<<endl;
                return false;
            }

            SetSF(sf);
            Process(multiplier, start_time, end_time);
            return true;
        }
};

void ParseConfig(int argc, char *argv[])
{
    const string config_dir = argv[2];
    const string output_dir = argv[3];
    const char mode = argv[1][1];

    MuteProcessor mpr;
    MixerProcessor mixpr;
    VolumeProcessor vpr;

    if (mode == 'h')
    {
        mpr.WriteInfo();
        mixpr.WriteInfo();
        vpr.WriteInfo();
        return;
    }

    vector<SoundFile> s_files;
    for (int i = 4; i < argc; i++)
    {
        SoundFile s_file(argv[i]);
        if (!s_file.isValid())
        {
            cerr<<"INVALID INPUT SOUND FILES"<<endl;
            return;
        }
        s_files.push_back(s_file);
    }
    if (s_files.empty()) 
    {
        cerr<<"NO VALID INPUT SOUND FILES"<<endl;
        return;
    }

    ifstream inputFile(config_dir);
    if (!inputFile.is_open() || inputFile.fail())
    {
        cerr<<"CONFIG FILE ERROR"<<endl;
    }

    string command_line;
    while (getline(inputFile, command_line))
    {
        //cout<<command_line<<endl;
        if (command_line[0] == '#') continue;
        cout<<command_line<<endl;
        string command = strtok(command_line.data(), " ");
        //cout<<command<<endl;

        if (command == "mute")
        {
            command = strtok(NULL, " ");
            if (!mpr.ParseCommandLine(command, &s_files)) return;
        }
        else if (command == "volume")
        {
            command = strtok(NULL, " ");
            if (!vpr.ParseCommandLine(command, &s_files)) return;
        }
        else if (command == "mix")
        {
            command = strtok(NULL, " ");
            if (!mixpr.ParseCommandLine(command, &s_files)) return;
        }
    }
    s_files[0].Write(output_dir);
    cout<<"DONE"<<endl;
}

int main(int argc, char *argv[])
{
    ParseConfig(argc, argv);
    return 0;
}
