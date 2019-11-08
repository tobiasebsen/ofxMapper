#pragma once

#include "ofMain.h"

class ResolumeFile {
public:

	ResolumeFile();

	bool load(string filePath);
	bool save(string filePath);

	bool isValid(string versionName = "Resolume Arena");
	void setVersion(string name = "Resolume Arena");

	ofRectangle getCompositionSize();
	void setCompositionSize(int width, int height);

	class Screen;
	class Slice;
    class Mask;

	// Screens
	int loadScreens();
	Screen getScreen(int screenIndex);
	Screen getScreen(string uniqueId);
	Screen addScreen(string uniqueId);

	class Screen {
	public:
		Screen(ofXml x) : xml(x) {
			loadSlices();
			loadMasks();
		}

		string getUniqueId();

        string getName();
		void setName(string name);
		bool getEnabled();
		void setEnabled(bool enabled);
		ofRectangle getSize();
		void setSize(int width, int height);

        int loadSlices();
		int getNumSlices();
		Slice getSlice(int sliceIndex);
		Slice getSlice(string uniqueId);
		Slice addSlice(string uniqueId);

        int loadMasks();
		int getNumMasks();
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

		string getUniqueId();

		string getName();
		void setName(string name);
		bool getEnabled();
		void setEnabled(bool enabled);

		ofRectangle getInputRect();
		void setInputRect(const ofRectangle & inputRect);
		string getWarperMode();
		void setWarperMode(string mode);
		void getWarperDim(int & width, int & height);

		vector<glm::vec2> getWarperVertices();
		void setWarperVertices(size_t controlWidth, size_t controlHeight, glm::vec2 * vertices);

		bool getSoftEdgeEnabled();
		float getSoftEdgeGamma(float gamma = 1);
		float getSoftEdgeLuminance(float luminance = 0.5);
		float getSoftEdgePower(float power = 2);
		
	private:
		ofXml xml;
	};
    
    // Masks
    
    class Mask {
    public:
        Mask(ofXml x) : xml(x) {}

		string getUniqueId();
        
        string getName();
        bool getEnabled();
        vector<glm::vec2> getPoints();
    private:
        ofXml xml;
    };

protected:
	template<typename T>
	static void setParam(ofXml & x, string params, string name, const T & value, string paramType = "");
	template<typename T>
	static void setParamRange(ofXml & x, string params, string name, const T & value) {
		setParam(x, params, name, value, "Range");
	}
	template<typename T>
	static void setParamChoice(ofXml & x, string params, string name, const T & value) {
		setParam(x, params, name, value, "Choice");
	}
	template<typename T>
	static void addVertex(ofXml & x, const T & vertex);

	ofXml xml;
	ofXml state;
	vector<ofXml> screens;
};

template<typename T>
inline void ResolumeFile::setParam(ofXml & x, string params, string name, const T & value, string paramType) {
	ofXml prms;
	if (!(prms = x.findFirst("./Params[@name='" + params + "']"))) {
		prms = x.appendChild("Params");
		prms.setAttribute("name", params);
	}
	ofXml p;
	if (!(p = prms.findFirst("./Param" + paramType + "[@name='" + name + "']"))) {
		p = prms.appendChild("Param");
		p.setAttribute("name", name);
	}
	p.setAttribute("value", value);
}

template<typename T>
inline void ResolumeFile::addVertex(ofXml & x, const T & vertex) {
	ofXml v = x.appendChild("v");
	v.setAttribute("x", vertex.x);
	v.setAttribute("y", vertex.y);
}
