//DH2323 skeleton code, Lab2 (SDL2 version)
#include <iostream>
#include <glm/glm.hpp>
#include "SDL2auxiliary.h"
#include "TestModel.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>
#include<cstdlib>
#include "math.h"
using namespace std;
using glm::vec3;
using glm::cross;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;
SDL2Aux* sdlAux;
int t;
vector<Triangle> triangles;

// Test for GPU rendering

// Camera variables
float focalLength = SCREEN_HEIGHT / 2.0;
vec3 cameraPos(0, 0, -2.0);
float cameraV = 0.5;

// Rotation
float rotateV = 0.5;
glm::mat3 R(1, 0, 0,
	0, 1, 0,
	0, 0, 1);
float yaw = 0;

// Light
vec3 lightPos(0, 0.9, -0.8);
vec3 lightColor = 14.f * vec3(1, 1, 1);
vec3 indirectLight = 0.5f * vec3(1, 1, 1);

vec3 black(0, 0, 0);

// PHOTON MAPPING CONSTS
int numberOfPhotons = 100000;
int numberOfDebugPhotons = 1000;
bool dynamic = true;
bool debug = false;
// ----------------------------------------------------------------------------
// STRUCTS

struct Intersection {
	vec3 position;
	float distance;
	int triangleIndex;
};

struct Photon {
	vec3 position, power;
	//float phi, theta;
	//short flag;
	bool hit = false;
	vector<vec3> path;
};

// ----------------------------------------------------------------------------
// FUNCTION DEFINITIONS

void Update(void);
void Draw(vector<Photon> photonMap);
void Rotate(void);

bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection,
	int sourceTriangleIndex
);

vec3 DirectLight(const Intersection& i);

void PhotonPass(vector<Photon>& photonMap);
void DebugPhotonPass(vector<Photon>& photonMap);
// ----------------------------------------------------------------------------
// MAIN FUNCTION

int main(int argc, char* argv[])
{
	sdlAux = new SDL2Aux(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.
	// Load scene values
	LoadTestModel(triangles);
	/*for (int i = 0; i < triangles.size(); i++) {
		cout << glm::to_string(triangles[i].photonProbabilities) << "\n";
	}*/

	vector<Photon> photonMap(numberOfPhotons);
	PhotonPass(photonMap);
	if (debug) {
		DebugPhotonPass(photonMap);
	}

	while (!sdlAux->quitEvent())
	{
		if (dynamic) {
			PhotonPass(photonMap);
		}
		Draw(photonMap);
		Update();
	}
	sdlAux->saveBMP("screenshot.bmp");
	return 0;
}

// ----------------------------------------------------------------------------
// FUNCTIONS

bool ClosestIntersection(vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection,
	int sourceTriangleIndex)
{
	// Init iterative values
	bool intersects = false;
	vec3 closestIntersectionFound(FLT_MAX, FLT_MAX, FLT_MAX);

	for (int i = 0; i < triangles.size(); i++) {
		// Define triangle and associated values
		if (i == sourceTriangleIndex) {
			continue;
		}
		Triangle triangle = triangles[i];
		vec3 v0 = triangle.v0;
		vec3 v1 = triangle.v1;
		vec3 v2 = triangle.v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start - v0;
		mat3 A(-dir, e1, e2);

		vec3 x = glm::inverse(A) * b;
		vec3 intersectionPoint = start + x.x * dir;

		if (x.x > 0 && x.y >= 0 && x.z >= 0 && x.y + x.z <= 1) {
			if (glm::length(intersectionPoint - start) < glm::length(closestIntersectionFound - start)) {
				closestIntersectionFound = intersectionPoint;
				closestIntersection.position = closestIntersectionFound;
				closestIntersection.distance = glm::length(closestIntersectionFound - start);
				closestIntersection.triangleIndex = i;
				intersects = true;
			}
		}
	}

	return intersects;
}

void Rotate() {
	R[0][0] = cos(yaw);
	R[2][0] = sin(yaw);
	R[0][2] = -sin(yaw);
	R[2][2] = cos(yaw);
}

vec3 DirectLight(const Intersection& i) {
	vec3 r = lightPos - i.position;
	Intersection shadowIntersection;
	bool intersect = ClosestIntersection(i.position, glm::normalize(r), triangles, shadowIntersection, i.triangleIndex);
	if (intersect && glm::length(shadowIntersection.position - i.position) < glm::length(r)) {
		return black;
	}

	vec3 n = glm::normalize(triangles[i.triangleIndex].normal);
	vec3 B = lightColor / float(4.0 * M_PI * pow(glm::length(r), 2));
	vec3 D = B * max(glm::dot(glm::normalize(r), n), 0.f);
	return D;
}

// ----------------------------------------------------------------------------
// SDL FUNCTIONS

void Update(void)
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t);
	t = t2;
	cout << "Render time: " << dt << " ms." << endl;

	// Camera control
	const Uint8* keystate = SDL_GetKeyboardState(NULL);
	if (keystate[SDL_SCANCODE_W]) {
		cameraPos += glm::column(R, 2) * (dt * cameraV / 1000);
	}
	if (keystate[SDL_SCANCODE_S]) {
		cameraPos -= glm::column(R, 2) * (dt * cameraV / 1000);
	}
	if (keystate[SDL_SCANCODE_A]) {
		cameraPos -= glm::column(R, 0) * (dt * cameraV / 1000);
	}
	if (keystate[SDL_SCANCODE_D]) {
		cameraPos += glm::column(R, 0) * (dt * cameraV / 1000);
	}
	if (keystate[SDL_SCANCODE_Q]) {
		yaw -= dt * rotateV / 1000;
		Rotate();
	}
	if (keystate[SDL_SCANCODE_E]) {
		yaw += dt * rotateV / 1000;
		Rotate();
	}
	// Light control
	if (keystate[SDL_SCANCODE_UP]) {
		lightPos.z += dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		lightPos.z -= dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_LEFT]) {
		lightPos.x -= dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		lightPos.x += dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_O]) {
		lightPos.y += dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_L]) {
		lightPos.y -= dt * cameraV / 1000;
	}

}

