//DH2323 skeleton code, Lab2 (SDL2 version)
#include <iostream>
#include <glm/glm.hpp>
#include "SDL2auxiliary.h"
#include "TestModel.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

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
vec3 lightPos(0, -0.5, -0.7);
vec3 lightColor = 14.f * vec3(1, 1, 1);
vec3 indirectLight = 0.5f * vec3(1, 1, 1);

vec3 black(0, 0, 0);

// PHOTON MAPPING CONSTS
int numberOfPhotons = 100000;

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

	while (!sdlAux->quitEvent())
	{
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
	//cout << "Render time: " << dt << " ms." << endl;

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
	if (keystate[SDL_SCANCODE_W]) {
		lightPos.z += dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_S]) {
		lightPos.z -= dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_A]) {
		lightPos.x -= dt * cameraV / 1000;
	}
	if (keystate[SDL_SCANCODE_D]) {
		lightPos.x += dt * cameraV / 1000;
	}
	//if (keystate[SDL_SCANCODE_Q]) {
	//	lightPos.y += dt * cameraV / 1000;
	//}
	//if (keystate[SDL_SCANCODE_E]) {
	//	lightPos.y -= dt * cameraV / 1000;
	//}

}

void Draw(vector<Photon> photonMap)
{
	sdlAux->clearPixels();

	for (int i = 0; i < photonMap.size(); i++) {
		Photon photon = photonMap[i];
		if (photon.hit) {
			vec3 tPos = (photon.position - cameraPos) * R;
			int x = focalLength * tPos.x / tPos.z + SCREEN_WIDTH / 2.0;
			int y = focalLength * tPos.y / tPos.z + SCREEN_HEIGHT / 2.0;
			sdlAux->putPixel(x, y, vec3(1.0, 1.0, 1.0));
		}
	}

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
	sdlAux->render();
}

// ----------------------------------------------------------------------------
// PHOTON MAPPING FUNCTIONS


void PhotonPass(vector<Photon>& photonMap) {
	for (int i = 0; i < photonMap.size(); i++) {
		photonMap[i].power = vec3(10, 10, 10) / (float) photonMap.size();
		Intersection closestIntersection;
		bool reflected = true;
		vec3 origin = lightPos;
		vec3 direction((2 * (float)rand() / RAND_MAX) - 1, (2 * (float)rand() / RAND_MAX) - 1, (2 * (float)rand() / RAND_MAX) - 1);
		while (reflected) {
			photonMap[i].hit = ClosestIntersection(origin, direction, triangles, closestIntersection, -1);
			if (photonMap[i].hit) {
				Triangle triangle = triangles[closestIntersection.triangleIndex];
				glm::vec2 surfaceMaterial = triangle.photonProbabilities;
				float epsilon = (float)rand() / RAND_MAX;
				if (epsilon > surfaceMaterial.x + surfaceMaterial.y) {
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
}

