/*
Semestre 2023-2
Proyecto final
*/
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

//variables para animación
float toffsetu = 0.0f;
float toffsetv = 0.0f;

float reproduciranimacion, habilitaranimacion,guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;

//variables de control de skybox
int momento_ciclo;
float skyboxTimeOffset = 0.0f;

// Animación compleja 1
int medusa_bandera = 1;
float medusa_X = 0;
float medusa_Y = 0;
float medusa_Z = 0;
float medusa_rot = 0;

// Animación compleja 2
float neritantan_rot = 0;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//MODELOS DEL PROYECTO 

Model habitat_pinguino;
Model escenario_temporal;
Model habitat_flamingo;
Model habitat_nutria;
Model entrada;
Model storage;
Model zoovenirs;
Model bancas;
Model contenedor;
Model bote_basura;
Model lampara;
Model mesas;
Model blazeReap;
Model brujula;
Model cuchillo;
Model faputa;
Model gaburoon;
Model neritantan;
Model planta;
Model reg;
Model reliquia;
Model roca;
Model medusa;
Model julien;

// Modelos skybox
Skybox skybox;
Skybox skybox_twilight;
Skybox skybox_night;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//PARA INPUT CON KEYFRAMES
void inputKeyframes(bool* keys);



//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;


//NEW// Keyframes
float posXJulien = 0.0f, posYJulien = -50.0f, posZJulien = 210.0f;
float	movJulien_x = 0.0f, movJulien_y = 0.0f, movJulien_z = 0.0f;
float giroJulienY = 0.0f, giroJulienX = 0.0f, giroJulienZ = 0.0f;

#define MAX_FRAMES 1000
int i_max_steps = 1000;
int i_curr_steps = 5;


typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movJulien_x;		//Variable para PosicionX
	float movJulien_y;		//Variable para PosicionY
	float movJulien_z;		//Variable para PosicionZ
	float movJulien_xInc;		//Variable para IncrementoX
	float movJulien_yInc;		//Variable para IncrementoY
	float movJulien_zInc;		//Variable para IncrementoZ
	float giroJulienY;
	float giroJulienYInc;
	float giroJulienX;
	float giroJulienXInc;
	float giroJulienZ;
	float giroJulienZInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];

int FrameIndex = 600;			//introducir datos
bool play = false;
int playIndex = 600;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movJulien_x = movJulien_x;
	KeyFrame[FrameIndex].movJulien_y = movJulien_y;
	KeyFrame[FrameIndex].movJulien_z = movJulien_z;
	KeyFrame[FrameIndex].giroJulienY = giroJulienY;
	KeyFrame[FrameIndex].giroJulienX = giroJulienX;
	KeyFrame[FrameIndex].giroJulienZ = giroJulienZ;

	FrameIndex++;
}

void resetElements(void)
{
	movJulien_x = KeyFrame[0].movJulien_x;
	movJulien_y = KeyFrame[0].movJulien_y;
	movJulien_z = KeyFrame[0].movJulien_z;
	giroJulienY = KeyFrame[0].giroJulienY;
	giroJulienX = KeyFrame[0].giroJulienX;
	giroJulienZ = KeyFrame[0].giroJulienZ;
}

void interpolation(void)
{
	KeyFrame[playIndex].movJulien_xInc = (KeyFrame[playIndex + 1].movJulien_x - KeyFrame[playIndex].movJulien_x) / i_max_steps;
	KeyFrame[playIndex].movJulien_yInc = (KeyFrame[playIndex + 1].movJulien_y - KeyFrame[playIndex].movJulien_y) / i_max_steps;
	KeyFrame[playIndex].movJulien_zInc = (KeyFrame[playIndex + 1].movJulien_z - KeyFrame[playIndex].movJulien_z) / i_max_steps;
	KeyFrame[playIndex].giroJulienYInc = (KeyFrame[playIndex + 1].giroJulienY - KeyFrame[playIndex].giroJulienY) / i_max_steps;
	KeyFrame[playIndex].giroJulienXInc = (KeyFrame[playIndex + 1].giroJulienX - KeyFrame[playIndex].giroJulienX) / i_max_steps;
	KeyFrame[playIndex].giroJulienZInc = (KeyFrame[playIndex + 1].giroJulienZ - KeyFrame[playIndex].giroJulienZ) / i_max_steps;
}


void animate(void)
{
	//Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			//printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				//printf("Frame index= %d\n", FrameIndex);
				//printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			movJulien_x += KeyFrame[playIndex].movJulien_xInc;
			movJulien_y += KeyFrame[playIndex].movJulien_yInc;
			movJulien_z += KeyFrame[playIndex].movJulien_zInc;
			giroJulienY += KeyFrame[playIndex].giroJulienYInc;
			giroJulienX += KeyFrame[playIndex].giroJulienXInc;
			giroJulienZ += KeyFrame[playIndex].giroJulienZInc;
			i_curr_steps++;
		}

	}
}



/* FIN KEYFRAMES*/


