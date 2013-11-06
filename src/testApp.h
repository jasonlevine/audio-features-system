#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"
#include "ofxAudioFeaturesChannel.h"


#define NUMTRACKS 7

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    
    void setupAudio();
    void updateAudio();
    //draw analytics
    void selectMode(int track, float height);
    void drawWaveForms(int track, float height);
    void drawAmpHistory(int track, float height);
    void drawDBHistory(int track, float height);
    void drawFFT(int track, float height);
    void drawPitchHistory(int track, float height);
    
    

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    //audio
    vector<ofxAudioUnitFilePlayer*> stems;
    vector<string> stemNames;
    vector<ofxAudioUnitTap*> taps;
    vector<ofxAudioUnitFftNode*> ffts;
    vector<ofPolyline> waves;
    ofxAudioUnitMixer mixer;
    ofxAudioUnitOutput output;
    
    vector<ofxAudioFeaturesChannel*> audioFeatures;
    
    vector<float> dB;
    vector<float> amp;
    vector<vector<float> > ampHistory;
    vector<vector<float> > dBHistory;
    vector<vector<float> > pitchHistory;
    
    bool setupComplete;
    int mode;
    int selectedTrack;
    //graphics
    ofColor gradientStart, gradientEnd;
    ofShader shader;
    ofFbo fbo;
    
    
    //analytics
    vector <float> maxdB, maxAmp, maxfft, maxPitch;
    float maxdBGlobal, maxAmpGlobal, maxfftGlobal, maxPitchGlobal;
    //xml
    ofXml xml;
    
    
    ofTrueTypeFont faucet;
    
    
};