void Draw(vector<Photon> photonMap)
{
	sdlAux->clearPixels();


	/*for (int y = 0; y < SCREEN_HEIGHT; ++y)
	{
		for (int x = 0; x < SCREEN_WIDTH; ++x)
		{
			vec3 d(x - SCREEN_WIDTH / 2.0, y - SCREEN_HEIGHT / 2.0, focalLength);
			d = R * d;

			Intersection intersection;
			bool intersects = ClosestIntersection(cameraPos, d, triangles, intersection, -1);

			if (intersects) {
				vec3 D = DirectLight(intersection);
				sdlAux->putPixel(x, y, triangles[intersection.triangleIndex].color * (indirectLight + D));
			}
			else {
				sdlAux->putPixel(x, y, black);
			}
		}
	}*/

	for (int i = 0; i < photonMap.size(); i++) {
		Photon photon = photonMap[i];
		
		if (photon.hit) {
			vec3 tPos = (photon.position - cameraPos) * R;
			int x = focalLength * tPos.x / tPos.z + SCREEN_WIDTH / 2.0;
			int y = focalLength * tPos.y / tPos.z + SCREEN_HEIGHT / 2.0;
			if(debug && i < numberOfDebugPhotons) {
				sdlAux->putPixel(x, y, vec3(1.0, 0.0, 0.0));
			}
			else {
				sdlAux->putPixel(x, y, vec3(1.0, 1.0, 1.0));
			}
		}
	}

	sdlAux->render();
}

// ----------------------------------------------------------------------------
// PHOTON MAPPING FUNCTIONS


