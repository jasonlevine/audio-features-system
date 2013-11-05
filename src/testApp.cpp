#include "testApp.h"
#include <mach/mach_time.h>

//--------------------------------------------------------------
void testApp::setup(){
    setupComplete = false;
    
    amp.assign(NUMTRACKS, 0.0);
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
        
        ofxAudioFeaturesChannel * channel = new ofxAudioFeaturesChannel();
        channel->setup(512, 64, 44100);
        channel->usingPitch = true;
        audioFeatures.push_back(channel);
        
        ofPolyline temp;
        waves.push_back(temp);
        
        dBHistory.push_back(volValues);
        ampHistory.push_back(volValues);
        pitchHistory.push_back(volValues);
    }
    
    mode = 1;
    
    
    //vox
    stems[0]->setFile(ofFilePath::getAbsolutePath("STEMS/bgVox.wav"));
    stemNames[0] = "vox";
    //bgVox
    stems[1]->setFile(ofFilePath::getAbsolutePath("STEMS/vox.wav"));
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
//        stems[i]->connectTo(mixer, i);
        mixer.setInputVolume(1.0, i);
        mixer.enableInputMetering(i);
    }
    
    mixer.connectTo(output);
    output.start();
    
    for ( int i = 0; i < NUMTRACKS; i++ ) {
        stems[i]->prime();
    }
    
    



    for ( int i = 0; i < NUMTRACKS; i++ ) {
        stems[i]->play();
    }
    
    stems[0]->play();
    stems[1]->play();
    stems[2]->play();
    stems[3]->play();
    stems[4]->play();
    stems[5]->play();
    stems[6]->play();

    
    //analytics
    mindB.assign(NUMTRACKS, 0);
    maxdB.assign(NUMTRACKS, -120);
    
    minAmp.assign(NUMTRACKS, 10);
    maxAmp.assign(NUMTRACKS, 0);
    
    minfft.assign(NUMTRACKS, 10);
    maxfft.assign(NUMTRACKS, 0);
    
    minPitch.assign(NUMTRACKS, 1000);
    maxPitch.assign(NUMTRACKS, 0);
    
    xml.load("dataMinMax.xml");
    
    xml.setTo("limits");
    
    for (int i = 0; i < NUMTRACKS; i++) {
        string tag = "track-" + ofToString(i);
        xml.setTo(tag);
        minAmp[i] = ofToFloat(xml.getValue("minAmp"));
        maxAmp[i] = ofToFloat(xml.getValue("maxAmp"));
        
        mindB[i] = ofToFloat(xml.getValue("mindB"));
        maxdB[i] = ofToFloat(xml.getValue("maxdB"));
        
        minfft[i] = ofToFloat(xml.getValue("minfft"));
        maxfft[i] = ofToFloat(xml.getValue("maxfft"));
        
        minPitch[i] = ofToFloat(xml.getValue("minPitch"));
        maxPitch[i] = ofToFloat(xml.getValue("maxPitch"));
        xml.setToParent();
        
    }
//    xml.addChild("root");
//    xml.setTo("root");
    
    
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
            
            //volume
            taps[i]->getLeftWaveform(waves[i], ofGetWidth(), ofGetHeight()/3);///NUMTRACKS
            dB[i] = mixer.getInputLevel(i);
            amp[i] = taps[i]->getRMS(0);
            
            dBHistory[i].push_back(dB[i]);
            if (dBHistory[i].size() > ofGetWidth() - 200) dBHistory[i].erase(dBHistory[i].begin());
            
            ampHistory[i].push_back(amp[i]);
            if (ampHistory[i].size() > ofGetWidth() - 200) ampHistory[i].erase(ampHistory[i].begin());
            
            //audio features
            taps[i]->getSamples(audioFeatures[i]->inputBuffer);
            audioFeatures[i]->process(0);
            
            pitchHistory[i].push_back(audioFeatures[i]->pitch);
            if (pitchHistory[i].size() > ofGetWidth() - 200) pitchHistory[i].erase(pitchHistory[i].begin());
            
            
            // min max
            /*
            if (amp[i] > maxAmp[i]) maxAmp[i] = amp[i];
            else if (amp[i] < minAmp[i]) minAmp[i] = amp[i];
            
            if (dB[i] > maxdB[i]) maxdB[i] = dB[i];
            else if (dB[i] < mindB[i]) mindB[i] = dB[i];
            
            for (int j = 0; j < audioFeatures[i]->spectrum.size(); j++){
                float bin = audioFeatures[i]->spectrum[j];
                if (bin > maxfft[i]) maxfft[i] = bin;
                else if (bin < minfft[i]) minfft[i] = bin;
            }
            
            float pitch = pitchHistory[i][pitchHistory.size()-1];
            if (pitch > maxPitch[i]) maxPitch[i] = pitch;
            else if (pitch < minPitch[i]) minPitch[i] = pitch;
            */
            //XML
