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
};

class HelpGenerator 
{
private:
    std::map<std::string, CommandInfo> commands;
    
public:
    HelpGenerator() 
    {
        initializeCommands();
    }
    
    void initializeCommands() 
    {
        commands["mute"] = 
        {
            "mute",
            "mute time interval",
            {"[$file]", "start_time", "end_time"},
            "mute $1 0 5"
        };
        
        commands["volume"] = 
        {
            "volume",
            "change volume of a time interval",
            {"[$file]", "multiplier", "start_time", "end_time"},
            "volume $1 2.0 10 20"
        };
        
        commands["mix"] = 
        {
            "mix",
            "mix 2 sound files",
            {"$target_file", "[$source_file]", "target_start", "target_end", "source_start"},
            "mix $1 $2 0 10 5"
        };
    }

    void Help()
    {
        for (const auto& cmd : commands) 
        {
            const CommandInfo& info = cmd.second;
            std::cout<< "\n" << info.name << "\n";
            std::cout<<info.description << "\n";
            std::cout<< "params: ";
            for (int i = 0; i < info.parameters.size(); ++i) 
            {
                if (i > 0) std::cout << ", ";
                std::cout<<info.parameters[i];
            }
            std::cout<<"\n";
            std::cout<<"example: "<<info.example<<"\n";
        }
    }
};

class SoundProcessor
{
    protected:
        SoundFile* sound;
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
};

class MuteProcessor : public SoundProcessor
{
    public:
        MuteProcessor() = default;
        MuteProcessor(SoundFile* soundfile) : SoundProcessor(soundfile) {};
    void Process(uint32 start, uint32 end)
    {
        WavHeader* Header = sound->getHeader();
        std::vector<char>* audioData = sound->getAudiodata();

        uint32 startpos = TimeToBytePosition(start, Header);
        uint32 endpos  = TimeToBytePosition(end, Header);
        memset(&(*audioData)[startpos], 0, (endpos - startpos));
    }
};

class MixerProcessor : public SoundProcessor
{
    private:
        SoundFile* audio2;
    public:
        MixerProcessor() = default;
        MixerProcessor(SoundFile* soundfile1, SoundFile* soundfile2) : SoundProcessor(soundfile1) 
        {
            audio2 = soundfile2;
        };
        void SetSF2(SoundFile* stoset)
        {
            audio2 = stoset;
        }
        void Process(uint32 start_time, uint32 end_time, uint32 start_time1 = 0)
        {
            WavHeader* Header = sound->getHeader();
            std::vector<char>* audioData = sound->getAudiodata();

            uint32 startpos = TimeToBytePosition(start_time, Header);
            uint32 endpos  = TimeToBytePosition(end_time, Header);

            if (endpos - startpos < 0) return;

            WavHeader* audio2head = audio2->getHeader();
            uint32 startpos1 = TimeToBytePosition(start_time1, audio2head);
            
            std::vector<char>* audio2data = audio2->getAudiodata();
            uint32 avaliable_bytes = audio2data->size() - startpos1;
            uint32 mixlen = endpos - startpos;
            uint32 bytes_to_mix = (avaliable_bytes < mixlen) ? avaliable_bytes : mixlen;

            for (uint32 i = 0; i < bytes_to_mix; i+=2)
            {
                short sample1 = (*audioData)[startpos + i] | ((*audioData)[startpos + i+1] << 8);
                short sample2 = (*audio2data)[startpos1 + i] | ((*audio2data)[startpos1+i+1] << 8);

                short mixed = sample1 + sample2;

                if (mixed > 32767) mixed = 32767;
                if (mixed < -32768) mixed = -32768;

                (*audioData)[startpos + i] = mixed & 0xFF;
                (*audioData)[startpos + i + 1] = (mixed >> 8) & 0xFF;
            }

            if (mixlen > bytes_to_mix)
            {
                memset(&(*audioData)[startpos + bytes_to_mix], 0, mixlen - bytes_to_mix);
            }

            // memcpy(&(*audioData)[startpos], &(*audio2data)[startpos1], endpos - startpos);
            // if (endpos - startpos > avaliable_bytes)
            // {
            //     memset(&(*audioData)[startpos + avaliable_bytes], 0, endpos - startpos - avaliable_bytes);
            // }
        }
};

