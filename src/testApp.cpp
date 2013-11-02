#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    setupComplete = false;
    
    dB.assign(NUMTRACKS, 0.0);
    stemNames.assign(NUMTRACKS, "");
    
    vector<float> fftValues;
    fftValues.assign(512, 0);
    
    vector<float> volValues;
    volValues.assign(ofGetWidth() -200, 0);
    
    for ( int i = 0; i < NUMTRACKS; i++ ) {
        ofxAudioUnitFilePlayer * stem = new ofxAudioUnitFilePlayer();
        stems.push_back(stem);
        
        ofxAudioUnitTap * tap = new ofxAudioUnitTap();
        taps.push_back(tap);
        
        ofxAudioUnitFftNode * fft = new ofxAudioUnitFftNode();
        ffts.push_back(fft);
        
        ofPolyline temp;
        waves.push_back(temp);
        
        volHistory.push_back(volValues);
        
        
        fftGraphs.push_back(fftValues);
        
    }
    
    
    mode = 1;
    
    
    
    //vox
    stems[0]->setFile(ofFilePath::getAbsolutePath("STEMS/vox.wav"));
    stemNames[0] = "vox";
    //bgVox
    stems[1]->setFile(ofFilePath::getAbsolutePath("STEMS/bgVox.wav"));
    stemNames[1] = "bgVox";
    //beats
    stems[2]->setFile(ofFilePath::getAbsolutePath("STEMS/beats.wav"));
    stemNames[2] = "beats";
    //bass
    stems[3]->setFile(ofFilePath::getAbsolutePath("STEMS/bass.wav"));
    stemNames[3] = "bass";
    //moog
    stems[4]->setFile(ofFilePath::getAbsolutePath("STEMS/moog.wav"));
    stemNames[4] = "moog";
    //gtr
    stems[5]->setFile(ofFilePath::getAbsolutePath("STEMS/gtr.wav"));
    stemNames[5] = "gtr";
    //gtr solo
    stems[6]->setFile(ofFilePath::getAbsolutePath("STEMS/gtrSolo.wav"));
    stemNames[6] = "gtrSolo";
    
    
    mixer.setInputBusCount(7);
    for ( int i = 0; i < NUMTRACKS; i++ ) {
        stems[i]->connectTo(*taps[i]);
        (*taps[i]).connectTo(mixer, i);
        mixer.setInputVolume(1.0, i);
        ffts[i]->setSource(stems[i]);
    }
    
    mixer.connectTo(output);
    output.start();
    
//    for ( int i = 0; i < NUMTRACKS; i++ ) {
//        stems[i]->play();
//    }
    
    stems[0]->loop();
    stems[1]->loop();
    stems[2]->loop();
    stems[3]->loop();
    stems[4]->loop();
    stems[5]->loop();
    stems[6]->loop();
    
    
    //graphics
    
    
    shader.load("shadersGL3/shader");
    
    fbo.allocate(1440, 900, GL_RGBA32F_ARB);
    
    faucet.loadFont("faucet.ttf", 18);
    
    setupComplete = true;
}

//--------------------------------------------------------------
void testApp::update(){
    if( setupComplete ) {
        for ( int i = 0; i < NUMTRACKS; i++ ) {
            taps[i]->getLeftWaveform(waves[i], ofGetWidth(), ofGetHeight()/3);///NUMTRACKS
            if (taps[i]->getRMS(0) == taps[i]->getRMS(0)) dB[i] = taps[i]->getRMS(0);
            volHistory[i].push_back(dB[i]);
            if (volHistory[i].size() > ofGetWidth() - 200) volHistory[i].erase(volHistory[i].begin());
            
            ffts[i]->getAmplitude(fftGraphs[i]);

        }
    }
    
    //graphics
    float sineWave = (sin(ofGetElapsedTimef()/4) + 1) / 2;
    gradientStart.set(100);
    gradientEnd.set(50);
    
    if (ofGetFrameNum() % 30 == 0) shader.load("shadersGL3/shader");
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackgroundGradient(gradientStart, gradientEnd);
    
    if ( setupComplete ) {
        for ( int i = 0; i < NUMTRACKS; i++ ) {
            ofSetColor(i * 30, 255 - i * 30, (i * 100) % 255);
            ofPushMatrix();
            ofTranslate( 0, ofGetHeight()/(NUMTRACKS+1) * i);
            faucet.drawString(stemNames[i], 30, ofGetHeight()/NUMTRACKS);
            
            switch (mode) {
                case 1:
                    ofSetLineWidth(10);
                    waves[i].draw();
                    break;
                    
                case 2:
                {
                    ofBeginShape();
                    for (unsigned int j = 0; j < volHistory[i].size(); j++){
                        if( j == 0 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
                        
                        ofVertex(j + 100, ofGetHeight()/NUMTRACKS - volHistory[i][j] * ofGetHeight()/NUMTRACKS);
                        
                        if( j == volHistory[i].size() -1 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
                    }
                    ofEndShape(false);
                    float size = dB[i] * ofGetHeight()/NUMTRACKS;
                    ofCircle(ofGetWidth() - 50, ofGetHeight()/NUMTRACKS - size / 2, size);
                    break;
                }
                    
                case 3:
                    ofBeginShape();
                    for (unsigned int j = 0; j < fftGraphs[i].size(); j++){
                        if( j == 0 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
                        
                        ofVertex(j + 100, ofGetHeight()/NUMTRACKS - fftGraphs[i][j] * ofGetHeight()/NUMTRACKS);
                        
                        if( j == fftGraphs[i].size() -1 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
                    }
                    ofEndShape(false);
                    break;
            }
            
            ofPopMatrix();
        }
    }


}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'f':
            ofToggleFullscreen();
            break;
            
        case '1':
        case '2':
        case '3':
            mode = key - 48;
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
