#include "ResolumeFile.h"

ResolumeFile::ResolumeFile() {
	if (!(state = xml.getChild("XmlState")))
		state = xml.appendChild("XmlState");
}

bool ResolumeFile::load(string filePath) {
	bool loaded = xml.load(filePath);
	if (!(state = xml.getChild("XmlState")))
		state = xml.appendChild("XmlState");
	return loaded;
}

bool ResolumeFile::save(string filePath) {
	return xml.save(filePath);
}

bool ResolumeFile::isValid(string versionName) {
    string name = state.getChild("versionInfo").getAttribute("name").getValue();
	return name == versionName;
}

void ResolumeFile::setVersion(string name) {
	ofXml version;
	if (!(version = state.getChild("versionInfo"))) {
		version = state.appendChild("versionInfo");
	}
	version.setAttribute("name", name);
}

ofRectangle ResolumeFile::getCompositionSize() {
	ofRectangle r;
	ofXml comp = xml.findFirst("//CurrentCompositionTextureSize");
    r.width = comp.getAttribute("width").getIntValue();
    r.height = comp.getAttribute("height").getIntValue();
	return r;
}

void ResolumeFile::setCompositionSize(int width, int height) {
	ofXml screenSetup;
	if (!(screenSetup = state.getChild("ScreenSetup"))) {
		screenSetup = state.appendChild("ScreenSetup");
	}
	ofXml compSize;
	if (!(compSize = screenSetup.getChild("CurrentCompositionTextureSize"))) {
		compSize = screenSetup.appendChild("CurrentCompositionTextureSize");
	}
	compSize.setAttribute("width", width);
	compSize.setAttribute("height", height);
}

int ResolumeFile::loadScreens() {
    ofXml::Search search = xml.find("//Screen");
	screens.clear();
    for (auto & s : search) {
        screens.push_back(s);
    }
	return screens.size();
}

ResolumeFile::Screen ResolumeFile::getScreen(int screenIndex) {
	return screens[screenIndex];
}

ResolumeFile::Screen ResolumeFile::getScreen(string uniqueId) {
	for (auto & s : screens) {
		if (s.getAttribute("uniqueId").getValue() == uniqueId) {
			return s;
		}
	}
	return addScreen(uniqueId);
}

ResolumeFile::Screen ResolumeFile::addScreen(string uniqueId) {
	ofXml screenSetup;
	if (!(screenSetup = state.getChild("ScreenSetup"))) {
		screenSetup = state.appendChild("ScreenSetup");
	}
	ofXml scrns;
	if (!(scrns = screenSetup.getChild("screens"))) {
		scrns = screenSetup.appendChild("screens");
	}
	ofXml scrn = scrns.appendChild("Screen");
	scrn.setAttribute("uniqueId", uniqueId);

	return Screen(scrn);
}

string ResolumeFile::Screen::getUniqueId() {
	return xml.getAttribute("uniqueId").getValue();
}

string ResolumeFile::Screen::getName() {
	return xml.getAttribute("name").getValue();
}

void ResolumeFile::Screen::setName(string name) {
	xml.setAttribute("name", name);
	ResolumeFile::setParam(xml, "Params", "Name", name);
}

ofRectangle ResolumeFile::Screen::getSize() {
	ofRectangle r;
	ofXml params = xml.findFirst("./OutputDevice/*//Params");
	r.width = params.findFirst("./ParamRange[@name='Width']").getAttribute("value").getIntValue();
	r.height = params.findFirst("./ParamRange[@name='Height']").getAttribute("value").getIntValue();
	return r;
}

void ResolumeFile::Screen::setSize(int width, int height) {
	ofXml odev;
	if (!(odev = xml.getChild("OutputDeive"))) {
		odev = xml.appendChild("OutputDevice");
	}
	ofXml virt;
	if (!(virt = odev.getFirstChild())) {
		virt = odev.appendChild("OutputDeviceVirtual");
	}
	virt.setAttribute("width", width);
	virt.setAttribute("height", height);
	ResolumeFile::setParamRange(virt, "Params", "Width", width);
	ResolumeFile::setParamRange(virt, "Params", "Height", height);
}

bool ResolumeFile::Screen::getEnabled() {
	return xml.findFirst("./Params[@name='Params']/Param[@name='Enabled']").getAttribute("value").getBoolValue();
}

void ResolumeFile::Screen::setEnabled(bool enabled) {
	ResolumeFile::setParam(xml, "Params", "Enabled", enabled);
}

int ResolumeFile::Screen::loadSlices() {
    ofXml::Search search = xml.find("./layers/Slice");
    for (auto & s : search) {
        slices.push_back(s);
    }
	return slices.size();
}

int ResolumeFile::Screen::getNumSlices() {
	return slices.size();
}

ResolumeFile::Slice ResolumeFile::Screen::getSlice(int sliceIndex) {
	return slices[sliceIndex];
}

ResolumeFile::Slice ResolumeFile::Screen::getSlice(string uniqueId) {
	for (auto & s : slices) {
		if (s.getAttribute("uniqueId").getValue() == uniqueId)
			return s;
	}
	return addSlice(uniqueId);
}

ResolumeFile::Slice ResolumeFile::Screen::addSlice(string uniqueId) {
	ofXml layers;
	if (!(layers = xml.getChild("layers"))) {
		layers = xml.appendChild("layers");
	}
	ofXml slice = layers.appendChild("Slice");
	slice.setAttribute("uniqueId", uniqueId);
	return Slice(slice);
}