int main()
{
	mainWindow = Window(1960, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	habitat_pinguino = Model();
	habitat_pinguino.LoadModel("Obj/habitatPinguinos.obj");
	escenario_temporal = Model();
	escenario_temporal.LoadModel("Obj/escenario_temporal/terreno_temporal.obj");
	habitat_flamingo = Model();
	habitat_flamingo.LoadModel("Obj/habitat_flamingo.obj");
	habitat_nutria = Model();
	habitat_nutria.LoadModel("Obj/habitatNutrias.obj");
	entrada = Model();
	entrada.LoadModel("Obj/entrada_zoo.obj");
	storage = Model();
	storage.LoadModel("Obj/zoo_storage.obj");
	zoovenirs = Model();
	zoovenirs.LoadModel("Obj/tienda_zoovenirs.obj");
	bancas = Model();
	bancas.LoadModel("Obj/banca.obj");
	contenedor = Model();
	contenedor.LoadModel("Obj/contenedor.obj");
	bote_basura = Model();
	bote_basura.LoadModel("Obj/bote_basura.obj");
	lampara = Model();
	lampara.LoadModel("Obj/lampara.obj");
	mesas = Model();
	mesas.LoadModel("Obj/mesas.obj");
	julien = Model();
	julien.LoadModel("Obj/carro.obj");


	// Importacion de texturas Skybox
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cubemap_sky_h1.jpg");
	skyboxFaces.push_back("Textures/Skybox/cubemap_sky_h3.jpg");
	skyboxFaces.push_back("Textures/Skybox/cubemap_sky_dn.jpg");
	skyboxFaces.push_back("Textures/Skybox/cubemap_sky_up.jpg");
	skyboxFaces.push_back("Textures/Skybox/cubemap_sky_h0.jpg");
	skyboxFaces.push_back("Textures/Skybox/cubemap_sky_h2.jpg");

	skybox = Skybox(skyboxFaces);

	std::vector<std::string> skyboxFaces_Twilight;
	skyboxFaces_Twilight.push_back("Textures/Skybox/cubebox_twilight_h1.jpg");
	skyboxFaces_Twilight.push_back("Textures/Skybox/cubebox_twilight_h3.jpg");
	skyboxFaces_Twilight.push_back("Textures/Skybox/cubebox_twilight_dn.jpg");
	skyboxFaces_Twilight.push_back("Textures/Skybox/cubebox_twilight_up.jpg");
	skyboxFaces_Twilight.push_back("Textures/Skybox/cubebox_twilight_h0.jpg");
	skyboxFaces_Twilight.push_back("Textures/Skybox/cubebox_twilight_h2.jpg");

	skybox_twilight = Skybox(skyboxFaces_Twilight);

	std::vector<std::string> skyboxFaces_Night;
	skyboxFaces_Night.push_back("Textures/Skybox/cubemap_sky_night_h1.jpg");
	skyboxFaces_Night.push_back("Textures/Skybox/cubemap_sky_night_h3.jpg");
	skyboxFaces_Night.push_back("Textures/Skybox/cubemap_sky_night_dn.jpg");
	skyboxFaces_Night.push_back("Textures/Skybox/cubemap_sky_night_up.jpg");
	skyboxFaces_Night.push_back("Textures/Skybox/cubemap_sky_night_h0.jpg");
	skyboxFaces_Night.push_back("Textures/Skybox/cubemap_sky_night_h2.jpg");

	skybox_night = Skybox(skyboxFaces_Night);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	//Posición inicial del objeto animado por keyframes
	glm::vec3 posInicialObj1 = glm::vec3(0.0f, -50.0f, 210.0f);

	// ILUMINACIÓN
	// Luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.6f, 0.6f,
		0.0f, 0.0f, -1.0f);

	// Luces puntuales (en todas direcciones)
	unsigned int pointLightCount = 0;

	// foco 0
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		2.0f, 1.0f,
		45.0f, 15.0f, 33.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// foco 1
	pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
		2.0f, 1.0f,
		45.0f, 5.0f, 41.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	// foco 2
	pointLights[2] = PointLight(1.0f, 0.5f, 0.0f,
		2.0f, 1.0f,
		62.0f, 16.0f, 9.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int savePointLightCount = pointLightCount;

	// Lamparas (en una dirección)
	unsigned int spotLightCount = 0;

	// linterna del mouse
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//linterna piso 1
	spotLights[1] = SpotLight(0.0f, 1.0f, 1.0f,
		2.0f, 2.0f,
		10.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// linterna piso 2
	spotLights[2] = SpotLight(0.0f, 1.0f, 1.0f,
		2.0f, 2.0f,
		0.0f, 10.0f, -10.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;

	// linternas inician apagadas
	spotLightCount = 1;

	// control de lampara
	bool banderaLampara = true;

	glm::vec3 posJulien = glm::vec3(0.0f, -50.0f, 210.0f);
	glm::vec3 giroJulien = glm::vec3(0.0f, 0.0f, 0.0f);

	//KEYFRAMES DECLARADOS INICIALES
	KeyFrame[0].movJulien_x = 0.0f;
	KeyFrame[0].movJulien_y = 0.0f;
	KeyFrame[0].movJulien_z = 0.0f;
	KeyFrame[0].giroJulienX = 0.0f;
	KeyFrame[0].giroJulienY = 0.0f;
	KeyFrame[0].giroJulienZ = 0.0f;

	KeyFrame[1].movJulien_x = 0.0f;
	KeyFrame[1].movJulien_y = 0.0f;
	KeyFrame[1].movJulien_z = 0.0f;
	KeyFrame[1].giroJulienX = 0.0f;
	KeyFrame[1].giroJulienY = 0.0f;
	KeyFrame[1].giroJulienZ = 0.0f;

	KeyFrame[2].movJulien_x = 0.0f;
	KeyFrame[2].movJulien_y = 0.0f;
	KeyFrame[2].movJulien_z = 0.0f;
	KeyFrame[2].giroJulienX = 0.0f;
	KeyFrame[2].giroJulienY = 0.0f;
	KeyFrame[2].giroJulienZ = 0.0f;

	KeyFrame[3].movJulien_x = 0.0f;
	KeyFrame[3].movJulien_y = 0.0f;
	KeyFrame[3].movJulien_z = 0.0f;
	KeyFrame[3].giroJulienX = 0.0f;
	KeyFrame[3].giroJulienY = 0.0f;
	KeyFrame[3].giroJulienZ = 0.0f;

	KeyFrame[4].movJulien_x = 0.0f;
	KeyFrame[4].movJulien_y = 0.0f;
	KeyFrame[4].movJulien_z = 0.0f;
	KeyFrame[4].giroJulienX = 0.0f;
	KeyFrame[4].giroJulienY = 0.0f;
	KeyFrame[4].giroJulienZ = 0.0f;

	KeyFrame[5].movJulien_x = 0.0f;
	KeyFrame[5].movJulien_y = 0.0f;
	KeyFrame[5].movJulien_z = 0.0f;
	KeyFrame[5].giroJulienX = 0.0f;
	KeyFrame[5].giroJulienY = 0.0f;
	KeyFrame[5].giroJulienZ = 0.0f;

	KeyFrame[6].movJulien_x = 0.0f;
	KeyFrame[6].movJulien_y = 0.0f;
	KeyFrame[6].movJulien_z = 0.0f;
	KeyFrame[6].giroJulienX = 0.0f;
	KeyFrame[6].giroJulienY = 0.0f;
	KeyFrame[6].giroJulienZ = 0.0f;

	KeyFrame[7].movJulien_x = 0.0f;
	KeyFrame[7].movJulien_y = 0.0f;
	KeyFrame[7].movJulien_z = 0.0f;
	KeyFrame[7].giroJulienX = 0.0f;
	KeyFrame[7].giroJulienY = 0.0f;
	KeyFrame[7].giroJulienZ = 0.0f;

	KeyFrame[8].movJulien_x = 0.0f;
	KeyFrame[8].movJulien_y = 0.0f;
	KeyFrame[8].movJulien_z = 0.0f;
	KeyFrame[8].giroJulienX = 0.0f;
	KeyFrame[8].giroJulienY = 0.0f;
	KeyFrame[8].giroJulienZ = 0.0f;

	KeyFrame[9].movJulien_x = 0.0f;
	KeyFrame[9].movJulien_y = 0.0f;
	KeyFrame[9].movJulien_z = 0.0f;
	KeyFrame[9].giroJulienX = 0.0f;
	KeyFrame[9].giroJulienY = 0.0f;
	KeyFrame[9].giroJulienZ = 0.0f;

	KeyFrame[10].movJulien_x = 0.0f;
	KeyFrame[10].movJulien_y = 0.0f;
	KeyFrame[10].movJulien_z = 0.0f;
	KeyFrame[10].giroJulienX = 0.0f;
	KeyFrame[10].giroJulienY = 0.0f;
	KeyFrame[10].giroJulienZ = 0.0f;

	//KEYFRAMES GENERADOS

	KeyFrame[11].movJulien_x = 0.000000;
	KeyFrame[11].movJulien_y = 0.000000;
	KeyFrame[11].movJulien_z = -20.000000;
	KeyFrame[11].giroJulienX = 0.000000;
	KeyFrame[11].giroJulienY = 0.000000;
	KeyFrame[11].giroJulienZ = 0.000000;

	KeyFrame[12].movJulien_y = 0.000000;
	KeyFrame[12].movJulien_z = -20.000000;
	KeyFrame[12].giroJulienX = 0.000000;
	KeyFrame[12].giroJulienY = 0.000000;
	KeyFrame[12].giroJulienZ = 0.000000;

	KeyFrame[13].movJulien_x = 0.000000;
	KeyFrame[13].movJulien_y = 0.000000;
	KeyFrame[13].movJulien_z = -20.000000;
	KeyFrame[13].giroJulienX = 0.000000;
	KeyFrame[13].giroJulienY = 0.000000;
	KeyFrame[13].giroJulienZ = 0.000000;

	KeyFrame[14].movJulien_x = 0.000000;
	KeyFrame[14].movJulien_y = 0.000000;
	KeyFrame[14].movJulien_z = 0.000000;
	KeyFrame[14].giroJulienX = 20.000000;
	KeyFrame[14].giroJulienY = 0.000000;
	KeyFrame[14].giroJulienZ = 0.000000;

	KeyFrame[15].movJulien_x = 0.000000;
	KeyFrame[15].movJulien_y = 0.000000;
	KeyFrame[15].movJulien_z = 0.000000;
	KeyFrame[15].giroJulienX = 20.000000;
	KeyFrame[15].giroJulienY = 0.000000;
	KeyFrame[15].giroJulienZ = 0.000000;

	KeyFrame[16].movJulien_x = 0.000000;
	KeyFrame[16].movJulien_y = 0.000000;
	KeyFrame[16].movJulien_z = 0.000000;
	KeyFrame[16].giroJulienX = 20.000000;
	KeyFrame[16].giroJulienY = 0.000000;
	KeyFrame[16].giroJulienZ = 0.000000;

	KeyFrame[17].movJulien_x = 0.000000;
	KeyFrame[17].movJulien_y = 0.000000;
	KeyFrame[17].movJulien_z = 0.000000;
	KeyFrame[17].giroJulienX = 20.000000;
	KeyFrame[17].giroJulienY = 0.000000;
	KeyFrame[17].giroJulienZ = 0.000000;

	KeyFrame[18].movJulien_x = 0.000000;
	KeyFrame[18].movJulien_y = 0.000000;
	KeyFrame[18].movJulien_z = 0.000000;
	KeyFrame[18].giroJulienX = 20.000000;
	KeyFrame[18].giroJulienY = 0.000000;
	KeyFrame[18].giroJulienZ = 0.000000;

	KeyFrame[19].movJulien_x = 0.000000;
	KeyFrame[19].movJulien_y = 0.000000;
	KeyFrame[19].movJulien_z = 0.000000;
	KeyFrame[19].giroJulienX = 20.000000;
	KeyFrame[19].giroJulienY = 0.000000;
	KeyFrame[19].giroJulienZ = 0.000000;

	KeyFrame[20].movJulien_x = 0.000000;
	KeyFrame[20].movJulien_y = 0.000000;
	KeyFrame[20].movJulien_z = 0.000000;
	KeyFrame[20].giroJulienX = 20.000000;
	KeyFrame[20].giroJulienY = 0.000000;
	KeyFrame[20].giroJulienZ = 0.000000;

	KeyFrame[21].movJulien_x = 0.000000;
	KeyFrame[21].movJulien_y = 0.000000;
	KeyFrame[21].movJulien_z = 0.000000;
	KeyFrame[21].giroJulienX = 40.000000;
	KeyFrame[21].giroJulienY = 0.000000;
	KeyFrame[21].giroJulienZ = 0.000000;

	KeyFrame[22].movJulien_x = 0.000000;
	KeyFrame[22].movJulien_y = 0.000000;
	KeyFrame[22].movJulien_z = 0.000000;
	KeyFrame[22].giroJulienX = 40.000000;
	KeyFrame[22].giroJulienY = 0.000000;
	KeyFrame[22].giroJulienZ = 0.000000;

	KeyFrame[23].movJulien_x = 0.000000;
	KeyFrame[23].movJulien_y = 0.000000;
	KeyFrame[23].movJulien_z = 0.000000;
	KeyFrame[23].giroJulienX = 40.000000;
	KeyFrame[23].giroJulienY = 0.000000;
	KeyFrame[23].giroJulienZ = 0.000000;

	KeyFrame[24].movJulien_x = 0.000000;
	KeyFrame[24].movJulien_y = 0.000000;
	KeyFrame[24].movJulien_z = 0.000000;
	KeyFrame[24].giroJulienX = 40.000000;
	KeyFrame[24].giroJulienY = 0.000000;
	KeyFrame[24].giroJulienZ = 0.000000;

	KeyFrame[25].movJulien_x = 0.000000;
	KeyFrame[25].movJulien_y = 0.000000;
	KeyFrame[25].movJulien_z = 0.000000;
	KeyFrame[25].giroJulienX = 40.000000;
	KeyFrame[25].giroJulienY = 0.000000;
	KeyFrame[25].giroJulienZ = 0.000000;

	KeyFrame[26].movJulien_x = 0.000000;
	KeyFrame[26].movJulien_y = 0.000000;
	KeyFrame[26].movJulien_z = 0.000000;
	KeyFrame[26].giroJulienX = 40.000000;
	KeyFrame[26].giroJulienY = 0.000000;
	KeyFrame[26].giroJulienZ = 0.000000;

	KeyFrame[27].movJulien_x = 0.000000;
	KeyFrame[27].movJulien_y = 0.000000;
	KeyFrame[27].movJulien_z = 0.000000;
	KeyFrame[27].giroJulienX = 60.000000;
	KeyFrame[27].giroJulienY = 0.000000;
	KeyFrame[27].giroJulienZ = 0.000000;

	KeyFrame[28].movJulien_x = 0.000000;
	KeyFrame[28].movJulien_y = 0.000000;
	KeyFrame[28].movJulien_z = 0.000000;
	KeyFrame[28].giroJulienX = 80.000000;
	KeyFrame[28].giroJulienY = 0.000000;
	KeyFrame[28].giroJulienZ = 0.000000;

	KeyFrame[29].movJulien_x = 20.000000;
	KeyFrame[29].movJulien_y = 0.000000;
	KeyFrame[29].movJulien_z = 0.000000;
	KeyFrame[29].giroJulienX = 100.000000;
	KeyFrame[29].giroJulienY = 0.000000;
	KeyFrame[29].giroJulienZ = 0.000000;

	KeyFrame[30].movJulien_x = 40.000000;
	KeyFrame[30].movJulien_y = 0.000000;
	KeyFrame[30].movJulien_z = 0.000000;
	KeyFrame[30].giroJulienX = 100.000000;
	KeyFrame[30].giroJulienY = 0.000000;
	KeyFrame[30].giroJulienZ = 0.000000;

	KeyFrame[31].movJulien_x = 60.000000;
	KeyFrame[31].movJulien_y = 0.000000;
	KeyFrame[31].movJulien_z = 0.000000;
	KeyFrame[31].giroJulienX = 100.000000;
	KeyFrame[31].giroJulienY = 0.000000;
	KeyFrame[31].giroJulienZ = 0.000000;

	KeyFrame[32].movJulien_x = 80.000000;
	KeyFrame[32].movJulien_y = 0.000000;
	KeyFrame[32].movJulien_z = 0.000000;
	KeyFrame[32].giroJulienX = 100.000000;
	KeyFrame[32].giroJulienY = 0.000000;
	KeyFrame[32].giroJulienZ = 0.000000;

	KeyFrame[33].movJulien_x = 100.000000;
	KeyFrame[33].movJulien_y = 0.000000;
	KeyFrame[33].movJulien_z = 0.000000;
	KeyFrame[33].giroJulienX = 100.000000;
	KeyFrame[33].giroJulienY = 0.000000;
	KeyFrame[33].giroJulienZ = 0.000000;

	KeyFrame[34].movJulien_x = 120.000000;
	KeyFrame[34].movJulien_y = 0.000000;
	KeyFrame[34].movJulien_z = 0.000000;
	KeyFrame[34].giroJulienX = 100.000000;
	KeyFrame[34].giroJulienY = 0.000000;
	KeyFrame[34].giroJulienZ = 0.000000;

	KeyFrame[35].movJulien_x = 140.000000;
	KeyFrame[35].movJulien_y = 0.000000;
	KeyFrame[35].movJulien_z = 0.000000;
	KeyFrame[35].giroJulienX = 100.000000;
	KeyFrame[35].giroJulienY = 0.000000;
	KeyFrame[35].giroJulienZ = 0.000000;

	KeyFrame[36].movJulien_x = 160.000000;
	KeyFrame[36].movJulien_y = 0.000000;
	KeyFrame[36].movJulien_z = 0.000000;
	KeyFrame[36].giroJulienX = 100.000000;
	KeyFrame[36].giroJulienY = 0.000000;
	KeyFrame[36].giroJulienZ = 0.000000;

	KeyFrame[37].movJulien_x = 180.000000;
	KeyFrame[37].movJulien_y = 0.000000;
	KeyFrame[37].movJulien_z = 0.000000;
	KeyFrame[37].giroJulienX = 100.000000;
	KeyFrame[37].giroJulienY = 0.000000;
	KeyFrame[37].giroJulienZ = 0.000000;

	KeyFrame[38].movJulien_x = 180.000000;
	KeyFrame[38].movJulien_y = 0.000000;
	KeyFrame[38].movJulien_z = 0.000000;
	KeyFrame[38].giroJulienX = 100.000000;
	KeyFrame[38].giroJulienY = 0.000000;
	KeyFrame[38].giroJulienZ = 0.000000;

	KeyFrame[39].movJulien_x = 200.000000;
	KeyFrame[39].movJulien_y = 0.000000;
	KeyFrame[39].movJulien_z = 0.000000;
	KeyFrame[39].giroJulienX = 100.000000;
	KeyFrame[39].giroJulienY = 0.000000;
	KeyFrame[39].giroJulienZ = 0.000000;

	KeyFrame[40].movJulien_x = 220.000000;
	KeyFrame[40].movJulien_y = 0.000000;
	KeyFrame[40].movJulien_z = 0.000000;
	KeyFrame[40].giroJulienX = 100.000000;
	KeyFrame[40].giroJulienY = 0.000000;
	KeyFrame[40].giroJulienZ = 0.000000;

	KeyFrame[41].movJulien_x = 220.000000;
	KeyFrame[41].movJulien_y = 0.000000;
	KeyFrame[41].movJulien_z = 0.000000;
	KeyFrame[41].giroJulienX = 100.000000;
	KeyFrame[41].giroJulienY = 0.000000;
	KeyFrame[41].giroJulienZ = 0.000000;

	KeyFrame[42].movJulien_x = 240.000000;
	KeyFrame[42].movJulien_y = 0.000000;
	KeyFrame[42].movJulien_z = 0.000000;
	KeyFrame[42].giroJulienX = 100.000000;
	KeyFrame[42].giroJulienY = 0.000000;
	KeyFrame[42].giroJulienZ = 0.000000;

	KeyFrame[43].movJulien_x = 240.000000;
	KeyFrame[43].movJulien_y = 0.000000;
	KeyFrame[43].movJulien_z = 0.000000;
	KeyFrame[43].giroJulienX = 100.000000;
	KeyFrame[43].giroJulienY = 0.000000;
	KeyFrame[43].giroJulienZ = 0.000000;

	KeyFrame[44].movJulien_x = 240.000000;
	KeyFrame[44].movJulien_y = 0.000000;
	KeyFrame[44].movJulien_z = 0.000000;
	KeyFrame[44].giroJulienX = 100.000000;
	KeyFrame[44].giroJulienY = 0.000000;
	KeyFrame[44].giroJulienZ = 0.000000;

	KeyFrame[45].movJulien_x = 240.000000;
	KeyFrame[45].movJulien_y = 0.000000;
	KeyFrame[45].movJulien_z = 0.000000;
	KeyFrame[45].giroJulienX = 120.000000;
	KeyFrame[45].giroJulienY = 0.000000;
	KeyFrame[45].giroJulienZ = 0.000000;

	KeyFrame[46].movJulien_x = 240.000000;
	KeyFrame[46].movJulien_y = 0.000000;
	KeyFrame[46].movJulien_z = 0.000000;
	KeyFrame[46].giroJulienX = 120.000000;
	KeyFrame[46].giroJulienY = 0.000000;
	KeyFrame[46].giroJulienZ = 0.000000;

	KeyFrame[47].movJulien_x = 240.000000;
	KeyFrame[47].movJulien_y = 0.000000;
	KeyFrame[47].movJulien_z = 0.000000;
	KeyFrame[47].giroJulienX = 120.000000;
	KeyFrame[47].giroJulienY = 0.000000;
	KeyFrame[47].giroJulienZ = 0.000000;

	KeyFrame[48].movJulien_x = 240.000000;
	KeyFrame[48].movJulien_y = 0.000000;
	KeyFrame[48].movJulien_z = 0.000000;
	KeyFrame[48].giroJulienX = 120.000000;
	KeyFrame[48].giroJulienY = 0.000000;
	KeyFrame[48].giroJulienZ = 0.000000;

	KeyFrame[49].movJulien_x = 240.000000;
	KeyFrame[49].movJulien_y = 0.000000;
	KeyFrame[49].movJulien_z = 0.000000;
	KeyFrame[49].giroJulienX = 120.000000;
	KeyFrame[49].giroJulienY = 0.000000;
	KeyFrame[49].giroJulienZ = 0.000000;

	KeyFrame[50].movJulien_x = 240.000000;
	KeyFrame[50].movJulien_y = 0.000000;
	KeyFrame[50].movJulien_z = 0.000000;
	KeyFrame[50].giroJulienX = 120.000000;
	KeyFrame[50].giroJulienY = 0.000000;
	KeyFrame[50].giroJulienZ = 0.000000;

	KeyFrame[51].movJulien_x = 240.000000;
	KeyFrame[51].movJulien_y = 0.000000;
	KeyFrame[51].movJulien_z = 0.000000;
	KeyFrame[51].giroJulienX = 140.000000;
	KeyFrame[51].giroJulienY = 0.000000;
	KeyFrame[51].giroJulienZ = 0.000000;

	KeyFrame[52].movJulien_x = 240.000000;
	KeyFrame[52].movJulien_y = 0.000000;
	KeyFrame[52].movJulien_z = 0.000000;
	KeyFrame[52].giroJulienX = 160.000000;
	KeyFrame[52].giroJulienY = 0.000000;
	KeyFrame[52].giroJulienZ = 0.000000;

	KeyFrame[53].movJulien_x = 240.000000;
	KeyFrame[53].movJulien_y = 0.000000;
	KeyFrame[53].movJulien_z = 0.000000;
	KeyFrame[53].giroJulienX = 180.000000;
	KeyFrame[53].giroJulienY = 0.000000;
	KeyFrame[53].giroJulienZ = 0.000000;

	KeyFrame[54].movJulien_x = 240.000000;
	KeyFrame[54].movJulien_y = 0.000000;
	KeyFrame[54].movJulien_z = -20.000000;
	KeyFrame[54].giroJulienX = 180.000000;
	KeyFrame[54].giroJulienY = 0.000000;
	KeyFrame[54].giroJulienZ = 0.000000;

	KeyFrame[55].movJulien_x = 240.000000;
	KeyFrame[55].movJulien_y = 0.000000;
	KeyFrame[55].movJulien_z = -20.000000;
	KeyFrame[55].giroJulienX = 180.000000;
	KeyFrame[55].giroJulienY = 0.000000;
	KeyFrame[55].giroJulienZ = 0.000000;

	KeyFrame[56].movJulien_x = 240.000000;
	KeyFrame[56].movJulien_y = 0.000000;
	KeyFrame[56].movJulien_z = -20.000000;
	KeyFrame[56].giroJulienX = 180.000000;
	KeyFrame[56].giroJulienY = 0.000000;
	KeyFrame[56].giroJulienZ = 0.000000;

	KeyFrame[57].movJulien_x = 240.000000;
	KeyFrame[57].movJulien_y = 0.000000;
	KeyFrame[57].movJulien_z = -20.000000;
	KeyFrame[57].giroJulienX = 180.000000;
	KeyFrame[57].giroJulienY = 0.000000;
	KeyFrame[57].giroJulienZ = 0.000000;

	KeyFrame[58].movJulien_x = 240.000000;
	KeyFrame[58].movJulien_y = 0.000000;
	KeyFrame[58].movJulien_z = -20.000000;
	KeyFrame[58].giroJulienX = 180.000000;
	KeyFrame[58].giroJulienY = 0.000000;
	KeyFrame[58].giroJulienZ = 0.000000;

	KeyFrame[59].movJulien_x = 240.000000;
	KeyFrame[59].movJulien_y = 0.000000;
	KeyFrame[59].movJulien_z = -20.000000;
	KeyFrame[59].giroJulienX = 180.000000;
	KeyFrame[59].giroJulienY = 0.000000;
	KeyFrame[59].giroJulienZ = 0.000000;

	KeyFrame[60].movJulien_x = 240.000000;
	KeyFrame[60].movJulien_y = 0.000000;
	KeyFrame[60].movJulien_z = -40.000000;
	KeyFrame[60].giroJulienX = 180.000000;
	KeyFrame[60].giroJulienY = 0.000000;
	KeyFrame[60].giroJulienZ = 0.000000;

	KeyFrame[61].movJulien_x = 240.000000;
	KeyFrame[61].movJulien_y = 0.000000;
	KeyFrame[61].movJulien_z = -60.000000;
	KeyFrame[61].giroJulienX = 180.000000;
	KeyFrame[61].giroJulienY = 0.000000;
	KeyFrame[61].giroJulienZ = 0.000000;

	KeyFrame[62].movJulien_x = 240.000000;
	KeyFrame[62].movJulien_y = 0.000000;
	KeyFrame[62].movJulien_z = -80.000000;
	KeyFrame[62].giroJulienX = 180.000000;
	KeyFrame[62].giroJulienY = 0.000000;
	KeyFrame[62].giroJulienZ = 0.000000;

	KeyFrame[63].movJulien_x = 240.000000;
	KeyFrame[63].movJulien_y = 0.000000;
	KeyFrame[63].movJulien_z = -80.000000;
	KeyFrame[63].giroJulienX = 180.000000;
	KeyFrame[63].giroJulienY = 0.000000;
	KeyFrame[63].giroJulienZ = 0.000000;

	KeyFrame[64].movJulien_x = 240.000000;
	KeyFrame[64].movJulien_y = 0.000000;
	KeyFrame[64].movJulien_z = -80.000000;
	KeyFrame[64].giroJulienX = 180.000000;
	KeyFrame[64].giroJulienY = 0.000000;
	KeyFrame[64].giroJulienZ = 0.000000;

	KeyFrame[65].movJulien_x = 240.000000;
	KeyFrame[65].movJulien_y = 0.000000;
	KeyFrame[65].movJulien_z = -80.000000;
	KeyFrame[65].giroJulienX = 180.000000;
	KeyFrame[65].giroJulienY = 0.000000;
	KeyFrame[65].giroJulienZ = 0.000000;

	KeyFrame[66].movJulien_x = 240.000000;
	KeyFrame[66].movJulien_y = 0.000000;
	KeyFrame[66].movJulien_z = -80.000000;
	KeyFrame[66].giroJulienX = 180.000000;
	KeyFrame[66].giroJulienY = 0.000000;
	KeyFrame[66].giroJulienZ = 0.000000;

	KeyFrame[67].movJulien_x = 240.000000;
	KeyFrame[67].movJulien_y = 0.000000;
	KeyFrame[67].movJulien_z = -100.000000;
	KeyFrame[67].giroJulienX = 180.000000;
	KeyFrame[67].giroJulienY = 0.000000;
	KeyFrame[67].giroJulienZ = 0.000000;

	KeyFrame[68].movJulien_x = 240.000000;
	KeyFrame[68].movJulien_y = 0.000000;
	KeyFrame[68].movJulien_z = -100.000000;
	KeyFrame[68].giroJulienX = 180.000000;
	KeyFrame[68].giroJulienY = 0.000000;
	KeyFrame[68].giroJulienZ = 0.000000;

	KeyFrame[69].movJulien_x = 240.000000;
	KeyFrame[69].movJulien_y = 0.000000;
	KeyFrame[69].movJulien_z = -100.000000;
	KeyFrame[69].giroJulienX = 180.000000;
	KeyFrame[69].giroJulienY = 0.000000;
	KeyFrame[69].giroJulienZ = 0.000000;

	KeyFrame[70].movJulien_x = 240.000000;
	KeyFrame[70].movJulien_y = 0.000000;
	KeyFrame[70].movJulien_z = -120.000000;
	KeyFrame[70].giroJulienX = 180.000000;
	KeyFrame[70].giroJulienY = 0.000000;
	KeyFrame[70].giroJulienZ = 0.000000;

	KeyFrame[71].movJulien_x = 240.000000;
	KeyFrame[71].movJulien_y = 0.000000;
	KeyFrame[71].movJulien_z = -140.000000;
	KeyFrame[71].giroJulienX = 180.000000;
	KeyFrame[71].giroJulienY = 0.000000;
	KeyFrame[71].giroJulienZ = 0.000000;

	KeyFrame[72].movJulien_x = 240.000000;
	KeyFrame[72].movJulien_y = 0.000000;
	KeyFrame[72].movJulien_z = -160.000000;
	KeyFrame[72].giroJulienX = 180.000000;
	KeyFrame[72].giroJulienY = 0.000000;
	KeyFrame[72].giroJulienZ = 0.000000;

	KeyFrame[73].movJulien_x = 240.000000;
	KeyFrame[73].movJulien_y = 0.000000;
	KeyFrame[73].movJulien_z = -180.000000;
	KeyFrame[73].giroJulienX = 180.000000;
	KeyFrame[73].giroJulienY = 0.000000;
	KeyFrame[73].giroJulienZ = 0.000000;

	KeyFrame[74].movJulien_x = 240.000000;
	KeyFrame[74].movJulien_y = 0.000000;
	KeyFrame[74].movJulien_z = -200.000000;
	KeyFrame[74].giroJulienX = 180.000000;
	KeyFrame[74].giroJulienY = 0.000000;
	KeyFrame[74].giroJulienZ = 0.000000;

	KeyFrame[75].movJulien_x = 240.000000;
	KeyFrame[75].movJulien_y = 0.000000;
	KeyFrame[75].movJulien_z = -220.000000;
	KeyFrame[75].giroJulienX = 180.000000;
	KeyFrame[75].giroJulienY = 0.000000;
	KeyFrame[75].giroJulienZ = 0.000000;

	KeyFrame[76].movJulien_x = 240.000000;
	KeyFrame[76].movJulien_y = 0.000000;
	KeyFrame[76].movJulien_z = -240.000000;
	KeyFrame[76].giroJulienX = 180.000000;
	KeyFrame[76].giroJulienY = 0.000000;
	KeyFrame[76].giroJulienZ = 0.000000;

	KeyFrame[77].movJulien_x = 240.000000;
	KeyFrame[77].movJulien_y = 0.000000;
	KeyFrame[77].movJulien_z = -280.000000;
	KeyFrame[77].giroJulienX = 180.000000;
	KeyFrame[77].giroJulienY = 0.000000;
	KeyFrame[77].giroJulienZ = 0.000000;

	KeyFrame[78].movJulien_x = 240.000000;
	KeyFrame[78].movJulien_y = 0.000000;
	KeyFrame[78].movJulien_z = -300.000000;
	KeyFrame[78].giroJulienX = 180.000000;
	KeyFrame[78].giroJulienY = 0.000000;
	KeyFrame[78].giroJulienZ = 0.000000;

	KeyFrame[79].movJulien_x = 240.000000;
	KeyFrame[79].movJulien_y = 0.000000;
	KeyFrame[79].movJulien_z = -320.000000;
	KeyFrame[79].giroJulienX = 180.000000;
	KeyFrame[79].giroJulienY = 0.000000;
	KeyFrame[79].giroJulienZ = 0.000000;

	KeyFrame[80].movJulien_x = 240.000000;
	KeyFrame[80].movJulien_y = 0.000000;
	KeyFrame[80].movJulien_z = -320.000000;
	KeyFrame[80].giroJulienX = 180.000000;
	KeyFrame[80].giroJulienY = 0.000000;
	KeyFrame[80].giroJulienZ = 0.000000;

	KeyFrame[81].movJulien_x = 240.000000;
	KeyFrame[81].movJulien_y = 0.000000;
	KeyFrame[81].movJulien_z = -320.000000;
	KeyFrame[81].giroJulienX = 180.000000;
	KeyFrame[81].giroJulienY = 0.000000;
	KeyFrame[81].giroJulienZ = 0.000000;

	KeyFrame[82].movJulien_x = 240.000000;
	KeyFrame[82].movJulien_y = 0.000000;
	KeyFrame[82].movJulien_z = -320.000000;
	KeyFrame[82].giroJulienX = 180.000000;
	KeyFrame[82].giroJulienY = 0.000000;
	KeyFrame[82].giroJulienZ = 0.000000;

	KeyFrame[83].movJulien_x = 240.000000;
	KeyFrame[83].movJulien_y = 0.000000;
	KeyFrame[83].movJulien_z = -320.000000;
	KeyFrame[83].giroJulienX = 180.000000;
	KeyFrame[83].giroJulienY = 0.000000;
	KeyFrame[83].giroJulienZ = 0.000000;

	KeyFrame[84].movJulien_x = 240.000000;
	KeyFrame[84].movJulien_y = 0.000000;
	KeyFrame[84].movJulien_z = -340.000000;
	KeyFrame[84].giroJulienX = 180.000000;
	KeyFrame[84].giroJulienY = 0.000000;
	KeyFrame[84].giroJulienZ = 0.000000;

	KeyFrame[85].movJulien_x = 240.000000;
	KeyFrame[85].movJulien_y = 0.000000;
	KeyFrame[85].movJulien_z = -340.000000;
	KeyFrame[85].giroJulienX = 180.000000;
	KeyFrame[85].giroJulienY = 0.000000;
	KeyFrame[85].giroJulienZ = 0.000000;

	KeyFrame[86].movJulien_x = 240.000000;
	KeyFrame[86].movJulien_y = 0.000000;
	KeyFrame[86].movJulien_z = -340.000000;
	KeyFrame[86].giroJulienX = 180.000000;
	KeyFrame[86].giroJulienY = 0.000000;
	KeyFrame[86].giroJulienZ = 0.000000;

	KeyFrame[87].movJulien_x = 240.000000;
	KeyFrame[87].movJulien_y = 0.000000;
	KeyFrame[87].movJulien_z = -360.000000;
	KeyFrame[87].giroJulienX = 180.000000;
	KeyFrame[87].giroJulienY = 0.000000;
	KeyFrame[87].giroJulienZ = 0.000000;

	KeyFrame[88].movJulien_x = 240.000000;
	KeyFrame[88].movJulien_y = 0.000000;
	KeyFrame[88].movJulien_z = -360.000000;
	KeyFrame[88].giroJulienX = 180.000000;
	KeyFrame[88].giroJulienY = 0.000000;
	KeyFrame[88].giroJulienZ = 0.000000;

	KeyFrame[89].movJulien_x = 240.000000;
	KeyFrame[89].movJulien_y = 0.000000;
	KeyFrame[89].movJulien_z = -360.000000;
	KeyFrame[89].giroJulienX = 180.000000;
	KeyFrame[89].giroJulienY = 0.000000;
	KeyFrame[89].giroJulienZ = 0.000000;

	KeyFrame[90].movJulien_x = 240.000000;
	KeyFrame[90].movJulien_y = 0.000000;
	KeyFrame[90].movJulien_z = -360.000000;
	KeyFrame[90].giroJulienX = 180.000000;
	KeyFrame[90].giroJulienY = 0.000000;
	KeyFrame[90].giroJulienZ = 0.000000;

	KeyFrame[91].movJulien_x = 240.000000;
	KeyFrame[91].movJulien_y = 0.000000;
	KeyFrame[91].movJulien_z = -360.000000;
	KeyFrame[91].giroJulienX = 180.000000;
	KeyFrame[91].giroJulienY = 0.000000;
	KeyFrame[91].giroJulienZ = 0.000000;

	KeyFrame[92].movJulien_x = 240.000000;
	KeyFrame[92].movJulien_y = 0.000000;
	KeyFrame[92].movJulien_z = -380.000000;
	KeyFrame[92].giroJulienX = 180.000000;
	KeyFrame[92].giroJulienY = 0.000000;
	KeyFrame[92].giroJulienZ = 0.000000;

	KeyFrame[93].movJulien_x = 240.000000;
	KeyFrame[93].movJulien_y = 0.000000;
	KeyFrame[93].movJulien_z = -380.000000;
	KeyFrame[93].giroJulienX = 180.000000;
	KeyFrame[93].giroJulienY = 0.000000;
	KeyFrame[93].giroJulienZ = 0.000000;

	KeyFrame[94].movJulien_x = 240.000000;
	KeyFrame[94].movJulien_y = 0.000000;
	KeyFrame[94].movJulien_z = -380.000000;
	KeyFrame[94].giroJulienX = 180.000000;
	KeyFrame[94].giroJulienY = 0.000000;
	KeyFrame[94].giroJulienZ = 0.000000;

	KeyFrame[95].movJulien_x = 240.000000;
	KeyFrame[95].movJulien_y = 0.000000;
	KeyFrame[95].movJulien_z = -400.000000;
	KeyFrame[95].giroJulienX = 180.000000;
	KeyFrame[95].giroJulienY = 0.000000;
	KeyFrame[95].giroJulienZ = 0.000000;

	KeyFrame[96].movJulien_x = 240.000000;
	KeyFrame[96].movJulien_y = 0.000000;
	KeyFrame[96].movJulien_z = -400.000000;
	KeyFrame[96].giroJulienX = 180.000000;
	KeyFrame[96].giroJulienY = 0.000000;
	KeyFrame[96].giroJulienZ = 0.000000;

	KeyFrame[97].movJulien_x = 240.000000;
	KeyFrame[97].movJulien_y = 0.000000;
	KeyFrame[97].movJulien_z = -400.000000;
	KeyFrame[97].giroJulienX = 180.000000;
	KeyFrame[97].giroJulienY = 0.000000;
	KeyFrame[97].giroJulienZ = 0.000000;

	KeyFrame[98].movJulien_x = 240.000000;
	KeyFrame[98].movJulien_y = 0.000000;
	KeyFrame[98].movJulien_z = -400.000000;
	KeyFrame[98].giroJulienX = 180.000000;
	KeyFrame[98].giroJulienY = 0.000000;
	KeyFrame[98].giroJulienZ = 0.000000;

	KeyFrame[99].movJulien_x = 240.000000;
	KeyFrame[99].movJulien_y = 0.000000;
	KeyFrame[99].movJulien_z = -400.000000;
	KeyFrame[99].giroJulienX = 180.000000;
	KeyFrame[99].giroJulienY = 0.000000;
	KeyFrame[99].giroJulienZ = 0.000000;

	KeyFrame[100].movJulien_x = 240.000000;
	KeyFrame[100].movJulien_y = 0.000000;
	KeyFrame[100].movJulien_z = -400.000000;
	KeyFrame[100].giroJulienX = 180.000000;
	KeyFrame[100].giroJulienY = 0.000000;
	KeyFrame[100].giroJulienZ = 0.000000;

	KeyFrame[101].movJulien_x = 240.000000;
	KeyFrame[101].movJulien_y = 0.000000;
	KeyFrame[101].movJulien_z = -400.000000;
	KeyFrame[101].giroJulienX = 200.000000;
	KeyFrame[101].giroJulienY = 0.000000;
	KeyFrame[101].giroJulienZ = 0.000000;

	KeyFrame[102].movJulien_x = 240.000000;
	KeyFrame[102].movJulien_y = 0.000000;
	KeyFrame[102].movJulien_z = -400.000000;
	KeyFrame[102].giroJulienX = 200.000000;
	KeyFrame[102].giroJulienY = 0.000000;
	KeyFrame[102].giroJulienZ = 0.000000;

	KeyFrame[103].movJulien_x = 240.000000;
	KeyFrame[103].movJulien_y = 0.000000;
	KeyFrame[103].movJulien_z = -400.000000;
	KeyFrame[103].giroJulienX = 200.000000;
	KeyFrame[103].giroJulienY = 0.000000;
	KeyFrame[103].giroJulienZ = 0.000000;

	KeyFrame[104].movJulien_x = 240.000000;
	KeyFrame[104].movJulien_y = 0.000000;
	KeyFrame[104].movJulien_z = -400.000000;
	KeyFrame[104].giroJulienX = 220.000000;
	KeyFrame[104].giroJulienY = 0.000000;
	KeyFrame[104].giroJulienZ = 0.000000;

	KeyFrame[105].movJulien_x = 240.000000;
	KeyFrame[105].movJulien_y = 0.000000;
	KeyFrame[105].movJulien_z = -400.000000;
	KeyFrame[105].giroJulienX = 220.000000;
	KeyFrame[105].giroJulienY = 0.000000;
	KeyFrame[105].giroJulienZ = 0.000000;

	KeyFrame[106].movJulien_x = 240.000000;
	KeyFrame[106].movJulien_y = 0.000000;
	KeyFrame[106].movJulien_z = -400.000000;
	KeyFrame[106].giroJulienX = 220.000000;
	KeyFrame[106].giroJulienY = 0.000000;
	KeyFrame[106].giroJulienZ = 0.000000;

	KeyFrame[107].movJulien_x = 240.000000;
	KeyFrame[107].movJulien_y = 0.000000;
	KeyFrame[107].movJulien_z = -400.000000;
	KeyFrame[107].giroJulienX = 220.000000;
	KeyFrame[107].giroJulienY = 0.000000;
	KeyFrame[107].giroJulienZ = 0.000000;

	KeyFrame[108].movJulien_x = 240.000000;
	KeyFrame[108].movJulien_y = 0.000000;
	KeyFrame[108].movJulien_z = -400.000000;
	KeyFrame[108].giroJulienX = 240.000000;
	KeyFrame[108].giroJulienY = 0.000000;
	KeyFrame[108].giroJulienZ = 0.000000;

	KeyFrame[109].movJulien_x = 240.000000;
	KeyFrame[109].movJulien_y = 0.000000;
	KeyFrame[109].movJulien_z = -400.000000;
	KeyFrame[109].giroJulienX = 240.000000;
	KeyFrame[109].giroJulienY = 0.000000;
	KeyFrame[109].giroJulienZ = 0.000000;

	KeyFrame[110].movJulien_x = 240.000000;
	KeyFrame[110].movJulien_y = 0.000000;
	KeyFrame[110].movJulien_z = -400.000000;
	KeyFrame[110].giroJulienX = 240.000000;
	KeyFrame[110].giroJulienY = 0.000000;
	KeyFrame[110].giroJulienZ = 0.000000;

	KeyFrame[111].movJulien_x = 240.000000;
	KeyFrame[111].movJulien_y = 0.000000;
	KeyFrame[111].movJulien_z = -400.000000;
	KeyFrame[111].giroJulienX = 240.000000;
	KeyFrame[111].giroJulienY = 0.000000;
	KeyFrame[111].giroJulienZ = 0.000000;

	KeyFrame[112].movJulien_x = 240.000000;
	KeyFrame[112].movJulien_y = 0.000000;
	KeyFrame[112].movJulien_z = -400.000000;
	KeyFrame[112].giroJulienX = 240.000000;
	KeyFrame[112].giroJulienY = 0.000000;
	KeyFrame[112].giroJulienZ = 0.000000;

	KeyFrame[113].movJulien_x = 240.000000;
	KeyFrame[113].movJulien_y = 0.000000;
	KeyFrame[113].movJulien_z = -400.000000;
	KeyFrame[113].giroJulienX = 260.000000;
	KeyFrame[113].giroJulienY = 0.000000;
	KeyFrame[113].giroJulienZ = 0.000000;

	KeyFrame[114].movJulien_x = 240.000000;
	KeyFrame[114].movJulien_y = 0.000000;
	KeyFrame[114].movJulien_z = -400.000000;
	KeyFrame[114].giroJulienX = 260.000000;
	KeyFrame[114].giroJulienY = 0.000000;
	KeyFrame[114].giroJulienZ = 0.000000;

	KeyFrame[115].movJulien_x = 240.000000;
	KeyFrame[115].movJulien_y = 0.000000;
	KeyFrame[115].movJulien_z = -400.000000;
	KeyFrame[115].giroJulienX = 260.000000;
	KeyFrame[115].giroJulienY = 0.000000;
	KeyFrame[115].giroJulienZ = 0.000000;

	KeyFrame[116].movJulien_x = 240.000000;
	KeyFrame[116].movJulien_y = 0.000000;
	KeyFrame[116].movJulien_z = -400.000000;
	KeyFrame[116].giroJulienX = 260.000000;
	KeyFrame[116].giroJulienY = 0.000000;
	KeyFrame[116].giroJulienZ = 0.000000;

	KeyFrame[117].movJulien_x = 220.000000;
	KeyFrame[117].movJulien_y = 0.000000;
	KeyFrame[117].movJulien_z = -400.000000;
	KeyFrame[117].giroJulienX = 260.000000;
	KeyFrame[117].giroJulienY = 0.000000;
	KeyFrame[117].giroJulienZ = 0.000000;

	KeyFrame[118].movJulien_x = 220.000000;
	KeyFrame[118].movJulien_y = 0.000000;
	KeyFrame[118].movJulien_z = -400.000000;
	KeyFrame[118].giroJulienX = 260.000000;
	KeyFrame[118].giroJulienY = 0.000000;
	KeyFrame[118].giroJulienZ = 0.000000;

	KeyFrame[119].movJulien_x = 220.000000;
	KeyFrame[119].movJulien_y = 0.000000;
	KeyFrame[119].movJulien_z = -400.000000;
	KeyFrame[119].giroJulienX = 260.000000;
	KeyFrame[119].giroJulienY = 0.000000;
	KeyFrame[119].giroJulienZ = 0.000000;

	KeyFrame[120].movJulien_x = 220.000000;
	KeyFrame[120].movJulien_y = 0.000000;
	KeyFrame[120].movJulien_z = -400.000000;
	KeyFrame[120].giroJulienX = 260.000000;
	KeyFrame[120].giroJulienY = 0.000000;
	KeyFrame[120].giroJulienZ = 0.000000;

	KeyFrame[121].movJulien_x = 220.000000;
	KeyFrame[121].movJulien_y = 0.000000;
	KeyFrame[121].movJulien_z = -400.000000;
	KeyFrame[121].giroJulienX = 260.000000;
	KeyFrame[121].giroJulienY = 0.000000;
	KeyFrame[121].giroJulienZ = 0.000000;

	KeyFrame[122].movJulien_x = 220.000000;
	KeyFrame[122].movJulien_y = 0.000000;
	KeyFrame[122].movJulien_z = -400.000000;
	KeyFrame[122].giroJulienX = 260.000000;
	KeyFrame[122].giroJulienY = 0.000000;
	KeyFrame[122].giroJulienZ = 0.000000;

	KeyFrame[123].movJulien_x = 200.000000;
	KeyFrame[123].movJulien_y = 0.000000;
	KeyFrame[123].movJulien_z = -400.000000;
	KeyFrame[123].giroJulienX = 260.000000;
	KeyFrame[123].giroJulienY = 0.000000;
	KeyFrame[123].giroJulienZ = 0.000000;

	KeyFrame[124].movJulien_x = 200.000000;
	KeyFrame[124].movJulien_y = 0.000000;
	KeyFrame[124].movJulien_z = -400.000000;
	KeyFrame[124].giroJulienX = 260.000000;
	KeyFrame[124].giroJulienY = 0.000000;
	KeyFrame[124].giroJulienZ = 0.000000;

	KeyFrame[125].movJulien_x = 200.000000;
	KeyFrame[125].movJulien_y = 0.000000;
	KeyFrame[125].movJulien_z = -400.000000;
	KeyFrame[125].giroJulienX = 260.000000;
	KeyFrame[125].giroJulienY = 0.000000;
	KeyFrame[125].giroJulienZ = 0.000000;

	KeyFrame[126].movJulien_x = 200.000000;
	KeyFrame[126].movJulien_y = 0.000000;
	KeyFrame[126].movJulien_z = -400.000000;
	KeyFrame[126].giroJulienX = 260.000000;
	KeyFrame[126].giroJulienY = 0.000000;
	KeyFrame[126].giroJulienZ = 0.000000;

	KeyFrame[127].movJulien_x = 200.000000;
	KeyFrame[127].movJulien_y = 0.000000;
	KeyFrame[127].movJulien_z = -400.000000;
	KeyFrame[127].giroJulienX = 260.000000;
	KeyFrame[127].giroJulienY = 0.000000;
	KeyFrame[127].giroJulienZ = 0.000000;

	KeyFrame[128].movJulien_x = 200.000000;
	KeyFrame[128].movJulien_y = 0.000000;
	KeyFrame[128].movJulien_z = -400.000000;
	KeyFrame[128].giroJulienX = 260.000000;
	KeyFrame[128].giroJulienY = 0.000000;
	KeyFrame[128].giroJulienZ = 0.000000;

	KeyFrame[129].movJulien_x = 200.000000;
	KeyFrame[129].movJulien_y = 0.000000;
	KeyFrame[129].movJulien_z = -400.000000;
	KeyFrame[129].giroJulienX = 260.000000;
	KeyFrame[129].giroJulienY = 0.000000;
	KeyFrame[129].giroJulienZ = 0.000000;

	KeyFrame[130].movJulien_x = 200.000000;
	KeyFrame[130].movJulien_y = 0.000000;
	KeyFrame[130].movJulien_z = -400.000000;
	KeyFrame[130].giroJulienX = 260.000000;
	KeyFrame[130].giroJulienY = 0.000000;
	KeyFrame[130].giroJulienZ = 0.000000;

	KeyFrame[131].movJulien_x = 180.000000;
	KeyFrame[131].movJulien_y = 0.000000;
	KeyFrame[131].movJulien_z = -400.000000;
	KeyFrame[131].giroJulienX = 260.000000;
	KeyFrame[131].giroJulienY = 0.000000;
	KeyFrame[131].giroJulienZ = 0.000000;

	KeyFrame[132].movJulien_x = 180.000000;
	KeyFrame[132].movJulien_y = 0.000000;
	KeyFrame[132].movJulien_z = -400.000000;
	KeyFrame[132].giroJulienX = 260.000000;
	KeyFrame[132].giroJulienY = 0.000000;
	KeyFrame[132].giroJulienZ = 0.000000;

	KeyFrame[133].movJulien_x = 180.000000;
	KeyFrame[133].movJulien_y = 0.000000;
	KeyFrame[133].movJulien_z = -400.000000;
	KeyFrame[133].giroJulienX = 260.000000;
	KeyFrame[133].giroJulienY = 0.000000;
	KeyFrame[133].giroJulienZ = 0.000000;

	KeyFrame[134].movJulien_x = 180.000000;
	KeyFrame[134].movJulien_y = 0.000000;
	KeyFrame[134].movJulien_z = -400.000000;
	KeyFrame[134].giroJulienX = 260.000000;
	KeyFrame[134].giroJulienY = 0.000000;
	KeyFrame[134].giroJulienZ = 0.000000;

	KeyFrame[135].movJulien_x = 180.000000;
	KeyFrame[135].movJulien_y = 0.000000;
	KeyFrame[135].movJulien_z = -400.000000;
	KeyFrame[135].giroJulienX = 260.000000;
	KeyFrame[135].giroJulienY = 0.000000;
	KeyFrame[135].giroJulienZ = 0.000000;

	KeyFrame[136].movJulien_x = 180.000000;
	KeyFrame[136].movJulien_y = 0.000000;
	KeyFrame[136].movJulien_z = -400.000000;
	KeyFrame[136].giroJulienX = 260.000000;
	KeyFrame[136].giroJulienY = 0.000000;
	KeyFrame[136].giroJulienZ = 0.000000;

	KeyFrame[137].movJulien_x = 180.000000;
	KeyFrame[137].movJulien_y = 0.000000;
	KeyFrame[137].movJulien_z = -400.000000;
	KeyFrame[137].giroJulienX = 260.000000;
	KeyFrame[137].giroJulienY = 0.000000;
	KeyFrame[137].giroJulienZ = 0.000000;

	KeyFrame[138].movJulien_x = 180.000000;
	KeyFrame[138].movJulien_y = 0.000000;
	KeyFrame[138].movJulien_z = -400.000000;
	KeyFrame[138].giroJulienX = 260.000000;
	KeyFrame[138].giroJulienY = 0.000000;
	KeyFrame[138].giroJulienZ = 0.000000;

	KeyFrame[139].movJulien_x = 180.000000;
	KeyFrame[139].movJulien_y = 0.000000;
	KeyFrame[139].movJulien_z = -400.000000;
	KeyFrame[139].giroJulienX = 260.000000;
	KeyFrame[139].giroJulienY = 0.000000;
	KeyFrame[139].giroJulienZ = 0.000000;

	KeyFrame[140].movJulien_x = 180.000000;
	KeyFrame[140].movJulien_y = 0.000000;
	KeyFrame[140].movJulien_z = -400.000000;
	KeyFrame[140].giroJulienX = 260.000000;
	KeyFrame[140].giroJulienY = 0.000000;
	KeyFrame[140].giroJulienZ = 0.000000;

	KeyFrame[141].movJulien_x = 160.000000;
	KeyFrame[141].movJulien_y = 0.000000;
	KeyFrame[141].movJulien_z = -400.000000;
	KeyFrame[141].giroJulienX = 260.000000;
	KeyFrame[141].giroJulienY = 0.000000;
	KeyFrame[141].giroJulienZ = 0.000000;

	KeyFrame[142].movJulien_x = 160.000000;
	KeyFrame[142].movJulien_y = 0.000000;
	KeyFrame[142].movJulien_z = -400.000000;
	KeyFrame[142].giroJulienX = 260.000000;
	KeyFrame[142].giroJulienY = 0.000000;
	KeyFrame[142].giroJulienZ = 0.000000;

	KeyFrame[143].movJulien_x = 160.000000;
	KeyFrame[143].movJulien_y = 0.000000;
	KeyFrame[143].movJulien_z = -400.000000;
	KeyFrame[143].giroJulienX = 260.000000;
	KeyFrame[143].giroJulienY = 0.000000;
	KeyFrame[143].giroJulienZ = 0.000000;
	KeyFrame[144].movJulien_x = 160.000000;
	KeyFrame[144].movJulien_y = 0.000000;
	KeyFrame[144].movJulien_z = -400.000000;
	KeyFrame[144].giroJulienX = 260.000000;
	KeyFrame[144].giroJulienY = 0.000000;
	KeyFrame[144].giroJulienZ = 0.000000;
	KeyFrame[145].movJulien_x = 160.000000;
	KeyFrame[145].movJulien_y = 0.000000;
	KeyFrame[145].movJulien_z = -400.000000;
	KeyFrame[145].giroJulienX = 260.000000;
	KeyFrame[145].giroJulienY = 0.000000;
	KeyFrame[145].giroJulienZ = 0.000000;
	KeyFrame[146].movJulien_x = 160.000000;
	KeyFrame[146].movJulien_y = 0.000000;
	KeyFrame[146].movJulien_z = -400.000000;
	KeyFrame[146].giroJulienX = 260.000000;
	KeyFrame[146].giroJulienY = 0.000000;
	KeyFrame[146].giroJulienZ = 0.000000;
	KeyFrame[147].movJulien_x = 160.000000;
	KeyFrame[147].movJulien_y = 0.000000;
	KeyFrame[147].movJulien_z = -400.000000;
	KeyFrame[147].giroJulienX = 260.000000;
	KeyFrame[147].giroJulienY = 0.000000;
	KeyFrame[147].giroJulienZ = 0.000000;
	KeyFrame[148].movJulien_x = 160.000000;
	KeyFrame[148].movJulien_y = 0.000000;
	KeyFrame[148].movJulien_z = -400.000000;
	KeyFrame[148].giroJulienX = 260.000000;
	KeyFrame[148].giroJulienY = 0.000000;
	KeyFrame[148].giroJulienZ = 0.000000;
	KeyFrame[149].movJulien_x = 160.000000;
	KeyFrame[149].movJulien_y = 0.000000;
	KeyFrame[149].movJulien_z = -400.000000;
	KeyFrame[149].giroJulienX = 260.000000;
	KeyFrame[149].giroJulienY = 0.000000;
	KeyFrame[149].giroJulienZ = 0.000000;
	KeyFrame[150].movJulien_x = 160.000000;
	KeyFrame[150].movJulien_y = 0.000000;
	KeyFrame[150].movJulien_z = -400.000000;
	KeyFrame[150].giroJulienX = 260.000000;
	KeyFrame[150].giroJulienY = 0.000000;
	KeyFrame[150].giroJulienZ = 0.000000;
	KeyFrame[151].movJulien_x = 160.000000;
	KeyFrame[151].movJulien_y = 0.000000;
	KeyFrame[151].movJulien_z = -400.000000;
	KeyFrame[151].giroJulienX = 260.000000;
	KeyFrame[151].giroJulienY = 0.000000;
	KeyFrame[151].giroJulienZ = 0.000000;
	KeyFrame[152].movJulien_x = 140.000000;
	KeyFrame[152].movJulien_y = 0.000000;
	KeyFrame[152].movJulien_z = -400.000000;
	KeyFrame[152].giroJulienX = 260.000000;
	KeyFrame[152].giroJulienY = 0.000000;
	KeyFrame[152].giroJulienZ = 0.000000;
	KeyFrame[153].movJulien_x = 140.000000;
	KeyFrame[153].movJulien_y = 0.000000;
	KeyFrame[153].movJulien_z = -400.000000;
	KeyFrame[153].giroJulienX = 260.000000;
	KeyFrame[153].giroJulienY = 0.000000;
	KeyFrame[153].giroJulienZ = 0.000000;
	KeyFrame[154].movJulien_x = 140.000000;
	KeyFrame[154].movJulien_y = 0.000000;
	KeyFrame[154].movJulien_z = -400.000000;
	KeyFrame[154].giroJulienX = 260.000000;
	KeyFrame[154].giroJulienY = 0.000000;
	KeyFrame[154].giroJulienZ = 0.000000;
	KeyFrame[155].movJulien_x = 140.000000;
	KeyFrame[155].movJulien_y = 0.000000;
	KeyFrame[155].movJulien_z = -400.000000;
	KeyFrame[155].giroJulienX = 260.000000;
	KeyFrame[155].giroJulienY = 0.000000;
	KeyFrame[155].giroJulienZ = 0.000000;
	KeyFrame[156].movJulien_x = 140.000000;
	KeyFrame[156].movJulien_y = 0.000000;
	KeyFrame[156].movJulien_z = -400.000000;
	KeyFrame[156].giroJulienX = 260.000000;
	KeyFrame[156].giroJulienY = 0.000000;
	KeyFrame[156].giroJulienZ = 0.000000;
	KeyFrame[157].movJulien_x = 140.000000;
	KeyFrame[157].movJulien_y = 0.000000;
	KeyFrame[157].movJulien_z = -400.000000;
	KeyFrame[157].giroJulienX = 260.000000;
	KeyFrame[157].giroJulienY = 0.000000;
	KeyFrame[157].giroJulienZ = 0.000000;
	KeyFrame[158].movJulien_x = 140.000000;
	KeyFrame[158].movJulien_y = 0.000000;
	KeyFrame[158].movJulien_z = -400.000000;
	KeyFrame[158].giroJulienX = 260.000000;
	KeyFrame[158].giroJulienY = 0.000000;
	KeyFrame[158].giroJulienZ = 0.000000;
	KeyFrame[159].movJulien_x = 140.000000;
	KeyFrame[159].movJulien_y = 0.000000;
	KeyFrame[159].movJulien_z = -400.000000;
	KeyFrame[159].giroJulienX = 260.000000;
	KeyFrame[159].giroJulienY = 0.000000;
	KeyFrame[159].giroJulienZ = 0.000000;
	KeyFrame[160].movJulien_x = 140.000000;
	KeyFrame[160].movJulien_y = 0.000000;
	KeyFrame[160].movJulien_z = -400.000000;
	KeyFrame[160].giroJulienX = 260.000000;
	KeyFrame[160].giroJulienY = 0.000000;
	KeyFrame[160].giroJulienZ = 0.000000;
	KeyFrame[161].movJulien_x = 140.000000;
	KeyFrame[161].movJulien_y = 0.000000;
	KeyFrame[161].movJulien_z = -400.000000;
	KeyFrame[161].giroJulienX = 260.000000;
	KeyFrame[161].giroJulienY = 0.000000;
	KeyFrame[161].giroJulienZ = 0.000000;
	KeyFrame[162].movJulien_x = 140.000000;
	KeyFrame[162].movJulien_y = 0.000000;
	KeyFrame[162].movJulien_z = -400.000000;
	KeyFrame[162].giroJulienX = 260.000000;
	KeyFrame[162].giroJulienY = 0.000000;
	KeyFrame[162].giroJulienZ = 0.000000;
	KeyFrame[163].movJulien_x = 140.000000;
	KeyFrame[163].movJulien_y = 0.000000;
	KeyFrame[163].movJulien_z = -400.000000;
	KeyFrame[163].giroJulienX = 260.000000;
	KeyFrame[163].giroJulienY = 0.000000;
	KeyFrame[163].giroJulienZ = 0.000000;
	KeyFrame[164].movJulien_x = 140.000000;
	KeyFrame[164].movJulien_y = 0.000000;
	KeyFrame[164].movJulien_z = -400.000000;
	KeyFrame[164].giroJulienX = 260.000000;
	KeyFrame[164].giroJulienY = 0.000000;
	KeyFrame[164].giroJulienZ = 0.000000;
	KeyFrame[165].movJulien_x = 140.000000;
	KeyFrame[165].movJulien_y = 0.000000;
	KeyFrame[165].movJulien_z = -400.000000;
	KeyFrame[165].giroJulienX = 260.000000;
	KeyFrame[165].giroJulienY = 0.000000;
	KeyFrame[165].giroJulienZ = 0.000000;
	KeyFrame[166].movJulien_x = 140.000000;
	KeyFrame[166].movJulien_y = 0.000000;
	KeyFrame[166].movJulien_z = -400.000000;
	KeyFrame[166].giroJulienX = 260.000000;
	KeyFrame[166].giroJulienY = 0.000000;
	KeyFrame[166].giroJulienZ = 0.000000;
	KeyFrame[167].movJulien_x = 140.000000;
	KeyFrame[167].movJulien_y = 0.000000;
	KeyFrame[167].movJulien_z = -400.000000;
	KeyFrame[167].giroJulienX = 260.000000;
	KeyFrame[167].giroJulienY = 0.000000;
	KeyFrame[167].giroJulienZ = 0.000000;
	KeyFrame[168].movJulien_x = 140.000000;
	KeyFrame[168].movJulien_y = 0.000000;
	KeyFrame[168].movJulien_z = -400.000000;
	KeyFrame[168].giroJulienX = 260.000000;
	KeyFrame[168].giroJulienY = 0.000000;
	KeyFrame[168].giroJulienZ = 0.000000;
	KeyFrame[169].movJulien_x = 120.000000;
	KeyFrame[169].movJulien_y = 0.000000;
	KeyFrame[169].movJulien_z = -400.000000;
	KeyFrame[169].giroJulienX = 260.000000;
	KeyFrame[169].giroJulienY = 0.000000;
	KeyFrame[169].giroJulienZ = 0.000000;
	KeyFrame[170].movJulien_x = 100.000000;
	KeyFrame[170].movJulien_y = 0.000000;
	KeyFrame[170].movJulien_z = -400.000000;
	KeyFrame[170].giroJulienX = 260.000000;
	KeyFrame[170].giroJulienY = 0.000000;
	KeyFrame[170].giroJulienZ = 0.000000;
	KeyFrame[171].movJulien_x = 100.000000;
	KeyFrame[171].movJulien_y = 0.000000;
	KeyFrame[171].movJulien_z = -400.000000;
	KeyFrame[171].giroJulienX = 260.000000;
	KeyFrame[171].giroJulienY = 0.000000;
	KeyFrame[171].giroJulienZ = 0.000000;
	KeyFrame[172].movJulien_x = 100.000000;
	KeyFrame[172].movJulien_y = 0.000000;
	KeyFrame[172].movJulien_z = -400.000000;
	KeyFrame[172].giroJulienX = 260.000000;
	KeyFrame[172].giroJulienY = 0.000000;
	KeyFrame[172].giroJulienZ = 0.000000;
	KeyFrame[173].movJulien_x = 60.000000;
	KeyFrame[173].movJulien_y = 0.000000;
	KeyFrame[173].movJulien_z = -400.000000;
	KeyFrame[173].giroJulienX = 260.000000;
	KeyFrame[173].giroJulienY = 0.000000;
	KeyFrame[173].giroJulienZ = 0.000000;
	KeyFrame[174].movJulien_x = 60.000000;
	KeyFrame[174].movJulien_y = 0.000000;
	KeyFrame[174].movJulien_z = -400.000000;
	KeyFrame[174].giroJulienX = 260.000000;
	KeyFrame[174].giroJulienY = 0.000000;
	KeyFrame[174].giroJulienZ = 0.000000;
	KeyFrame[175].movJulien_x = 60.000000;
	KeyFrame[175].movJulien_y = 0.000000;
	KeyFrame[175].movJulien_z = -400.000000;
	KeyFrame[175].giroJulienX = 260.000000;
	KeyFrame[175].giroJulienY = 0.000000;
	KeyFrame[175].giroJulienZ = 0.000000;
	KeyFrame[176].movJulien_x = 60.000000;
	KeyFrame[176].movJulien_y = 0.000000;
	KeyFrame[176].movJulien_z = -400.000000;
	KeyFrame[176].giroJulienX = 260.000000;
	KeyFrame[176].giroJulienY = 0.000000;
	KeyFrame[176].giroJulienZ = 0.000000;
	KeyFrame[177].movJulien_x = 60.000000;
	KeyFrame[177].movJulien_y = 0.000000;
	KeyFrame[177].movJulien_z = -400.000000;
	KeyFrame[177].giroJulienX = 260.000000;
	KeyFrame[177].giroJulienY = 0.000000;
	KeyFrame[177].giroJulienZ = 0.000000;
	KeyFrame[178].movJulien_x = 40.000000;
	KeyFrame[178].movJulien_y = 0.000000;
	KeyFrame[178].movJulien_z = -400.000000;
	KeyFrame[178].giroJulienX = 260.000000;
	KeyFrame[178].giroJulienY = 0.000000;
	KeyFrame[178].giroJulienZ = 0.000000;
	KeyFrame[179].movJulien_x = 40.000000;
	KeyFrame[179].movJulien_y = 0.000000;
	KeyFrame[179].movJulien_z = -400.000000;
	KeyFrame[179].giroJulienX = 260.000000;
	KeyFrame[179].giroJulienY = 0.000000;
	KeyFrame[179].giroJulienZ = 0.000000;
	KeyFrame[180].movJulien_x = 20.000000;
	KeyFrame[180].movJulien_y = 0.000000;
	KeyFrame[180].movJulien_z = -400.000000;
	KeyFrame[180].giroJulienX = 260.000000;
	KeyFrame[180].giroJulienY = 0.000000;
	KeyFrame[180].giroJulienZ = 0.000000;
	KeyFrame[181].movJulien_x = 20.000000;
	KeyFrame[181].movJulien_y = 0.000000;
	KeyFrame[181].movJulien_z = -400.000000;
	KeyFrame[181].giroJulienX = 260.000000;
	KeyFrame[181].giroJulienY = 0.000000;
	KeyFrame[181].giroJulienZ = 0.000000;
	KeyFrame[182].movJulien_x = 20.000000;
	KeyFrame[182].movJulien_y = 0.000000;
	KeyFrame[182].movJulien_z = -400.000000;
	KeyFrame[182].giroJulienX = 260.000000;
	KeyFrame[182].giroJulienY = 0.000000;
	KeyFrame[182].giroJulienZ = 0.000000;
	KeyFrame[183].movJulien_x = 20.000000;
	KeyFrame[183].movJulien_y = 0.000000;
	KeyFrame[183].movJulien_z = -400.000000;
	KeyFrame[183].giroJulienX = 260.000000;
	KeyFrame[183].giroJulienY = 0.000000;
	KeyFrame[183].giroJulienZ = 0.000000;
	KeyFrame[184].movJulien_x = 20.000000;
	KeyFrame[184].movJulien_y = 0.000000;
	KeyFrame[184].movJulien_z = -400.000000;
	KeyFrame[184].giroJulienX = 260.000000;
	KeyFrame[184].giroJulienY = 0.000000;
	KeyFrame[184].giroJulienZ = 0.000000;
	KeyFrame[185].movJulien_x = 20.000000;
	KeyFrame[185].movJulien_y = 0.000000;
	KeyFrame[185].movJulien_z = -400.000000;
	KeyFrame[185].giroJulienX = 260.000000;
	KeyFrame[185].giroJulienY = 0.000000;
	KeyFrame[185].giroJulienZ = 0.000000;
	KeyFrame[186].movJulien_x = 0.000000;
	KeyFrame[186].movJulien_y = 0.000000;
	KeyFrame[186].movJulien_z = -400.000000;
	KeyFrame[186].giroJulienX = 260.000000;
	KeyFrame[186].giroJulienY = 0.000000;
	KeyFrame[186].giroJulienZ = 0.000000;
	KeyFrame[187].movJulien_x = 0.000000;
	KeyFrame[187].movJulien_y = 0.000000;
	KeyFrame[187].movJulien_z = -400.000000;
	KeyFrame[187].giroJulienX = 260.000000;
	KeyFrame[187].giroJulienY = 0.000000;
	KeyFrame[187].giroJulienZ = 0.000000;
	KeyFrame[188].movJulien_x = 0.000000;
	KeyFrame[188].movJulien_y = 0.000000;
	KeyFrame[188].movJulien_z = -400.000000;
	KeyFrame[188].giroJulienX = 260.000000;
	KeyFrame[188].giroJulienY = 0.000000;
	KeyFrame[188].giroJulienZ = 0.000000;
	KeyFrame[189].movJulien_x = 0.000000;
	KeyFrame[189].movJulien_y = 0.000000;
	KeyFrame[189].movJulien_z = -400.000000;
	KeyFrame[189].giroJulienX = 260.000000;
	KeyFrame[189].giroJulienY = 0.000000;
	KeyFrame[189].giroJulienZ = 0.000000;
	KeyFrame[190].movJulien_x = 0.000000;
	KeyFrame[190].movJulien_y = 0.000000;
	KeyFrame[190].movJulien_z = -400.000000;
	KeyFrame[190].giroJulienX = 260.000000;
	KeyFrame[190].giroJulienY = 0.000000;
	KeyFrame[190].giroJulienZ = 0.000000;
	KeyFrame[191].movJulien_x = 0.000000;
	KeyFrame[191].movJulien_y = 0.000000;
	KeyFrame[191].movJulien_z = -400.000000;
	KeyFrame[191].giroJulienX = 260.000000;
	KeyFrame[191].giroJulienY = 0.000000;
	KeyFrame[191].giroJulienZ = 0.000000;
	KeyFrame[192].movJulien_x = 0.000000;
	KeyFrame[192].movJulien_y = 0.000000;
	KeyFrame[192].movJulien_z = -400.000000;
	KeyFrame[192].giroJulienX = 260.000000;
	KeyFrame[192].giroJulienY = 0.000000;
	KeyFrame[192].giroJulienZ = 0.000000;
	KeyFrame[193].movJulien_x = -20.000000;
	KeyFrame[193].movJulien_y = 0.000000;
	KeyFrame[193].movJulien_z = -400.000000;
	KeyFrame[193].giroJulienX = 260.000000;
	KeyFrame[193].giroJulienY = 0.000000;
	KeyFrame[193].giroJulienZ = 0.000000;
	KeyFrame[194].movJulien_x = -20.000000;
	KeyFrame[194].movJulien_y = 0.000000;
	KeyFrame[194].movJulien_z = -400.000000;
	KeyFrame[194].giroJulienX = 260.000000;
	KeyFrame[194].giroJulienY = 0.000000;
	KeyFrame[194].giroJulienZ = 0.000000;
	KeyFrame[195].movJulien_x = -20.000000;
	KeyFrame[195].movJulien_y = 0.000000;
	KeyFrame[195].movJulien_z = -400.000000;
	KeyFrame[195].giroJulienX = 260.000000;
	KeyFrame[195].giroJulienY = 0.000000;
	KeyFrame[195].giroJulienZ = 0.000000;
	KeyFrame[196].movJulien_x = -20.000000;
	KeyFrame[196].movJulien_y = 0.000000;
	KeyFrame[196].movJulien_z = -400.000000;
	KeyFrame[196].giroJulienX = 260.000000;
	KeyFrame[196].giroJulienY = 0.000000;
	KeyFrame[196].giroJulienZ = 0.000000;
	KeyFrame[197].movJulien_x = -40.000000;
	KeyFrame[197].movJulien_y = 0.000000;
	KeyFrame[197].movJulien_z = -400.000000;
	KeyFrame[197].giroJulienX = 260.000000;
	KeyFrame[197].giroJulienY = 0.000000;
	KeyFrame[197].giroJulienZ = 0.000000;
	KeyFrame[198].movJulien_x = -40.000000;
	KeyFrame[198].movJulien_y = 0.000000;
	KeyFrame[198].movJulien_z = -400.000000;
	KeyFrame[198].giroJulienX = 260.000000;
	KeyFrame[198].giroJulienY = 0.000000;
	KeyFrame[198].giroJulienZ = 0.000000;
	KeyFrame[199].movJulien_x = -40.000000;
	KeyFrame[199].movJulien_y = 0.000000;
	KeyFrame[199].movJulien_z = -400.000000;
	KeyFrame[199].giroJulienX = 260.000000;
	KeyFrame[199].giroJulienY = 0.000000;
	KeyFrame[199].giroJulienZ = 0.000000;
	KeyFrame[200].movJulien_x = -40.000000;
	KeyFrame[200].movJulien_y = 0.000000;
	KeyFrame[200].movJulien_z = -400.000000;
	KeyFrame[200].giroJulienX = 260.000000;
	KeyFrame[200].giroJulienY = 0.000000;
	KeyFrame[200].giroJulienZ = 0.000000;
	KeyFrame[201].movJulien_x = -40.000000;
	KeyFrame[201].movJulien_y = 0.000000;
	KeyFrame[201].movJulien_z = -400.000000;
	KeyFrame[201].giroJulienX = 260.000000;
	KeyFrame[201].giroJulienY = 0.000000;
	KeyFrame[201].giroJulienZ = 0.000000;
	KeyFrame[202].movJulien_x = -60.000000;
	KeyFrame[202].movJulien_y = 0.000000;
	KeyFrame[202].movJulien_z = -400.000000;
	KeyFrame[202].giroJulienX = 260.000000;
	KeyFrame[202].giroJulienY = 0.000000;
	KeyFrame[202].giroJulienZ = 0.000000;
	KeyFrame[203].movJulien_x = -60.000000;
	KeyFrame[203].movJulien_y = 0.000000;
	KeyFrame[203].movJulien_z = -400.000000;
	KeyFrame[203].giroJulienX = 260.000000;
	KeyFrame[203].giroJulienY = 0.000000;
	KeyFrame[203].giroJulienZ = 0.000000;
	KeyFrame[204].movJulien_x = -60.000000;
	KeyFrame[204].movJulien_y = 0.000000;
	KeyFrame[204].movJulien_z = -400.000000;
	KeyFrame[204].giroJulienX = 260.000000;
	KeyFrame[204].giroJulienY = 0.000000;
	KeyFrame[204].giroJulienZ = 0.000000;
	KeyFrame[205].movJulien_x = -60.000000;
	KeyFrame[205].movJulien_y = 0.000000;
	KeyFrame[205].movJulien_z = -400.000000;
	KeyFrame[205].giroJulienX = 260.000000;
	KeyFrame[205].giroJulienY = 0.000000;
	KeyFrame[205].giroJulienZ = 0.000000;
	KeyFrame[206].movJulien_x = -60.000000;
	KeyFrame[206].movJulien_y = 0.000000;
	KeyFrame[206].movJulien_z = -400.000000;
	KeyFrame[206].giroJulienX = 260.000000;
	KeyFrame[206].giroJulienY = 0.000000;
	KeyFrame[206].giroJulienZ = 0.000000;
	KeyFrame[207].movJulien_x = -80.000000;
	KeyFrame[207].movJulien_y = 0.000000;
	KeyFrame[207].movJulien_z = -400.000000;
	KeyFrame[207].giroJulienX = 260.000000;
	KeyFrame[207].giroJulienY = 0.000000;
	KeyFrame[207].giroJulienZ = 0.000000;
	KeyFrame[208].movJulien_x = -80.000000;
	KeyFrame[208].movJulien_y = 0.000000;
	KeyFrame[208].movJulien_z = -400.000000;
	KeyFrame[208].giroJulienX = 260.000000;
	KeyFrame[208].giroJulienY = 0.000000;
	KeyFrame[208].giroJulienZ = 0.000000;
	KeyFrame[209].movJulien_x = -80.000000;
	KeyFrame[209].movJulien_y = 0.000000;
	KeyFrame[209].movJulien_z = -400.000000;
	KeyFrame[209].giroJulienX = 260.000000;
	KeyFrame[209].giroJulienY = 0.000000;
	KeyFrame[209].giroJulienZ = 0.000000;
	KeyFrame[210].movJulien_x = -80.000000;
	KeyFrame[210].movJulien_y = 0.000000;
	KeyFrame[210].movJulien_z = -400.000000;
	KeyFrame[210].giroJulienX = 260.000000;
	KeyFrame[210].giroJulienY = 0.000000;
	KeyFrame[210].giroJulienZ = 0.000000;
	KeyFrame[211].movJulien_x = -80.000000;
	KeyFrame[211].movJulien_y = 0.000000;
	KeyFrame[211].movJulien_z = -400.000000;
	KeyFrame[211].giroJulienX = 260.000000;
	KeyFrame[211].giroJulienY = 0.000000;
	KeyFrame[211].giroJulienZ = 0.000000;
	KeyFrame[212].movJulien_x = -80.000000;
	KeyFrame[212].movJulien_y = 0.000000;
	KeyFrame[212].movJulien_z = -400.000000;
	KeyFrame[212].giroJulienX = 260.000000;
	KeyFrame[212].giroJulienY = 0.000000;
	KeyFrame[212].giroJulienZ = 0.000000;
	KeyFrame[213].movJulien_x = -80.000000;
	KeyFrame[213].movJulien_y = 0.000000;
	KeyFrame[213].movJulien_z = -400.000000;
	KeyFrame[213].giroJulienX = 260.000000;
	KeyFrame[213].giroJulienY = 0.000000;
	KeyFrame[213].giroJulienZ = 0.000000;
	KeyFrame[214].movJulien_x = -100.000000;
	KeyFrame[214].movJulien_y = 0.000000;
	KeyFrame[214].movJulien_z = -400.000000;
	KeyFrame[214].giroJulienX = 260.000000;
	KeyFrame[214].giroJulienY = 0.000000;
	KeyFrame[214].giroJulienZ = 0.000000;
	KeyFrame[215].movJulien_x = -100.000000;
	KeyFrame[215].movJulien_y = 0.000000;
	KeyFrame[215].movJulien_z = -400.000000;
	KeyFrame[215].giroJulienX = 260.000000;
	KeyFrame[215].giroJulienY = 0.000000;
	KeyFrame[215].giroJulienZ = 0.000000;
	KeyFrame[216].movJulien_x = -100.000000;
	KeyFrame[216].movJulien_y = 0.000000;
	KeyFrame[216].movJulien_z = -400.000000;
	KeyFrame[216].giroJulienX = 260.000000;
	KeyFrame[216].giroJulienY = 0.000000;
	KeyFrame[216].giroJulienZ = 0.000000;
	KeyFrame[217].movJulien_x = -100.000000;
	KeyFrame[217].movJulien_y = 0.000000;
	KeyFrame[217].movJulien_z = -400.000000;
	KeyFrame[217].giroJulienX = 260.000000;
	KeyFrame[217].giroJulienY = 0.000000;
	KeyFrame[217].giroJulienZ = 0.000000;
	KeyFrame[218].movJulien_x = -100.000000;
	KeyFrame[218].movJulien_y = 0.000000;
	KeyFrame[218].movJulien_z = -400.000000;
	KeyFrame[218].giroJulienX = 260.000000;
	KeyFrame[218].giroJulienY = 0.000000;
	KeyFrame[218].giroJulienZ = 0.000000;
	KeyFrame[219].movJulien_x = -100.000000;
	KeyFrame[219].movJulien_y = 0.000000;
	KeyFrame[219].movJulien_z = -400.000000;
	KeyFrame[219].giroJulienX = 260.000000;
	KeyFrame[219].giroJulienY = 0.000000;
	KeyFrame[219].giroJulienZ = 0.000000;
	KeyFrame[220].movJulien_x = -100.000000;
	KeyFrame[220].movJulien_y = 0.000000;
	KeyFrame[220].movJulien_z = -400.000000;
	KeyFrame[220].giroJulienX = 260.000000;
	KeyFrame[220].giroJulienY = 0.000000;
	KeyFrame[220].giroJulienZ = 0.000000;
	KeyFrame[221].movJulien_x = -120.000000;
	KeyFrame[221].movJulien_y = 0.000000;
	KeyFrame[221].movJulien_z = -400.000000;
	KeyFrame[221].giroJulienX = 260.000000;
	KeyFrame[221].giroJulienY = 0.000000;
	KeyFrame[221].giroJulienZ = 0.000000;
	KeyFrame[222].movJulien_x = -120.000000;
	KeyFrame[222].movJulien_y = 0.000000;
	KeyFrame[222].movJulien_z = -400.000000;
	KeyFrame[222].giroJulienX = 260.000000;
	KeyFrame[222].giroJulienY = 0.000000;
	KeyFrame[222].giroJulienZ = 0.000000;
	KeyFrame[223].movJulien_x = -120.000000;
	KeyFrame[223].movJulien_y = 0.000000;
	KeyFrame[223].movJulien_z = -400.000000;
	KeyFrame[223].giroJulienX = 260.000000;
	KeyFrame[223].giroJulienY = 0.000000;
	KeyFrame[223].giroJulienZ = 0.000000;
	KeyFrame[224].movJulien_x = -120.000000;
	KeyFrame[224].movJulien_y = 0.000000;
	KeyFrame[224].movJulien_z = -400.000000;
	KeyFrame[224].giroJulienX = 260.000000;
	KeyFrame[224].giroJulienY = 0.000000;
	KeyFrame[224].giroJulienZ = 0.000000;
	KeyFrame[225].movJulien_x = -120.000000;
	KeyFrame[225].movJulien_y = 0.000000;
	KeyFrame[225].movJulien_z = -400.000000;
	KeyFrame[225].giroJulienX = 260.000000;
	KeyFrame[225].giroJulienY = 0.000000;
	KeyFrame[225].giroJulienZ = 0.000000;
	KeyFrame[226].movJulien_x = -120.000000;
	KeyFrame[226].movJulien_y = 0.000000;
	KeyFrame[226].movJulien_z = -400.000000;
	KeyFrame[226].giroJulienX = 260.000000;
	KeyFrame[226].giroJulienY = 0.000000;
	KeyFrame[226].giroJulienZ = 0.000000;
	KeyFrame[227].movJulien_x = -140.000000;
	KeyFrame[227].movJulien_y = 0.000000;
	KeyFrame[227].movJulien_z = -400.000000;
	KeyFrame[227].giroJulienX = 260.000000;
	KeyFrame[227].giroJulienY = 0.000000;
	KeyFrame[227].giroJulienZ = 0.000000;
	KeyFrame[228].movJulien_x = -140.000000;
	KeyFrame[228].movJulien_y = 0.000000;
	KeyFrame[228].movJulien_z = -400.000000;
	KeyFrame[228].giroJulienX = 260.000000;
	KeyFrame[228].giroJulienY = 0.000000;
	KeyFrame[228].giroJulienZ = 0.000000;
	KeyFrame[229].movJulien_x = -140.000000;
	KeyFrame[229].movJulien_y = 0.000000;
	KeyFrame[229].movJulien_z = -400.000000;
	KeyFrame[229].giroJulienX = 260.000000;
	KeyFrame[229].giroJulienY = 0.000000;
	KeyFrame[229].giroJulienZ = 0.000000;
	KeyFrame[230].movJulien_x = -140.000000;
	KeyFrame[230].movJulien_y = 0.000000;
	KeyFrame[230].movJulien_z = -400.000000;
	KeyFrame[230].giroJulienX = 260.000000;
	KeyFrame[230].giroJulienY = 0.000000;
	KeyFrame[230].giroJulienZ = 0.000000;
	KeyFrame[231].movJulien_x = -160.000000;
	KeyFrame[231].movJulien_y = 0.000000;
	KeyFrame[231].movJulien_z = -400.000000;
	KeyFrame[231].giroJulienX = 260.000000;
	KeyFrame[231].giroJulienY = 0.000000;
	KeyFrame[231].giroJulienZ = 0.000000;
	KeyFrame[232].movJulien_x = -160.000000;
	KeyFrame[232].movJulien_y = 0.000000;
	KeyFrame[232].movJulien_z = -400.000000;
	KeyFrame[232].giroJulienX = 260.000000;
	KeyFrame[232].giroJulienY = 0.000000;
	KeyFrame[232].giroJulienZ = 0.000000;
	KeyFrame[233].movJulien_x = -160.000000;
	KeyFrame[233].movJulien_y = 0.000000;
	KeyFrame[233].movJulien_z = -400.000000;
	KeyFrame[233].giroJulienX = 260.000000;
	KeyFrame[233].giroJulienY = 0.000000;
	KeyFrame[233].giroJulienZ = 0.000000;



	KeyFrame[234].movJulien_x = -160.000000;
	KeyFrame[234].movJulien_y = 0.000000;
	KeyFrame[234].movJulien_z = -400.000000;
	KeyFrame[234].giroJulienX = 260.000000;
	KeyFrame[234].giroJulienY = 0.000000;
	KeyFrame[234].giroJulienZ = 0.000000;



	KeyFrame[235].movJulien_x = -160.000000;
	KeyFrame[235].movJulien_y = 0.000000;
	KeyFrame[235].movJulien_z = -400.000000;
	KeyFrame[235].giroJulienX = 260.000000;
	KeyFrame[235].giroJulienY = 0.000000;
	KeyFrame[235].giroJulienZ = 0.000000;



	KeyFrame[236].movJulien_x = -160.000000;
	KeyFrame[236].movJulien_y = 0.000000;
	KeyFrame[236].movJulien_z = -400.000000;
	KeyFrame[236].giroJulienX = 260.000000;
	KeyFrame[236].giroJulienY = 0.000000;
	KeyFrame[236].giroJulienZ = 0.000000;
	KeyFrame[237].movJulien_x = -180.000000;
	KeyFrame[237].movJulien_y = 0.000000;
	KeyFrame[237].movJulien_z = -400.000000;
	KeyFrame[237].giroJulienX = 260.000000;
	KeyFrame[237].giroJulienY = 0.000000;
	KeyFrame[237].giroJulienZ = 0.000000;



	KeyFrame[238].movJulien_x = -180.000000;
	KeyFrame[238].movJulien_y = 0.000000;
	KeyFrame[238].movJulien_z = -400.000000;
	KeyFrame[238].giroJulienX = 260.000000;
	KeyFrame[238].giroJulienY = 0.000000;
	KeyFrame[238].giroJulienZ = 0.000000;



	KeyFrame[239].movJulien_x = -180.000000;
	KeyFrame[239].movJulien_y = 0.000000;
	KeyFrame[239].movJulien_z = -400.000000;
	KeyFrame[239].giroJulienX = 260.000000;
	KeyFrame[239].giroJulienY = 0.000000;
	KeyFrame[239].giroJulienZ = 0.000000;



	KeyFrame[240].movJulien_x = -180.000000;
	KeyFrame[240].movJulien_y = 0.000000;
	KeyFrame[240].movJulien_z = -400.000000;
	KeyFrame[240].giroJulienX = 260.000000;
	KeyFrame[240].giroJulienY = 0.000000;
	KeyFrame[240].giroJulienZ = 0.000000;



	KeyFrame[241].movJulien_x = -180.000000;
	KeyFrame[241].movJulien_y = 0.000000;
	KeyFrame[241].movJulien_z = -400.000000;
	KeyFrame[241].giroJulienX = 260.000000;
	KeyFrame[241].giroJulienY = 0.000000;
	KeyFrame[241].giroJulienZ = 0.000000;



	KeyFrame[242].movJulien_x = -180.000000;
	KeyFrame[242].movJulien_y = 0.000000;
	KeyFrame[242].movJulien_z = -400.000000;
	KeyFrame[242].giroJulienX = 260.000000;
	KeyFrame[242].giroJulienY = 0.000000;
	KeyFrame[242].giroJulienZ = 0.000000;



	KeyFrame[243].movJulien_x = -180.000000;
	KeyFrame[243].movJulien_y = 0.000000;
	KeyFrame[243].movJulien_z = -400.000000;
	KeyFrame[243].giroJulienX = 260.000000;
	KeyFrame[243].giroJulienY = 0.000000;
	KeyFrame[243].giroJulienZ = 0.000000;







	KeyFrame[244].movJulien_x = -200.000000;
	KeyFrame[244].movJulien_y = 0.000000;
	KeyFrame[244].movJulien_z = -400.000000;
	KeyFrame[244].giroJulienX = 260.000000;
	KeyFrame[244].giroJulienY = 0.000000;
	KeyFrame[244].giroJulienZ = 0.000000;



	KeyFrame[245].movJulien_x = -200.000000;
	KeyFrame[245].movJulien_y = 0.000000;
	KeyFrame[245].movJulien_z = -400.000000;
	KeyFrame[245].giroJulienX = 260.000000;
	KeyFrame[245].giroJulienY = 0.000000;
	KeyFrame[245].giroJulienZ = 0.000000;



	KeyFrame[246].movJulien_x = -200.000000;
	KeyFrame[246].movJulien_y = 0.000000;
	KeyFrame[246].movJulien_z = -400.000000;
	KeyFrame[246].giroJulienX = 260.000000;
	KeyFrame[246].giroJulienY = 0.000000;
	KeyFrame[246].giroJulienZ = 0.000000;



	KeyFrame[247].movJulien_x = -200.000000;
	KeyFrame[247].movJulien_y = 0.000000;
	KeyFrame[247].movJulien_z = -400.000000;
	KeyFrame[247].giroJulienX = 260.000000;
	KeyFrame[247].giroJulienY = 0.000000;
	KeyFrame[247].giroJulienZ = 0.000000;



	KeyFrame[248].movJulien_x = -200.000000;
	KeyFrame[248].movJulien_y = 0.000000;
	KeyFrame[248].movJulien_z = -400.000000;
	KeyFrame[248].giroJulienX = 260.000000;
	KeyFrame[248].giroJulienY = 0.000000;
	KeyFrame[248].giroJulienZ = 0.000000;







	KeyFrame[249].movJulien_x = -220.000000;
	KeyFrame[249].movJulien_y = 0.000000;
	KeyFrame[249].movJulien_z = -400.000000;
	KeyFrame[249].giroJulienX = 260.000000;
	KeyFrame[249].giroJulienY = 0.000000;
	KeyFrame[249].giroJulienZ = 0.000000;



	KeyFrame[250].movJulien_x = -220.000000;
	KeyFrame[250].movJulien_y = 0.000000;
	KeyFrame[250].movJulien_z = -400.000000;
	KeyFrame[250].giroJulienX = 260.000000;
	KeyFrame[250].giroJulienY = 0.000000;
	KeyFrame[250].giroJulienZ = 0.000000;



	KeyFrame[251].movJulien_x = -220.000000;
	KeyFrame[251].movJulien_y = 0.000000;
	KeyFrame[251].movJulien_z = -400.000000;
	KeyFrame[251].giroJulienX = 260.000000;
	KeyFrame[251].giroJulienY = 0.000000;
	KeyFrame[251].giroJulienZ = 0.000000;



	KeyFrame[252].movJulien_x = -220.000000;
	KeyFrame[252].movJulien_y = 0.000000;
	KeyFrame[252].movJulien_z = -400.000000;
	KeyFrame[252].giroJulienX = 260.000000;
	KeyFrame[252].giroJulienY = 0.000000;
	KeyFrame[252].giroJulienZ = 0.000000;



	KeyFrame[253].movJulien_x = -220.000000;
	KeyFrame[253].movJulien_y = 0.000000;
	KeyFrame[253].movJulien_z = -400.000000;
	KeyFrame[253].giroJulienX = 260.000000;
	KeyFrame[253].giroJulienY = 0.000000;
	KeyFrame[253].giroJulienZ = 0.000000;



	KeyFrame[254].movJulien_x = -220.000000;
	KeyFrame[254].movJulien_y = 0.000000;
	KeyFrame[254].movJulien_z = -400.000000;
	KeyFrame[254].giroJulienX = 260.000000;
	KeyFrame[254].giroJulienY = 0.000000;
	KeyFrame[254].giroJulienZ = 0.000000;



	KeyFrame[255].movJulien_x = -220.000000;
	KeyFrame[255].movJulien_y = 0.000000;
	KeyFrame[255].movJulien_z = -400.000000;
	KeyFrame[255].giroJulienX = 260.000000;
	KeyFrame[255].giroJulienY = 0.000000;
	KeyFrame[255].giroJulienZ = 0.000000;







	KeyFrame[256].movJulien_x = -240.000000;
	KeyFrame[256].movJulien_y = 0.000000;
	KeyFrame[256].movJulien_z = -400.000000;
	KeyFrame[256].giroJulienX = 260.000000;
	KeyFrame[256].giroJulienY = 0.000000;
	KeyFrame[256].giroJulienZ = 0.000000;



	KeyFrame[257].movJulien_x = -240.000000;
	KeyFrame[257].movJulien_y = 0.000000;
	KeyFrame[257].movJulien_z = -400.000000;
	KeyFrame[257].giroJulienX = 260.000000;
	KeyFrame[257].giroJulienY = 0.000000;
	KeyFrame[257].giroJulienZ = 0.000000;



	KeyFrame[258].movJulien_x = -240.000000;
	KeyFrame[258].movJulien_y = 0.000000;
	KeyFrame[258].movJulien_z = -400.000000;
	KeyFrame[258].giroJulienX = 260.000000;
	KeyFrame[258].giroJulienY = 0.000000;
	KeyFrame[258].giroJulienZ = 0.000000;



	KeyFrame[259].movJulien_x = -240.000000;
	KeyFrame[259].movJulien_y = 0.000000;
	KeyFrame[259].movJulien_z = -400.000000;
	KeyFrame[259].giroJulienX = 260.000000;
	KeyFrame[259].giroJulienY = 0.000000;
	KeyFrame[259].giroJulienZ = 0.000000;



	KeyFrame[260].movJulien_x = -240.000000;
	KeyFrame[260].movJulien_y = 0.000000;
	KeyFrame[260].movJulien_z = -400.000000;
	KeyFrame[260].giroJulienX = 260.000000;
	KeyFrame[260].giroJulienY = 0.000000;
	KeyFrame[260].giroJulienZ = 0.000000;



	KeyFrame[261].movJulien_x = -240.000000;
	KeyFrame[261].movJulien_y = 0.000000;
	KeyFrame[261].movJulien_z = -400.000000;
	KeyFrame[261].giroJulienX = 260.000000;
	KeyFrame[261].giroJulienY = 0.000000;
	KeyFrame[261].giroJulienZ = 0.000000;



	KeyFrame[262].movJulien_x = -240.000000;
	KeyFrame[262].movJulien_y = 0.000000;
	KeyFrame[262].movJulien_z = -400.000000;
	KeyFrame[262].giroJulienX = 260.000000;
	KeyFrame[262].giroJulienY = 0.000000;
	KeyFrame[262].giroJulienZ = 0.000000;



	KeyFrame[263].movJulien_x = -240.000000;
	KeyFrame[263].movJulien_y = 0.000000;
	KeyFrame[263].movJulien_z = -400.000000;
	KeyFrame[263].giroJulienX = 260.000000;
	KeyFrame[263].giroJulienY = 0.000000;
	KeyFrame[263].giroJulienZ = 0.000000;



	KeyFrame[264].movJulien_x = -240.000000;
	KeyFrame[264].movJulien_y = 0.000000;
	KeyFrame[264].movJulien_z = -400.000000;
	KeyFrame[264].giroJulienX = 260.000000;
	KeyFrame[264].giroJulienY = 0.000000;
	KeyFrame[264].giroJulienZ = 0.000000;







	KeyFrame[265].movJulien_x = -240.000000;
	KeyFrame[265].movJulien_y = 0.000000;
	KeyFrame[265].movJulien_z = -400.000000;
	KeyFrame[265].giroJulienX = 280.000000;
	KeyFrame[265].giroJulienY = 0.000000;
	KeyFrame[265].giroJulienZ = 0.000000;



	KeyFrame[266].movJulien_x = -240.000000;
	KeyFrame[266].movJulien_y = 0.000000;
	KeyFrame[266].movJulien_z = -400.000000;
	KeyFrame[266].giroJulienX = 280.000000;
	KeyFrame[266].giroJulienY = 0.000000;
	KeyFrame[266].giroJulienZ = 0.000000;



	KeyFrame[267].movJulien_x = -240.000000;
	KeyFrame[267].movJulien_y = 0.000000;
	KeyFrame[267].movJulien_z = -400.000000;
	KeyFrame[267].giroJulienX = 280.000000;
	KeyFrame[267].giroJulienY = 0.000000;
	KeyFrame[267].giroJulienZ = 0.000000;



	KeyFrame[268].movJulien_x = -240.000000;
	KeyFrame[268].movJulien_y = 0.000000;
	KeyFrame[268].movJulien_z = -400.000000;
	KeyFrame[268].giroJulienX = 280.000000;
	KeyFrame[268].giroJulienY = 0.000000;
	KeyFrame[268].giroJulienZ = 0.000000;







	KeyFrame[269].movJulien_x = -240.000000;
	KeyFrame[269].movJulien_y = 0.000000;
	KeyFrame[269].movJulien_z = -400.000000;
	KeyFrame[269].giroJulienX = 300.000000;
	KeyFrame[269].giroJulienY = 0.000000;
	KeyFrame[269].giroJulienZ = 0.000000;



	KeyFrame[270].movJulien_x = -240.000000;
	KeyFrame[270].movJulien_y = 0.000000;
	KeyFrame[270].movJulien_z = -400.000000;
	KeyFrame[270].giroJulienX = 300.000000;
	KeyFrame[270].giroJulienY = 0.000000;
	KeyFrame[270].giroJulienZ = 0.000000;







	KeyFrame[271].movJulien_x = -240.000000;
	KeyFrame[271].movJulien_y = 0.000000;
	KeyFrame[271].movJulien_z = -400.000000;
	KeyFrame[271].giroJulienX = 320.000000;
	KeyFrame[271].giroJulienY = 0.000000;
	KeyFrame[271].giroJulienZ = 0.000000;



	KeyFrame[272].movJulien_x = -240.000000;
	KeyFrame[272].movJulien_y = 0.000000;
	KeyFrame[272].movJulien_z = -400.000000;
	KeyFrame[272].giroJulienX = 320.000000;
	KeyFrame[272].giroJulienY = 0.000000;
	KeyFrame[272].giroJulienZ = 0.000000;



	KeyFrame[273].movJulien_x = -240.000000;
	KeyFrame[273].movJulien_y = 0.000000;
	KeyFrame[273].movJulien_z = -400.000000;
	KeyFrame[273].giroJulienX = 320.000000;
	KeyFrame[273].giroJulienY = 0.000000;
	KeyFrame[273].giroJulienZ = 0.000000;



	KeyFrame[274].movJulien_x = -240.000000;
	KeyFrame[274].movJulien_y = 0.000000;
	KeyFrame[274].movJulien_z = -400.000000;
	KeyFrame[274].giroJulienX = 320.000000;
	KeyFrame[274].giroJulienY = 0.000000;
	KeyFrame[274].giroJulienZ = 0.000000;



	KeyFrame[275].movJulien_x = -240.000000;
	KeyFrame[275].movJulien_y = 0.000000;
	KeyFrame[275].movJulien_z = -400.000000;
	KeyFrame[275].giroJulienX = 320.000000;
	KeyFrame[275].giroJulienY = 0.000000;
	KeyFrame[275].giroJulienZ = 0.000000;



	KeyFrame[276].movJulien_x = -240.000000;
	KeyFrame[276].movJulien_y = 0.000000;
	KeyFrame[276].movJulien_z = -400.000000;
	KeyFrame[276].giroJulienX = 320.000000;
	KeyFrame[276].giroJulienY = 0.000000;
	KeyFrame[276].giroJulienZ = 0.000000;







	KeyFrame[277].movJulien_x = -240.000000;
	KeyFrame[277].movJulien_y = 0.000000;
	KeyFrame[277].movJulien_z = -400.000000;
	KeyFrame[277].giroJulienX = 340.000000;
	KeyFrame[277].giroJulienY = 0.000000;
	KeyFrame[277].giroJulienZ = 0.000000;



	KeyFrame[278].movJulien_x = -240.000000;
	KeyFrame[278].movJulien_y = 0.000000;
	KeyFrame[278].movJulien_z = -400.000000;
	KeyFrame[278].giroJulienX = 340.000000;
	KeyFrame[278].giroJulienY = 0.000000;
	KeyFrame[278].giroJulienZ = 0.000000;



	KeyFrame[279].movJulien_x = -240.000000;
	KeyFrame[279].movJulien_y = 0.000000;
	KeyFrame[279].movJulien_z = -400.000000;
	KeyFrame[279].giroJulienX = 340.000000;
	KeyFrame[279].giroJulienY = 0.000000;
	KeyFrame[279].giroJulienZ = 0.000000;



	KeyFrame[280].movJulien_x = -240.000000;
	KeyFrame[280].movJulien_y = 0.000000;
	KeyFrame[280].movJulien_z = -400.000000;
	KeyFrame[280].giroJulienX = 340.000000;
	KeyFrame[280].giroJulienY = 0.000000;
	KeyFrame[280].giroJulienZ = 0.000000;



	KeyFrame[281].movJulien_x = -240.000000;
	KeyFrame[281].movJulien_y = 0.000000;
	KeyFrame[281].movJulien_z = -400.000000;
	KeyFrame[281].giroJulienX = 340.000000;
	KeyFrame[281].giroJulienY = 0.000000;
	KeyFrame[281].giroJulienZ = 0.000000;



	KeyFrame[282].movJulien_x = -240.000000;
	KeyFrame[282].movJulien_y = 0.000000;
	KeyFrame[282].movJulien_z = -400.000000;
	KeyFrame[282].giroJulienX = 340.000000;
	KeyFrame[282].giroJulienY = 0.000000;
	KeyFrame[282].giroJulienZ = 0.000000;







	KeyFrame[283].movJulien_x = -240.000000;
	KeyFrame[283].movJulien_y = 0.000000;
	KeyFrame[283].movJulien_z = -400.000000;
	KeyFrame[283].giroJulienX = 360.000000;
	KeyFrame[283].giroJulienY = 0.000000;
	KeyFrame[283].giroJulienZ = 0.000000;



	KeyFrame[284].movJulien_x = -240.000000;
	KeyFrame[284].movJulien_y = 0.000000;
	KeyFrame[284].movJulien_z = -400.000000;
	KeyFrame[284].giroJulienX = 360.000000;
	KeyFrame[284].giroJulienY = 0.000000;
	KeyFrame[284].giroJulienZ = 0.000000;



	KeyFrame[285].movJulien_x = -240.000000;
	KeyFrame[285].movJulien_y = 0.000000;
	KeyFrame[285].movJulien_z = -400.000000;
	KeyFrame[285].giroJulienX = 360.000000;
	KeyFrame[285].giroJulienY = 0.000000;
	KeyFrame[285].giroJulienZ = 0.000000;



	KeyFrame[286].movJulien_x = -240.000000;
	KeyFrame[286].movJulien_y = 0.000000;
	KeyFrame[286].movJulien_z = -400.000000;
	KeyFrame[286].giroJulienX = 360.000000;
	KeyFrame[286].giroJulienY = 0.000000;
	KeyFrame[286].giroJulienZ = 0.000000;



	KeyFrame[287].movJulien_x = -240.000000;
	KeyFrame[287].movJulien_y = 0.000000;
	KeyFrame[287].movJulien_z = -400.000000;
	KeyFrame[287].giroJulienX = 360.000000;
	KeyFrame[287].giroJulienY = 0.000000;
	KeyFrame[287].giroJulienZ = 0.000000;



	KeyFrame[288].movJulien_x = -240.000000;
	KeyFrame[288].movJulien_y = 0.000000;
	KeyFrame[288].movJulien_z = -400.000000;
	KeyFrame[288].giroJulienX = 360.000000;
	KeyFrame[288].giroJulienY = 0.000000;
	KeyFrame[288].giroJulienZ = 0.000000;



	KeyFrame[289].movJulien_x = -240.000000;
	KeyFrame[289].movJulien_y = 0.000000;
	KeyFrame[289].movJulien_z = -400.000000;
	KeyFrame[289].giroJulienX = 360.000000;
	KeyFrame[289].giroJulienY = 0.000000;
	KeyFrame[289].giroJulienZ = 0.000000;



	KeyFrame[290].movJulien_x = -240.000000;
	KeyFrame[290].movJulien_y = 0.000000;
	KeyFrame[290].movJulien_z = -400.000000;
	KeyFrame[290].giroJulienX = 360.000000;
	KeyFrame[290].giroJulienY = 0.000000;
	KeyFrame[290].giroJulienZ = 0.000000;



	KeyFrame[291].movJulien_x = -240.000000;
	KeyFrame[291].movJulien_y = 0.000000;
	KeyFrame[291].movJulien_z = -400.000000;
	KeyFrame[291].giroJulienX = 360.000000;
	KeyFrame[291].giroJulienY = 0.000000;
	KeyFrame[291].giroJulienZ = 0.000000;



	KeyFrame[292].movJulien_x = -240.000000;
	KeyFrame[292].movJulien_y = 0.000000;
	KeyFrame[292].movJulien_z = -400.000000;
	KeyFrame[292].giroJulienX = 360.000000;
	KeyFrame[292].giroJulienY = 0.000000;
	KeyFrame[292].giroJulienZ = 0.000000;



	KeyFrame[293].movJulien_x = -240.000000;
	KeyFrame[293].movJulien_y = 0.000000;
	KeyFrame[293].movJulien_z = -400.000000;
	KeyFrame[293].giroJulienX = 360.000000;
	KeyFrame[293].giroJulienY = 0.000000;
	KeyFrame[293].giroJulienZ = 0.000000;



	KeyFrame[294].movJulien_x = -240.000000;
	KeyFrame[294].movJulien_y = 0.000000;
	KeyFrame[294].movJulien_z = -400.000000;
	KeyFrame[294].giroJulienX = 360.000000;
	KeyFrame[294].giroJulienY = 0.000000;
	KeyFrame[294].giroJulienZ = 0.000000;











	KeyFrame[295].movJulien_x = -240.000000;
	KeyFrame[295].movJulien_y = 0.000000;
	KeyFrame[295].movJulien_z = -400.000000;
	KeyFrame[295].giroJulienX = 360.000000;
	KeyFrame[295].giroJulienY = 0.000000;
	KeyFrame[295].giroJulienZ = 0.000000;



	KeyFrame[296].movJulien_x = -240.000000;
	KeyFrame[296].movJulien_y = 0.000000;
	KeyFrame[296].movJulien_z = -400.000000;
	KeyFrame[296].giroJulienX = 360.000000;
	KeyFrame[296].giroJulienY = 0.000000;
	KeyFrame[296].giroJulienZ = 0.000000;



	KeyFrame[297].movJulien_x = -240.000000;
	KeyFrame[297].movJulien_y = 0.000000;
	KeyFrame[297].movJulien_z = -400.000000;
	KeyFrame[297].giroJulienX = 360.000000;
	KeyFrame[297].giroJulienY = 0.000000;
	KeyFrame[297].giroJulienZ = 0.000000;



	KeyFrame[298].movJulien_x = -240.000000;
	KeyFrame[298].movJulien_y = 0.000000;
	KeyFrame[298].movJulien_z = -400.000000;
	KeyFrame[298].giroJulienX = 360.000000;
	KeyFrame[298].giroJulienY = 0.000000;
	KeyFrame[298].giroJulienZ = 0.000000;



	KeyFrame[299].movJulien_x = -240.000000;
	KeyFrame[299].movJulien_y = 0.000000;
	KeyFrame[299].movJulien_z = -400.000000;
	KeyFrame[299].giroJulienX = 360.000000;
	KeyFrame[299].giroJulienY = 0.000000;
	KeyFrame[299].giroJulienZ = 0.000000;



	KeyFrame[300].movJulien_x = -240.000000;
	KeyFrame[300].movJulien_y = 0.000000;
	KeyFrame[300].movJulien_z = -400.000000;
	KeyFrame[300].giroJulienX = 360.000000;
	KeyFrame[300].giroJulienY = 0.000000;
	KeyFrame[300].giroJulienZ = 0.000000;



	KeyFrame[301].movJulien_x = -240.000000;
	KeyFrame[301].movJulien_y = 0.000000;
	KeyFrame[301].movJulien_z = -400.000000;
	KeyFrame[301].giroJulienX = 360.000000;
	KeyFrame[301].giroJulienY = 0.000000;
	KeyFrame[301].giroJulienZ = 0.000000;



	KeyFrame[302].movJulien_x = -240.000000;
	KeyFrame[302].movJulien_y = 0.000000;
	KeyFrame[302].movJulien_z = -400.000000;
	KeyFrame[302].giroJulienX = 360.000000;
	KeyFrame[302].giroJulienY = 0.000000;
	KeyFrame[302].giroJulienZ = 0.000000;







	KeyFrame[303].movJulien_x = -240.000000;
	KeyFrame[303].movJulien_y = 0.000000;
	KeyFrame[303].movJulien_z = -380.000000;
	KeyFrame[303].giroJulienX = 360.000000;
	KeyFrame[303].giroJulienY = 0.000000;
	KeyFrame[303].giroJulienZ = 0.000000;



	KeyFrame[304].movJulien_x = -240.000000;
	KeyFrame[304].movJulien_y = 0.000000;
	KeyFrame[304].movJulien_z = -380.000000;
	KeyFrame[304].giroJulienX = 360.000000;
	KeyFrame[304].giroJulienY = 0.000000;
	KeyFrame[304].giroJulienZ = 0.000000;



	KeyFrame[305].movJulien_x = -240.000000;
	KeyFrame[305].movJulien_y = 0.000000;
	KeyFrame[305].movJulien_z = -380.000000;
	KeyFrame[305].giroJulienX = 360.000000;
	KeyFrame[305].giroJulienY = 0.000000;
	KeyFrame[305].giroJulienZ = 0.000000;



	KeyFrame[306].movJulien_x = -240.000000;
	KeyFrame[306].movJulien_y = 0.000000;
	KeyFrame[306].movJulien_z = -380.000000;
	KeyFrame[306].giroJulienX = 360.000000;
	KeyFrame[306].giroJulienY = 0.000000;
	KeyFrame[306].giroJulienZ = 0.000000;



	KeyFrame[307].movJulien_x = -240.000000;
	KeyFrame[307].movJulien_y = 0.000000;
	KeyFrame[307].movJulien_z = -380.000000;
	KeyFrame[307].giroJulienX = 360.000000;
	KeyFrame[307].giroJulienY = 0.000000;
	KeyFrame[307].giroJulienZ = 0.000000;



	KeyFrame[308].movJulien_x = -240.000000;
	KeyFrame[308].movJulien_y = 0.000000;
	KeyFrame[308].movJulien_z = -380.000000;
	KeyFrame[308].giroJulienX = 360.000000;
	KeyFrame[308].giroJulienY = 0.000000;
	KeyFrame[308].giroJulienZ = 0.000000;



	KeyFrame[309].movJulien_x = -240.000000;
	KeyFrame[309].movJulien_y = 0.000000;
	KeyFrame[309].movJulien_z = -380.000000;
	KeyFrame[309].giroJulienX = 360.000000;
	KeyFrame[309].giroJulienY = 0.000000;
	KeyFrame[309].giroJulienZ = 0.000000;



	KeyFrame[310].movJulien_x = -240.000000;
	KeyFrame[310].movJulien_y = 0.000000;
	KeyFrame[310].movJulien_z = -380.000000;
	KeyFrame[310].giroJulienX = 360.000000;
	KeyFrame[310].giroJulienY = 0.000000;
	KeyFrame[310].giroJulienZ = 0.000000;



	KeyFrame[311].movJulien_x = -240.000000;
	KeyFrame[311].movJulien_y = 0.000000;
	KeyFrame[311].movJulien_z = -380.000000;
	KeyFrame[311].giroJulienX = 360.000000;
	KeyFrame[311].giroJulienY = 0.000000;
	KeyFrame[311].giroJulienZ = 0.000000;







	KeyFrame[312].movJulien_x = -240.000000;
	KeyFrame[312].movJulien_y = 0.000000;
	KeyFrame[312].movJulien_z = -360.000000;
	KeyFrame[312].giroJulienX = 360.000000;
	KeyFrame[312].giroJulienY = 0.000000;
	KeyFrame[312].giroJulienZ = 0.000000;



	KeyFrame[313].movJulien_x = -240.000000;
	KeyFrame[313].movJulien_y = 0.000000;
	KeyFrame[313].movJulien_z = -360.000000;
	KeyFrame[313].giroJulienX = 360.000000;
	KeyFrame[313].giroJulienY = 0.000000;
	KeyFrame[313].giroJulienZ = 0.000000;



	KeyFrame[314].movJulien_x = -240.000000;
	KeyFrame[314].movJulien_y = 0.000000;
	KeyFrame[314].movJulien_z = -360.000000;
	KeyFrame[314].giroJulienX = 360.000000;
	KeyFrame[314].giroJulienY = 0.000000;
	KeyFrame[314].giroJulienZ = 0.000000;



	KeyFrame[315].movJulien_x = -240.000000;
	KeyFrame[315].movJulien_y = 0.000000;
	KeyFrame[315].movJulien_z = -360.000000;
	KeyFrame[315].giroJulienX = 360.000000;
	KeyFrame[315].giroJulienY = 0.000000;
	KeyFrame[315].giroJulienZ = 0.000000;



	KeyFrame[316].movJulien_x = -240.000000;
	KeyFrame[316].movJulien_y = 0.000000;
	KeyFrame[316].movJulien_z = -360.000000;
	KeyFrame[316].giroJulienX = 360.000000;
	KeyFrame[316].giroJulienY = 0.000000;
	KeyFrame[316].giroJulienZ = 0.000000;



	KeyFrame[317].movJulien_x = -240.000000;
	KeyFrame[317].movJulien_y = 0.000000;
	KeyFrame[317].movJulien_z = -360.000000;
	KeyFrame[317].giroJulienX = 360.000000;
	KeyFrame[317].giroJulienY = 0.000000;
	KeyFrame[317].giroJulienZ = 0.000000;







	KeyFrame[318].movJulien_x = -240.000000;
	KeyFrame[318].movJulien_y = 0.000000;
	KeyFrame[318].movJulien_z = -340.000000;
	KeyFrame[318].giroJulienX = 360.000000;
	KeyFrame[318].giroJulienY = 0.000000;
	KeyFrame[318].giroJulienZ = 0.000000;



	KeyFrame[319].movJulien_x = -240.000000;
	KeyFrame[319].movJulien_y = 0.000000;
	KeyFrame[319].movJulien_z = -340.000000;
	KeyFrame[319].giroJulienX = 360.000000;
	KeyFrame[319].giroJulienY = 0.000000;
	KeyFrame[319].giroJulienZ = 0.000000;



	KeyFrame[320].movJulien_x = -240.000000;
	KeyFrame[320].movJulien_y = 0.000000;
	KeyFrame[320].movJulien_z = -340.000000;
	KeyFrame[320].giroJulienX = 360.000000;
	KeyFrame[320].giroJulienY = 0.000000;
	KeyFrame[320].giroJulienZ = 0.000000;







	KeyFrame[321].movJulien_x = -240.000000;
	KeyFrame[321].movJulien_y = 0.000000;
	KeyFrame[321].movJulien_z = -320.000000;
	KeyFrame[321].giroJulienX = 360.000000;
	KeyFrame[321].giroJulienY = 0.000000;
	KeyFrame[321].giroJulienZ = 0.000000;



	KeyFrame[322].movJulien_x = -240.000000;
	KeyFrame[322].movJulien_y = 0.000000;
	KeyFrame[322].movJulien_z = -320.000000;
	KeyFrame[322].giroJulienX = 360.000000;
	KeyFrame[322].giroJulienY = 0.000000;
	KeyFrame[322].giroJulienZ = 0.000000;



	KeyFrame[323].movJulien_x = -240.000000;
	KeyFrame[323].movJulien_y = 0.000000;
	KeyFrame[323].movJulien_z = -320.000000;
	KeyFrame[323].giroJulienX = 360.000000;
	KeyFrame[323].giroJulienY = 0.000000;
	KeyFrame[323].giroJulienZ = 0.000000;







	KeyFrame[324].movJulien_x = -240.000000;
	KeyFrame[324].movJulien_y = 0.000000;
	KeyFrame[324].movJulien_z = -300.000000;
	KeyFrame[324].giroJulienX = 360.000000;
	KeyFrame[324].giroJulienY = 0.000000;
	KeyFrame[324].giroJulienZ = 0.000000;







	KeyFrame[325].movJulien_x = -240.000000;
	KeyFrame[325].movJulien_y = 0.000000;
	KeyFrame[325].movJulien_z = -280.000000;
	KeyFrame[325].giroJulienX = 360.000000;
	KeyFrame[325].giroJulienY = 0.000000;
	KeyFrame[325].giroJulienZ = 0.000000;



	KeyFrame[326].movJulien_x = -240.000000;
	KeyFrame[326].movJulien_y = 0.000000;
	KeyFrame[326].movJulien_z = -280.000000;
	KeyFrame[326].giroJulienX = 360.000000;
	KeyFrame[326].giroJulienY = 0.000000;
	KeyFrame[326].giroJulienZ = 0.000000;



	KeyFrame[327].movJulien_x = -240.000000;
	KeyFrame[327].movJulien_y = 0.000000;
	KeyFrame[327].movJulien_z = -280.000000;
	KeyFrame[327].giroJulienX = 360.000000;
	KeyFrame[327].giroJulienY = 0.000000;
	KeyFrame[327].giroJulienZ = 0.000000;



	KeyFrame[328].movJulien_x = -240.000000;
	KeyFrame[328].movJulien_y = 0.000000;
	KeyFrame[328].movJulien_z = -280.000000;
	KeyFrame[328].giroJulienX = 360.000000;
	KeyFrame[328].giroJulienY = 0.000000;
	KeyFrame[328].giroJulienZ = 0.000000;



	KeyFrame[329].movJulien_x = -240.000000;
	KeyFrame[329].movJulien_y = 0.000000;
	KeyFrame[329].movJulien_z = -280.000000;
	KeyFrame[329].giroJulienX = 360.000000;
	KeyFrame[329].giroJulienY = 0.000000;
	KeyFrame[329].giroJulienZ = 0.000000;







	KeyFrame[330].movJulien_x = -240.000000;
	KeyFrame[330].movJulien_y = 0.000000;
	KeyFrame[330].movJulien_z = -260.000000;
	KeyFrame[330].giroJulienX = 360.000000;
	KeyFrame[330].giroJulienY = 0.000000;
	KeyFrame[330].giroJulienZ = 0.000000;



	KeyFrame[331].movJulien_x = -240.000000;
	KeyFrame[331].movJulien_y = 0.000000;
	KeyFrame[331].movJulien_z = -260.000000;
	KeyFrame[331].giroJulienX = 360.000000;
	KeyFrame[331].giroJulienY = 0.000000;
	KeyFrame[331].giroJulienZ = 0.000000;



	KeyFrame[332].movJulien_x = -240.000000;
	KeyFrame[332].movJulien_y = 0.000000;
	KeyFrame[332].movJulien_z = -260.000000;
	KeyFrame[332].giroJulienX = 360.000000;
	KeyFrame[332].giroJulienY = 0.000000;
	KeyFrame[332].giroJulienZ = 0.000000;



	KeyFrame[333].movJulien_x = -240.000000;
	KeyFrame[333].movJulien_y = 0.000000;
	KeyFrame[333].movJulien_z = -260.000000;
	KeyFrame[333].giroJulienX = 360.000000;
	KeyFrame[333].giroJulienY = 0.000000;
	KeyFrame[333].giroJulienZ = 0.000000;



	KeyFrame[334].movJulien_x = -240.000000;
	KeyFrame[334].movJulien_y = 0.000000;
	KeyFrame[334].movJulien_z = -260.000000;
	KeyFrame[334].giroJulienX = 360.000000;
	KeyFrame[334].giroJulienY = 0.000000;
	KeyFrame[334].giroJulienZ = 0.000000;



	KeyFrame[335].movJulien_x = -240.000000;
	KeyFrame[335].movJulien_y = 0.000000;
	KeyFrame[335].movJulien_z = -260.000000;
	KeyFrame[335].giroJulienX = 360.000000;
	KeyFrame[335].giroJulienY = 0.000000;
	KeyFrame[335].giroJulienZ = 0.000000;



	KeyFrame[336].movJulien_x = -240.000000;
	KeyFrame[336].movJulien_y = 0.000000;
	KeyFrame[336].movJulien_z = -260.000000;
	KeyFrame[336].giroJulienX = 360.000000;
	KeyFrame[336].giroJulienY = 0.000000;
	KeyFrame[336].giroJulienZ = 0.000000;



	KeyFrame[337].movJulien_x = -240.000000;
	KeyFrame[337].movJulien_y = 0.000000;
	KeyFrame[337].movJulien_z = -260.000000;
	KeyFrame[337].giroJulienX = 360.000000;
	KeyFrame[337].giroJulienY = 0.000000;
	KeyFrame[337].giroJulienZ = 0.000000;



	KeyFrame[338].movJulien_x = -240.000000;
	KeyFrame[338].movJulien_y = 0.000000;
	KeyFrame[338].movJulien_z = -260.000000;
	KeyFrame[338].giroJulienX = 360.000000;
	KeyFrame[338].giroJulienY = 0.000000;
	KeyFrame[338].giroJulienZ = 0.000000;



	KeyFrame[339].movJulien_x = -240.000000;
	KeyFrame[339].movJulien_y = 0.000000;
	KeyFrame[339].movJulien_z = -260.000000;
	KeyFrame[339].giroJulienX = 360.000000;
	KeyFrame[339].giroJulienY = 0.000000;
	KeyFrame[339].giroJulienZ = 0.000000;







	KeyFrame[340].movJulien_x = -240.000000;
	KeyFrame[340].movJulien_y = 0.000000;
	KeyFrame[340].movJulien_z = -240.000000;
	KeyFrame[340].giroJulienX = 360.000000;
	KeyFrame[340].giroJulienY = 0.000000;
	KeyFrame[340].giroJulienZ = 0.000000;



	KeyFrame[341].movJulien_x = -240.000000;
	KeyFrame[341].movJulien_y = 0.000000;
	KeyFrame[341].movJulien_z = -240.000000;
	KeyFrame[341].giroJulienX = 360.000000;
	KeyFrame[341].giroJulienY = 0.000000;
	KeyFrame[341].giroJulienZ = 0.000000;



	KeyFrame[342].movJulien_x = -240.000000;
	KeyFrame[342].movJulien_y = 0.000000;
	KeyFrame[342].movJulien_z = -240.000000;
	KeyFrame[342].giroJulienX = 360.000000;
	KeyFrame[342].giroJulienY = 0.000000;
	KeyFrame[342].giroJulienZ = 0.000000;



	KeyFrame[343].movJulien_x = -240.000000;
	KeyFrame[343].movJulien_y = 0.000000;
	KeyFrame[343].movJulien_z = -240.000000;
	KeyFrame[343].giroJulienX = 360.000000;
	KeyFrame[343].giroJulienY = 0.000000;
	KeyFrame[343].giroJulienZ = 0.000000;



	KeyFrame[344].movJulien_x = -240.000000;
	KeyFrame[344].movJulien_y = 0.000000;
	KeyFrame[344].movJulien_z = -240.000000;
	KeyFrame[344].giroJulienX = 360.000000;
	KeyFrame[344].giroJulienY = 0.000000;
	KeyFrame[344].giroJulienZ = 0.000000;



	KeyFrame[345].movJulien_x = -240.000000;
	KeyFrame[345].movJulien_y = 0.000000;
	KeyFrame[345].movJulien_z = -240.000000;
	KeyFrame[345].giroJulienX = 360.000000;
	KeyFrame[345].giroJulienY = 0.000000;
	KeyFrame[345].giroJulienZ = 0.000000;



	KeyFrame[346].movJulien_x = -240.000000;
	KeyFrame[346].movJulien_y = 0.000000;
	KeyFrame[346].movJulien_z = -240.000000;
	KeyFrame[346].giroJulienX = 360.000000;
	KeyFrame[346].giroJulienY = 0.000000;
	KeyFrame[346].giroJulienZ = 0.000000;







	KeyFrame[347].movJulien_x = -240.000000;
	KeyFrame[347].movJulien_y = 0.000000;
	KeyFrame[347].movJulien_z = -220.000000;
	KeyFrame[347].giroJulienX = 360.000000;
	KeyFrame[347].giroJulienY = 0.000000;
	KeyFrame[347].giroJulienZ = 0.000000;



	KeyFrame[348].movJulien_x = -240.000000;
	KeyFrame[348].movJulien_y = 0.000000;
	KeyFrame[348].movJulien_z = -220.000000;
	KeyFrame[348].giroJulienX = 360.000000;
	KeyFrame[348].giroJulienY = 0.000000;
	KeyFrame[348].giroJulienZ = 0.000000;



	KeyFrame[349].movJulien_x = -240.000000;
	KeyFrame[349].movJulien_y = 0.000000;
	KeyFrame[349].movJulien_z = -220.000000;
	KeyFrame[349].giroJulienX = 360.000000;
	KeyFrame[349].giroJulienY = 0.000000;
	KeyFrame[349].giroJulienZ = 0.000000;



	KeyFrame[350].movJulien_x = -240.000000;
	KeyFrame[350].movJulien_y = 0.000000;
	KeyFrame[350].movJulien_z = -220.000000;
	KeyFrame[350].giroJulienX = 360.000000;
	KeyFrame[350].giroJulienY = 0.000000;
	KeyFrame[350].giroJulienZ = 0.000000;



	KeyFrame[351].movJulien_x = -240.000000;
	KeyFrame[351].movJulien_y = 0.000000;
	KeyFrame[351].movJulien_z = -220.000000;
	KeyFrame[351].giroJulienX = 360.000000;
	KeyFrame[351].giroJulienY = 0.000000;
	KeyFrame[351].giroJulienZ = 0.000000;



	KeyFrame[352].movJulien_x = -240.000000;
	KeyFrame[352].movJulien_y = 0.000000;
	KeyFrame[352].movJulien_z = -220.000000;
	KeyFrame[352].giroJulienX = 360.000000;
	KeyFrame[352].giroJulienY = 0.000000;
	KeyFrame[352].giroJulienZ = 0.000000;



	KeyFrame[353].movJulien_x = -240.000000;
	KeyFrame[353].movJulien_y = 0.000000;
	KeyFrame[353].movJulien_z = -220.000000;
	KeyFrame[353].giroJulienX = 360.000000;
	KeyFrame[353].giroJulienY = 0.000000;
	KeyFrame[353].giroJulienZ = 0.000000;



	KeyFrame[354].movJulien_x = -240.000000;
	KeyFrame[354].movJulien_y = 0.000000;
	KeyFrame[354].movJulien_z = -220.000000;
	KeyFrame[354].giroJulienX = 360.000000;
	KeyFrame[354].giroJulienY = 0.000000;
	KeyFrame[354].giroJulienZ = 0.000000;







	KeyFrame[355].movJulien_x = -240.000000;
	KeyFrame[355].movJulien_y = 0.000000;
	KeyFrame[355].movJulien_z = -200.000000;
	KeyFrame[355].giroJulienX = 360.000000;
	KeyFrame[355].giroJulienY = 0.000000;
	KeyFrame[355].giroJulienZ = 0.000000;



	KeyFrame[356].movJulien_x = -240.000000;
	KeyFrame[356].movJulien_y = 0.000000;
	KeyFrame[356].movJulien_z = -200.000000;
	KeyFrame[356].giroJulienX = 360.000000;
	KeyFrame[356].giroJulienY = 0.000000;
	KeyFrame[356].giroJulienZ = 0.000000;



	KeyFrame[357].movJulien_x = -240.000000;
	KeyFrame[357].movJulien_y = 0.000000;
	KeyFrame[357].movJulien_z = -200.000000;
	KeyFrame[357].giroJulienX = 360.000000;
	KeyFrame[357].giroJulienY = 0.000000;
	KeyFrame[357].giroJulienZ = 0.000000;



	KeyFrame[358].movJulien_x = -240.000000;
	KeyFrame[358].movJulien_y = 0.000000;
	KeyFrame[358].movJulien_z = -200.000000;
	KeyFrame[358].giroJulienX = 360.000000;
	KeyFrame[358].giroJulienY = 0.000000;
	KeyFrame[358].giroJulienZ = 0.000000;



	KeyFrame[359].movJulien_x = -240.000000;
	KeyFrame[359].movJulien_y = 0.000000;
	KeyFrame[359].movJulien_z = -200.000000;
	KeyFrame[359].giroJulienX = 360.000000;
	KeyFrame[359].giroJulienY = 0.000000;
	KeyFrame[359].giroJulienZ = 0.000000;



	KeyFrame[360].movJulien_x = -240.000000;
	KeyFrame[360].movJulien_y = 0.000000;
	KeyFrame[360].movJulien_z = -200.000000;
	KeyFrame[360].giroJulienX = 360.000000;
	KeyFrame[360].giroJulienY = 0.000000;
	KeyFrame[360].giroJulienZ = 0.000000;



	KeyFrame[361].movJulien_x = -240.000000;
	KeyFrame[361].movJulien_y = 0.000000;
	KeyFrame[361].movJulien_z = -200.000000;
	KeyFrame[361].giroJulienX = 360.000000;
	KeyFrame[361].giroJulienY = 0.000000;
	KeyFrame[361].giroJulienZ = 0.000000;



	KeyFrame[362].movJulien_x = -240.000000;
	KeyFrame[362].movJulien_y = 0.000000;
	KeyFrame[362].movJulien_z = -200.000000;
	KeyFrame[362].giroJulienX = 360.000000;
	KeyFrame[362].giroJulienY = 0.000000;
	KeyFrame[362].giroJulienZ = 0.000000;



	KeyFrame[363].movJulien_x = -240.000000;
	KeyFrame[363].movJulien_y = 0.000000;
	KeyFrame[363].movJulien_z = -200.000000;
	KeyFrame[363].giroJulienX = 360.000000;
	KeyFrame[363].giroJulienY = 0.000000;
	KeyFrame[363].giroJulienZ = 0.000000;



	KeyFrame[364].movJulien_x = -240.000000;
	KeyFrame[364].movJulien_y = 0.000000;
	KeyFrame[364].movJulien_z = -200.000000;
	KeyFrame[364].giroJulienX = 360.000000;
	KeyFrame[364].giroJulienY = 0.000000;
	KeyFrame[364].giroJulienZ = 0.000000;



	KeyFrame[365].movJulien_x = -240.000000;
	KeyFrame[365].movJulien_y = 0.000000;
	KeyFrame[365].movJulien_z = -200.000000;
	KeyFrame[365].giroJulienX = 360.000000;
	KeyFrame[365].giroJulienY = 0.000000;
	KeyFrame[365].giroJulienZ = 0.000000;







	KeyFrame[366].movJulien_x = -240.000000;
	KeyFrame[366].movJulien_y = 0.000000;
	KeyFrame[366].movJulien_z = -180.000000;
	KeyFrame[366].giroJulienX = 360.000000;
	KeyFrame[366].giroJulienY = 0.000000;
	KeyFrame[366].giroJulienZ = 0.000000;



	KeyFrame[367].movJulien_x = -240.000000;
	KeyFrame[367].movJulien_y = 0.000000;
	KeyFrame[367].movJulien_z = -180.000000;
	KeyFrame[367].giroJulienX = 360.000000;
	KeyFrame[367].giroJulienY = 0.000000;
	KeyFrame[367].giroJulienZ = 0.000000;



	KeyFrame[368].movJulien_x = -240.000000;
	KeyFrame[368].movJulien_y = 0.000000;
	KeyFrame[368].movJulien_z = -180.000000;
	KeyFrame[368].giroJulienX = 360.000000;
	KeyFrame[368].giroJulienY = 0.000000;
	KeyFrame[368].giroJulienZ = 0.000000;



	KeyFrame[369].movJulien_x = -240.000000;
	KeyFrame[369].movJulien_y = 0.000000;
	KeyFrame[369].movJulien_z = -180.000000;
	KeyFrame[369].giroJulienX = 360.000000;
	KeyFrame[369].giroJulienY = 0.000000;
	KeyFrame[369].giroJulienZ = 0.000000;



	KeyFrame[370].movJulien_x = -240.000000;
	KeyFrame[370].movJulien_y = 0.000000;
	KeyFrame[370].movJulien_z = -180.000000;
	KeyFrame[370].giroJulienX = 360.000000;
	KeyFrame[370].giroJulienY = 0.000000;
	KeyFrame[370].giroJulienZ = 0.000000;



	KeyFrame[371].movJulien_x = -240.000000;
	KeyFrame[371].movJulien_y = 0.000000;
	KeyFrame[371].movJulien_z = -180.000000;
	KeyFrame[371].giroJulienX = 360.000000;
	KeyFrame[371].giroJulienY = 0.000000;
	KeyFrame[371].giroJulienZ = 0.000000;



	KeyFrame[372].movJulien_x = -240.000000;
	KeyFrame[372].movJulien_y = 0.000000;
	KeyFrame[372].movJulien_z = -180.000000;
	KeyFrame[372].giroJulienX = 360.000000;
	KeyFrame[372].giroJulienY = 0.000000;
	KeyFrame[372].giroJulienZ = 0.000000;



	KeyFrame[373].movJulien_x = -240.000000;
	KeyFrame[373].movJulien_y = 0.000000;
	KeyFrame[373].movJulien_z = -180.000000;
	KeyFrame[373].giroJulienX = 360.000000;
	KeyFrame[373].giroJulienY = 0.000000;
	KeyFrame[373].giroJulienZ = 0.000000;







	KeyFrame[374].movJulien_x = -240.000000;
	KeyFrame[374].movJulien_y = 0.000000;
	KeyFrame[374].movJulien_z = -160.000000;
	KeyFrame[374].giroJulienX = 360.000000;
	KeyFrame[374].giroJulienY = 0.000000;
	KeyFrame[374].giroJulienZ = 0.000000;



	KeyFrame[375].movJulien_x = -240.000000;
	KeyFrame[375].movJulien_y = 0.000000;
	KeyFrame[375].movJulien_z = -160.000000;
	KeyFrame[375].giroJulienX = 360.000000;
	KeyFrame[375].giroJulienY = 0.000000;
	KeyFrame[375].giroJulienZ = 0.000000;



	KeyFrame[376].movJulien_x = -240.000000;
	KeyFrame[376].movJulien_y = 0.000000;
	KeyFrame[376].movJulien_z = -160.000000;
	KeyFrame[376].giroJulienX = 360.000000;
	KeyFrame[376].giroJulienY = 0.000000;
	KeyFrame[376].giroJulienZ = 0.000000;



	KeyFrame[377].movJulien_x = -240.000000;
	KeyFrame[377].movJulien_y = 0.000000;
	KeyFrame[377].movJulien_z = -160.000000;
	KeyFrame[377].giroJulienX = 360.000000;
	KeyFrame[377].giroJulienY = 0.000000;
	KeyFrame[377].giroJulienZ = 0.000000;



	KeyFrame[378].movJulien_x = -240.000000;
	KeyFrame[378].movJulien_y = 0.000000;
	KeyFrame[378].movJulien_z = -160.000000;
	KeyFrame[378].giroJulienX = 360.000000;
	KeyFrame[378].giroJulienY = 0.000000;
	KeyFrame[378].giroJulienZ = 0.000000;



	KeyFrame[379].movJulien_x = -240.000000;
	KeyFrame[379].movJulien_y = 0.000000;
	KeyFrame[379].movJulien_z = -160.000000;
	KeyFrame[379].giroJulienX = 360.000000;
	KeyFrame[379].giroJulienY = 0.000000;
	KeyFrame[379].giroJulienZ = 0.000000;



	KeyFrame[380].movJulien_x = -240.000000;
	KeyFrame[380].movJulien_y = 0.000000;
	KeyFrame[380].movJulien_z = -160.000000;
	KeyFrame[380].giroJulienX = 360.000000;
	KeyFrame[380].giroJulienY = 0.000000;
	KeyFrame[380].giroJulienZ = 0.000000;



	KeyFrame[381].movJulien_x = -240.000000;
	KeyFrame[381].movJulien_y = 0.000000;
	KeyFrame[381].movJulien_z = -160.000000;
	KeyFrame[381].giroJulienX = 360.000000;
	KeyFrame[381].giroJulienY = 0.000000;
	KeyFrame[381].giroJulienZ = 0.000000;



	KeyFrame[382].movJulien_x = -240.000000;
	KeyFrame[382].movJulien_y = 0.000000;
	KeyFrame[382].movJulien_z = -160.000000;
	KeyFrame[382].giroJulienX = 360.000000;
	KeyFrame[382].giroJulienY = 0.000000;
	KeyFrame[382].giroJulienZ = 0.000000;



	KeyFrame[383].movJulien_x = -240.000000;
	KeyFrame[383].movJulien_y = 0.000000;
	KeyFrame[383].movJulien_z = -160.000000;
	KeyFrame[383].giroJulienX = 360.000000;
	KeyFrame[383].giroJulienY = 0.000000;
	KeyFrame[383].giroJulienZ = 0.000000;







	KeyFrame[384].movJulien_x = -240.000000;
	KeyFrame[384].movJulien_y = 0.000000;
	KeyFrame[384].movJulien_z = -140.000000;
	KeyFrame[384].giroJulienX = 360.000000;
	KeyFrame[384].giroJulienY = 0.000000;
	KeyFrame[384].giroJulienZ = 0.000000;



	KeyFrame[385].movJulien_x = -240.000000;
	KeyFrame[385].movJulien_y = 0.000000;
	KeyFrame[385].movJulien_z = -140.000000;
	KeyFrame[385].giroJulienX = 360.000000;
	KeyFrame[385].giroJulienY = 0.000000;
	KeyFrame[385].giroJulienZ = 0.000000;



	KeyFrame[386].movJulien_x = -240.000000;
	KeyFrame[386].movJulien_y = 0.000000;
	KeyFrame[386].movJulien_z = -140.000000;
	KeyFrame[386].giroJulienX = 360.000000;
	KeyFrame[386].giroJulienY = 0.000000;
	KeyFrame[386].giroJulienZ = 0.000000;



	KeyFrame[387].movJulien_x = -240.000000;
	KeyFrame[387].movJulien_y = 0.000000;
	KeyFrame[387].movJulien_z = -140.000000;
	KeyFrame[387].giroJulienX = 360.000000;
	KeyFrame[387].giroJulienY = 0.000000;
	KeyFrame[387].giroJulienZ = 0.000000;



	KeyFrame[388].movJulien_x = -240.000000;
	KeyFrame[388].movJulien_y = 0.000000;
	KeyFrame[388].movJulien_z = -140.000000;
	KeyFrame[388].giroJulienX = 360.000000;
	KeyFrame[388].giroJulienY = 0.000000;
	KeyFrame[388].giroJulienZ = 0.000000;



	KeyFrame[389].movJulien_x = -240.000000;
	KeyFrame[389].movJulien_y = 0.000000;
	KeyFrame[389].movJulien_z = -140.000000;
	KeyFrame[389].giroJulienX = 360.000000;
	KeyFrame[389].giroJulienY = 0.000000;
	KeyFrame[389].giroJulienZ = 0.000000;



	KeyFrame[390].movJulien_x = -240.000000;
	KeyFrame[390].movJulien_y = 0.000000;
	KeyFrame[390].movJulien_z = -140.000000;
	KeyFrame[390].giroJulienX = 360.000000;
	KeyFrame[390].giroJulienY = 0.000000;
	KeyFrame[390].giroJulienZ = 0.000000;







	KeyFrame[391].movJulien_x = -240.000000;
	KeyFrame[391].movJulien_y = 0.000000;
	KeyFrame[391].movJulien_z = -120.000000;
	KeyFrame[391].giroJulienX = 360.000000;
	KeyFrame[391].giroJulienY = 0.000000;
	KeyFrame[391].giroJulienZ = 0.000000;



	KeyFrame[392].movJulien_x = -240.000000;
	KeyFrame[392].movJulien_y = 0.000000;
	KeyFrame[392].movJulien_z = -120.000000;
	KeyFrame[392].giroJulienX = 360.000000;
	KeyFrame[392].giroJulienY = 0.000000;
	KeyFrame[392].giroJulienZ = 0.000000;



	KeyFrame[393].movJulien_x = -240.000000;
	KeyFrame[393].movJulien_y = 0.000000;
	KeyFrame[393].movJulien_z = -120.000000;
	KeyFrame[393].giroJulienX = 360.000000;
	KeyFrame[393].giroJulienY = 0.000000;
	KeyFrame[393].giroJulienZ = 0.000000;



	KeyFrame[394].movJulien_x = -240.000000;
	KeyFrame[394].movJulien_y = 0.000000;
	KeyFrame[394].movJulien_z = -120.000000;
	KeyFrame[394].giroJulienX = 360.000000;
	KeyFrame[394].giroJulienY = 0.000000;
	KeyFrame[394].giroJulienZ = 0.000000;



	KeyFrame[395].movJulien_x = -240.000000;
	KeyFrame[395].movJulien_y = 0.000000;
	KeyFrame[395].movJulien_z = -120.000000;
	KeyFrame[395].giroJulienX = 360.000000;
	KeyFrame[395].giroJulienY = 0.000000;
	KeyFrame[395].giroJulienZ = 0.000000;



	KeyFrame[396].movJulien_x = -240.000000;
	KeyFrame[396].movJulien_y = 0.000000;
	KeyFrame[396].movJulien_z = -120.000000;
	KeyFrame[396].giroJulienX = 360.000000;
	KeyFrame[396].giroJulienY = 0.000000;
	KeyFrame[396].giroJulienZ = 0.000000;



	KeyFrame[397].movJulien_x = -240.000000;
	KeyFrame[397].movJulien_y = 0.000000;
	KeyFrame[397].movJulien_z = -120.000000;
	KeyFrame[397].giroJulienX = 360.000000;
	KeyFrame[397].giroJulienY = 0.000000;
	KeyFrame[397].giroJulienZ = 0.000000;







	KeyFrame[398].movJulien_x = -240.000000;
	KeyFrame[398].movJulien_y = 0.000000;
	KeyFrame[398].movJulien_z = -100.000000;
	KeyFrame[398].giroJulienX = 360.000000;
	KeyFrame[398].giroJulienY = 0.000000;
	KeyFrame[398].giroJulienZ = 0.000000;



	KeyFrame[399].movJulien_x = -240.000000;
	KeyFrame[399].movJulien_y = 0.000000;
	KeyFrame[399].movJulien_z = -100.000000;
	KeyFrame[399].giroJulienX = 360.000000;
	KeyFrame[399].giroJulienY = 0.000000;
	KeyFrame[399].giroJulienZ = 0.000000;



	KeyFrame[400].movJulien_x = -240.000000;
	KeyFrame[400].movJulien_y = 0.000000;
	KeyFrame[400].movJulien_z = -100.000000;
	KeyFrame[400].giroJulienX = 360.000000;
	KeyFrame[400].giroJulienY = 0.000000;
	KeyFrame[400].giroJulienZ = 0.000000;



	KeyFrame[401].movJulien_x = -240.000000;
	KeyFrame[401].movJulien_y = 0.000000;
	KeyFrame[401].movJulien_z = -100.000000;
	KeyFrame[401].giroJulienX = 360.000000;
	KeyFrame[401].giroJulienY = 0.000000;
	KeyFrame[401].giroJulienZ = 0.000000;



	KeyFrame[402].movJulien_x = -240.000000;
	KeyFrame[402].movJulien_y = 0.000000;
	KeyFrame[402].movJulien_z = -100.000000;
	KeyFrame[402].giroJulienX = 360.000000;
	KeyFrame[402].giroJulienY = 0.000000;
	KeyFrame[402].giroJulienZ = 0.000000;



	KeyFrame[403].movJulien_x = -240.000000;
	KeyFrame[403].movJulien_y = 0.000000;
	KeyFrame[403].movJulien_z = -100.000000;
	KeyFrame[403].giroJulienX = 360.000000;
	KeyFrame[403].giroJulienY = 0.000000;
	KeyFrame[403].giroJulienZ = 0.000000;



	KeyFrame[404].movJulien_x = -240.000000;
	KeyFrame[404].movJulien_y = 0.000000;
	KeyFrame[404].movJulien_z = -100.000000;
	KeyFrame[404].giroJulienX = 360.000000;
	KeyFrame[404].giroJulienY = 0.000000;
	KeyFrame[404].giroJulienZ = 0.000000;



	KeyFrame[405].movJulien_x = -240.000000;
	KeyFrame[405].movJulien_y = 0.000000;
	KeyFrame[405].movJulien_z = -100.000000;
	KeyFrame[405].giroJulienX = 360.000000;
	KeyFrame[405].giroJulienY = 0.000000;
	KeyFrame[405].giroJulienZ = 0.000000;







	KeyFrame[406].movJulien_x = -240.000000;
	KeyFrame[406].movJulien_y = 0.000000;
	KeyFrame[406].movJulien_z = -80.000000;
	KeyFrame[406].giroJulienX = 360.000000;
	KeyFrame[406].giroJulienY = 0.000000;
	KeyFrame[406].giroJulienZ = 0.000000;



	KeyFrame[407].movJulien_x = -240.000000;
	KeyFrame[407].movJulien_y = 0.000000;
	KeyFrame[407].movJulien_z = -80.000000;
	KeyFrame[407].giroJulienX = 360.000000;
	KeyFrame[407].giroJulienY = 0.000000;
	KeyFrame[407].giroJulienZ = 0.000000;



	KeyFrame[408].movJulien_x = -240.000000;
	KeyFrame[408].movJulien_y = 0.000000;
	KeyFrame[408].movJulien_z = -80.000000;
	KeyFrame[408].giroJulienX = 360.000000;
	KeyFrame[408].giroJulienY = 0.000000;
	KeyFrame[408].giroJulienZ = 0.000000;







	KeyFrame[409].movJulien_x = -240.000000;
	KeyFrame[409].movJulien_y = 0.000000;
	KeyFrame[409].movJulien_z = -60.000000;
	KeyFrame[409].giroJulienX = 360.000000;
	KeyFrame[409].giroJulienY = 0.000000;
	KeyFrame[409].giroJulienZ = 0.000000;



	KeyFrame[410].movJulien_x = -240.000000;
	KeyFrame[410].movJulien_y = 0.000000;
	KeyFrame[410].movJulien_z = -60.000000;
	KeyFrame[410].giroJulienX = 360.000000;
	KeyFrame[410].giroJulienY = 0.000000;
	KeyFrame[410].giroJulienZ = 0.000000;



	KeyFrame[411].movJulien_x = -240.000000;
	KeyFrame[411].movJulien_y = 0.000000;
	KeyFrame[411].movJulien_z = -60.000000;
	KeyFrame[411].giroJulienX = 360.000000;
	KeyFrame[411].giroJulienY = 0.000000;
	KeyFrame[411].giroJulienZ = 0.000000;



	KeyFrame[412].movJulien_x = -240.000000;
	KeyFrame[412].movJulien_y = 0.000000;
	KeyFrame[412].movJulien_z = -60.000000;
	KeyFrame[412].giroJulienX = 360.000000;
	KeyFrame[412].giroJulienY = 0.000000;
	KeyFrame[412].giroJulienZ = 0.000000;



	KeyFrame[413].movJulien_x = -240.000000;
	KeyFrame[413].movJulien_y = 0.000000;
	KeyFrame[413].movJulien_z = -60.000000;
	KeyFrame[413].giroJulienX = 360.000000;
	KeyFrame[413].giroJulienY = 0.000000;
	KeyFrame[413].giroJulienZ = 0.000000;



	KeyFrame[414].movJulien_x = -240.000000;
	KeyFrame[414].movJulien_y = 0.000000;
	KeyFrame[414].movJulien_z = -60.000000;
	KeyFrame[414].giroJulienX = 360.000000;
	KeyFrame[414].giroJulienY = 0.000000;
	KeyFrame[414].giroJulienZ = 0.000000;



	KeyFrame[415].movJulien_x = -240.000000;
	KeyFrame[415].movJulien_y = 0.000000;
	KeyFrame[415].movJulien_z = -60.000000;
	KeyFrame[415].giroJulienX = 360.000000;
	KeyFrame[415].giroJulienY = 0.000000;
	KeyFrame[415].giroJulienZ = 0.000000;



	KeyFrame[416].movJulien_x = -240.000000;
	KeyFrame[416].movJulien_y = 0.000000;
	KeyFrame[416].movJulien_z = -60.000000;
	KeyFrame[416].giroJulienX = 360.000000;
	KeyFrame[416].giroJulienY = 0.000000;
	KeyFrame[416].giroJulienZ = 0.000000;



	KeyFrame[417].movJulien_x = -240.000000;
	KeyFrame[417].movJulien_y = 0.000000;
	KeyFrame[417].movJulien_z = -60.000000;
	KeyFrame[417].giroJulienX = 360.000000;
	KeyFrame[417].giroJulienY = 0.000000;
	KeyFrame[417].giroJulienZ = 0.000000;



	KeyFrame[418].movJulien_x = -240.000000;
	KeyFrame[418].movJulien_y = 0.000000;
	KeyFrame[418].movJulien_z = -60.000000;
	KeyFrame[418].giroJulienX = 360.000000;
	KeyFrame[418].giroJulienY = 0.000000;
	KeyFrame[418].giroJulienZ = 0.000000;



	KeyFrame[419].movJulien_x = -240.000000;
	KeyFrame[419].movJulien_y = 0.000000;
	KeyFrame[419].movJulien_z = -60.000000;
	KeyFrame[419].giroJulienX = 360.000000;
	KeyFrame[419].giroJulienY = 0.000000;
	KeyFrame[419].giroJulienZ = 0.000000;



	KeyFrame[420].movJulien_x = -240.000000;
	KeyFrame[420].movJulien_y = 0.000000;
	KeyFrame[420].movJulien_z = -60.000000;
	KeyFrame[420].giroJulienX = 360.000000;
	KeyFrame[420].giroJulienY = 0.000000;
	KeyFrame[420].giroJulienZ = 0.000000;



	KeyFrame[421].movJulien_x = -240.000000;
	KeyFrame[421].movJulien_y = 0.000000;
	KeyFrame[421].movJulien_z = -60.000000;
	KeyFrame[421].giroJulienX = 360.000000;
	KeyFrame[421].giroJulienY = 0.000000;
	KeyFrame[421].giroJulienZ = 0.000000;



	KeyFrame[422].movJulien_x = -240.000000;
	KeyFrame[422].movJulien_y = 0.000000;
	KeyFrame[422].movJulien_z = -60.000000;
	KeyFrame[422].giroJulienX = 360.000000;
	KeyFrame[422].giroJulienY = 0.000000;
	KeyFrame[422].giroJulienZ = 0.000000;







	KeyFrame[423].movJulien_x = -240.000000;
	KeyFrame[423].movJulien_y = 0.000000;
	KeyFrame[423].movJulien_z = -40.000000;
	KeyFrame[423].giroJulienX = 360.000000;
	KeyFrame[423].giroJulienY = 0.000000;
	KeyFrame[423].giroJulienZ = 0.000000;



	KeyFrame[424].movJulien_x = -240.000000;
	KeyFrame[424].movJulien_y = 0.000000;
	KeyFrame[424].movJulien_z = -40.000000;
	KeyFrame[424].giroJulienX = 360.000000;
	KeyFrame[424].giroJulienY = 0.000000;
	KeyFrame[424].giroJulienZ = 0.000000;



	KeyFrame[425].movJulien_x = -240.000000;
	KeyFrame[425].movJulien_y = 0.000000;
	KeyFrame[425].movJulien_z = -40.000000;
	KeyFrame[425].giroJulienX = 360.000000;
	KeyFrame[425].giroJulienY = 0.000000;
	KeyFrame[425].giroJulienZ = 0.000000;







	KeyFrame[426].movJulien_x = -240.000000;
	KeyFrame[426].movJulien_y = 0.000000;
	KeyFrame[426].movJulien_z = -20.000000;
	KeyFrame[426].giroJulienX = 360.000000;
	KeyFrame[426].giroJulienY = 0.000000;
	KeyFrame[426].giroJulienZ = 0.000000;



	KeyFrame[427].movJulien_x = -240.000000;
	KeyFrame[427].movJulien_y = 0.000000;
	KeyFrame[427].movJulien_z = -20.000000;
	KeyFrame[427].giroJulienX = 360.000000;
	KeyFrame[427].giroJulienY = 0.000000;
	KeyFrame[427].giroJulienZ = 0.000000;











	KeyFrame[428].movJulien_x = -240.000000;
	KeyFrame[428].movJulien_y = 0.000000;
	KeyFrame[428].movJulien_z = -20.000000;
	KeyFrame[428].giroJulienX = 400.000000;
	KeyFrame[428].giroJulienY = 0.000000;
	KeyFrame[428].giroJulienZ = 0.000000;



	KeyFrame[429].movJulien_x = -240.000000;
	KeyFrame[429].movJulien_y = 0.000000;
	KeyFrame[429].movJulien_z = -20.000000;
	KeyFrame[429].giroJulienX = 400.000000;
	KeyFrame[429].giroJulienY = 0.000000;
	KeyFrame[429].giroJulienZ = 0.000000;



	KeyFrame[430].movJulien_x = -240.000000;
	KeyFrame[430].movJulien_y = 0.000000;
	KeyFrame[430].movJulien_z = -20.000000;
	KeyFrame[430].giroJulienX = 400.000000;
	KeyFrame[430].giroJulienY = 0.000000;
	KeyFrame[430].giroJulienZ = 0.000000;



	KeyFrame[431].movJulien_x = -240.000000;
	KeyFrame[431].movJulien_y = 0.000000;
	KeyFrame[431].movJulien_z = -20.000000;
	KeyFrame[431].giroJulienX = 400.000000;
	KeyFrame[431].giroJulienY = 0.000000;
	KeyFrame[431].giroJulienZ = 0.000000;



	KeyFrame[432].movJulien_x = -240.000000;
	KeyFrame[432].movJulien_y = 0.000000;
	KeyFrame[432].movJulien_z = -20.000000;
	KeyFrame[432].giroJulienX = 400.000000;
	KeyFrame[432].giroJulienY = 0.000000;
	KeyFrame[432].giroJulienZ = 0.000000;



	KeyFrame[433].movJulien_x = -240.000000;
	KeyFrame[433].movJulien_y = 0.000000;
	KeyFrame[433].movJulien_z = -20.000000;
	KeyFrame[433].giroJulienX = 400.000000;
	KeyFrame[433].giroJulienY = 0.000000;
	KeyFrame[433].giroJulienZ = 0.000000;



	KeyFrame[434].movJulien_x = -240.000000;
	KeyFrame[434].movJulien_y = 0.000000;
	KeyFrame[434].movJulien_z = -20.000000;
	KeyFrame[434].giroJulienX = 400.000000;
	KeyFrame[434].giroJulienY = 0.000000;
	KeyFrame[434].giroJulienZ = 0.000000;



	KeyFrame[435].movJulien_x = -240.000000;
	KeyFrame[435].movJulien_y = 0.000000;
	KeyFrame[435].movJulien_z = -20.000000;
	KeyFrame[435].giroJulienX = 400.000000;
	KeyFrame[435].giroJulienY = 0.000000;
	KeyFrame[435].giroJulienZ = 0.000000;



	KeyFrame[436].movJulien_x = -240.000000;
	KeyFrame[436].movJulien_y = 0.000000;
	KeyFrame[436].movJulien_z = -20.000000;
	KeyFrame[436].giroJulienX = 400.000000;
	KeyFrame[436].giroJulienY = 0.000000;
	KeyFrame[436].giroJulienZ = 0.000000;



	KeyFrame[437].movJulien_x = -240.000000;
	KeyFrame[437].movJulien_y = 0.000000;
	KeyFrame[437].movJulien_z = -20.000000;
	KeyFrame[437].giroJulienX = 400.000000;
	KeyFrame[437].giroJulienY = 0.000000;
	KeyFrame[437].giroJulienZ = 0.000000;


	KeyFrame[438].movJulien_x = -240.000000;
	KeyFrame[438].movJulien_y = 0.000000;
	KeyFrame[438].movJulien_z = -20.000000;
	KeyFrame[438].giroJulienX = 440.000000;
	KeyFrame[438].giroJulienY = 0.000000;
	KeyFrame[438].giroJulienZ = 0.000000;



	KeyFrame[439].movJulien_x = -240.000000;
	KeyFrame[439].movJulien_y = 0.000000;
	KeyFrame[439].movJulien_z = -20.000000;
	KeyFrame[439].giroJulienX = 440.000000;
	KeyFrame[439].giroJulienY = 0.000000;
	KeyFrame[439].giroJulienZ = 0.000000;



	KeyFrame[440].movJulien_x = -240.000000;
	KeyFrame[440].movJulien_y = 0.000000;
	KeyFrame[440].movJulien_z = -20.000000;
	KeyFrame[440].giroJulienX = 440.000000;
	KeyFrame[440].giroJulienY = 0.000000;
	KeyFrame[440].giroJulienZ = 0.000000;



	KeyFrame[441].movJulien_x = -240.000000;
	KeyFrame[441].movJulien_y = 0.000000;
	KeyFrame[441].movJulien_z = -20.000000;
	KeyFrame[441].giroJulienX = 440.000000;
	KeyFrame[441].giroJulienY = 0.000000;
	KeyFrame[441].giroJulienZ = 0.000000;



	KeyFrame[442].movJulien_x = -240.000000;
	KeyFrame[442].movJulien_y = 0.000000;
	KeyFrame[442].movJulien_z = -20.000000;
	KeyFrame[442].giroJulienX = 440.000000;
	KeyFrame[442].giroJulienY = 0.000000;
	KeyFrame[442].giroJulienZ = 0.000000;



	KeyFrame[443].movJulien_x = -240.000000;
	KeyFrame[443].movJulien_y = 0.000000;
	KeyFrame[443].movJulien_z = -20.000000;
	KeyFrame[443].giroJulienX = 440.000000;
	KeyFrame[443].giroJulienY = 0.000000;
	KeyFrame[443].giroJulienZ = 0.000000;

	KeyFrame[444].movJulien_x = -220.000000;
	KeyFrame[444].movJulien_y = 0.000000;
	KeyFrame[444].movJulien_z = -20.000000;
	KeyFrame[444].giroJulienX = 440.000000;
	KeyFrame[444].giroJulienY = 0.000000;
	KeyFrame[444].giroJulienZ = 0.000000;



	KeyFrame[445].movJulien_x = -220.000000;
	KeyFrame[445].movJulien_y = 0.000000;
	KeyFrame[445].movJulien_z = -20.000000;
	KeyFrame[445].giroJulienX = 440.000000;
	KeyFrame[445].giroJulienY = 0.000000;
	KeyFrame[445].giroJulienZ = 0.000000;



	KeyFrame[446].movJulien_x = -220.000000;
	KeyFrame[446].movJulien_y = 0.000000;
	KeyFrame[446].movJulien_z = -20.000000;
	KeyFrame[446].giroJulienX = 440.000000;
	KeyFrame[446].giroJulienY = 0.000000;
	KeyFrame[446].giroJulienZ = 0.000000;



	KeyFrame[447].movJulien_x = -220.000000;
	KeyFrame[447].movJulien_y = 0.000000;
	KeyFrame[447].movJulien_z = -20.000000;
	KeyFrame[447].giroJulienX = 440.000000;
	KeyFrame[447].giroJulienY = 0.000000;
	KeyFrame[447].giroJulienZ = 0.000000;



	KeyFrame[448].movJulien_x = -220.000000;
	KeyFrame[448].movJulien_y = 0.000000;
	KeyFrame[448].movJulien_z = -20.000000;
	KeyFrame[448].giroJulienX = 440.000000;
	KeyFrame[448].giroJulienY = 0.000000;
	KeyFrame[448].giroJulienZ = 0.000000;



	KeyFrame[449].movJulien_x = -220.000000;
	KeyFrame[449].movJulien_y = 0.000000;
	KeyFrame[449].movJulien_z = -20.000000;
	KeyFrame[449].giroJulienX = 440.000000;
	KeyFrame[449].giroJulienY = 0.000000;
	KeyFrame[449].giroJulienZ = 0.000000;



	KeyFrame[450].movJulien_x = -220.000000;
	KeyFrame[450].movJulien_y = 0.000000;
	KeyFrame[450].movJulien_z = -20.000000;
	KeyFrame[450].giroJulienX = 440.000000;
	KeyFrame[450].giroJulienY = 0.000000;
	KeyFrame[450].giroJulienZ = 0.000000;



	KeyFrame[451].movJulien_x = -220.000000;
	KeyFrame[451].movJulien_y = 0.000000;
	KeyFrame[451].movJulien_z = -20.000000;
	KeyFrame[451].giroJulienX = 440.000000;
	KeyFrame[451].giroJulienY = 0.000000;
	KeyFrame[451].giroJulienZ = 0.000000;



	KeyFrame[452].movJulien_x = -220.000000;
	KeyFrame[452].movJulien_y = 0.000000;
	KeyFrame[452].movJulien_z = -20.000000;
	KeyFrame[452].giroJulienX = 440.000000;
	KeyFrame[452].giroJulienY = 0.000000;
	KeyFrame[452].giroJulienZ = 0.000000;



	KeyFrame[453].movJulien_x = -220.000000;
	KeyFrame[453].movJulien_y = 0.000000;
	KeyFrame[453].movJulien_z = -20.000000;
	KeyFrame[453].giroJulienX = 440.000000;
	KeyFrame[453].giroJulienY = 0.000000;
	KeyFrame[453].giroJulienZ = 0.000000;



	KeyFrame[454].movJulien_x = -220.000000;
	KeyFrame[454].movJulien_y = 0.000000;
	KeyFrame[454].movJulien_z = -20.000000;
	KeyFrame[454].giroJulienX = 440.000000;
	KeyFrame[454].giroJulienY = 0.000000;
	KeyFrame[454].giroJulienZ = 0.000000;







	KeyFrame[455].movJulien_x = -200.000000;
	KeyFrame[455].movJulien_y = 0.000000;
	KeyFrame[455].movJulien_z = -20.000000;
	KeyFrame[455].giroJulienX = 440.000000;
	KeyFrame[455].giroJulienY = 0.000000;
	KeyFrame[455].giroJulienZ = 0.000000;



	KeyFrame[456].movJulien_x = -200.000000;
	KeyFrame[456].movJulien_y = 0.000000;
	KeyFrame[456].movJulien_z = -20.000000;
	KeyFrame[456].giroJulienX = 440.000000;
	KeyFrame[456].giroJulienY = 0.000000;
	KeyFrame[456].giroJulienZ = 0.000000;



	KeyFrame[457].movJulien_x = -200.000000;
	KeyFrame[457].movJulien_y = 0.000000;
	KeyFrame[457].movJulien_z = -20.000000;
	KeyFrame[457].giroJulienX = 440.000000;
	KeyFrame[457].giroJulienY = 0.000000;
	KeyFrame[457].giroJulienZ = 0.000000;



	KeyFrame[458].movJulien_x = -200.000000;
	KeyFrame[458].movJulien_y = 0.000000;
	KeyFrame[458].movJulien_z = -20.000000;
	KeyFrame[458].giroJulienX = 440.000000;
	KeyFrame[458].giroJulienY = 0.000000;
	KeyFrame[458].giroJulienZ = 0.000000;



	KeyFrame[459].movJulien_x = -200.000000;
	KeyFrame[459].movJulien_y = 0.000000;
	KeyFrame[459].movJulien_z = -20.000000;
	KeyFrame[459].giroJulienX = 440.000000;
	KeyFrame[459].giroJulienY = 0.000000;
	KeyFrame[459].giroJulienZ = 0.000000;



	KeyFrame[460].movJulien_x = -200.000000;
	KeyFrame[460].movJulien_y = 0.000000;
	KeyFrame[460].movJulien_z = -20.000000;
	KeyFrame[460].giroJulienX = 440.000000;
	KeyFrame[460].giroJulienY = 0.000000;
	KeyFrame[460].giroJulienZ = 0.000000;



	KeyFrame[461].movJulien_x = -200.000000;
	KeyFrame[461].movJulien_y = 0.000000;
	KeyFrame[461].movJulien_z = -20.000000;
	KeyFrame[461].giroJulienX = 440.000000;
	KeyFrame[461].giroJulienY = 0.000000;
	KeyFrame[461].giroJulienZ = 0.000000;



	KeyFrame[462].movJulien_x = -200.000000;
	KeyFrame[462].movJulien_y = 0.000000;
	KeyFrame[462].movJulien_z = -20.000000;
	KeyFrame[462].giroJulienX = 440.000000;
	KeyFrame[462].giroJulienY = 0.000000;
	KeyFrame[462].giroJulienZ = 0.000000;



	KeyFrame[463].movJulien_x = -200.000000;
	KeyFrame[463].movJulien_y = 0.000000;
	KeyFrame[463].movJulien_z = -20.000000;
	KeyFrame[463].giroJulienX = 440.000000;
	KeyFrame[463].giroJulienY = 0.000000;
	KeyFrame[463].giroJulienZ = 0.000000;



	KeyFrame[464].movJulien_x = -200.000000;
	KeyFrame[464].movJulien_y = 0.000000;
	KeyFrame[464].movJulien_z = -20.000000;
	KeyFrame[464].giroJulienX = 440.000000;
	KeyFrame[464].giroJulienY = 0.000000;
	KeyFrame[464].giroJulienZ = 0.000000;







	KeyFrame[465].movJulien_x = -180.000000;
	KeyFrame[465].movJulien_y = 0.000000;
	KeyFrame[465].movJulien_z = -20.000000;
	KeyFrame[465].giroJulienX = 440.000000;
	KeyFrame[465].giroJulienY = 0.000000;
	KeyFrame[465].giroJulienZ = 0.000000;



	KeyFrame[466].movJulien_x = -180.000000;
	KeyFrame[466].movJulien_y = 0.000000;
	KeyFrame[466].movJulien_z = -20.000000;
	KeyFrame[466].giroJulienX = 440.000000;
	KeyFrame[466].giroJulienY = 0.000000;
	KeyFrame[466].giroJulienZ = 0.000000;



	KeyFrame[467].movJulien_x = -180.000000;
	KeyFrame[467].movJulien_y = 0.000000;
	KeyFrame[467].movJulien_z = -20.000000;
	KeyFrame[467].giroJulienX = 440.000000;
	KeyFrame[467].giroJulienY = 0.000000;
	KeyFrame[467].giroJulienZ = 0.000000;



	KeyFrame[468].movJulien_x = -180.000000;
	KeyFrame[468].movJulien_y = 0.000000;
	KeyFrame[468].movJulien_z = -20.000000;
	KeyFrame[468].giroJulienX = 440.000000;
	KeyFrame[468].giroJulienY = 0.000000;
	KeyFrame[468].giroJulienZ = 0.000000;



	KeyFrame[469].movJulien_x = -180.000000;
	KeyFrame[469].movJulien_y = 0.000000;
	KeyFrame[469].movJulien_z = -20.000000;
	KeyFrame[469].giroJulienX = 440.000000;
	KeyFrame[469].giroJulienY = 0.000000;
	KeyFrame[469].giroJulienZ = 0.000000;



	KeyFrame[470].movJulien_x = -180.000000;
	KeyFrame[470].movJulien_y = 0.000000;
	KeyFrame[470].movJulien_z = -20.000000;
	KeyFrame[470].giroJulienX = 440.000000;
	KeyFrame[470].giroJulienY = 0.000000;
	KeyFrame[470].giroJulienZ = 0.000000;



	KeyFrame[471].movJulien_x = -180.000000;
	KeyFrame[471].movJulien_y = 0.000000;
	KeyFrame[471].movJulien_z = -20.000000;
	KeyFrame[471].giroJulienX = 440.000000;
	KeyFrame[471].giroJulienY = 0.000000;
	KeyFrame[471].giroJulienZ = 0.000000;



	KeyFrame[472].movJulien_x = -180.000000;
	KeyFrame[472].movJulien_y = 0.000000;
	KeyFrame[472].movJulien_z = -20.000000;
	KeyFrame[472].giroJulienX = 440.000000;
	KeyFrame[472].giroJulienY = 0.000000;
	KeyFrame[472].giroJulienZ = 0.000000;







	KeyFrame[473].movJulien_x = -160.000000;
	KeyFrame[473].movJulien_y = 0.000000;
	KeyFrame[473].movJulien_z = -20.000000;
	KeyFrame[473].giroJulienX = 440.000000;
	KeyFrame[473].giroJulienY = 0.000000;
	KeyFrame[473].giroJulienZ = 0.000000;



	KeyFrame[474].movJulien_x = -160.000000;
	KeyFrame[474].movJulien_y = 0.000000;
	KeyFrame[474].movJulien_z = -20.000000;
	KeyFrame[474].giroJulienX = 440.000000;
	KeyFrame[474].giroJulienY = 0.000000;
	KeyFrame[474].giroJulienZ = 0.000000;



	KeyFrame[475].movJulien_x = -160.000000;
	KeyFrame[475].movJulien_y = 0.000000;
	KeyFrame[475].movJulien_z = -20.000000;
	KeyFrame[475].giroJulienX = 440.000000;
	KeyFrame[475].giroJulienY = 0.000000;
	KeyFrame[475].giroJulienZ = 0.000000;



	KeyFrame[476].movJulien_x = -160.000000;
	KeyFrame[476].movJulien_y = 0.000000;
	KeyFrame[476].movJulien_z = -20.000000;
	KeyFrame[476].giroJulienX = 440.000000;
	KeyFrame[476].giroJulienY = 0.000000;
	KeyFrame[476].giroJulienZ = 0.000000;



	KeyFrame[477].movJulien_x = -160.000000;
	KeyFrame[477].movJulien_y = 0.000000;
	KeyFrame[477].movJulien_z = -20.000000;
	KeyFrame[477].giroJulienX = 440.000000;
	KeyFrame[477].giroJulienY = 0.000000;
	KeyFrame[477].giroJulienZ = 0.000000;



	KeyFrame[478].movJulien_x = -160.000000;
	KeyFrame[478].movJulien_y = 0.000000;
	KeyFrame[478].movJulien_z = -20.000000;
	KeyFrame[478].giroJulienX = 440.000000;
	KeyFrame[478].giroJulienY = 0.000000;
	KeyFrame[478].giroJulienZ = 0.000000;







	KeyFrame[479].movJulien_x = -140.000000;
	KeyFrame[479].movJulien_y = 0.000000;
	KeyFrame[479].movJulien_z = -20.000000;
	KeyFrame[479].giroJulienX = 440.000000;
	KeyFrame[479].giroJulienY = 0.000000;
	KeyFrame[479].giroJulienZ = 0.000000;



	KeyFrame[480].movJulien_x = -140.000000;
	KeyFrame[480].movJulien_y = 0.000000;
	KeyFrame[480].movJulien_z = -20.000000;
	KeyFrame[480].giroJulienX = 440.000000;
	KeyFrame[480].giroJulienY = 0.000000;
	KeyFrame[480].giroJulienZ = 0.000000;



	KeyFrame[481].movJulien_x = -140.000000;
	KeyFrame[481].movJulien_y = 0.000000;
	KeyFrame[481].movJulien_z = -20.000000;
	KeyFrame[481].giroJulienX = 440.000000;
	KeyFrame[481].giroJulienY = 0.000000;
	KeyFrame[481].giroJulienZ = 0.000000;



	KeyFrame[482].movJulien_x = -140.000000;
	KeyFrame[482].movJulien_y = 0.000000;
	KeyFrame[482].movJulien_z = -20.000000;
	KeyFrame[482].giroJulienX = 440.000000;
	KeyFrame[482].giroJulienY = 0.000000;
	KeyFrame[482].giroJulienZ = 0.000000;



	KeyFrame[483].movJulien_x = -140.000000;
	KeyFrame[483].movJulien_y = 0.000000;
	KeyFrame[483].movJulien_z = -20.000000;
	KeyFrame[483].giroJulienX = 440.000000;
	KeyFrame[483].giroJulienY = 0.000000;
	KeyFrame[483].giroJulienZ = 0.000000;



	KeyFrame[484].movJulien_x = -140.000000;
	KeyFrame[484].movJulien_y = 0.000000;
	KeyFrame[484].movJulien_z = -20.000000;
	KeyFrame[484].giroJulienX = 440.000000;
	KeyFrame[484].giroJulienY = 0.000000;
	KeyFrame[484].giroJulienZ = 0.000000;



	KeyFrame[485].movJulien_x = -140.000000;
	KeyFrame[485].movJulien_y = 0.000000;
	KeyFrame[485].movJulien_z = -20.000000;
	KeyFrame[485].giroJulienX = 440.000000;
	KeyFrame[485].giroJulienY = 0.000000;
	KeyFrame[485].giroJulienZ = 0.000000;







	KeyFrame[486].movJulien_x = -120.000000;
	KeyFrame[486].movJulien_y = 0.000000;
	KeyFrame[486].movJulien_z = -20.000000;
	KeyFrame[486].giroJulienX = 440.000000;
	KeyFrame[486].giroJulienY = 0.000000;
	KeyFrame[486].giroJulienZ = 0.000000;



	KeyFrame[487].movJulien_x = -120.000000;
	KeyFrame[487].movJulien_y = 0.000000;
	KeyFrame[487].movJulien_z = -20.000000;
	KeyFrame[487].giroJulienX = 440.000000;
	KeyFrame[487].giroJulienY = 0.000000;
	KeyFrame[487].giroJulienZ = 0.000000;



	KeyFrame[488].movJulien_x = -120.000000;
	KeyFrame[488].movJulien_y = 0.000000;
	KeyFrame[488].movJulien_z = -20.000000;
	KeyFrame[488].giroJulienX = 440.000000;
	KeyFrame[488].giroJulienY = 0.000000;
	KeyFrame[488].giroJulienZ = 0.000000;



	KeyFrame[489].movJulien_x = -120.000000;
	KeyFrame[489].movJulien_y = 0.000000;
	KeyFrame[489].movJulien_z = -20.000000;
	KeyFrame[489].giroJulienX = 440.000000;
	KeyFrame[489].giroJulienY = 0.000000;
	KeyFrame[489].giroJulienZ = 0.000000;



	KeyFrame[490].movJulien_x = -120.000000;
	KeyFrame[490].movJulien_y = 0.000000;
	KeyFrame[490].movJulien_z = -20.000000;
	KeyFrame[490].giroJulienX = 440.000000;
	KeyFrame[490].giroJulienY = 0.000000;
	KeyFrame[490].giroJulienZ = 0.000000;



	KeyFrame[491].movJulien_x = -120.000000;
	KeyFrame[491].movJulien_y = 0.000000;
	KeyFrame[491].movJulien_z = -20.000000;
	KeyFrame[491].giroJulienX = 440.000000;
	KeyFrame[491].giroJulienY = 0.000000;
	KeyFrame[491].giroJulienZ = 0.000000;



	KeyFrame[492].movJulien_x = -120.000000;
	KeyFrame[492].movJulien_y = 0.000000;
	KeyFrame[492].movJulien_z = -20.000000;
	KeyFrame[492].giroJulienX = 440.000000;
	KeyFrame[492].giroJulienY = 0.000000;
	KeyFrame[492].giroJulienZ = 0.000000;



	KeyFrame[493].movJulien_x = -120.000000;
	KeyFrame[493].movJulien_y = 0.000000;
	KeyFrame[493].movJulien_z = -20.000000;
	KeyFrame[493].giroJulienX = 440.000000;
	KeyFrame[493].giroJulienY = 0.000000;
	KeyFrame[493].giroJulienZ = 0.000000;

	KeyFrame[494].movJulien_x = -80.000000;
	KeyFrame[494].movJulien_y = 0.000000;
	KeyFrame[494].movJulien_z = -20.000000;
	KeyFrame[494].giroJulienX = 440.000000;
	KeyFrame[494].giroJulienY = 0.000000;
	KeyFrame[494].giroJulienZ = 0.000000;

	KeyFrame[495].movJulien_x = -80.000000;
	KeyFrame[495].movJulien_y = 0.000000;
	KeyFrame[495].movJulien_z = -20.000000;
	KeyFrame[495].giroJulienX = 440.000000;
	KeyFrame[495].giroJulienY = 0.000000;
	KeyFrame[495].giroJulienZ = 0.000000;

	KeyFrame[496].movJulien_x = -80.000000;
	KeyFrame[496].movJulien_y = 0.000000;
	KeyFrame[496].movJulien_z = -20.000000;
	KeyFrame[496].giroJulienX = 440.000000;
	KeyFrame[496].giroJulienY = 0.000000;
	KeyFrame[496].giroJulienZ = 0.000000;

	KeyFrame[497].movJulien_x = -80.000000;
	KeyFrame[497].movJulien_y = 0.000000;
	KeyFrame[497].movJulien_z = -20.000000;
	KeyFrame[497].giroJulienX = 440.000000;
	KeyFrame[497].giroJulienY = 0.000000;
	KeyFrame[497].giroJulienZ = 0.000000;

	KeyFrame[498].movJulien_x = -80.000000;
	KeyFrame[498].movJulien_y = 0.000000;
	KeyFrame[498].movJulien_z = -20.000000;
	KeyFrame[498].giroJulienX = 440.000000;
	KeyFrame[498].giroJulienY = 0.000000;
	KeyFrame[498].giroJulienZ = 0.000000;

	KeyFrame[499].movJulien_x = -80.000000;
	KeyFrame[499].movJulien_y = 0.000000;
	KeyFrame[499].movJulien_z = -20.000000;
	KeyFrame[499].giroJulienX = 440.000000;
	KeyFrame[499].giroJulienY = 0.000000;
	KeyFrame[499].giroJulienZ = 0.000000;

	KeyFrame[500].movJulien_x = -80.000000;
	KeyFrame[500].movJulien_y = 0.000000;
	KeyFrame[500].movJulien_z = -20.000000;
	KeyFrame[500].giroJulienX = 440.000000;
	KeyFrame[500].giroJulienY = 0.000000;
	KeyFrame[500].giroJulienZ = 0.000000;

	KeyFrame[501].movJulien_x = -80.000000;
	KeyFrame[501].movJulien_y = 0.000000;
	KeyFrame[501].movJulien_z = -20.000000;
	KeyFrame[501].giroJulienX = 440.000000;
	KeyFrame[501].giroJulienY = 0.000000;
	KeyFrame[501].giroJulienZ = 0.000000;



	KeyFrame[502].movJulien_x = -80.000000;
	KeyFrame[502].movJulien_y = 0.000000;
	KeyFrame[502].movJulien_z = -20.000000;
	KeyFrame[502].giroJulienX = 440.000000;
	KeyFrame[502].giroJulienY = 0.000000;
	KeyFrame[502].giroJulienZ = 0.000000;



	KeyFrame[503].movJulien_x = -80.000000;
	KeyFrame[503].movJulien_y = 0.000000;
	KeyFrame[503].movJulien_z = -20.000000;
	KeyFrame[503].giroJulienX = 440.000000;
	KeyFrame[503].giroJulienY = 0.000000;
	KeyFrame[503].giroJulienZ = 0.000000;



	KeyFrame[504].movJulien_x = -80.000000;
	KeyFrame[504].movJulien_y = 0.000000;
	KeyFrame[504].movJulien_z = -20.000000;
	KeyFrame[504].giroJulienX = 440.000000;
	KeyFrame[504].giroJulienY = 0.000000;
	KeyFrame[504].giroJulienZ = 0.000000;



	KeyFrame[505].movJulien_x = -80.000000;
	KeyFrame[505].movJulien_y = 0.000000;
	KeyFrame[505].movJulien_z = -20.000000;
	KeyFrame[505].giroJulienX = 440.000000;
	KeyFrame[505].giroJulienY = 0.000000;
	KeyFrame[505].giroJulienZ = 0.000000;



	KeyFrame[506].movJulien_x = -80.000000;
	KeyFrame[506].movJulien_y = 0.000000;
	KeyFrame[506].movJulien_z = -20.000000;
	KeyFrame[506].giroJulienX = 440.000000;
	KeyFrame[506].giroJulienY = 0.000000;
	KeyFrame[506].giroJulienZ = 0.000000;

	KeyFrame[507].movJulien_x = -60.000000;
	KeyFrame[507].movJulien_y = 0.000000;
	KeyFrame[507].movJulien_z = -20.000000;
	KeyFrame[507].giroJulienX = 440.000000;
	KeyFrame[507].giroJulienY = 0.000000;
	KeyFrame[507].giroJulienZ = 0.000000;

	KeyFrame[508].movJulien_x = -60.000000;
	KeyFrame[508].movJulien_y = 0.000000;
	KeyFrame[508].movJulien_z = -20.000000;
	KeyFrame[508].giroJulienX = 440.000000;
	KeyFrame[508].giroJulienY = 0.000000;
	KeyFrame[508].giroJulienZ = 0.000000;

	KeyFrame[509].movJulien_x = -60.000000;
	KeyFrame[509].movJulien_y = 0.000000;
	KeyFrame[509].movJulien_z = -20.000000;
	KeyFrame[509].giroJulienX = 440.000000;
	KeyFrame[509].giroJulienY = 0.000000;
	KeyFrame[509].giroJulienZ = 0.000000;

	KeyFrame[510].movJulien_x = -60.000000;
	KeyFrame[510].movJulien_y = 0.000000;
	KeyFrame[510].movJulien_z = -20.000000;
	KeyFrame[510].giroJulienX = 440.000000;
	KeyFrame[510].giroJulienY = 0.000000;
	KeyFrame[510].giroJulienZ = 0.000000;

	KeyFrame[511].movJulien_x = -60.000000;
	KeyFrame[511].movJulien_y = 0.000000;
	KeyFrame[511].movJulien_z = -20.000000;
	KeyFrame[511].giroJulienX = 440.000000;
	KeyFrame[511].giroJulienY = 0.000000;
	KeyFrame[511].giroJulienZ = 0.000000;

	KeyFrame[512].movJulien_x = -60.000000;
	KeyFrame[512].movJulien_y = 0.000000;
	KeyFrame[512].movJulien_z = -20.000000;
	KeyFrame[512].giroJulienX = 440.000000;
	KeyFrame[512].giroJulienY = 0.000000;
	KeyFrame[512].giroJulienZ = 0.000000;

	KeyFrame[513].movJulien_x = -60.000000;
	KeyFrame[513].movJulien_y = 0.000000;
	KeyFrame[513].movJulien_z = -20.000000;
	KeyFrame[513].giroJulienX = 440.000000;
	KeyFrame[513].giroJulienY = 0.000000;
	KeyFrame[513].giroJulienZ = 0.000000;

	KeyFrame[514].movJulien_x = -60.000000;
	KeyFrame[514].movJulien_y = 0.000000;
	KeyFrame[514].movJulien_z = -20.000000;
	KeyFrame[514].giroJulienX = 440.000000;
	KeyFrame[514].giroJulienY = 0.000000;
	KeyFrame[514].giroJulienZ = 0.000000;





	KeyFrame[514].movJulien_x = -60.000000;
	KeyFrame[514].movJulien_y = 0.000000;
	KeyFrame[514].movJulien_z = -20.000000;
	KeyFrame[514].giroJulienX = 440.000000;
	KeyFrame[514].giroJulienY = 0.000000;
	KeyFrame[514].giroJulienZ = 0.000000;

	KeyFrame[515].movJulien_x = -60.000000;
	KeyFrame[515].movJulien_y = 0.000000;
	KeyFrame[515].movJulien_z = -20.000000;
	KeyFrame[515].giroJulienX = 440.000000;
	KeyFrame[515].giroJulienY = 0.000000;
	KeyFrame[515].giroJulienZ = 0.000000;

	KeyFrame[516].movJulien_x = -60.000000;
	KeyFrame[516].movJulien_y = 0.000000;
	KeyFrame[516].movJulien_z = -20.000000;
	KeyFrame[516].giroJulienX = 440.000000;
	KeyFrame[516].giroJulienY = 0.000000;
	KeyFrame[516].giroJulienZ = 0.000000;

	KeyFrame[517].movJulien_x = -40.000000;
	KeyFrame[517].movJulien_y = 0.000000;
	KeyFrame[517].movJulien_z = -20.000000;
	KeyFrame[517].giroJulienX = 440.000000;
	KeyFrame[517].giroJulienY = 0.000000;
	KeyFrame[517].giroJulienZ = 0.000000;

	KeyFrame[518].movJulien_x = -40.000000;
	KeyFrame[518].movJulien_y = 0.000000;
	KeyFrame[518].movJulien_z = -20.000000;
	KeyFrame[518].giroJulienX = 440.000000;
	KeyFrame[518].giroJulienY = 0.000000;
	KeyFrame[518].giroJulienZ = 0.000000;

	KeyFrame[519].movJulien_x = -40.000000;
	KeyFrame[519].movJulien_y = 0.000000;
	KeyFrame[519].movJulien_z = -20.000000;
	KeyFrame[519].giroJulienX = 440.000000;
	KeyFrame[519].giroJulienY = 0.000000;
	KeyFrame[519].giroJulienZ = 0.000000;

	KeyFrame[520].movJulien_x = -40.000000;
	KeyFrame[520].movJulien_y = 0.000000;
	KeyFrame[520].movJulien_z = -20.000000;
	KeyFrame[520].giroJulienX = 440.000000;
	KeyFrame[520].giroJulienY = 0.000000;
	KeyFrame[520].giroJulienZ = 0.000000;

	KeyFrame[521].movJulien_x = -40.000000;
	KeyFrame[521].movJulien_y = 0.000000;
	KeyFrame[521].movJulien_z = -20.000000;
	KeyFrame[521].giroJulienX = 440.000000;
	KeyFrame[521].giroJulienY = 0.000000;
	KeyFrame[521].giroJulienZ = 0.000000;

	KeyFrame[522].movJulien_x = -40.000000;
	KeyFrame[522].movJulien_y = 0.000000;
	KeyFrame[522].movJulien_z = -20.000000;
	KeyFrame[522].giroJulienX = 440.000000;
	KeyFrame[522].giroJulienY = 0.000000;
	KeyFrame[522].giroJulienZ = 0.000000;

	KeyFrame[523].movJulien_x = -40.000000;
	KeyFrame[523].movJulien_y = 0.000000;
	KeyFrame[523].movJulien_z = -20.000000;
	KeyFrame[523].giroJulienX = 440.000000;
	KeyFrame[523].giroJulienY = 0.000000;
	KeyFrame[523].giroJulienZ = 0.000000;

	KeyFrame[524].movJulien_x = -40.000000;
	KeyFrame[524].movJulien_y = 0.000000;
	KeyFrame[524].movJulien_z = -20.000000;
	KeyFrame[524].giroJulienX = 440.000000;
	KeyFrame[524].giroJulienY = 0.000000;
	KeyFrame[524].giroJulienZ = 0.000000;

	KeyFrame[525].movJulien_x = -40.000000;
	KeyFrame[525].movJulien_y = 0.000000;
	KeyFrame[525].movJulien_z = -20.000000;
	KeyFrame[525].giroJulienX = 440.000000;
	KeyFrame[525].giroJulienY = 0.000000;
	KeyFrame[525].giroJulienZ = 0.000000;

	KeyFrame[526].movJulien_x = -20.000000;
	KeyFrame[526].movJulien_y = 0.000000;
	KeyFrame[526].movJulien_z = -20.000000;
	KeyFrame[526].giroJulienX = 440.000000;
	KeyFrame[526].giroJulienY = 0.000000;
	KeyFrame[526].giroJulienZ = 0.000000;

	KeyFrame[527].movJulien_x = -20.000000;
	KeyFrame[527].movJulien_y = 0.000000;
	KeyFrame[527].movJulien_z = -20.000000;
	KeyFrame[527].giroJulienX = 440.000000;
	KeyFrame[527].giroJulienY = 0.000000;
	KeyFrame[527].giroJulienZ = 0.000000;

	KeyFrame[528].movJulien_x = -20.000000;
	KeyFrame[528].movJulien_y = 0.000000;
	KeyFrame[528].movJulien_z = -20.000000;
	KeyFrame[528].giroJulienX = 440.000000;
	KeyFrame[528].giroJulienY = 0.000000;
	KeyFrame[528].giroJulienZ = 0.000000;

	KeyFrame[529].movJulien_x = -20.000000;
	KeyFrame[529].movJulien_y = 0.000000;
	KeyFrame[529].movJulien_z = -20.000000;
	KeyFrame[529].giroJulienX = 440.000000;
	KeyFrame[529].giroJulienY = 0.000000;
	KeyFrame[529].giroJulienZ = 0.000000;

	KeyFrame[530].movJulien_x = -20.000000;
	KeyFrame[530].movJulien_y = 0.000000;
	KeyFrame[530].movJulien_z = -20.000000;
	KeyFrame[530].giroJulienX = 440.000000;
	KeyFrame[530].giroJulienY = 0.000000;
	KeyFrame[530].giroJulienZ = 0.000000;

	KeyFrame[531].movJulien_x = -20.000000;
	KeyFrame[531].movJulien_y = 0.000000;
	KeyFrame[531].movJulien_z = -20.000000;
	KeyFrame[531].giroJulienX = 440.000000;
	KeyFrame[531].giroJulienY = 0.000000;
	KeyFrame[531].giroJulienZ = 0.000000;

	KeyFrame[532].movJulien_x = -20.000000;
	KeyFrame[532].movJulien_y = 0.000000;
	KeyFrame[532].movJulien_z = -20.000000;
	KeyFrame[532].giroJulienX = 440.000000;
	KeyFrame[532].giroJulienY = 0.000000;
	KeyFrame[532].giroJulienZ = 0.000000;

	KeyFrame[533].movJulien_x = -20.000000;
	KeyFrame[533].movJulien_y = 0.000000;
	KeyFrame[533].movJulien_z = -20.000000;
	KeyFrame[533].giroJulienX = 440.000000;
	KeyFrame[533].giroJulienY = 0.000000;
	KeyFrame[533].giroJulienZ = 0.000000;

	KeyFrame[534].movJulien_x = -20.000000;
	KeyFrame[534].movJulien_y = 0.000000;
	KeyFrame[534].movJulien_z = -20.000000;
	KeyFrame[534].giroJulienX = 440.000000;
	KeyFrame[534].giroJulienY = 0.000000;
	KeyFrame[534].giroJulienZ = 0.000000;

	KeyFrame[535].movJulien_x = -20.000000;
	KeyFrame[535].movJulien_y = 0.000000;
	KeyFrame[535].movJulien_z = -20.000000;
	KeyFrame[535].giroJulienX = 440.000000;
	KeyFrame[535].giroJulienY = 0.000000;
	KeyFrame[535].giroJulienZ = 0.000000;

	KeyFrame[536].movJulien_x = 0.000000;
	KeyFrame[536].movJulien_y = 0.000000;
	KeyFrame[536].movJulien_z = -20.000000;
	KeyFrame[536].giroJulienX = 440.000000;
	KeyFrame[536].giroJulienY = 0.000000;
	KeyFrame[536].giroJulienZ = 0.000000;

	KeyFrame[537].movJulien_x = 0.000000;
	KeyFrame[537].movJulien_y = 0.000000;
	KeyFrame[537].movJulien_z = -20.000000;
	KeyFrame[537].giroJulienX = 440.000000;
	KeyFrame[537].giroJulienY = 0.000000;
	KeyFrame[537].giroJulienZ = 0.000000;

	KeyFrame[538].movJulien_x = 0.000000;
	KeyFrame[538].movJulien_y = 0.000000;
	KeyFrame[538].movJulien_z = -20.000000;
	KeyFrame[538].giroJulienX = 440.000000;
	KeyFrame[538].giroJulienY = 0.000000;
	KeyFrame[538].giroJulienZ = 0.000000;

	KeyFrame[539].movJulien_x = 0.000000;
	KeyFrame[539].movJulien_y = 0.000000;
	KeyFrame[539].movJulien_z = -20.000000;
	KeyFrame[539].giroJulienX = 440.000000;
	KeyFrame[539].giroJulienY = 0.000000;
	KeyFrame[539].giroJulienZ = 0.000000;

	KeyFrame[540].movJulien_x = 0.000000;
	KeyFrame[540].movJulien_y = 0.000000;
	KeyFrame[540].movJulien_z = -20.000000;
	KeyFrame[540].giroJulienX = 440.000000;
	KeyFrame[540].giroJulienY = 0.000000;
	KeyFrame[540].giroJulienZ = 0.000000;

	KeyFrame[541].movJulien_x = 0.000000;
	KeyFrame[541].movJulien_y = 0.000000;
	KeyFrame[541].movJulien_z = -20.000000;
	KeyFrame[541].giroJulienX = 440.000000;
	KeyFrame[541].giroJulienY = 0.000000;
	KeyFrame[541].giroJulienZ = 0.000000;

	KeyFrame[542].movJulien_x = 0.000000;
	KeyFrame[542].movJulien_y = 0.000000;
	KeyFrame[542].movJulien_z = -20.000000;
	KeyFrame[542].giroJulienX = 440.000000;
	KeyFrame[542].giroJulienY = 0.000000;
	KeyFrame[542].giroJulienZ = 0.000000;

	KeyFrame[543].movJulien_x = 0.000000;
	KeyFrame[543].movJulien_y = 0.000000;
	KeyFrame[543].movJulien_z = -20.000000;
	KeyFrame[543].giroJulienX = 440.000000;
	KeyFrame[543].giroJulienY = 0.000000;
	KeyFrame[543].giroJulienZ = 0.000000;

	KeyFrame[544].movJulien_x = 0.000000;
	KeyFrame[544].movJulien_y = 0.000000;
	KeyFrame[544].movJulien_z = -20.000000;
	KeyFrame[544].giroJulienX = 440.000000;
	KeyFrame[544].giroJulienY = 0.000000;
	KeyFrame[544].giroJulienZ = 0.000000;

	KeyFrame[545].movJulien_x = 0.000000;
	KeyFrame[545].movJulien_y = 0.000000;
	KeyFrame[545].movJulien_z = -20.000000;
	KeyFrame[545].giroJulienX = 440.000000;
	KeyFrame[545].giroJulienY = 0.000000;
	KeyFrame[545].giroJulienZ = 0.000000;

	KeyFrame[546].movJulien_x = 0.000000;
	KeyFrame[546].movJulien_y = 0.000000;
	KeyFrame[546].movJulien_z = -20.000000;
	KeyFrame[546].giroJulienX = 440.000000;
	KeyFrame[546].giroJulienY = 0.000000;
	KeyFrame[546].giroJulienZ = 0.000000;

	KeyFrame[547].movJulien_x = 0.000000;
	KeyFrame[547].movJulien_y = 0.000000;
	KeyFrame[547].movJulien_z = -20.000000;
	KeyFrame[547].giroJulienX = 440.000000;
	KeyFrame[547].giroJulienY = 0.000000;
	KeyFrame[547].giroJulienZ = 0.000000;

	KeyFrame[548].movJulien_x = 20.000000;
	KeyFrame[548].movJulien_y = 0.000000;
	KeyFrame[548].movJulien_z = -20.000000;
	KeyFrame[548].giroJulienX = 440.000000;
	KeyFrame[548].giroJulienY = 0.000000;
	KeyFrame[548].giroJulienZ = 0.000000;

	KeyFrame[549].movJulien_x = 20.000000;
	KeyFrame[549].movJulien_y = 0.000000;
	KeyFrame[549].movJulien_z = -20.000000;
	KeyFrame[549].giroJulienX = 440.000000;
	KeyFrame[549].giroJulienY = 0.000000;
	KeyFrame[549].giroJulienZ = 0.000000;

	KeyFrame[550].movJulien_x = 20.000000;
	KeyFrame[550].movJulien_y = 0.000000;
	KeyFrame[550].movJulien_z = -20.000000;
	KeyFrame[550].giroJulienX = 440.000000;
	KeyFrame[550].giroJulienY = 0.000000;
	KeyFrame[550].giroJulienZ = 0.000000;

	KeyFrame[551].movJulien_x = 20.000000;
	KeyFrame[551].movJulien_y = 0.000000;
	KeyFrame[551].movJulien_z = -20.000000;
	KeyFrame[551].giroJulienX = 440.000000;
	KeyFrame[551].giroJulienY = 0.000000;
	KeyFrame[551].giroJulienZ = 0.000000;

	KeyFrame[552].movJulien_x = 20.000000;
	KeyFrame[552].movJulien_y = 0.000000;
	KeyFrame[552].movJulien_z = -20.000000;
	KeyFrame[552].giroJulienX = 440.000000;
	KeyFrame[552].giroJulienY = 0.000000;
	KeyFrame[552].giroJulienZ = 0.000000;

	KeyFrame[553].movJulien_x = 20.000000;
	KeyFrame[553].movJulien_y = 0.000000;
	KeyFrame[553].movJulien_z = -20.000000;
	KeyFrame[553].giroJulienX = 440.000000;
	KeyFrame[553].giroJulienY = 0.000000;
	KeyFrame[553].giroJulienZ = 0.000000;

	KeyFrame[554].movJulien_x = 20.000000;
	KeyFrame[554].movJulien_y = 0.000000;
	KeyFrame[554].movJulien_z = -20.000000;
	KeyFrame[554].giroJulienX = 440.000000;
	KeyFrame[554].giroJulienY = 0.000000;
	KeyFrame[554].giroJulienZ = 0.000000;

	KeyFrame[555].movJulien_x = 20.000000;
	KeyFrame[555].movJulien_y = 0.000000;
	KeyFrame[555].movJulien_z = -20.000000;
	KeyFrame[555].giroJulienX = 440.000000;
	KeyFrame[555].giroJulienY = 0.000000;
	KeyFrame[555].giroJulienZ = 0.000000;

	KeyFrame[556].movJulien_x = 20.000000;
	KeyFrame[556].movJulien_y = 0.000000;
	KeyFrame[556].movJulien_z = -20.000000;
	KeyFrame[556].giroJulienX = 440.000000;
	KeyFrame[556].giroJulienY = 0.000000;
	KeyFrame[556].giroJulienZ = 0.000000;

	KeyFrame[557].movJulien_x = 20.000000;
	KeyFrame[557].movJulien_y = 0.000000;
	KeyFrame[557].movJulien_z = -20.000000;
	KeyFrame[557].giroJulienX = 440.000000;
	KeyFrame[557].giroJulienY = 0.000000;
	KeyFrame[557].giroJulienZ = 0.000000;

	KeyFrame[558].movJulien_x = 0.000000;
	KeyFrame[558].movJulien_y = 0.000000;
	KeyFrame[558].movJulien_z = -20.000000;
	KeyFrame[558].giroJulienX = 440.000000;
	KeyFrame[558].giroJulienY = 0.000000;
	KeyFrame[558].giroJulienZ = 0.000000;

	KeyFrame[559].movJulien_x = 0.000000;
	KeyFrame[559].movJulien_y = 0.000000;
	KeyFrame[559].movJulien_z = -20.000000;
	KeyFrame[559].giroJulienX = 440.000000;
	KeyFrame[559].giroJulienY = 0.000000;
	KeyFrame[559].giroJulienZ = 0.000000;

	KeyFrame[560].movJulien_x = 0.000000;
	KeyFrame[560].movJulien_y = 0.000000;
	KeyFrame[560].movJulien_z = -20.000000;
	KeyFrame[560].giroJulienX = 440.000000;
	KeyFrame[560].giroJulienY = 0.000000;
	KeyFrame[560].giroJulienZ = 0.000000;

	KeyFrame[561].movJulien_x = 0.000000;
	KeyFrame[561].movJulien_y = 0.000000;
	KeyFrame[561].movJulien_z = -20.000000;
	KeyFrame[561].giroJulienX = 440.000000;
	KeyFrame[561].giroJulienY = 0.000000;
	KeyFrame[561].giroJulienZ = 0.000000;

	KeyFrame[562].movJulien_x = 0.000000;
	KeyFrame[562].movJulien_y = 0.000000;
	KeyFrame[562].movJulien_z = -20.000000;
	KeyFrame[562].giroJulienX = 440.000000;
	KeyFrame[562].giroJulienY = 0.000000;
	KeyFrame[562].giroJulienZ = 0.000000;

	KeyFrame[563].movJulien_x = 0.000000;
	KeyFrame[563].movJulien_y = 0.000000;
	KeyFrame[563].movJulien_z = -20.000000;
	KeyFrame[563].giroJulienX = 440.000000;
	KeyFrame[563].giroJulienY = 0.000000;
	KeyFrame[563].giroJulienZ = 0.000000;

	KeyFrame[564].movJulien_x = 0.000000;
	KeyFrame[564].movJulien_y = 0.000000;
	KeyFrame[564].movJulien_z = -20.000000;
	KeyFrame[564].giroJulienX = 440.000000;
	KeyFrame[564].giroJulienY = 0.000000;
	KeyFrame[564].giroJulienZ = 0.000000;

	KeyFrame[565].movJulien_x = 0.000000;
	KeyFrame[565].movJulien_y = 0.000000;
	KeyFrame[565].movJulien_z = -20.000000;
	KeyFrame[565].giroJulienX = 440.000000;
	KeyFrame[565].giroJulienY = 0.000000;
	KeyFrame[565].giroJulienZ = 0.000000;

	KeyFrame[566].movJulien_x = 0.000000;
	KeyFrame[566].movJulien_y = 0.000000;
	KeyFrame[566].movJulien_z = -20.000000;
	KeyFrame[566].giroJulienX = 440.000000;
	KeyFrame[566].giroJulienY = 0.000000;
	KeyFrame[566].giroJulienZ = 0.000000;

	KeyFrame[567].movJulien_x = -20.000000;
	KeyFrame[567].movJulien_y = 0.000000;
	KeyFrame[567].movJulien_z = -20.000000;
	KeyFrame[567].giroJulienX = 440.000000;
	KeyFrame[567].giroJulienY = 0.000000;
	KeyFrame[567].giroJulienZ = 0.000000;

	KeyFrame[568].movJulien_x = -20.000000;
	KeyFrame[568].movJulien_y = 0.000000;
	KeyFrame[568].movJulien_z = -20.000000;
	KeyFrame[568].giroJulienX = 440.000000;
	KeyFrame[568].giroJulienY = 0.000000;
	KeyFrame[568].giroJulienZ = 0.000000;

	KeyFrame[569].movJulien_x = -20.000000;
	KeyFrame[569].movJulien_y = 0.000000;
	KeyFrame[569].movJulien_z = -20.000000;
	KeyFrame[569].giroJulienX = 440.000000;
	KeyFrame[569].giroJulienY = 0.000000;
	KeyFrame[569].giroJulienZ = 0.000000;

	KeyFrame[570].movJulien_x = -20.000000;
	KeyFrame[570].movJulien_y = 0.000000;
	KeyFrame[570].movJulien_z = -20.000000;
	KeyFrame[570].giroJulienX = 440.000000;
	KeyFrame[570].giroJulienY = 0.000000;
	KeyFrame[570].giroJulienZ = 0.000000;

	KeyFrame[571].movJulien_x = -20.000000;
	KeyFrame[571].movJulien_y = 0.000000;
	KeyFrame[571].movJulien_z = -20.000000;
	KeyFrame[571].giroJulienX = 440.000000;
	KeyFrame[571].giroJulienY = 0.000000;
	KeyFrame[571].giroJulienZ = 0.000000;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	int i = 0;
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;


		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());






		//para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Control del ciclo dia-noche skybox
		skyboxTimeOffset += 0.1 * deltaTime;
		momento_ciclo = skyboxTimeOffset < 30.0f ? 0 :
			skyboxTimeOffset < 60.0f ? 1 :
			skyboxTimeOffset < 90.0f ? 2 :
			skyboxTimeOffset < 120.0f ? 1 : 3;

		switch (momento_ciclo) {
		case 0:
			// textura skybox
			skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

			// Luz del Sol
			mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,	// Color total
				0.6f, 0.6f,									// Bastante iluminado
				0.0f, 0.0f, -1.0f);

			// luces puntuales
			pointLightCount = 0;
			break;
		case 1:
			// textura skybox
			skybox_twilight.DrawSkybox(camera.calculateViewMatrix(), projection);

			// Luz del Sol
			mainLight = DirectionalLight(1.0f, 0.52f, 0.34f,	// Un poco decolorido
				0.4f, 0.4f,									// Poco iluminado
				0.0f, 0.0f, -1.0f);

			// luces puntuales
			pointLightCount = savePointLightCount;
			break;
		case 2:
			// textura skybox
			skybox_night.DrawSkybox(camera.calculateViewMatrix(), projection);

			// Luz del Sol
			mainLight = DirectionalLight(0.6f, 0.6f, 0.6f,	// Poco color
				0.2f, 0.2f,									// Oscuro
				0.0f, 0.0f, -1.0f);
			break;
		case 3:
			skyboxTimeOffset = 0.0f;
			break;
		}

		// Control lamparas
		if (mainWindow.getsKeys()[GLFW_KEY_ENTER] && banderaLampara)
		{
			banderaLampara = false;
			if (spotLightCount > 1) {
				spotLightCount = 1;
			}
			else {
				spotLightCount = 3;
			}

		}
		if (mainWindow.getsKeys()[GLFW_KEY_O]) {
			banderaLampara = true;
		}

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);


		////ESCENARIO
		////Entrada zoologico
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(1000.0f, -50.0f, -550.0f));
		////model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//entrada.RenderModel();

		////Almacen
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-300.0f, -50.0f, 1000.0f));
		//model = glm::rotate(model, glm::radians(-80.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//storage.RenderModel();

		////Tienda zoovenirs
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-350.0f, -50.0f, 800.0f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//zoovenirs.RenderModel();

		////Banca 1
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(300.0f, -50.0f, -600.0f));
		//model = glm::rotate(model, glm::radians(-225.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//bancas.RenderModel();

		////Contenedor
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(90.0f, -50.0f, 1100.0f));
		//model = glm::rotate(model, glm::radians(185.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//contenedor.RenderModel();

		////Bote basura
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(-100.0f, -50.0f, -500.0f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//bote_basura.RenderModel();

		////Lampara 1
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(370.0f, -50.0f, 30.0f));
		//model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//lampara.RenderModel();

		////Mesa 1
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(20.0f, -50.0f, 700.0f));
		//model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//mesas.RenderModel();

		//Habitat pingüinos
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -50.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 4.0f, 7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		habitat_pinguino.RenderModel();

		////Escenario temporal
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -50.0f, 0.0f));
		////model = glm::scale(model, glm::vec3(7.0f, 4.0f, 7.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//escenario_temporal.RenderModel();

		////Habitat flamingos
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(600.0f, -40.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		////model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		////glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		////Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		////habitat_flamingo.RenderModel();

		////Habitat nutria
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -40.0f, -600.0f));
		//model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//habitat_nutria.RenderModel();

		//Habitat nutria
		model = glm::mat4(1.0);
		posJulien = glm::vec3(posXJulien + movJulien_x, posYJulien + movJulien_y, posZJulien + movJulien_z) ;
		model = glm::translate(model, posJulien);
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, giroJulienX * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		julien.RenderModel();

		
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				play = true;
				playIndex = 10;
				i_curr_steps = 0;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			else
			{
				play = false;
			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1)
		{
			reproduciranimacion = 0;
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("KeyFrame[%d].movAvion_x = %f;\n", FrameIndex, movJulien_x);
			printf("KeyFrame[%d].movAvion_y = %f;\n", FrameIndex, movJulien_y);
			printf("KeyFrame[%d].movAvion_z = %f;\n", FrameIndex, movJulien_z);
			printf("KeyFrame[%d].giroAvionX = %f;\n", FrameIndex, giroJulienX);
			printf("KeyFrame[%d].giroAvionY = %f;\n", FrameIndex, giroJulienY);
			printf("KeyFrame[%d].giroAvionZ = %f;\n", FrameIndex, giroJulienZ);
			//printf("movAvion_y es: %f\n", movAvion_y);
			printf(" \npresiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}



	if (keys[GLFW_KEY_T])
	{
		if (ciclo < 1)
		{
			movJulien_y += 20.0f;
			printf("\n movAvion_y es: %f\n", movJulien_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_G])
	{
		if (ciclo < 1)
		{
			movJulien_y -= 20.0f;
			printf("\n movAvion_y es: %f\n", movJulien_y);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_H])
	{
		if (ciclo < 1)
		{
			movJulien_x += 20.0f;
			printf("\n movAvion_x es: %f\n", movJulien_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_F])
	{
		if (ciclo < 1)
		{
			movJulien_x -= 20.0f;
			printf("\n movAvion_x es: %f\n", movJulien_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}

	if (keys[GLFW_KEY_R])
	{
		if (ciclo < 1)
		{
			movJulien_z += 20.0f;
			printf("\n movAvion_z es: %f\n", movJulien_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_E])
	{
		if (ciclo < 1)
		{
			movJulien_z -= 20.0f;
			printf("\n movAvion_z es: %f\n", movJulien_z);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}

	if (keys[GLFW_KEY_X])
	{
		if (ciclo < 1)
		{
			giroJulienX += 20.0f;
			printf("\n giroAvionX es: %f\n", giroJulienX);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_Z])
	{
		if (ciclo < 1)
		{
			giroJulienX -= 20.0f;
			printf("\n giroAvionX es: %f\n", giroJulienX);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}

	if (keys[GLFW_KEY_V])
	{
		if (ciclo < 1)
		{
			giroJulienY += 2.0f;
			printf("\n giroAvionY es: %f\n", giroJulienY);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_C])
	{
		if (ciclo < 1)
		{
			giroJulienY -= 2.0f;
			printf("\n giroAvionY es: %f\n", giroJulienY);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_N])
	{
		if (ciclo < 1)
		{
			giroJulienZ += 2.0f;
			printf("\n giroAvionZ es: %f\n", giroJulienZ);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_B])
	{
		if (ciclo < 1)
		{
			giroJulienZ -= 2.0f;
			printf("\n giroAvionZ es: %f\n", giroJulienZ);
			ciclo++;
			ciclo2 = 0;
			printf("\n reinicia con 1\n");
		}

	}
	if (keys[GLFW_KEY_1])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
		}
	}



}
