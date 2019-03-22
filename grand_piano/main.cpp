// Zuni Preece
// Student ID: 300486880
// zuni.zkep@gmail.com
//
// Takes a list of frequencies within a .txt and converts to a .wav song
// 14.03.18

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <math.h>
#include <string>
#include "wav.hpp"

int main() {
    // setup variables
    int sample_rate = 44100;  // number of samples per second (Hz)
    double duration = 0.2;    // duration of tone (s)
    double amp = 5000;        // amplitude of tone
    
    int n_samples = duration*sample_rate;  // number of samples within the duration
    double dt = 1/double(sample_rate);     // timestep between samples
    double pi = M_PI;
    
    // gets instrument preset from user
    std::string instrument;
    std::cout << "(Options: trumpet, toypiano, sine [default])" << std::endl;
    std::cout << "Instrument preset: ";
    std::cin >> instrument;
    
    // exponential decay constants for the attack and decay on each note
    double init_attack = 0.001;
    double end_decay = 0.001;
    if (instrument == "trumpet") {
        init_attack = 0.001;
        end_decay = 0.0005;
    } else if (instrument == "toypiano") {
        init_attack = 0.008;
        end_decay = 0.001;
    }
    
    // gets filename from user
    std::string name;
    std::cout << "Name of file containing frequency list: ";
    std::cin >> name;
    string txt_name = name + ".txt";
    
    // open up music file
    fstream music_file(txt_name);
    string line;
    
    // count number of lines in music file
    int line_count = 0;
    if (music_file.is_open()) {
        while (getline(music_file, line)) {
            line_count += 1;
        }
    }
    else {
        std::cout << "Unable to open file";
        return 1;
    }
    
    double* freq = new double[line_count];
    music_file.clear();             // clear eof flag reached when counting lines
    music_file.seekg(0, ios::beg);  // return to beginning of file
    
    // write each frequency in the list to an index on freq array
    for (int i = 0; i < line_count; i ++) {
        getline(music_file, line);   // read each line
        freq[i] = std::stod(line);  // convert to double
    }
    
    // generate each tone and write the values into waveform array
    int* waveform = new int[n_samples*line_count];
    
    // iterates through each note
    for (int i = 0; i < line_count; i++) {
        // iterates through each sample of the current note
        for (int k = 0; k < n_samples; k++) {
            // if there are multiple listed notes of the exact same frequency, we want
            // them to play as a single longer tone, so we have to adjust for the amount of
            // time passed to avoid 'ticking' that comes from an instantaneous phase shift
            int curr_pos = i*n_samples+k;
            
            // calcs. initial attack if in first half of tone, final decay for second half
            // no attack/decay where the previous/following tone is the same frequency
            double decay;
            if (k < n_samples/2 && (freq[i-1] != freq[i])) {
                decay = 1 - pow(1-init_attack, double(k));
            } else if (freq[i+1] != freq[i]) {
                decay = 1 - pow(1-end_decay, n_samples-double(k));
            } else decay = 1;
            
            // superimposes the main tone and its higher and lower harmonics (if applicable)
            waveform[curr_pos] = decay*amp*sin(2*pi*freq[i]*(curr_pos*dt)); // base frequency
            
            if (instrument == "trumpet"){
                for (int j = 2; j < 30; j++) {
                    // these higher harmonics create a sawtooth wave
                    waveform[curr_pos] += decay*amp/(j)*sin(2*pi*freq[i]*j*(curr_pos*dt));
                }
            } else if (instrument == "toypiano") {
                for (int j = 2; j < 15; j++) {
                    waveform[curr_pos] += decay*amp/(j*j)*sin(2*pi*freq[i]*j*(curr_pos*dt));
                }
                waveform[curr_pos] += decay*amp/8*sin(2*pi*freq[i]*(curr_pos*dt));
            }
        }
    }

    // generate .wav from waveform
    WavSound sound;
    string wav_name = name + ".wav";
    sound.MakeWavFromInt(wav_name, sample_rate, waveform, n_samples*line_count);
    return 0;
}
