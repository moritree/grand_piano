// Zuni Preece
// Student ID: 300486880
// zuni.zkep@gmail.com
//
// Takes a list of frequencies within a .txt and converts to a .wav song
// 14.03.19

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <string>
#include "wav.hpp"

int main() {
    // setup variables
    int samplerate = 44100;  // number of samples per second (Hz)
    double duration = 0.2;   // duration of tone (s)
    double amp = 5000;       // amplitude of tone
    
    int nSamples = (int)(duration*samplerate);  // number of samples within the duration
    double dt = 1/double(samplerate);    // timestep between samples
    double pi = M_PI;
    
    // gets instrument preset
    std::string instrument;
    std::cout << "(Options: trumpet, toypiano, sine [default])" << std::endl;
    std::cout << "Instrument preset: ";
    std::cin >> instrument;
    
    // exponential decay constants for the attack and decay on each note
    double initAttack = 0.001;
    double endDecay = 0.001;
    if (instrument == "trumpet") {
        initAttack = 0.001;
        endDecay = 0.0005;
    } else if (instrument == "toypiano") {
        initAttack = 0.008;
        endDecay = 0.001;
    }
    
    // gets filename from user
    std::string name;
    std::cout << "Name of file containing frequency list: ";
    std::cin >> name;
    string filename = name + ".txt";
    
    // open up music file
    fstream musicFile(filename);
    string line;
    
    // count number of lines in music file
    int linecount = 0;
    if (musicFile.is_open()) {
        while (getline(musicFile, line)) {
            linecount += 1;
        }
    }
    else {
        std::cout<< "Unable to open file.";
        return 1;  // exit, error
    }
    
    double* freq = new double[linecount];
    musicFile.clear();             // clear eof flag we reached when counting
    musicFile.seekg(0, ios::beg);  // return to beginning of file
    
    // write the list of frequencies from file into freq[]
    for (int i = 0; i < linecount; i ++) {
        getline(musicFile, line);
        freq[i] = std::stod(line);
    }
    
    // generate each tone and write the values into waveform array
    int totalSamples = nSamples*linecount;
    int* waveform = new int[totalSamples];
    
    // iterates through each note
    for (int i = 0; i < linecount; i++) {
        // iterates through each sample of the current note
        for (int k = 0; k < nSamples; k++) {
            // if there are multiple listed notes of the exact same frequency, we want
            // them to play as a single longer tone, so we have to adjust for the length of
            // the note to avoid 'ticking' that comes from an instantaneous phase shift
            int currpos = i*nSamples+k;
            
            // calcs. initial attack if in first half of tone, final decay for second half
            // no attack/decay where the previous/following tone is the same frequency
            double decay;
            if (k < nSamples/2 && (freq[i-1] != freq[i])) {
                decay = 1 - pow(1-initAttack, double(k));
            } else if (freq[i+1] != freq[i]) {
                decay = 1 - pow(1-endDecay, nSamples-double(k));
            } else decay = 1;
            
            // base tone
            waveform[i*nSamples+k] = 0;
            waveform[currpos] = (int)(decay*amp*sin(2*pi*freq[i]*(currpos*dt)));
            
            if (instrument == "trumpet"){
                for (int j = 1; j < 30; j++) { // higher harmonics create saw wave
                    waveform[currpos] += (int)(decay*amp/(j)*sin(2*pi*freq[i]*j*(currpos*dt)));
                }
            } else if (instrument == "toypiano") {
                for (int j = 1; j < 15; j++) {
                    waveform[currpos] += (int)(decay*amp/(j*j)*sin(2*pi*freq[i]*j*(currpos*dt)));
                }
                waveform[currpos] += (int)(decay*amp/8*sin(2*pi*freq[i]*(currpos*dt)));
            }
        }
    }

    // generate .wav from waveform
    WavSound sound;
    string songfilename = name + ".wav";
    sound.MakeWavFromInt(songfilename, samplerate, waveform, totalSamples);
    return 0;
}
