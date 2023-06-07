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
#include "Model.h"
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
float angTMarlene = 0.0f; 
float angTMarleneOffset = 0.005f;
float angMarlene = -135.0f; 
float movBrazoMarlene = 0.0;
float movPiernaMarleneI = 45.0f; 
float movPiernaMarleneD = -45.0f; 
float movPiernaMBI = true;
float movPiernaMBD = false;
float movCMono = 0.0f; 
float movCMonoB = true; 

bool aniPinguinos = false; 
bool aniMarlene = false; 
bool aniMono = false; 


//variables de control de skybox
int momento_ciclo;
float skyboxTimeOffset = 0.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

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
Model cuerpoMarlene; 
Model brazoIzqMarlene; 
Model brazoDerMarlene; 
Model piernaIzqMarlene; 
Model piernaDerMarlene; 
Model cuerpoMono; 
Model brazoIzqMono; 
Model brazoDerMono; 
Model piernaIzqMono; 
Model piernaDerMono; 

//-- ESCENARIO
Model entrada;
Model muros;
Model habitat_pinguino;
Model suelo;
Model habitat_flamingo;
Model habitat_nutria;
Model habitat_lemur;
Model habitat_mono;
Model storage;
Model zoovenirs;
Model bancas;
Model contenedor;
Model botes_basura;
Model lamparas;
Model mesas;


//-- POSICION PERSONAJES 
glm::vec3 posSkipper; 
glm::vec3 posRico; 
glm::vec3 posKowalski;
glm::vec3 posCabo;
glm::vec3 posMarlene;
glm::vec3 posMono; 
glm::vec3 lemniscate;

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

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

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

void movLemniscateMarlene() {
	float a = 20; 
	float x = 0; 
	float z = 0; 
	float t = angTMarlene; 

	x = (a * cos(t))/ (1 + (sin(t) * sin(t))); 
	z = (a * cos(t) * sin(t)) / (1 + (sin(t) * sin(t)));
	lemniscate = glm::vec3(x, 0.0f, z);

	angTMarlene += angTMarleneOffset * deltaTime;

	if (angTMarlene >= 360)
		angTMarlene = 0;

	if (posMarlene.x + lemniscate.x >= 5 )
		angMarlene -= 0.75 * deltaTime;

	if (posMarlene.x + lemniscate.x <= -5)
		angMarlene += 0.75 * deltaTime;
}

void movBrazosPiernasMarlene() {
	
	if (movBrazoMarlene <= 360)
		movBrazoMarlene += 1.2 * deltaTime;
	else
		movBrazoMarlene = 0; 

	if (movPiernaMarleneI <= 45.0f and movPiernaMBI)
		movPiernaMarleneI += +0.9 * deltaTime;
	else
		movPiernaMBI = false;

	if (movPiernaMarleneI >= -45.0f and !movPiernaMBI)
		movPiernaMarleneI -= 0.9 * deltaTime;
	else
		movPiernaMBI = true;

	if (movPiernaMarleneD <= 45.0f and movPiernaMBD)
		movPiernaMarleneD += +0.9 * deltaTime;
	else
		movPiernaMBD = false;

	if (movPiernaMarleneD >= -45.0f and !movPiernaMBD)
		movPiernaMarleneD -= 0.9 * deltaTime;
	else
		movPiernaMBD = true;
}

void movColumpioMono() {
	if (movCMono <= 60.0f and movCMonoB)
		movCMono += +0.8 * deltaTime;
	else
		movCMonoB = false;

	if (movCMono >= -60.0f and !movCMonoB)
		movCMono -= 0.8 * deltaTime;
	else
		movCMonoB = true;
}