string ResolumeFile::Slice::getUniqueId() {
	return xml.getAttribute("uniqueId").getValue();
}

string ResolumeFile::Slice::getName() {
	return xml.findFirst("./Params[@name='Common']/Param[@name='Name']").getAttribute("value").getValue();
}

void ResolumeFile::Slice::setName(string name) {
	ResolumeFile::setParam(xml, "Common", "Name", name);
}

bool ResolumeFile::Slice::getEnabled() {
	return xml.findFirst("./Params[@name='Common']/Param[@name='Enabled']").getAttribute("value").getBoolValue();
}

void ResolumeFile::Slice::setEnabled(bool enabled) {
	ResolumeFile::setParam(xml, "Common", "Enabled", enabled);
}

bool ResolumeFile::Slice::getSoftEdgeEnabled() {
	return xml.findFirst("./Params[@name='Input']/Param[@name='SoftEdgeEnable']").getAttribute("value").getBoolValue();
}

float ResolumeFile::Slice::getSoftEdgeGamma(float gamma) {
	ofXml first = xml.findFirst("./SoftEdgeGroup/Params/ParamRange[@name='Gamma']");
	return first ? first.getAttribute("value").getFloatValue() : gamma;
}

float ResolumeFile::Slice::getSoftEdgeLuminance(float luminance) {
	ofXml first = xml.findFirst("./SoftEdgeGroup/Params/ParamRange[@name='Luminance']");
	return first ? first.getAttribute("value").getFloatValue() : luminance;
}

float ResolumeFile::Slice::getSoftEdgePower(float power) {
	ofXml first = xml.findFirst("./SoftEdgeGroup/Params/ParamRange[@name='Power']");
	return first ? first.getAttribute("value").getFloatValue() : power;
}

ofRectangle ResolumeFile::Slice::getInputRect() {
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

void ResolumeFile::Slice::setInputRect(const ofRectangle & inputRect) {
	ofXml irct;
	if (!(irct = xml.getChild("InputRect"))) {
		irct = xml.appendChild("InputRect");
	}
	ofXml::Search v = irct.find("./v");
	if (v.size() == 0) {
		ResolumeFile::addVertex(irct, inputRect.getTopLeft());
		ResolumeFile::addVertex(irct, inputRect.getTopRight());
		ResolumeFile::addVertex(irct, inputRect.getBottomRight());
		ResolumeFile::addVertex(irct, inputRect.getBottomLeft());
	}
	else if (v.size() == 4) {
		v[0].setAttribute("x", inputRect.getLeft()); v[0].setAttribute("y", inputRect.getTop());
		v[1].setAttribute("x", inputRect.getRight()); v[1].setAttribute("y", inputRect.getTop());
		v[2].setAttribute("x", inputRect.getRight()); v[2].setAttribute("y", inputRect.getBottom());
		v[3].setAttribute("x", inputRect.getLeft()); v[3].setAttribute("y", inputRect.getBottom());
	}
}

string ResolumeFile::Slice::getWarperMode() {
	ofXml params = xml.findFirst("./Warper/Params[@name='Warper']");
	ofXml mode = params.findFirst("./ParamChoice[@name='Point Mode']");
	return mode.getAttribute("value").getValue();
}

void ResolumeFile::Slice::setWarperMode(string mode) {
	ofXml warper;
	if (!(warper = xml.getChild("Warper"))) {
		warper = xml.appendChild("Warper");
	}
	ResolumeFile::setParamChoice(warper, "Warper", "Point Mode", mode);
}

void ResolumeFile::Slice::getWarperDim(int & width, int & height) {
	ofXml bwarper = xml.findFirst("./Warper/BezierWarper");
	width = bwarper.getAttribute("controlWidth").getIntValue();
	height = bwarper.getAttribute("controlHeight").getIntValue();
}

vector<glm::vec2> ResolumeFile::Slice::getWarperVertices() {
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

void ResolumeFile::Slice::setWarperVertices(size_t controlWidth, size_t controlHeight, glm::vec2 * vertices) {
	ofXml warper;
	if (!(warper = xml.getChild("Warper"))) {
		warper = xml.appendChild("Warper");
	}
	ofXml bezier;
	if (!(bezier = warper.getChild("BezierWarper"))) {
		bezier = warper.appendChild("BezierWarper");
	}
	bezier.setAttribute("controlWidth", controlWidth);
	bezier.setAttribute("controlHeight", controlHeight);

	bezier.removeChild("vertices");
	ofXml vert = bezier.appendChild("vertices");

	size_t n = controlWidth * controlHeight;
	for (size_t i = 0; i < n; i++) {
		ResolumeFile::addVertex(vert, vertices[i]);
	}
}

int ResolumeFile::Screen::loadMasks() {
    ofXml::Search search = xml.find("./layers/Mask");
	masks.clear();
    for (auto & s : search) {
        masks.push_back(s);
    }
    return masks.size();
}

int ResolumeFile::Screen::getNumMasks() {
	return masks.size();
}

ResolumeFile::Mask ResolumeFile::Screen::getMask(int maskIndex) {
    return masks[maskIndex];
}

string ResolumeFile::Mask::getUniqueId() {
	return xml.getAttribute("uniqueId").getValue();
}

string ResolumeFile::Mask::getName() {
    return xml.findFirst("./Params/Param[@name='Name']").getAttribute("value").getValue();
}

bool ResolumeFile::Mask::getEnabled() {
	return xml.findFirst("./Params/Param[@name='Enabled']").getAttribute("value").getBoolValue();
}

vector<glm::vec2> ResolumeFile::Mask::getPoints() {
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
