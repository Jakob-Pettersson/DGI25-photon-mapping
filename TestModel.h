#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

// Defines a simple test model: The Cornel Box

#include <glm/glm.hpp>
#include <vector>

// Used to describe a triangular surface:
class Triangle
{
public:
	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec3 photonProbabilities;
	float refIndex;

	Triangle( glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 color, glm::vec3 photonProbabilities, float refIndex)
		: v0(v0), v1(v1), v2(v2), color(color), photonProbabilities(photonProbabilities), refIndex(refIndex)
	{
		ComputeNormal();
	}

	void ComputeNormal()
	{
		glm::vec3 e1 = v1-v0;
		glm::vec3 e2 = v2-v0;
		normal = glm::normalize( glm::cross( e2, e1 ) );
	}
};

// Loads the Cornell Box. It is scaled to fill the volume:
// -1 <= x <= +1
// -1 <= y <= +1
// -1 <= z <= +1
void LoadTestModel( std::vector<Triangle>& triangles )
{
	using glm::vec3;
	using glm::vec2;

	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  0.75f, 0.75f, 0.75f );

	// Defines material properties
	vec3 diffuse(0.7f, 0.1f, 0.0f);
	vec3 reflective(0.05f, 0.9f, 0.0f);
	vec3 transparent(0.0f, 0.2f, 0.7f);
	vec3 perfectlyReflective(1.0f, 0.0f, 0.0f);
	vec3 perfectlyDiffuse(0.0f, 0.0f, 0.0f);

	triangles.clear();
	triangles.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec3 A(L,0,0);
	vec3 B(0,0,0);
	vec3 C(L,0,L);
	vec3 D(0,0,L);

	vec3 E(L,L,0);
	vec3 F(0,L,0);
	vec3 G(L,L,L);
	vec3 H(0,L,L);

	// Floor:
	triangles.push_back( Triangle( C, B, A, green, perfectlyDiffuse, 1.0) );
	triangles.push_back( Triangle( C, D, B, green, perfectlyDiffuse, 1.0) );
	
	// Left wall
	triangles.push_back( Triangle( A, E, C, purple, perfectlyDiffuse, 1.0) );
	triangles.push_back( Triangle( C, E, G, purple, perfectlyDiffuse, 1.0) );

	// Right wall
	triangles.push_back( Triangle( F, B, D, yellow, perfectlyDiffuse, 1.0) );
	triangles.push_back( Triangle( H, F, D, yellow, perfectlyDiffuse, 1.0) );

	// Ceiling
	triangles.push_back( Triangle( E, F, G, cyan, perfectlyDiffuse, 1.0) );
	triangles.push_back( Triangle( F, H, G, cyan, perfectlyDiffuse, 1.0) );

	// Back wall
	triangles.push_back( Triangle( G, D, C, white, perfectlyDiffuse, 1.0) );
	triangles.push_back( Triangle( G, H, D, white, perfectlyDiffuse, 1.0) );

	//// ---------------------------------------------------------------------------
	//// Short block

	//A = vec3(290,0,114);
	//B = vec3(130,0, 65);
	//C = vec3(240,0,272);
	//D = vec3( 82,0,225);

	//E = vec3(290,165,114);
	//F = vec3(130,165, 65);
	//G = vec3(240,165,272);
	//H = vec3( 82,165,225);

	//// Front
	//triangles.push_back( Triangle(E,B,A,red, diffuse));
	//triangles.push_back(Triangle(E, F, B, red, diffuse));

	//// Front
	//triangles.push_back(Triangle(F, D, B, red, diffuse));
	//triangles.push_back(Triangle(F, H, D, red, diffuse));

	//// BACK
	//triangles.push_back(Triangle(H, C, D, red, diffuse));
	//triangles.push_back(Triangle(H, G, C, red, diffuse));

	//// LEFT
	//triangles.push_back(Triangle(G, E, C, red, diffuse));
	//triangles.push_back(Triangle(E, A, C, red, diffuse));

	//// TOP
	//triangles.push_back(Triangle(G, F, E, red, diffuse));
	//triangles.push_back(Triangle(G, H, F, red, diffuse) );

	//// ---------------------------------------------------------------------------
	//// Tall block

	//A = vec3(423,0,247);
	//B = vec3(265,0,296);
	//C = vec3(472,0,406);
	//D = vec3(314,0,456);

	//E = vec3(423,330,247);
	//F = vec3(265,330,296);
	//G = vec3(472,330,406);
	//H = vec3(314,330,456);

	//// Front
	//triangles.push_back( Triangle(E,B,A,blue, reflective));
	//triangles.push_back(Triangle(E, F, B, blue, reflective));

	//// Front
	//triangles.push_back(Triangle(F, D, B, blue, reflective));
	//triangles.push_back(Triangle(F, H, D, blue, reflective));

	//// BACK
	//triangles.push_back(Triangle(H, C, D, blue, reflective));
	//triangles.push_back(Triangle(H, G, C, blue, reflective));

	//// LEFT
	//triangles.push_back(Triangle(G, E, C, blue, reflective));
	//triangles.push_back(Triangle(E, A, C, blue, reflective));

	//// TOP
	//triangles.push_back(Triangle(G, F, E, blue, reflective));
	//triangles.push_back(Triangle(G, H, F, blue, reflective) );

	// ---------------------------------------------------------------------------
	// Prism
	float prismTriSideLength = 100;

	A = vec3(200, 0, 200);
	B = vec3(200, 0, 200 + prismTriSideLength);
	C = vec3(200.0 + (prismTriSideLength * sqrt(3)) / 2.0, 0, 200.0 + prismTriSideLength / 2.0);

	D = vec3(200, 330, 200);
	E = vec3(200, 330, 200 + prismTriSideLength);
	F = vec3(200.0 + (prismTriSideLength * sqrt(3)) / 2.0, 330, 200.0 + prismTriSideLength / 2.0);

	// Front
	triangles.push_back(Triangle(D, B, A, blue, transparent, 1.5));
	triangles.push_back(Triangle(D, E, B, blue, transparent, 1.5));

	// Front
	triangles.push_back(Triangle(E, C, B, blue, transparent, 1.5));
	triangles.push_back(Triangle(E, F, C, blue, transparent, 1.5));

	triangles.push_back(Triangle(F, A, C, blue, transparent, 1.5));
	triangles.push_back(Triangle(F, D, A, blue, transparent, 1.5));

	// TOP
	triangles.push_back(Triangle(D, E, F, blue, transparent, 1.5));

	//// ---------------------------------------------------------------------------
	//// Center cube
	//float center = L / 2.0;
	//float sideL = 100;

	//A = vec3(center + sideL, 0, center - sideL);
	//B = vec3(center - sideL, 0, center - sideL);
	//C = vec3(center + sideL, 0, center + sideL);
	//D = vec3(center - sideL, 0, center + sideL);

	//E = vec3(center + sideL, 2.0 * sideL, center - sideL);
	//F = vec3(center - sideL, 2.0 * sideL, center - sideL);
	//G = vec3(center + sideL, 2.0 * sideL, center + sideL);
	//H = vec3(center - sideL, 2.0 * sideL, center + sideL);

	//// Front
	//triangles.push_back(Triangle(E, A, B, red, perfectlyReflective));
	//triangles.push_back(Triangle(E, B, F, red, perfectlyReflective));

	//// RIGHT
	//triangles.push_back(Triangle(F, D, B, red, perfectlyReflective));
	//triangles.push_back(Triangle(F, H, D, red, perfectlyReflective));

	//// BACK
	//triangles.push_back(Triangle(H, D, C, red, perfectlyReflective));
	//triangles.push_back(Triangle(H, C, G, red, perfectlyReflective));

	//// LEFT
	//triangles.push_back(Triangle(G, E, C, red, perfectlyReflective));
	//triangles.push_back(Triangle(E, A, C, red, perfectlyReflective));

	//// TOP
	//triangles.push_back(Triangle(G, F, E, red, perfectlyReflective));
	//triangles.push_back(Triangle(G, H, F, red, perfectlyReflective) );

	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for( size_t i=0; i<triangles.size(); ++i )
	{
		triangles[i].v0 *= 2/L;
		triangles[i].v1 *= 2/L;
		triangles[i].v2 *= 2/L;

		triangles[i].v0 -= vec3(1,1,1);
		triangles[i].v1 -= vec3(1,1,1);
		triangles[i].v2 -= vec3(1,1,1);

		triangles[i].v0.x *= -1;
		triangles[i].v1.x *= -1;
		triangles[i].v2.x *= -1;

		triangles[i].v0.y *= -1;
		triangles[i].v1.y *= -1;
		triangles[i].v2.y *= -1;

		triangles[i].ComputeNormal();
	}
}

#endif