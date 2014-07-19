#include "allocore/al_Allocore.hpp"
#include "allocore/graphics/al_Isosurface.hpp"
#include "allocore/io/al_ControlNav.hpp"

using namespace al;

Graphics gl;
Light light;
Material material;
Lens lens;
Nav nav(Vec3d(0,0,5));
Stereographic stereo;

const int N = 8;
const float RADIUS = 1.7;
float GRID[N*N*N];
Isosurface iso;
double phase=0;
bool evolve = true;
bool wireframe = false;
bool icosahedron = false;

struct CELL{
	Vec3f pos;
	int alive;
};

class MySimulation : public Window, public Drawable{
public:

	 std::vector<CELL> cells;
	 std::vector<int> indices;
	 MySimulation(){
	 	cells.resize(N*N*N);
	 	int index = 0;
	 	for (int i = 0; i < N; ++i){
			for (int j = 0; j < N; ++j){
				for (int k = 0; k < N; ++k){
					cells[index].pos = Vec3f(i,j,k);
					cells[index].alive = index % 2 == 0 ? 1 : 0;
					indices.push_back(i*N*N + j*N + k);
					index++;
				}
			}
	 	}
	 }

	void onDraw(Graphics& gl){
		gl.depthTesting(1);
		light.dir(1,1,1);
		light.ambient(Color(1));
		material.useColorMaterial(true);
		material.ambient(Color(0.1,0,0));
		material.diffuse(Color(0.7,0,0));
		material.specular(HSV(0.1,1,0.7));
		material();		
		light();

		iso.level(1.);
		iso.generate(GRID, N, 1./N);

		if(wireframe)	gl.polygonMode(gl.LINE);
		else if (icosahedron) addIcosahedron(iso);
		else			gl.polygonMode(gl.FILL);
		for (int i = 0; i < cells.size(); ++i){
		gl.pushMatrix(gl.MODELVIEW);
			glEnable(GL_RESCALE_NORMAL);
			gl.color(HSV(cos(N*N)/N,0.8,0.3));
			gl.translate(Vec3f(rand()%5));
			gl.scale(rand()%5);
			gl.draw(iso);
		gl.popMatrix();
	}
	}

	bool onFrame(){
		nav.smooth(0.8);
		nav.step(1.);
		stereo.draw(gl, lens, nav, Viewport(1000, 800), *this);
		
		//std::cout << sphere << std::endl;
		 if(evolve){
			if((phase += drand48()*0.0009) > 2*M_PI) phase -= 2*M_PI;
			double sphere = (4.0/3.0) * M_PI*phase * pow(RADIUS,3);
			int index = 0;
			for(int k=0; k<N; ++k){ double z = double(k)/N * 6*M_PI;
			for(int j=0; j<N; ++j){ double y = double(j)/N * 6*M_PI;
			for(int i=0; i<N; ++i){ double x = double(i)/N * 6*M_PI;
				//tangent works nicely!
				Vec3f c(k,j,i);
				CELL current = find_cell(c);

				std::cout << current.alive << std::endl;
				GRID[index] = ( sin(x * sin(phase*7)) + sin(y * sin(phase*8)) + sin(z * sin(phase*9)) ) + sin(sphere);
				index++;
					//std::cout << "value: " << GRID[k*N*N + j*N + i] << std::endl;
					
			}}}
		 }

		return true;
	}

	CELL find_cell(Vec3f address){
		int index = 0;
		bool found = false;
		while (index < cells.size() && (!found)){
			if (cells[index].pos == address) found = true;
			else index++;
		}
		return cells[index];
	}

	virtual bool onKeyDown(const Keyboard& k){
		switch(k.key()){
		case 'f': wireframe^=1; return false;
		case 'i': icosahedron^=1; return false;
		case ' ': evolve^=1; return false;
		}
		return true;
	}

};

MySimulation sim;
	
int main(){
	iso.primitive(Graphics::TRIANGLES);

	sim.create(Window::Dim(1000,800), "Rob's Game of Life", 140);
	sim.add(new StandardWindowKeyControls);
	sim.add(new NavInputControl(nav));
	Window::startLoop();
}