class VolumeProcessor : public SoundProcessor
{
    public:
        VolumeProcessor() = default;
        VolumeProcessor(SoundFile* soundfile) : SoundProcessor(soundfile) {};
        void Process(double multiplier, uint32 start = 0, uint32 end = 0)
        {
            if (multiplier == 1) return;
            end = (!end) * (sound->audioLength()) + end; 
            WavHeader* Header = sound->getHeader();
            vector<char>* audioData = sound->getAudiodata();

            uint32 startpos = TimeToBytePosition(start, Header);
            uint32 endpos  = TimeToBytePosition(end, Header);

            for (uint32 i = startpos; i < endpos; i+=2)
            {
                short sample = ((*audioData)[i] | ((*audioData)[i+1] << 8));
                double res_sample = sample * multiplier;
                //if (res_sample > 16 * 1024) res_sample = 16 * 1024;

                if (res_sample > 32767.0) res_sample = 32767.0;
                if (res_sample < -32768.0) res_sample = -32768.0;

                sample = (short)(res_sample);
                (*audioData)[i] = (sample & 0xFF);
                (*audioData)[i+1] = ((sample>>8) & 0xFF);
            }
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

void ParseConfig(int argc, char *argv[])
{
    const string config_dir = argv[2];
    const string output_dir = argv[3];

    MuteProcessor mpr;
    MixerProcessor mixpr;
    VolumeProcessor vpr;

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
        string token_str;
        //cout<<command<<endl;

        if (command == "mute")
        {
            SoundFile* sf;
            int number = 0;
            uint32 start_time = 0;
            uint32 end_time = 0;

            command = strtok(NULL, " ");
            token_str = command;

            if (command[0] == '$')
            {
                ExtractFileNumber(token_str, number, &s_files);
                command = strtok(NULL, " "); 
            }
            ExtractTimeParam(token_str, &start_time);
            command = strtok(NULL, " ");
            token_str = command;
            ExtractTimeParam(token_str, &end_time);
            sf = &s_files[number];

            if (start_time >= end_time || !sf->isValid())
            {
                cerr<<"INVALID MUTEPROCESSOR PARAMS"<<endl;
                return;
            }

            mpr.SetSF(sf);
            mpr.Process(start_time, end_time);
        }
        else if (command == "volume")
        {
            SoundFile* sf;
            int number = 0;
            double multiplier = 0;
            uint32 start_time = 0;
            uint32 end_time = 0;

            command = strtok(NULL, " ");
            token_str = command;

            if (command[0] == '$')
            {
                ExtractFileNumber(token_str, number, &s_files);
                command = strtok(NULL, " "); 
            }
            multiplier = stod(command);
            command = strtok(NULL, " ");
            token_str = command;
            ExtractTimeParam(token_str, &start_time);

            command = strtok(NULL, " ");
            token_str = command;
            ExtractTimeParam(token_str, &end_time);

            sf = &s_files[number];
            if (start_time >= end_time || multiplier < 0 || !sf->isValid())
            {
                cerr<<"INVALID VOLUMEPROCESSOR PARAMS"<<endl;
                return;
            }

            vpr.SetSF(sf);
            vpr.Process(multiplier, start_time, end_time);
        }
        else if (command == "mix")
        {
            SoundFile* sf1;
            SoundFile* sf2;
            int number = 0;
            uint32 start_time = 0;
            uint32 end_time = 0;
            uint32 start_time1 = 0;

            command = strtok(NULL, " ");
            token_str = command;

            ExtractFileNumber(token_str, number, &s_files);
            sf1 = &s_files[0];
            sf2 = &s_files[number];
            command = strtok(NULL, " ");
            token_str = command;
            if (command[0] == '$')
            {
                ExtractFileNumber(token_str, number, &s_files);
                sf1 = sf2;
                sf2 = &s_files[number];
                command = strtok(NULL, " ");
            }
            // cout<<sf1->audioLength()<<" "<<sf2->audioLength()<<endl;
            //cout<<number<<"A"<<command<<endl;
            ExtractTimeParam(token_str, &start_time);
            command = strtok(NULL, " ");
            token_str = command;

            ExtractTimeParam(token_str, &end_time);
            command = strtok(NULL, " ");
            token_str = command;

            start_time1 = stoi(command);
            //cout<<sf1->audioLength()<<" "<<sf2->audioLength()<<" "<<par1<<" "<<par2<<" "<<par3<<endl;
            //cout<<par1<<" "<<par2<<" "<<par3<<endl;

            if (start_time > end_time || start_time1 > sf2->audioLength() || !sf1->isValid() || !sf2->isValid())
            {
                cerr<<"INVALID MIXERPROCESSOR PARAMS"<<endl;
                return;
            }

            mixpr.SetSF(sf1);
            mixpr.SetSF2(sf2);
            mixpr.Process(start_time, end_time, start_time1);
        }
    }
    s_files[0].Write(output_dir);
    cout<<"DONE"<<endl;
}

int main(int argc, char *argv[])
{
    HelpGenerator hgen;
    char mode = argv[1][1];
    if (mode == 'c') ParseConfig(argc, argv);
    else if (mode == 'h') hgen.Help();
    else cout<<"INVALID FLAG"<<endl;
    return 0;
}
