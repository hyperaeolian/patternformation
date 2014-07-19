#include <iostream>
#include <cmath>
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

const int NODES = 10;
const int NUM_NODES = NODES * NODES;
const float K = 1.0;
const float REST_LENGTH = 0.8;
const int NEIGHBORS = 8;


struct NODE{
	Vec3f pos;
	float mass;
};

typedef struct NODE* node_ptr;

struct SPRING{
	vector<node_ptr> neighbors(NEIGHBORS);
	node_ptr anchor;
	Vec3f currentLength, displacement;
	float constant;
	float restLength;
};

class AlloApp : public App{
public:
	Material material;
	Light light;
	vector<NODE> nodes;
	vector<SPRING> springs;
	Mesh mesh;
	vector<Vec3f> forces;
	float angle;
	
	float theta[NUM_NODES];
	float RPM[NUM_NODES];
	Vec3f position[NUM_NODES];

	AlloApp(){
		nav().pos(10,4.5,30);
		light.pos(2.5,-5.5,20);
		nodes.resize(NUM_NODES);
		springs.resize(NUM_NODES);
		forces.resize(NUM_NODES);
		angle = 0;
		int index = 0;
		for (int i = 0; i < NODES; ++i){
			for (int j = 0; j < NODES; ++j){
				nodes[index].pos = Vec3f(i,j,0);
				springs[index].constant = K;
				springs[index].restLength = REST_LENGTH;
				springs[index].anchor = nodes[index].pos;
				index++;
			}
		}
		mesh.primitive(Graphics::TRIANGLES);
		addSphere(mesh);
		mesh.scale(0.2);
		mesh.generateNormals();
		mesh.decompress();

		initWindow(Window::Dim(0,0,1000,800), "Mass-Spring", 30);
		initAudio(44100,128,2,1);


	}

	virtual void onAnimate(double dt){
		int index = 0;
		for (int i = 1; i < NODES-1; ++i){
			for (int j = 1; j < NODES-1; ++j){
				insert_spring_neighbors(nodes[index]);
				//nodes[index].pos.x = rnd::uniformS() * 0.1;
				index++;
			}
		}
	}

	virtual void onDraw(Graphics& g, const Viewpoint& v){
		material();
		light();
		for (int i = 0; i < NUM_NODES; i++){
			g.clearColor(0,0,0,0);
			g.matrixMode(g.PROJECTION);
			g.depthTesting(1);
			g.blending(true);
			
			g.pushMatrix();
				g.color(HSV((float)i/20,1,0.5));
				g.translate(nodes[i].pos);
				g.draw(mesh);
			g.popMatrix();
		}
	}

	Vec3f insert_spring(Vec3f a, Vec3f b, float r){
		Vec3f currLen, disp, f;
		float restLen = r;
		currLen = (a - b).mag();
		disp = currLen - restLen;
		f = disp * K;
		f.z = 0.0;
		return f;
	}

	void insert_spring_neighbors(NODE a, vector<Vec3f> n){
		Vec3f f;
		/*
This is static info, each spring needs to know of its force
and the two masses that it's connected to; spring is a struct
with pointers to masses and spring constant
*/
		int x = a.pos.x;
		int y = a.pos.y;
		vector<Vec3f> neighbors;

		neighbors.push_back(Vec3f(x-1,y+1,0));
		neighbors.push_back(Vec3f(x,y+1,0));
		neighbors.push_back(Vec3f(x+1,y+1,0));
		neighbors.push_back(Vec3f(x-1,y,0));
		neighbors.push_back(Vec3f(x+1,y,0));
		neighbors.push_back(Vec3f(x-1,y-1,0));
		neighbors.push_back(Vec3f(x,y-1,0));
		neighbors.push_back(Vec3f(x+1,y-1,0));
		// for (int i = 0; i < neighbors.size(); ++i){
		// 	a.pos += insert_spring(a.pos, neighbors[i], 1.8);
		// 	neighbors[i] += -(insert_spring(a.pos, neighbors[i], 1.8));
		// }

	}


};


int main(){
	AlloApp app;
	app.start();
	return 0;
}

/*
	Force = a * (spring force)


*/

			// theta[i] += RPM[i] * (2.0*M_PI) * (dt / 90);
			// int j = i + 1;
			// float x = radius * cos(theta[i]);
			// float y = radius * sin(theta[i]);
			// position[i] = Vec3f(x,y,0);
			// RPM[i] += ROTATIONAL_CONSTANT;
			// forces[i] = 0.0;
			// float restLength = 1.3;
			// dist = position[i] - position[j];
			// currentLength = dist.mag();
			// displacement = currentLength - restLength;
			// forces[i] = displacement * K;
			// forces[i].z = 0.0;
			// forces[j] = -forces[i];
			// position[i] += forces[i];
			// if (position[i].mag() > 20.0){
			// 	forces[i] = -forces[i]; 
			// }