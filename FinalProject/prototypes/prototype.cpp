#include <iostream>
#include <cmath>
#include "allocore/io/al_App.hpp"

using namespace al;
using namespace std;

const int xyzRes = 6;
const int RESOLUTION = xyzRes * xyzRes * xyzRes;
const float SCALE_FACTOR = 1.f/(float)xyzRes;
const int BOUND1 = RESOLUTION, BOUND2 = -RESOLUTION;
const float RADIUS = 3.5;

struct PARTICLE{
	Vec3f position;
	int alive;
	Mesh p;
};

struct find_neighbor{
	Vec3f p;
	find_neighbor(Vec3f p) : p(p){}
	bool operator()(const position& m) const {
		return m.position == p;
	}
};

std::vector<Vec3f> return_neighbors(Vec3f c);

class AlloApp : public App{
public:
	Material material;
	Light light;
	double phase;
	//Mesh p;
	int N;
	std::vector<PARTICLE> particles, np;

	float RPM, theta;

	AlloApp(): phase(0){
		nav().pos(2.5,2.5,20);
		light.pos(2.5,2.5,20);

		particles.resize(RESOLUTION);
		np.resize(RESOLUTION);
		p.primitive(Graphics::TRIANGLES);
		//addWireBox(p,SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR);
		// N = addCube(p);
		int index = 0;
		for (int i = 0; i < xyzRes; ++i){
			for (int j = 0; j < xyzRes; ++j){
				for (int k = 0; k < xyzRes; ++k){
					particles[index].position = Vec3f(i,j,k);
					particles[index].alive = index % 2 == 0 ? rand() % 2 : 1;
					addCube(particles[index].p);
					index++;
				}
			}
		}
		theta = 0.5;
		RPM = 2;
	//	particles[0].alive = 1;
// 		 particles[5].alive = 1;
// 		 particles[6].alive = 1;
		initWindow(Window::Dim(0,0,800,600), "Game of Life", 40);
		initAudio(44100, 128, 2, 1);
	}

	virtual void onAnimate(double dt){
		//reset_values();
		// theta += RPM * (2*M_PI) * (dt/30);ß
		// float x = RADIUS * cos(theta);
		// float y = RADIUS * sin(theta);
		int neighbors;
		for (int i = 0; i < particles.size(); i++){
			if (particles[i].position == 0) continue;
			if (particles[i].alive == 1){ 
				neighbors = calculate_neighbors(particles[i].position);
				particles[i].alive = neighbors == 5 || neighbors == 6 || neighbors == 7 ? 1 : 0;
			} 
			if (particles[i].alive == 0){
				neighbors = calculate_neighbors(particles[i].position);
				particles[i].alive = neighbors == 5 ? 1 : 0;
			}
		}
		phase += dt;
		if (phase >= 1.) phase -= 1.;
	}

	virtual void onSound(AudioIOData& io){
		while(io()){}
	}

	virtual void onDraw(Graphics& g, const Viewpoint& v, Mesh& m){
		material();
		light();
		g.blending(true);
		g.antialiasing(Graphics::NICEST);

		for (int i = 0; i < particles.size(); ++i){
			if (particles[i].alive == 0){ 
				continue;
			}
			else if (particles[i].alive == 1){ 
				g.pushMatrix();
					g.color(HSV((float)i/20,1,al::fold(phase + i*0.5/N, 0.5)+0.5));
					g.translate(particles[i].position);
					//g.draw(m);
				g.popMatrix();
			}	
		}
		g.draw(m);

	}

	virtual void onKeyDown(const ViewpointWindow& w, const Keyboard& k){}
	virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m){}
	virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m){}

	int calculate_neighbors(Vec3f& center){
		std::vector<Vec3f> current_poses, tz;
		current_poses = return_neighbors(center);
		int neighbors = 0;
			//if (out_of_bounds(current_poses[i])) continue;
			auto current = find(particles.begin(), particles.end(), find_neighbor(current_poses[i]));
			neighbors += current->alive;	
		return neighbors;
	}

	void reset_values(){for (int i = 0; i < particles.size(); ++i) particles[i].alive = rand()%2;}

	bool out_of_bounds(Vec3f p){
		if (p.x > BOUND1 || p.x < BOUND2 || p.y > BOUND1 || p.y < BOUND2 || p.z > BOUND1 || p.z < BOUND2){
			return true;
		} else{
			return false;
		}
	}


};

int main(){
	AlloApp app;
	app.start();
	return 0;
}

std::vector<Vec3f> return_neighbors(Vec3f c){
	int x = c.x;
	int y = c.y;
	int z = c.z;
		std::vector<Vec3f> current_poses;
		current_poses.push_back(Vec3f(x-1,y-1,z));
		current_poses.push_back(Vec3f(x-1,y,z));
		current_poses.push_back(Vec3f(x-1,y+1,z));
		current_poses.push_back(Vec3f(x,y+1,z));
		current_poses.push_back(Vec3f(x+1,y+1,z));
		current_poses.push_back(Vec3f(x+1,y,z));
		current_poses.push_back(Vec3f(x+1,y-1,z));
		current_poses.push_back(Vec3f(x-1,y-1,z));
		current_poses.push_back(Vec3f(x-1,y-1,z+1));
		current_poses.push_back(Vec3f(x-1,y,z+1));
		current_poses.push_back(Vec3f(x-1,y+1,z+1));
		current_poses.push_back(Vec3f(x,y+1,z+1));
		current_poses.push_back(Vec3f(x+1,y+1,z+1));
		current_poses.push_back(Vec3f(x+1,y,z+1));
		current_poses.push_back(Vec3f(x+1,y-1,z+1));
		current_poses.push_back(Vec3f(x,y-1,z+1));
		current_poses.push_back(Vec3f(x-1,y-1,z+1));
		current_poses.push_back(Vec3f(x-1,y,z+1));
		current_poses.push_back(Vec3f(x,y,z+1));
		current_poses.push_back(Vec3f(x-1,y-1,z-1));
		current_poses.push_back(Vec3f(x-1,y,z-1));
		current_poses.push_back(Vec3f(x-1,y+1,z-1));
		current_poses.push_back(Vec3f(x,y+1,z-1));
		current_poses.push_back(Vec3f(x+1,y+1,z-1));
		current_poses.push_back(Vec3f(x+1,y,z-1));
		current_poses.push_back(Vec3f(x+1,y-1,z-1));
		current_poses.push_back(Vec3f(x,y-1,z-1));
		current_poses.push_back(Vec3f(x-1,y-1,z-1));
		current_poses.push_back(Vec3f(x-1,y,z-1));
		current_poses.push_back(Vec3f(x,y,z-1));

		return current_poses;
}