//            float amplitude = taps[1]->getRMS(0);
//            if (i == 1 && amplitude != 0) { //
//                ofXml frame;
//                string child = "frame-" + ofToString(ofGetFrameNum);
//                frame.addChild(child);
//                frame.setTo(child);
//                
//                frame.addValue("pitch", audioFeatures[1]->pitch);
//                frame.addValue("amplitude", taps[1]->getRMS(0));
//                
//                xml.addXml(frame);
//            }
            
                


        }
    }
    
    //graphics
//    float sineWave = (sin(ofGetElapsedTimef()/4) + 1) / 2;
//    gradientStart.set(100);
//    gradientEnd.set(50);
//    
//    if (ofGetFrameNum() % 30 == 0) shader.load("shadersGL3/shader");
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackgroundGradient(gradientStart, gradientEnd);
    
    if ( setupComplete ) {
        for ( int i = 0; i < NUMTRACKS; i++ ) {
            ofSetColor(i * 30, 255 - i * 30, (i * 100) % 255);
            ofPushMatrix();
            ofTranslate( 0, ofGetHeight()/(NUMTRACKS) * i);
            faucet.drawString(stemNames[i], 30, ofGetHeight()/NUMTRACKS);
            
            switch (mode) {
                case 1:
                    drawWaveForms(i);
                    break;
                    
                case 2:
                    drawAmpHistory(i);
                    break;
                
                    
                case 3:
                    drawDBHistory(i);
                    break;
                    
                case 4:
                    drawFFT(i);
                    break;
                    
                case 5:
                    drawPitchHistory(i);
                    break;
                    
            }
            
            ofPopMatrix();
            string output = "FPS: " + ofToString(ofGetFrameRate()) + "\n"
            + "minAmp = " + ofToString(minAmp) + "\n"
            + "maxAmp = " + ofToString(maxAmp) + "\n"
            + "mindB = " + ofToString(mindB) + "\n"
            + "maxdB = " + ofToString(maxdB) + "\n"
            + "minfft = " + ofToString(minfft) + "\n"
            + "maxfft = " + ofToString(maxfft) + "\n"
            + "minPitch = " + ofToString(minPitch) + "\n"
            + "maxPitch = " + ofToString(maxPitch) + "\n";
            faucet.drawString(output, ofGetWidth() - 480, 20);
        }
    }


}

//--------------------------------------------------------------
void testApp::drawWaveForms(int track){
    ofSetLineWidth(10);
    waves[track].draw();
}