int main()
{
	mainWindow = Window(1960, 1080); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

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

	cuerpoMarlene = Model();
	cuerpoMarlene.LoadModel("Obj/model_marlene/source/Marlene/CuerpoMarlene.obj");
	brazoIzqMarlene = Model();
	brazoIzqMarlene.LoadModel("Obj/model_marlene/source/Marlene/BrazoIzqMarlene.obj");
	brazoDerMarlene = Model();
	brazoDerMarlene.LoadModel("Obj/model_marlene/source/Marlene/BrazoDerMarlene.obj");
	piernaIzqMarlene = Model();
	piernaIzqMarlene.LoadModel("Obj/model_marlene/source/Marlene/PiernaIzqMarlene.obj");
	piernaDerMarlene = Model();
	piernaDerMarlene.LoadModel("Obj/model_marlene/source/Marlene/PiernaDerMarlene.obj");

	cuerpoMono = Model();
	cuerpoMono.LoadModel("Obj/madagascar-monkey/source/Monkey/CuerpoMono2.obj");
	brazoIzqMono = Model();
	brazoIzqMono.LoadModel("Obj/madagascar-monkey/source/Monkey/BrazoIzqMono.obj");
	brazoDerMono = Model();
	brazoDerMono.LoadModel("Obj/madagascar-monkey/source/Monkey/BrazoDerMono.obj");
	piernaIzqMono = Model();
	piernaIzqMono.LoadModel("Obj/madagascar-monkey/source/Monkey/PiernaIzqMono2.obj");
	piernaDerMono = Model();
	piernaDerMono.LoadModel("Obj/madagascar-monkey/source/Monkey/PiernaDerMono2.obj");



	posSkipper = glm::vec3(-5.0f, 4.5f, 0.0f);
	posRico = glm::vec3(-10.0f, 4.5f, 0.0f);
	posKowalski = glm::vec3(5.0f, 4.5f, 0.0f);
	posCabo = glm::vec3(0.0f, 4.5f, 0.0f); 
	posMarlene = glm::vec3(0.0f, 2.0f, -87.0f);
	posMono = glm::vec3(0.0f, 20.0f, 0.0f);
	lemniscate = glm::vec3(0.0f, 5.0f, 0.0f);

	glm::vec3 escalaPinguinos = glm::vec3(12.0f, 12.0f, 12.0f);
	glm::vec3 escalaMarlene = glm::vec3(0.6f, 0.6f, 0.6f);
	glm::vec3 escalaMono = glm::vec3(10.0f, 10.0f, 10.0f);

	//Modeloa hábitats 

	suelo = Model();
	suelo.LoadModel("Obj/suelo.obj");
	/*
	habitat_pinguino = Model();
	habitat_pinguino.LoadModel("Obj/habitat_pinguinos.obj");
	habitat_flamingo = Model();
	habitat_flamingo.LoadModel("Obj/habitat_flamingo.obj");
	*/

	habitat_nutria = Model();
	habitat_nutria.LoadModel("Obj/habitat_nutrias.obj");
	/*
	habitat_lemur = Model();
	habitat_lemur.LoadModel("Obj/habitat_lemures.obj");
	habitat_mono = Model();
	habitat_mono.LoadModel("Obj/habitat_monos.obj");

	
	
	entrada = Model();
	entrada.LoadModel("Obj/entrada_completa.obj");
	storage = Model();
	storage.LoadModel("Obj/zoo_storage.obj");
	zoovenirs = Model();
	zoovenirs.LoadModel("Obj/tiendas_zoovenirs.obj");
	bancas = Model();
	bancas.LoadModel("Obj/bancas_objeto.obj");
	contenedor = Model();
	contenedor.LoadModel("Obj/contenedor.obj");
	botes_basura = Model();
	botes_basura.LoadModel("Obj/botes_basura.obj");
	lamparas = Model();
	lamparas.LoadModel("Obj/lamparas.obj");
	mesas = Model();
	mesas.LoadModel("Obj/mesas.obj");
	muros = Model();
	muros.LoadModel("Obj/muros.obj");
	*/


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
		/*
		//Entrada zoologico
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		entrada.RenderModel();

		//Almacen
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		storage.RenderModel();

		//Tienda zoovenirs
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		zoovenirs.RenderModel();

		//Banca 1
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		bancas.RenderModel();

		//Contenedor
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		contenedor.RenderModel();

		//Bote basura
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		botes_basura.RenderModel();

		//Lampara 1
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lamparas.RenderModel();

		//Mesas
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mesas.RenderModel();

		//Muros
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		muros.RenderModel();
		*/

		//Suelo
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		suelo.RenderModel();
		

		//Habitat pingüinos
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		habitat_pinguino.RenderModel();

		/*
		//Habitat lemures
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		habitat_lemur.RenderModel();

		//Habitat monos
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		habitat_mono.RenderModel();
	
		//Habitat flamingos
		model = glm::mat4(1.0);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		habitat_flamingo.RenderModel();
		*/ 

		//Habitat nutria
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		habitat_nutria.RenderModel();
		
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

		

		// ---------------  M A R L E N E  ----------------------

		
		model = glm::mat4(1.0); 
		modelaux = glm::mat4(1.0); 
		model = glm::translate(model, posMarlene + lemniscate); 
		model = glm::scale(model, escalaMarlene); 
		modelaux = model;

		model = glm::rotate(model, (-135 + angMarlene) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); 
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess); 
		cuerpoMarlene.RenderModel(); 

		modelaux = model;

		model = glm::translate(model, glm::vec3(-1.0f, 0.1f, -1.8f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (movBrazoMarlene)* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model)); 
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess); 
		brazoIzqMarlene.RenderModel(); 
		

		model = modelaux;

		model = glm::translate(model, glm::vec3(-1.0f, 0.1f, 1.8f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (movBrazoMarlene) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoDerMarlene.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(5.2f, 0.2f, -1.80f));
		model = glm::rotate(model, movPiernaMarleneI*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		piernaIzqMarlene.RenderModel();
		
		model = modelaux;
		
		model = glm::translate(model, glm::vec3(5.2f, 0.2f, 1.8f));
		model = glm::rotate(model, movPiernaMarleneD *toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		piernaDerMarlene.RenderModel();
		

		// ---------------  M O N O  ----------------------
		/*
		
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		model = glm::translate(model, posMono);
		model = glm::scale(model, escalaMono);
		modelaux = model;

		model = glm::rotate(model, movCMono * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cuerpoMono.RenderModel();

		modelaux = model;

		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
		model = glm::rotate(model, -45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, (movBrazoMarlene)* toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoIzqMono.RenderModel();


		model = modelaux;

		model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
		model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, (movBrazoMarlene) * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		brazoDerMono.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(-0.1f, -1.36f, 0.26f));
		//model = glm::rotate(model, movPiernaMarleneI*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		piernaIzqMono.RenderModel();

		model = modelaux;

		model = glm::translate(model, glm::vec3(0.1f, -1.36f, 0.26f));
		//model = glm::rotate(model, 45 *toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		piernaDerMono.RenderModel();
		
		*/
		
		// ----------- M O V  ----  P I N G U I N O S ---------------

		if (mainWindow.getsKeys()[GLFW_KEY_2])
			aniPinguinos = true; 

		if (mainWindow.getsKeys()[GLFW_KEY_3])
			aniMarlene = true;

		if (mainWindow.getsKeys()[GLFW_KEY_4])
			aniMono = true;
		
		if (mainWindow.getsKeys()[GLFW_KEY_X]) {
			aniPinguinos = false;
			aniMarlene = false;
			aniMono = false;
		}
			

		if(aniPinguinos)
			movAletas(); 
		
		// ----------- M O V  ----  M A R L E N E ---------------
		
		if (aniMarlene) {
			movLemniscateMarlene();
			movBrazosPiernasMarlene();
		}
	
		if (aniMono) {
			movColumpioMono();
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}