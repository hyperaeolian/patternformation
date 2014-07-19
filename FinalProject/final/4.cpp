#include "allocore/al_Allocore.hpp"
#include "allocore/graphics/al_Isosurface.hpp"
#include "allocore/io/al_ControlNav.hpp"
#include "allocore/io/al_App.hpp"
#include "Gamma/Oscillator.h"
#include "Gamma/SamplePlayer.h"


using namespace al;

Graphics gl;
Light light;
Material material;
Lens lens;
Nav nav(Vec3d(0,0,20));
Stereographic stereo;

const int N = 8;
const float RADIUS = 1.7;
const int WIN_HEIGHT = 1000, WIN_WIDTH = 800;
const int HOP_SIZE = 100;
const int WINDOW_LENGTH = 3000;
float GRID[N*N*N];
Isosurface iso;

double phase=0;
bool progressing = true;
bool wireframe = false;
bool tangent = false;


struct CELL{
	Vec3f pos;
	float alive;
};

class MySimulation : public Window, public Drawable{
public:

	 std::vector<CELL> cells;
	 std::vector<int> indices;
	 // std::vector<GRAIN> allGrains;
	 // gam::SamplePlayer<> samplePlayer;
	 // gam::SamplePlayer<float, gam::ipl::Cubic, gam::tap::Wrap> rmsPlayer, zcrPlayer;

