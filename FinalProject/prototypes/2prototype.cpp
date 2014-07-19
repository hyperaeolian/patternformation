#include <iostream>
#include <cmath>
#include "allocore/io/al_App.hpp"
#include "allocore/graphics/al_Isosurface.hpp"

using namespace al;
using namespace std;

const int xyzRes         = 6;
const int RESOLUTION     = pow(xyzRes,3.0);
const int WIN_HEIGHT = 1000,
		  WIN_WIDTH  = 800;
const float SCALE_FACTOR = 1.f/(float)xyzRes;
const int BOUND1         = RESOLUTION, 
          BOUND2         = -RESOLUTION;


struct PARTICLE{
	Vec3f position;
	float alive;
	float memory;
};

struct SPRING{
	int from, to;
	float constant;
	float rest_length;
};


std::vector<Vec3f> return_neighbors(Vec3f c);

class AlloApp : public App{
public:
	Material material;
	Light light;

	double phase;
	Mesh p;
	int N;
	std::vector<PARTICLE> particles;


/*********** CONSTRUCTOR *******************/
	AlloApp(): phase(0){
		nav().pos(0,4.5,30);
		light.pos(2.5,-5.5,20);
		particles.resize(RESOLUTION);
		 p.primitive(Graphics::TRIANGLES);
		 p.scale(0.2);
		 addCube(p);
		// addSphere(p);
		int index = 0;
		for (int i = 0; i < xyzRes; ++i){
			for (int j = 0; j < xyzRes; ++j){
				for (int k = 0; k < xyzRes; ++k){
				particles[index].position = Vec3f(i,j,k);
				particles[index].alive = index % 2 == 0 ? 1 : 0;
				particles[index].memory = particles[index].alive;
				index++;
				}
			}
		}
		initWindow(Window::Dim(0,0,WIN_HEIGHT,WIN_WIDTH), "Game of Life", 40);
		initAudio(44100, 128, 2, 1);
	}

/*********** >END< CONSTRUCTOR *******************/



/*********** ON ANIMATE ******************/
	virtual void onAnimate(double dt){
		int neighbors = 0;
		 for (int i = 0; i < particles.size(); i++){
		 	if (particles[i].alive == 1){ 
		 		neighbors = calculate_neighbors(particles[i].position);
		 		particles[i].alive = neighbors == 2 || neighbors == 3 ? 1 : 0;
		 	} 
		 	if (particles[i].alive == 0){
		 		neighbors = calculate_neighbors(particles[i].position);
		 		particles[i].alive = neighbors == 3 ? 1 : 0;
		 	}
		 }
		phase += dt;
		if (phase >= 1.) phase -= 1.;
		
	}
/*********** >END< ON ANIMATE ******************/


/*********** ON DRAW *******************/
	 virtual void onDraw(Graphics& g, const Viewpoint& v){
		g.blending(true);
		material();
		light();
		for (int i = 0; i < particles.size(); ++i){
			if (particles[i].alive == 0){ 
				continue;
			}
			if(particles[i].alive ==1){ 
				g.pushMatrix();
					g.color(HSV((float)i/N,drand48()*1.0,drand48()*1.0));
					g.translate(particles[i].position);
					g.draw(p);
				g.popMatrix();
				particles[i].memory = particles[i].alive;
			}	
		}

	}
/*********** >END< ON DRAW *******************/

	virtual void onSound(AudioIOData& io){while(io()){}}
	virtual void onKeyDown(const ViewpointWindow& w, const Keyboard& k){}
	virtual void onMouseDown(const ViewpointWindow& w, const Mouse& m){}
	virtual void onMouseDrag(const ViewpointWindow& w, const Mouse& m){}

	int calculate_neighbors(Vec3f center){
		std::vector<Vec3f> current_poses;
		current_poses = return_neighbors(center);
		int neighbors = 0;
		for (int i = 0; i < current_poses.size(); ++i){
			for (int j = 0; j < particles.size(); ++j){
				if (current_poses[i] == particles[j].position){
					neighbors += particles[j].alive;
				}
			}	
		}
		return neighbors;
	}

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

		// current_poses.push_back(Vec3f(x-1,y+1,z));
		// current_poses.push_back(Vec3f(x,y+1,z));
		// current_poses.push_back(Vec3f(x+1,y+1,z));
		// current_poses.push_back(Vec3f(x-1,y,z));
		// current_poses.push_back(Vec3f(x+1,y,z));
		// current_poses.push_back(Vec3f(x-1,y-1,z));
		// current_poses.push_back(Vec3f(x,y-1,z));
		// current_poses.push_back(Vec3f(x+1,y-1,z));

		return current_poses;
}