//--------------------------------------------------------------
void testApp::drawAmpHistory(int track){
    ofBeginShape();
    for (unsigned int j = 0; j < ampHistory[track].size(); j++){
        if( j == 0 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
        float ampNormalized = ofMap(ampHistory[track][j], minAmp[track], maxAmp[track], 0.0, 1.0);
        ofVertex(j + 100, ofGetHeight()/(NUMTRACKS) - ampNormalized * ofGetHeight()/(NUMTRACKS));
        
        if( j == ampHistory[track].size() -1 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
    }
    ofEndShape(false);
    
    float ampNormalized = ofMap(amp[track], minAmp[track], maxAmp[track], 0.0, 1.0);
    float size = ampNormalized * ofGetHeight()/(NUMTRACKS);
    ofCircle(ofGetWidth() - 50, ofGetHeight()/NUMTRACKS - size / 2, size/2);
}

//--------------------------------------------------------------
void testApp::drawDBHistory(int track){
    ofBeginShape();
    for (unsigned int j = 0; j < dBHistory[track].size(); j++){
        if( j == 0 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
        float dBNormalized = ofMap(dBHistory[track][j], mindB[track], maxdB[track], 0.0, 1.0);
        ofVertex(j + 100, ofGetHeight()/(NUMTRACKS) - dBNormalized * ofGetHeight()/(NUMTRACKS));
        
        if( j == dBHistory[track].size() -1 ) ofVertex(j + 100, ofGetHeight()/NUMTRACKS);
    }
    ofEndShape(false);
    
    float dBNormalized = ofMap(dB[track], mindB[track], maxdB[track], 0.0, 1.0);
    float size = dBNormalized * ofGetHeight()/(NUMTRACKS+1);
    ofCircle(ofGetWidth() - 50, ofGetHeight()/NUMTRACKS - size / 2, size/2);
}

//--------------------------------------------------------------
void testApp::drawFFT(int track){
    ofBeginShape();
    for (unsigned int j = 0; j < audioFeatures[track]->spectrum.size(); j++){
        int x = (float)j / audioFeatures[track]->spectrum.size() * (ofGetWidth()-100) + 100;
        
        if( j == 0 ) ofVertex(x, ofGetHeight()/NUMTRACKS);
        
        float binNormalized = ofMap(audioFeatures[track]->spectrum[j], minfft[track], maxfft[track], 0.0, 1.0);
        ofVertex(x, ofGetHeight()/NUMTRACKS - binNormalized * ofGetHeight()/NUMTRACKS);
        
        if( j == audioFeatures[track]->spectrum.size() -1 ) ofVertex(x, ofGetHeight()/NUMTRACKS);
    }
    ofEndShape(false);
}

//--------------------------------------------------------------
void testApp::drawPitchHistory(int track){
    ofNoFill();
    ofBeginShape();
    for (unsigned int j = 0; j < pitchHistory[track].size(); j++){
        float pitchNormalized = ofMap(pitchHistory[track][j], minPitch[track], maxPitch[track], 0.0, 1.0);
        ofVertex(j + 100, ofGetHeight()/NUMTRACKS - pitchNormalized * ofGetHeight()/NUMTRACKS);
    }
    ofEndShape(false);
    ofFill();
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
        case '4':
        case '5':
            mode = key - 48;
            break;
            
//        case 'p':
            
        case 'a':
//            cout << "min dB = " << mindB << endl
//            << "max dB = " << maxdB << endl;
//            for (int i = 0; i < NUMTRACKS; i++) {
//                cout << "max fft[" << i << "] = " << maxfft[i] << endl;
//            }
            break;
            
        case 's':
            xml.save("voice data.xml");
            break;
            
        case 'v': //save min max xml;
            xml.addChild("limits");
            xml.setTo("limits");
            
            for (int i = 0; i < NUMTRACKS; i++) {
                ofXml track;
                string tag = "track-" + ofToString(i);
                track.addChild(tag);
                track.setTo(tag);
                track.addValue("minAmp", minAmp[i]);
                track.addValue("maxAmp", maxAmp[i]);
                track.addValue("mindB", mindB[i]);
                track.addValue("maxdB", maxdB[i]);
                track.addValue("minfft", minfft[i]);
                track.addValue("maxfft", maxfft[i]);
                track.addValue("minPitch", minPitch[i]);
                track.addValue("maxPitch", maxPitch[i]);
                
                xml.addXml(track);
            }
            
            xml.save("dataMinMax");
                
            
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
