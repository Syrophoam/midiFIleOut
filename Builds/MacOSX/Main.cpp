//
//  Main.cpp
//  midiFIleOut
//
//  Created by Syro Fullerton on 19/09/2024.
//

#include "Main.hpp"
#include <stdio.h>
#include <JuceHeader.h>
#include <cstdio>

double lerp(double a, double b, double interp){
    interp = pow(interp, 2);
    return (((b*2*interp)+(a*-2*interp))/2) + a;
}

juce::MidiMessageSequence writeSequence(double values[]){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq;
    
    // put this loop in a loop going over values
    for (int j = 0; j < 4; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%4];
        
        for (int i = 0; i < (96*4); i++) {
            ccMsg = ccMsg.controllerEvent(1, 11, uint8_t(lerp(val1, val2, double(i)/(96.f*4.f) ) * 127) );
            ccMsg.addToTimeStamp(i + (j*96*4));
            seq.addEvent(ccMsg);
        }
        
    }
    
    return seq;
    
}

int main(){
    
    std::remove("/Users/syrofullerton/JUCE/Syro/midiFIleOut/Midi.mid");
    
    juce::MidiMessageSequence msgSequence;
    juce::MidiFile midiFile;
    juce::MidiMessage msg;
    int cMaj[7] = {0,2,4,5,7,9,11};
  
    midiFile.setTicksPerQuarterNote(96); // time stamp unit
    
    int halfNote = 96*2;
    int bar = 96*4;
    int quaterNote = 96;
    int eigthNote = 96/2;
    int sixtenthNote = 96/4;

    
    for (int i = 0; i < 4; i++) {
        int noteNumber = cMaj[rand()%6] + (12*4);
        msg = msg.noteOn(1, noteNumber, uint8_t(70));
        msg.setTimeStamp(quaterNote*i);
        juce::MidiMessage noteOff = noteOff.noteOff(1, msg.getNoteNumber(), uint8_t(70));
        noteOff.setTimeStamp((quaterNote*i)+sixtenthNote);

        msgSequence.addEvent(msg);
        msgSequence.addEvent(noteOff);
    }
    
    juce::MidiMessage ccMsg;
    for (int i = 0; i < bar; i++) {
        ccMsg = ccMsg.controllerEvent(1, 10, uint8_t(i%127));
        ccMsg.addToTimeStamp(i);
        msgSequence.addEvent(ccMsg);
    }
    
    msgSequence.updateMatchedPairs();
    juce::MidiMessageSequence outputSeq;
    
    
    for (int i = 0; i < 4; i++) {
        juce::MidiMessageSequence seqBuffer;
        seqBuffer = msgSequence;
        outputSeq.addSequence(seqBuffer, bar*i);
    }
    
    // consolodate all for loops into one
    

    ccMsg.addToTimeStamp(bar);
    outputSeq.addEvent(ccMsg);
    
    /*
     example of goal midi function
     
     function returns midi sequence,
     main file adds it to midi file
     
     function takes
                L track index for e.g Rain, clouds
                  refrence to api value array to compose cc
                  midi files? from ben
                  name midi tracks?
                  
                  try host all the vst's in the program
     */
    
    double values[] = {0.5,0.,0.5,0.37};
    
    juce::MidiMessageSequence funcSeq = writeSequence(values);

   // midiFile.addTrack(outputSeq);
    midiFile.addTrack(funcSeq);
    
    juce::FileOutputStream stream = juce::File("/Users/syrofullerton/JUCE/Syro/midiFIleOut/Midi.mid");
    midiFile.writeTo(stream);

    return 0;
}
