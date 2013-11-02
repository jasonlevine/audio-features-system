#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"

#define NUMTRACKS 7

class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

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
    
    vector<float> dB;
    vector<vector<float> > volHistory;
    vector<vector<float> > fftGraphs;
    
    bool setupComplete;
    
    
    ofTrueTypeFont faucet;

    int mode;
    
    //
    ofColor gradientStart, gradientEnd;
    ofShader shader;
    ofFbo fbo;
		
    
};