void PhotonPass(vector<Photon>& photonMap) {
	int t1 = SDL_GetTicks();
	
	std::srand(2);
	for (int i = 0; i < photonMap.size(); i++) {
		photonMap[i].power = vec3(10, 10, 10) / (float) photonMap.size();
		photonMap[i].path.clear();
		photonMap[i].path.push_back(lightPos);
		Intersection closestIntersection;
		bool reflected = true;
		vec3 origin = lightPos;
		vec3 direction((2 * (float)rand() / RAND_MAX) - 1, (2 * (float)rand() / RAND_MAX) - 1, (2 * (float)rand() / RAND_MAX) - 1);
		int reflectionTriangleIndex = -1;
		while (reflected) {
			photonMap[i].hit = ClosestIntersection(origin, direction, triangles, closestIntersection, reflectionTriangleIndex);
			if (photonMap[i].hit) {
				photonMap[i].path.push_back(closestIntersection.position);
				reflectionTriangleIndex = closestIntersection.triangleIndex;
				Triangle triangle = triangles[reflectionTriangleIndex];
				glm::vec3 surfaceMaterial = triangle.photonProbabilities;
				float epsilon = (float)rand() / RAND_MAX;
				if (epsilon > surfaceMaterial.x + surfaceMaterial.y + surfaceMaterial.z) {
					reflected = false;
				}
				else if (epsilon > surfaceMaterial.x + surfaceMaterial.y){
					origin = closestIntersection.position;
					vec3 normal = triangle.normal;
					float n1 = 1.0;
					float n2 = triangle.refIndex;
					float r = n1 / n2;
					float c = glm::dot(-normal, direction);
					direction = r * direction + normal * (float)(r * c - sqrt(1 - pow(r, 2) * (1 - pow(c, 2))));
					ClosestIntersection(origin, direction, triangles, closestIntersection, reflectionTriangleIndex);
					origin = closestIntersection.position;
					n1 = triangle.refIndex;
					n2 = 1.0;
					c = glm::dot(-normal, direction);
					direction = r * direction + normal * (float)(r * c - sqrt(1 - pow(r, 2) * (1 - pow(c, 2))));
				}
				else {
					origin = closestIntersection.position;
					vec3 normal = triangle.normal;
					vec3 projection = (direction * normal) / (float)pow(glm::length(normal), 2.0) * normal;
					if ((direction - (2.0f * projection)).z < 0) {
						//cout << "Direction: " << glm::to_string(direction) << " Normal: " << glm::to_string(normal) << " Projection: " << glm::to_string(projection) << " New direction: " << glm::to_string(direction - (2.0f * projection)) << "\n";
					}
					direction -= 2.0f * projection;
				}
			}
			else {
				reflected = false;
			}
		}
		photonMap[i].position = closestIntersection.position;
	}
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t1);
	//cout << "Photon pass time: " << dt << " ms." << endl;
}

void DebugPhotonPass(vector<Photon>& photonMap) {
	int t1 = SDL_GetTicks();

	for (int i = 0; i < numberOfDebugPhotons; i++) {
		photonMap[i].power = vec3(10, 10, 10) / (float)photonMap.size();
		Intersection closestIntersection;
		bool reflected = true;
		vec3 origin = lightPos;
		vec3 direction(cos(2 * M_PI * i / (numberOfDebugPhotons / 10)), sin(2 * M_PI * i / (numberOfDebugPhotons / 10)), -10);
		while (reflected) {
			photonMap[i].hit = ClosestIntersection(origin, direction, triangles, closestIntersection, -1);
			if (photonMap[i].hit) {
				Triangle triangle = triangles[closestIntersection.triangleIndex];
				glm::vec3 surfaceMaterial = triangle.photonProbabilities;
				float epsilon = (float)rand() / RAND_MAX;
				if (epsilon > surfaceMaterial.x + surfaceMaterial.y + surfaceMaterial.z) {
					reflected = false;
				}
				else {
					origin = closestIntersection.position;
					vec3 normal = triangle.normal;
					vec3 projection = (direction * normal) / (float)pow(glm::length(normal), 2.0) * normal;
					direction -= 2.0f * projection;
				}
			}
			else {
				reflected = false;
			}
		}

		photonMap[i].position = closestIntersection.position;
	}
	int t2 = SDL_GetTicks();
	float dt = float(t2 - t1);
	cout << "Debug photon pass time: " << dt << " ms." << endl;
}

