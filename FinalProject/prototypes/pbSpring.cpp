#include <iostream>
#include <cmath>
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

const int NUM_NODES = 5;
const int NUM_SPRINGS = 9;
const float K = 0.1;
const float DAMPING_K = 1.5;
const float REST_LENGTH = 4.0;
const int BOUNDARY = 8;

struct NODE{
	double mass;
	Vec3f pos;
	Vec3f vel;
	Vec3f force;
};

typedef struct NODE* node_ptr;

struct SPRING{
	int from, to;
	float constant;
	float damping;
	float restLength;
};


class AlloApp : public App{
public:
	Material material;
	Light light;
	Mesh mesh, lines;

	vector<NODE> nodes;
	vector<SPRING> springs;

	float gravity;
	float viscosity;

	AlloApp(){
		nav().pos(0,4.5,30);
		light.pos(0,-5.5,20);
		
		mesh.primitive(Graphics::TRIANGLES);
		//addSphere(mesh);
		addIcosahedron(mesh);
		mesh.scale(0.2);
		mesh.generateNormals();

		nodes.resize(NUM_NODES);
		springs.resize(NUM_SPRINGS);

		for (int i = 0; i < NUM_NODES; i++){
			nodes[i].mass = 1.0;
			nodes[i].pos = Vec3f((rand()%5)+2,(rand()%5)+2,0);
			nodes[i].force = 0.0;
			nodes[i].vel = 0.0;
		}
		for (int i = 0; i < springs.size(); ++i){
			springs[i].constant = K;
			springs[i].damping = DAMPING_K;
			springs[i].restLength = REST_LENGTH;
		}

		viscosity = 2.1;
		gravity = 0.0;
		init_springs();

		initWindow(Window::Dim(0,0,1000,800), "Mass-Spring", 30);
		initAudio(44100,128,2,1);


	}

	virtual void onAnimate(double dt){
		//dt *= 0.01;
		for (int i = 0; i < NUM_SPRINGS; ++i){
			int ni               = springs[i].from, 
			    nj               = springs[i].to;
			nodes[ni].force = 0.0;
			nodes[ni].vel = 0.0;
			Vec3f difference     = nodes[ni].pos - nodes[nj].pos; 
			Vec3f spring_axis    = difference.normalize();
			float current_length = difference.mag();
			float disp           = current_length - springs[i].restLength;
			nodes[ni].force      = disp * springs[i].constant;
			nodes[ni].force.z = 0.0;
			nodes[nj].force      = -(nodes[ni].force);
			nodes[ni].vel        = nodes[ni].force * 1/nodes[ni].mass * (-spring_axis) * dt;
			nodes[nj].vel        = nodes[nj].force * 1/nodes[nj].mass * spring_axis * dt;

			//nodes[ni].vel = checkBounds(nodes[ni].pos, nodes[ni].vel);
			//nodes[nj].vel = checkBounds(nodes[nj].pos, nodes[nj].vel);

			nodes[ni].pos += nodes[ni].vel;
			nodes[nj].pos += nodes[nj].vel;
			
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

	Vec3f checkBounds(Vec3f p, Vec3f v){
		if (p.x > BOUNDARY || p.y > BOUNDARY || p.z > BOUNDARY ||
			p.x < -BOUNDARY || p.y < -BOUNDARY || p.z < -BOUNDARY){
			return -(v); 
		} else
			return v;
	}

	void init_springs(){
		springs[0].from = 0; springs[0].to = 1;
		springs[1].from = 0; springs[1].to = 2;
		springs[2].from = 0; springs[2].to = 3;
		springs[3].from = 4; springs[3].to = 1;
		springs[4].from = 4; springs[4].to = 2;
		springs[5].from = 4; springs[5].to = 3;
		springs[6].from = 1; springs[6].to = 2;
		springs[7].from = 2; springs[7].to = 3;
		springs[8].from = 3; springs[8].to = 1;
	}


};


int main(){
	AlloApp app;
	app.start();
	return 0;
}

