#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(255);
	ofSetLineWidth(2);
	ofEnableDepthTest();

	auto radius = 12;
	auto x_span = radius * sqrt(3);
	auto flg = true;
	for (float y = -650; y < 650; y += radius * 1.5) {

		for (float x = -650; x < 650; x += x_span) {

			glm::vec2 location;
			if (flg) {

				location = glm::vec2(x, y);
			}
			else {

				location = glm::vec2(x + (x_span / 2), y);
			}

			this->location_list.push_back(location);
			this->noise_seed_list.push_back(glm::vec3(ofRandom(1000), ofRandom(1000), ofRandom(1000)));
			this->color_list.push_back(ofColor(0));
		}
		flg = !flg;
	}

	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

}

//--------------------------------------------------------------
void ofApp::update() {
	
	this->face.clear();
	this->frame.clear();

	auto radius = 12;
	auto height = 5.f;

	// 配色デザイン ビビッドブルー P144
	vector<ofColor> color_palette;
	color_palette.push_back(ofColor(18, 85, 163));
	color_palette.push_back(ofColor(0, 165, 231));
	color_palette.push_back(ofColor(93, 177, 186));
	color_palette.push_back(ofColor(44, 121, 191));
	color_palette.push_back(ofColor(136, 185, 79));
	color_palette.push_back(ofColor(246, 221, 80));
	color_palette.push_back(ofColor(200, 59, 57));
	//color_palette.push_back(ofColor(255, 255, 255)); // gbcolor

	for (int i = 0; i < this->location_list.size(); i++) {

		auto noise_deg = glm::vec3(
			ofMap(ofNoise(glm::vec3(this->location_list[i] * 0.005, this->noise_seed_list[i].x + ofGetFrameNum() * 0.05)), 0, 1, -360, 360),
			ofMap(ofNoise(glm::vec3(this->location_list[i] * 0.005, this->noise_seed_list[i].y + ofGetFrameNum() * 0.05)), 0, 1, -360, 360),
			ofMap(ofNoise(glm::vec3(this->location_list[i] * 0.005, this->noise_seed_list[i].z + ofGetFrameNum() * 0.05)), 0, 1, -360, 360));

		auto len = glm::length(this->location_list[i]);
		auto param = (ofGetFrameNum() * 10) % 630 - 30;
		auto tmp = abs(len - param);
		if (tmp > 0 && tmp < 30) {

			this->color_list[i] = color_palette[ofMap(ofNoise(glm::vec3(this->location_list[i] * 0.003, ofGetFrameNum() * 0.05)), 0, 1, 0, color_palette.size())];
		}
		else {

			noise_deg *= 0;
		}

		this->setHexagonToMesh(this->face, this->frame, glm::vec3(this->location_list[i], 0), radius, height, noise_deg, this->color_list[i], ofColor(255));
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	this->face.draw();
	this->frame.draw();

	this->cam.end();
}

//--------------------------------------------------------------
void ofApp::setHexagonToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float radius, float height, glm::vec3 noise_deg, ofColor face_color, ofColor frame_color) {

	auto rotation_x = glm::rotate(glm::mat4(), noise_deg.x * (float)DEG_TO_RAD, glm::vec3(1, 0, 0));
	auto rotation_y = glm::rotate(glm::mat4(), noise_deg.y * (float)DEG_TO_RAD, glm::vec3(0, 1, 0));
	auto rotation_z = glm::rotate(glm::mat4(), noise_deg.z * (float)DEG_TO_RAD, glm::vec3(0, 0, 1));

	for (int deg = 90; deg < 450; deg += 60) {

		auto face_radius = radius - 0.5;
		auto face_index = face_target.getNumVertices();

		vector<glm::vec3> vertices;
		vertices.push_back(glm::vec3(0, 0, 0));
		vertices.push_back(glm::vec3(face_radius * cos(deg * DEG_TO_RAD), face_radius * sin(deg * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(face_radius * cos((deg + 60) * DEG_TO_RAD), face_radius * sin((deg + 60) * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(0, 0, height));
		vertices.push_back(glm::vec3(face_radius * cos((deg + 60) * DEG_TO_RAD), face_radius * sin((deg + 60) * DEG_TO_RAD), height));
		vertices.push_back(glm::vec3(face_radius * cos(deg * DEG_TO_RAD), face_radius * sin(deg * DEG_TO_RAD), height));

		for (auto& vertex : vertices) {

			vertex = location + glm::vec4(vertex, 0) * rotation_z * rotation_y * rotation_x;
		}

		face_target.addVertices(vertices);

		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2);
		face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);
		face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 4);
		face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);

		auto frame_index = frame_target.getNumVertices();

		vertices.clear();
		vertices.push_back(glm::vec3(0, 0, 0));
		vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(radius * cos((deg + 60) * DEG_TO_RAD), radius * sin((deg + 60) * DEG_TO_RAD), 0));
		vertices.push_back(glm::vec3(0, 0, height));
		vertices.push_back(glm::vec3(radius * cos((deg + 60) * DEG_TO_RAD), radius * sin((deg + 60) * DEG_TO_RAD), height));
		vertices.push_back(glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), height));

		for (auto& vertex : vertices) {

			vertex = location + glm::vec4(vertex, 0) * rotation_z * rotation_y * rotation_x;
		}

		frame_target.addVertices(vertices);

		frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 2);
		frame_target.addIndex(frame_index + 4); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 2); frame_target.addIndex(frame_index + 4);

		for (int i = 0; i < vertices.size(); i++) {

			face_target.addColor(face_color);
			frame_target.addColor(frame_color);
		}
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}