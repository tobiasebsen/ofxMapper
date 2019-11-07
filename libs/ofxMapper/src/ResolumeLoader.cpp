#include "ResolumeLoader.h"

bool ResolumeLoader::isValid(string versionName) {
    string name = xml.findFirst("//versionInfo").getAttribute("name").getValue();
	return name == versionName;
}

ofRectangle ResolumeLoader::getCompositionSize() {
	ofRectangle r;
	ofXml comp = xml.findFirst("//CurrentCompositionTextureSize");
    r.width = comp.getAttribute("width").getIntValue();
    r.height = comp.getAttribute("height").getIntValue();
	return r;
}

int ResolumeLoader::loadScreens() {
    ofXml::Search search = xml.find("//Screen");
    for (auto & s : search) {
        screens.push_back(s);
    }
	return screens.size();
}

ResolumeLoader::Screen ResolumeLoader::getScreen(int screenIndex) {
	return screens[screenIndex];
}

string ResolumeLoader::Screen::getName() {
	return xml.getAttribute("name").getValue();
}

ofRectangle ResolumeLoader::Screen::getSize() {
	ofRectangle r;
	ofXml params = xml.findFirst("./OutputDevice/*//Params");
	r.width = params.findFirst("./ParamRange[@name='Width']").getAttribute("value").getIntValue();
	r.height = params.findFirst("./ParamRange[@name='Height']").getAttribute("value").getIntValue();
	return r;
}

bool ResolumeLoader::Screen::getEnabled() {
	return xml.findFirst("./Params/Param[@name='Enabled']").getAttribute("value").getBoolValue();
}

int ResolumeLoader::Screen::loadSlices() {
    ofXml::Search search = xml.find("./layers/Slice");
    for (auto & s : search) {
        slices.push_back(s);
    }
	return slices.size();
}

ResolumeLoader::Slice ResolumeLoader::Screen::getSlice(int sliceIndex) {
	return slices[sliceIndex];
}

string ResolumeLoader::Slice::getName() {
	return xml.findFirst("./Params/Param[@name='Name']").getAttribute("value").getValue();
}

bool ResolumeLoader::Slice::getEnabled() {
	return xml.findFirst("./Params/Param[@name='Enabled']").getAttribute("value").getBoolValue();
}

bool ResolumeLoader::Slice::getSoftEdgeEnabled() {
	return xml.findFirst("./Params/Param[@name='SoftEdgeEnable']").getAttribute("value").getBoolValue();
}

float ResolumeLoader::Slice::getSoftEdgeGamma(float gamma) {
	ofXml first = xml.findFirst("./SoftEdgeGroup/Params/ParamRange[@name='Gamma']");
	return first ? first.getAttribute("value").getFloatValue() : gamma;
}

float ResolumeLoader::Slice::getSoftEdgeLuminance(float luminance) {
	ofXml first = xml.findFirst("./SoftEdgeGroup/Params/ParamRange[@name='Luminance']");
	return first ? first.getAttribute("value").getFloatValue() : luminance;
}

float ResolumeLoader::Slice::getSoftEdgePower(float power) {
	ofXml first = xml.findFirst("./SoftEdgeGroup/Params/ParamRange[@name='Power']");
	return first ? first.getAttribute("value").getFloatValue() : power;
}

ofRectangle ResolumeLoader::Slice::getInputRect() {
	ofRectangle r;
	ofXml::Search v = xml.find("./InputRect/v");
	if (v.size() == 4) {
		r.x = v[0].getAttribute("x").getFloatValue();
		r.y = v[0].getAttribute("y").getFloatValue();
		r.width = v[2].getAttribute("x").getFloatValue() - r.x;
		r.height = v[2].getAttribute("y").getFloatValue() - r.y;
	}
	return r;
}

string ResolumeLoader::Slice::getWarperMode() {
	ofXml params = xml.findFirst("./Warper/Params[@name='Warper']");
	ofXml mode = params.findFirst("./ParamChoice[@name='Point Mode']");
	return mode.getAttribute("value").getValue();
}

void ResolumeLoader::Slice::getWarperDim(int & width, int & height) {
	ofXml bwarper = xml.findFirst("./Warper/BezierWarper");
	width = bwarper.getAttribute("controlWidth").getIntValue();
	height = bwarper.getAttribute("controlHeight").getIntValue();
}

vector<glm::vec2> ResolumeLoader::Slice::getWarperVertices() {
	vector<glm::vec2> vertices;
	ofXml::Search vts = xml.find("./Warper/BezierWarper/vertices/v");
	for (auto & v : vts) {
		glm::vec2 p;
		p.x = v.getAttribute("x").getFloatValue();
		p.y = v.getAttribute("y").getFloatValue();
		vertices.push_back(p);
	}
	return vertices;
}

int ResolumeLoader::Screen::loadMasks() {
    ofXml::Search search = xml.find("./layers/Mask");
    for (auto & s : search) {
        masks.push_back(s);
    }
    return masks.size();
}

ResolumeLoader::Mask ResolumeLoader::Screen::getMask(int maskIndex) {
    return masks[maskIndex];
}

string ResolumeLoader::Mask::getName() {
    return xml.findFirst("./Params/Param[@name='Name']").getAttribute("value").getValue();
}

vector<glm::vec2> ResolumeLoader::Mask::getPoints() {
    vector<glm::vec2> points;
    ofXml::Search pts = xml.find("./ShapeObject/Shape/Contour/points/v");
    for (auto & v : pts) {
        glm::vec2 p;
        p.x = v.getAttribute("x").getFloatValue();
        p.y = v.getAttribute("y").getFloatValue();
        points.push_back(p);
    }
    return points;
}