	 MySimulation(){


	 	cells.resize(N*N*N);
	 	int index = 0;
	 	for (int i = 0; i < N; ++i){
			for (int j = 0; j < N; ++j){
				for (int k = 0; k < N; ++k){
					cells[index].pos = Vec3f(i,j,k);
					//else cells[index].pos = Vec3f(rand()%N,rand()%N,rand()%N);
					cells[index].alive = index % 4 == 0 ? drand48()*8.0 : drand48()*2.4;
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
		material.useColorMaterial(false);
		material.ambient(Color(0.1,0,0,1.0));
		material.diffuse(Color(0.7,0,0,1.0));
		material.specular(HSV(0.1,1,0.7));
		material();		
		light();

		iso.level(1.);
		iso.generate(GRID, N, 1./N);

		if(wireframe)	gl.polygonMode(gl.LINE);
		else			gl.polygonMode(gl.FILL);

		for (int i = 0; i < cells.size(); ++i){
			if (cells[i].alive <= 5.2) continue;
			else{
				gl.pushMatrix(gl.MODELVIEW);
					glEnable(GL_RESCALE_NORMAL);
					gl.color(HSV((float)i*i/N,1,al::fold(phase + i*0.5/8, 0.5)+0.5));
					gl.translate(cells[i].pos);
					gl.scale(2);
					gl.draw(iso);
				gl.popMatrix();
			
			}
		}
	}


	bool onFrame(){
		nav.smooth(0.8);
		nav.step(1.);
		stereo.draw(gl, lens, nav, Viewport(WIN_HEIGHT, WIN_WIDTH), *this);
		
		 if(progressing){ //0.0002
			if((phase += 0.0002) > 2*M_PI) phase -= 2*M_PI;
			double sphere = (4.0/3.0) * M_PI*phase * pow(RADIUS,3);
			int index = 0;
			for(int k=0; k<N; ++k){         double z = double(k)/N * 6*M_PI;
				for(int j=0; j<N; ++j){     double y = double(j)/N * 6*M_PI;
					for(int i=0; i<N; ++i){ double x = double(i)/N * 6*M_PI;

						CELL current  = find_cell(Vec3f(k,j,i));
						int neighbors = compute_neighbors(current.pos);
						//std::cout << "Neighbors: " << neighbors << std::endl;
						if (current.alive >= 3.0){
							current.alive = neighbors >= 3.5 && neighbors <= 4.2 ? 1.3 : current.alive-2.1;
						} else {
							current.alive = neighbors < 1.7 ? current.alive-1.3 : drand48()*10.3;
							//current.alive = symbiosis(current);
						}
						
						if (tangent) 
							GRID[indices[index]] = tan(x*phase*9) + tan(y*phase*8) + tan(z*phase*7);
						else
							//GRID[indices[index]] = cos(x*phase*9) + cos(y*phase*8) + cos(z*phase*7);

						GRID[index] = ( cos(x * cos(phase*cells[i].alive)) + cos(y * cos(phase*cells[i].alive)) + cos(z * cos(phase*cells[i].alive)) ) - cos(sphere);
						index++;
					
					}
				}		
			}
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

	float compute_neighbors(Vec3f center){
		CELL current = find_cell(center);
		float x = current.pos.x;
		float y = current.pos.y;
		float z = current.pos.z;
		std::vector<CELL> neighbors;
		float n = 0.0;
		neighbors.push_back(find_cell(Vec3f(x+1,y+1,z)));
		neighbors.push_back(find_cell(Vec3f(x+1,y,z)));
		neighbors.push_back(find_cell(Vec3f(x+1,y-1,z)));
		neighbors.push_back(find_cell(Vec3f(x,y+1,z)));
		neighbors.push_back(find_cell(Vec3f(x,y-1,z)));
		neighbors.push_back(find_cell(Vec3f(x-1,y,z)));
		neighbors.push_back(find_cell(Vec3f(x-1,y+1,z)));
		neighbors.push_back(find_cell(Vec3f(x-1,y-1,z)));
		for (int i = 0; i < neighbors.size(); ++i) n += neighbors[i].alive;
		std::cout << "N: " << n << std::endl;
		return n;
	}

	int symbiosis(CELL current){
		int x = current.pos.x;
		int y = current.pos.y;
		int z = current.pos.z;
		std::vector<CELL> current_poses;
		// neighbors.push_back(find_cell(Vec3f(x,y+1,z)));
		// neighbors.push_back(find_cell(Vec3f(x-1,y,z)));
		// neighbors.push_back(find_cell(Vec3f(x,y-1,z)));
		// neighbors.push_back(find_cell(Vec3f(x+1,y,z)));
		current_poses.push_back(find_cell(Vec3f(x-1,y-1,z)));
		current_poses.push_back(find_cell(Vec3f(x-1,y,z)));
		current_poses.push_back(find_cell(Vec3f(x-1,y+1,z)));
		current_poses.push_back(find_cell(Vec3f(x,y+1,z)));
		current_poses.push_back(find_cell(Vec3f(x+1,y+1,z)));
		current_poses.push_back(find_cell(Vec3f(x+1,y,z)));
		current_poses.push_back(find_cell(Vec3f(x+1,y-1,z)));
		current_poses.push_back(find_cell(Vec3f(x-1,y-1,z)));
		current_poses.push_back(find_cell(Vec3f(x-1,y-1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y,z+1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y+1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x,y+1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x+1,y+1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x+1,y,z+1)));
		current_poses.push_back(find_cell(Vec3f(x+1,y-1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x,y-1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y-1,z+1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y,z+1)));
		current_poses.push_back(find_cell(Vec3f(x,y,z+1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y-1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y,z-1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y+1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x,y+1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x+1,y+1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x+1,y,z-1)));
		current_poses.push_back(find_cell(Vec3f(x+1,y-1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x,y-1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y-1,z-1)));
		current_poses.push_back(find_cell(Vec3f(x-1,y,z-1)));
		current_poses.push_back(find_cell(Vec3f(x,y,z-1)));
		int total = 0;
		for (int i = 0; i < current_poses.size(); ++i) 
			total += current_poses[i].alive;
		if (total == 4) return 1;
		else return 0; 
	}

	virtual bool onKeyDown(const Keyboard& k){
		switch(k.key()){
		case 'f': wireframe^=1; return false;
		case 't': tangent^=1; return false;
		case ' ': progressing^=1; return false;
		}
		return true;
	}

};

MySimulation sim;
	
int main(){
	iso.primitive(Graphics::TRIANGLES);

	sim.create(Window::Dim(WIN_HEIGHT,WIN_WIDTH), "Rob's Game of Life", 140);
	sim.add(new StandardWindowKeyControls);
	sim.add(new NavInputControl(nav));
	Window::startLoop();
}
// struct GRAIN{
// 	int start;
// 	int stop;
// 	float RMS;
// 	int ZCR;
// };

	// void compute_RMS_ZCR(int totalSamples){
	// 	int itr = 0;
	// 	while ((HOP_SIZE * itr) + WINDOW_LENGTH <= totalSamples-1){
	// 		float sumOfSquares = 0;
	// 		int zcr = 0;
	// 		float prevSamp = 0;
	// 		for (int j = 0; j < WINDOW_LENGTH; ++j){
	// 			float s = samplePlayer[itr * HOP_SIZE + j];
	// 			sumOfSquares += s * s;
	// 			if (prevSamp*s < 0) zcr++;
	// 			prevSamp = s;
	// 		}
	// 		float rms = sqrt(sumOfSquares/WINDOW_LENGTH);
	// 		allGrains[itr].start = itr * HOP_SIZE;
	// 		allGrains[itr].stop = itr * HOP_SIZE + (WINDOW_LENGTH-1);
	// 		allGrains[itr].RMS = rms;
	// 		allGrains[itr].ZCR = zcr;
	// 		itr++;
	// 	}
	// }

// bool compareRMS(const GRAIN& one, const GRAIN& other){return one.RMS < other.RMS;}
// bool compareZCR(const GRAIN& one, const GRAIN& other){return one.ZCR < other.ZCR;}

// 	void audioCB(AudioIOData& io){
// 		gam::Sync::master().spu(al::App::audioIO().fps());
// 		while(io()){
// 			float s = samplePlayer();
// 			io.out(0) = io.out(1) = s;
// 		}
// }

	 	// SearchPaths searchPaths;
   //  	searchPaths.addAppPaths();
   //  	std::string soundFilePath = searchPaths.find("sohn.wav").filepath();
   //  	std::cout << "soundFilePath is " << soundFilePath << std::endl;
   //  	if (!samplePlayer.load(soundFilePath.c_str())) {
   //    		std::cerr << "FAIL: your sample did not load." << std::endl;
   //    		exit(1);
   //  	}

   //  	int totalSamples = samplePlayer.size();
   //  	allGrains.resize(totalSamples/HOP_SIZE);
   //  	float audioWin[WINDOW_LENGTH];
   //  	gam::tbl::window(audioWin, WINDOW_LENGTH, gam::HAMMING);

   //  	compute_RMS_ZCR(totalSamples);

   //  	sort(allGrains.begin(), allGrains.end(), compareRMS);
   //  	gam::Array<float> outArray;
   //  	outArray.resize(samplePlayer.size(), 0);

   //  	int itr = 0;
   //  	while ((HOP_SIZE * itr) + WINDOW_LENGTH <= totalSamples-1){
   //  		GRAIN& g = allGrains[itr];
   //  		int placement = itr * HOP_SIZE;
   //  		for (int j = 0; j < WINDOW_LENGTH; ++j){
   //  			outArray[placement + j] = outArray[placement + j] + 
   //  			(samplePlayer[g.start + j] * audioWin[j]);
   //  		}
   //  		itr++;
   //  	}
   //  	scaleFeature(outArray);
   //  	rmsPlayer.buffer(outArray, samplePlayer.frameRate(), 1);

   //  	sort(allGrains.begin(), allGrains.end(), compareZCR);
   //  	gam::Array<float> zcrArray;
   //  	zcrArray.resize(samplePlayer.size(), 0);

   //  	itr = 0;
   //  	while ((HOP_SIZE * itr) + WINDOW_LENGTH <= totalSamples-1){
   //  		GRAIN& g = allGrains[itr];
   //  		int placement = itr * HOP_SIZE;
   //  		for (int j = 0; j < WINDOW_LENGTH; ++j){
   //  			zcrArray[placement + j] = zcrArray[placement + j] + 
   //  			(samplePlayer[g.start + j] * audioWin[j]);
   //  		}
   //  		itr++;
   //  	}

   //  	scaleFeature(zcrArray);
   //  	zcrPlayer.buffer(zcrArray, samplePlayer.frameRate(), 1);

   //  	samplePlayer.phase(0.99999);
   //  	rmsPlayer.phase(0.99999);
   //  	zcrPlayer.phase(0.99999);
