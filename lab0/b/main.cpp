#include <iostream>
#include <fstream>
#include <string>
#include <map>

bool is_separator(char letter)
{
    char separators[]{' ', '\t', '\r', '\n', '\0', ',', '.', '!', '?', ';', ':', '[', ']', '(', ')', '{', '}', '%', '-', '~'};
    for (int i = 0; i < 18; i++)
    {
        if (letter == separators[i])
        {
            return true;
        }
    }
    return false;
}

std::string FindMaxKey(std::map<std::string, int> *freq)
{
    int max = -1;
    std::string key = "";
    for (std::pair<const std::string, int> word : *freq)
    {
        if (word.second >= max)
        {
            max = word.second;
            key = word.first;
        }
    }
    return key;
}

int main(int argc, char *argv[])
{
    using namespace std;

    if (argc != 3)
    {
        cout<<"Wrong amount of files"<<endl;
        return 0;
    }

    string inputFileName = argv[1];
    string outputFileName = argv[2];

    cout<<inputFileName<<endl;
    cout<<outputFileName<<endl;

    ifstream inputFile(inputFileName);
    if (inputFile.fail() || !inputFile.is_open())
    {
        cerr<<"Input file error" << endl;
    }

    string tmp, word;
    int total_amount = 0;
    map<string, int> frequency;
    while (getline(inputFile, tmp))
    {
        int len = tmp.length();
        for (int i = 0; i < len; i++)
        {
            if (is_separator(tmp[i]))
            {
                if (word != "")
                {
                    frequency[word]++;
                    total_amount++;
                    word = "";
                }
                
                // if (frequency[word])
                // {
                //     frequency[word]++;
                //     //cout<<"XXXX"<<endl;
                // }
                // else
                // {
                //     frequency[word] = 1;
                //     //cout<<"YYYY"<<endl;
                // }
                // cout<<word<<frequency[word]<<endl;
                // return 0;
            }
            else
            {
                word += tmp[i];
            }
            
        }
    }
    if (word != "")
    {
        frequency[word]++;
        total_amount++;
    }

    int size = frequency.size();
    ofstream outputFile(outputFileName);

    if (outputFile.fail() || !outputFile.is_open())
    {
        cerr<<"Output file error"<<endl;
    }

    outputFile << "word,frequency in %\n";
    double percentage;
    //cout<<total_amount<<endl;
    for (int i = 0; i < size; i++)
    {
        string key = FindMaxKey(&frequency);
        // cout<<key<<" - "<<frequency[key]<<endl;

        percentage = (double)frequency[key] / (double)total_amount * 100;
        //cout<< key << "," << percentage << endl;
        outputFile << key << ',' << percentage << "%" << endl;
        frequency.erase(key);
    }
    outputFile.close();
    inputFile.close();

}
