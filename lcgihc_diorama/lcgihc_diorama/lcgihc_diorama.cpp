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
float angAletas = 0.0;
float angAletasOffset = 0.5;
bool movAletasB = true; 

//variables de control de skybox
int momento_ciclo;
float skyboxTimeOffset = 0.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

//PISO 
Texture pisoTexture;

//MODELOS DEL PROYECTO 

//-- PEROSONAJES 
Model cuerpoSkipper;
Model brazoIzqSkipper; 
Model brazoDerSkipper; 
Model cuerpoRico; 
Model brazoIzqRico;
Model brazoDerRico;
Model cuerpoKowalski; 
Model brazoIzqKowalski;
Model brazoDerKowalski;
Model cuerpoCabo; 
Model brazoIzqCabo;
Model brazoDerCabo;

//-- POSICION PERSONAJES 
glm::vec3 posSkipper; 
glm::vec3 posRico; 
glm::vec3 posKowalski;
glm::vec3 posCabo;

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
float posYroca = 0.0;
float movRoca_y = 0.0f;

#define MAX_FRAMES 30

int i_max_steps = 90;
int i_curr_steps = 10;


typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movRoca_y;		//Variable para PosicionY
	float movRoca_yInc;		//Variable para IncrementoY
}FRAME;

FRAME KeyFrame[MAX_FRAMES];

int FrameIndex = 10;			//introducir datos
bool play = true;
int playIndex = 0;


void resetElements(void)
{
	movRoca_y = KeyFrame[0].movRoca_y;
}

void interpolation(void)
{
	KeyFrame[playIndex].movRoca_yInc = (KeyFrame[playIndex + 1].movRoca_y - KeyFrame[playIndex].movRoca_y) / i_max_steps;
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
			movRoca_y += KeyFrame[playIndex].movRoca_yInc;
			i_curr_steps++;
		}

	}
}

void CreateObjects()
{

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};


	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

}




/* FIN KEYFRAMES*/

void movAletas() {
	if (angAletas <= 30.0f and movAletasB)
		angAletas += angAletasOffset * deltaTime;
	else
		movAletasB = false;

	if (angAletas >= -30.0f and !movAletasB)
		angAletas -= angAletasOffset * deltaTime;
	else
		movAletasB = true;
}


int main()
{
	mainWindow = Window(1960, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

    // Modelo y posición de los personajes 
	cuerpoSkipper = Model();
	cuerpoSkipper.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/CuerpoSkipper.obj");
	brazoIzqSkipper = Model();
	brazoIzqSkipper.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoIzqSkipper.obj");
	brazoDerSkipper = Model();
	brazoDerSkipper.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoDerSkipper.obj");

	cuerpoRico = Model();
	cuerpoRico.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/CuerpoRico.obj");
	brazoIzqRico = Model();
	brazoIzqRico.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoIzqRico.obj");
	brazoDerRico = Model();
	brazoDerRico.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoDerRico.obj");


	cuerpoKowalski = Model();
	cuerpoKowalski.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/CuerpoKowalski.obj");
	brazoIzqKowalski = Model();
	brazoIzqKowalski.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoIzqKowalski.obj");
	brazoDerKowalski = Model();
	brazoDerKowalski.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoDerKowalski.obj");

	cuerpoCabo = Model();
	cuerpoCabo.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/CuerpoCabo.obj");
	brazoIzqCabo = Model();
	brazoIzqCabo.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoIzqCabo.obj");
	brazoDerCabo = Model();
	brazoDerCabo.LoadModel("Obj/model_penguins-of-madagascar/source/Penguins of Madagascar/BrazoDerCabo.obj");

	posSkipper = glm::vec3(10.0f, 0.0f, 0.0f);
	posRico = glm::vec3(0.0f, 0.0f, 0.0f);
	posKowalski = glm::vec3(20.0f, 0.0f, 0.0f);
	posCabo = glm::vec3(30.0f, 0.0f, 0.0f); 

	glm::vec3 escalaPinguinos = glm::vec3(20.0f, 20.0f, 20.0f);


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

	//KEYFRAMES DECLARADOS INICIALES


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


		//ESCENARIO

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[0]->RenderMesh();

		//PERSONAJES 

		// ---------------  S K I P P E R  ----------------------

		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, posSkipper);
		model = glm::scale(model, escalaPinguinos);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cuerpoSkipper.RenderModel();

		modelaux = model;

		model = glm::translate(model, glm::vec3(-0.05f, 0.25f, 0.0f));
		model = glm::rotate(model, (-90 + angAletas) * toRadians, glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoIzqSkipper.RenderModel();

		model = modelaux; 

		model = glm::translate(model, glm::vec3(0.08f, 0.25f, 0.0f));
		model = glm::rotate(model, -55 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoDerSkipper.RenderModel();

		// ---------------  R I C O  ----------------------

		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, posRico);
		model = glm::scale(model, escalaPinguinos);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cuerpoRico.RenderModel();

		modelaux = model;

		model = glm::translate(model, glm::vec3(-0.05f, 0.25f, 0.0f));
		model = glm::rotate(model, (-90 + angAletas) * toRadians, glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoIzqRico.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.08f, 0.25f, 0.0f));
		model = glm::rotate(model, -55 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoDerRico.RenderModel();

		// ---------------  K O W A L S K I  ----------------------
		
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, posKowalski);
		model = glm::scale(model, escalaPinguinos);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cuerpoKowalski.RenderModel();

		modelaux = model;

		model = glm::translate(model, glm::vec3(0.15f, 0.3f, 0.0f));
		model = glm::rotate(model, (-90 + angAletas) * toRadians, glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoIzqKowalski.RenderModel();
		
		model = modelaux; 

		model = glm::translate(model, glm::vec3(0.28f, 0.3f, 0.0f));
		model = glm::rotate(model, -55 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoDerKowalski.RenderModel();
				

		// ---------------  C A B O  ----------------------

		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, posCabo);
		model = glm::scale(model, escalaPinguinos);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cuerpoCabo.RenderModel();

		modelaux = model;

		model = glm::translate(model, glm::vec3(-0.05f, 0.20f, 0.0f));
		model = glm::rotate(model, (-90 + angAletas) * toRadians, glm::vec3(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoIzqCabo.RenderModel();
		
		model = modelaux;

		model = glm::translate(model, glm::vec3(0.08f, 0.20f, 0.0f));
		model = glm::rotate(model, -55 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoDerCabo.RenderModel();

		
		movAletas(); 


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}