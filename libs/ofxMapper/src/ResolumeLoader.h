#pragma once

#include "ofMain.h"

class ResolumeLoader {
public:

	ResolumeLoader(ofXml & x) : xml(x) { }

	bool isValid(string versionName = "Resolume Arena");
	ofRectangle getCompositionSize();

	class Screen;
	class Slice;
    class Mask;

	// Screens
	int loadScreens();
	Screen getScreen(int screenIndex);

	class Screen {
	public:
		Screen(ofXml x) : xml(x) {}

        string getName();
		bool getEnabled();
		ofRectangle getSize();

        int loadSlices();
		Slice getSlice(int sliceIndex);

        int loadMasks();
        Mask getMask(int maskIndex);

    private:
		ofXml xml;
		vector<ofXml> slices;
        vector<ofXml> masks;
	};

	// Slices

	class Slice {
	public:
		Slice(ofXml x) : xml(x) {}

		string getName();
		bool getEnabled();

		ofRectangle getInputRect();
		string getWarperMode();
		void getWarperDim(int & width, int & height);

		bool getSoftEdgeEnabled();
		float getSoftEdgeGamma(float gamma = 1);
		float getSoftEdgeLuminance(float luminance = 0.5);
		float getSoftEdgePower(float power = 2);
		
		vector<glm::vec2> getWarperVertices();
	private:
		ofXml xml;
	};
    
    // Masks
    
    class Mask {
    public:
        Mask(ofXml x) : xml(x) {}
        
        string getName();
        bool getEnabled();
        vector<glm::vec2> getPoints();
    private:
        ofXml xml;
    };

private:
	ofXml & xml;
	vector<ofXml> screens;
};
