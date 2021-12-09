 /*------------------ PROYECTO FINAL ----------------
	Alumna: Sandoval Lara Lesly Mayte
	No. cuenta: 315021475

*/
#include <Windows.h>
#include <MMSystem.h>
#include <glad/glad.h>
#include <glfw3.h>	//main
#include <stdlib.h>		
#include <glm/glm.hpp>	//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>	//Texture

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>

//#pragma comment(lib, "winmm.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void my_input(GLFWwindow *window);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

void getResolution(void);

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 90.0f));
float MovementSpeed = 0.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
//vector que controla la dirrección de mis rayos luminosos
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);
//vector para el color de luz puntual
glm::vec3 lightColor(0.0f, 0.0f, 0.0f);
float aR = 0.3f,//ambiente Roja
aG = 0.3f, //ambiente green
aB = 0.3f, //ambiente blue.
dR = 0.6f, //difusa Roja
dG = 0.7, //difusa Green
dB = 0.3; // difusa Blue

//variable para el cambio de posicion
double myVariable = 0.0;
// posiciones
//float x = 0.0f;
//float y = 0.0f;


//variables movimiento de puertas:
float movP_y = 0.0f,
movPB_y = 0.0f,
movPP_y = 180.0f,//inicia en 180 por como esta ubicado el modelo
movPba_y = 0.0f,//puerta baño abajo
movPbaA_y = 0.0f,//puerta baño arriba
movPg_x = 0.0f,//movimiento de puerta del garage para carro
movc_x = 0.0f, //movimiento carro
movc_z = 200.0f,
giro_ca = -90.0f,
giro_llantas = 0.0f,
movGlob = 0.0f,
movx_glob = 253.5f,
moveRatonx = 35.0f,
moveRatony = 0.0f,
giroraton = 0.0f,
movGall = -95.0f;

bool	animacion = false, //control animación´puerta de entrada y puerta bebé
		animacionP = false, //control puerta papás y garage
		animacionB = false,
		animacion1 = false,
		animacionC = false; //control de animación en carro y puerta garage
	

//variables de estado para las animaciones de puertas
int estado = 0, //estado 0 = abre, 1= cierra
	stateP = 0,
	stateBa = 0,
	statec = 0,
	stateN = 0,
	stateM = 0,
	stateRata = 0,
	stateG = 0; 
	

//Keyframes (Manipulación y dibujo)
float
posz = 30.0f,
posx = 52.0f, 
movCa_y = 0.0f,//Movimiento cabeza
movTor = 180.0f,
movPie = 0.0f,
movbraz = 0.0f;
float
incZ = 0.0f,
incX = 0.0f,
rotCab = 0.0f,
rotPies = 0.0f,
rotTor = 0.0f,
rotbra = 0.0f;

#define MAX_FRAMES 9

int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posz;
	float posx;
	float movCa_y;//Movimiento cabeza
	float movTor;
	float movPie;
	float movbraz;


}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;
	
	KeyFrame[FrameIndex].posz = posz;
	KeyFrame[FrameIndex].posx = posx;
	KeyFrame[FrameIndex].movCa_y = movCa_y;
	KeyFrame[FrameIndex].movPie = movPie;

	KeyFrame[FrameIndex].movTor = movTor;
	KeyFrame[FrameIndex].movbraz = movbraz;

	std::cout << "posz = " << posz << std::endl;
	std::cout << "movCa_y = " << movCa_y << std::endl;
	std::cout << "movPie = " << movPie << std::endl;
	std::cout << "movTor = " << movTor << std::endl;
	std::cout << "movbraz = " << movbraz << std::endl;
	std::cout << "posx = " << posx << std::endl;

	FrameIndex++;
}

void resetElements(void)
{
	posz = KeyFrame[0].posz;
	posx = KeyFrame[0].posx;
	movCa_y = KeyFrame[0].movCa_y;
	movPie = KeyFrame[0].movPie;

	movTor = KeyFrame[0].movTor;
	movbraz = KeyFrame[0].movbraz;
}

void interpolation(void)
{
	posz = (KeyFrame[playIndex + 1].posz - KeyFrame[playIndex].posz) / i_max_steps;
	posx = (KeyFrame[playIndex + 1].posx - KeyFrame[playIndex].posx) / i_max_steps;
	movCa_y = (KeyFrame[playIndex + 1].movCa_y - KeyFrame[playIndex].movCa_y) / i_max_steps;
	movPie = (KeyFrame[playIndex + 1].movPie - KeyFrame[playIndex].movPie) / i_max_steps;

	movTor = (KeyFrame[playIndex + 1].movTor - KeyFrame[playIndex].movTor) / i_max_steps;
	movbraz = (KeyFrame[playIndex + 1].movbraz - KeyFrame[playIndex].movbraz) / i_max_steps;

}

void animate(void)
{
	//animacion del gallo y globos moviendose por viento
	if (stateG == 0) {
		movGall += 2.0;
		movGlob -= 1.0;
		movx_glob += 0.1f;
		if (movGall >= 180.0f)
			stateG = 1;
	}
	if (stateG == 1) {
		movGall -= 2.0;
		movGlob += 1.0;
		movx_glob -= 0.1f;
		if (movGall <= -140.0f)
			stateG = 0;
	}

	//movimiento del niño
	
		if (stateN == 0) {
			movTor = 180.0f;
			posz -= 0.5f;
			if (posz <= -18) {
				stateN = 1;
				movTor = 90.0f;
			}

		}
		if (stateN == 1) {
			posx += 0.5f;
			if (posx >= 72.0f) {
				stateN = 2;
				movTor = 0.0f;
			}

		}
		if (stateN == 2) {
			posz += 1.0f; //calculado por pendiente para que vaya en diagonal
			posx -= 0.41f;
			movTor = -67.3801f;//calculado con tan^-1(-2.4) 
			if (posz >= 30.0f && posx<= 52.0f) {
				stateN = 0;
				//movTor = 180.0f;
			}

		}

		if (play)
		{
			if (i_curr_steps >= i_max_steps) //end of animation between frames?
			{
				playIndex++;
				if (playIndex > FrameIndex - 2)	//end of total animation?
				{
					std::cout << "Animation ended" << std::endl;
					//printf("termina anim\n");
					playIndex = 0;
					play = false;
				}
				else //Next frame interpolations
				{
					i_curr_steps = 0; //Reset counter
									  //Interpolation
					interpolation();
				}
			}
			else
			{
				//Draw animation
				posz += incZ;
				posx += incX;
				movCa_y += rotCab;
				movPie += rotPies;

				movTor += rotTor;
				movbraz += rotbra;

				i_curr_steps++;
			}
		}
		//Animacion de la puerta de entrada y cuato de bebé
		if (animacion)
		{
			if (estado == 0) {
				movP_y -= 1.0f;
				movPB_y -= 1.0f;
				if (movP_y <= -90.0f && movPB_y <= -90.0f)
				{
					animacion = false;
					estado = 1;
				}
			}
			if (estado == 1) {
				movP_y += 1.0f;
				movPB_y += 1.0f;
				if (movP_y >= 0.0f && movPB_y >= 0.0f)
				{
					animacion = false;
					estado = 0;
				}
			}

		}
		//animacion de puerta en segundo cuarto y garage
		if (animacionP)
		{
			if (stateP == 0) {
				movPP_y -= 1.0f;
				movPg_x -= 1.0;
				if (movPP_y <= 90.0f && movPg_x <= -90.0) {
					animacionP = false;
					stateP = 1;
				}
			}
			if (stateP == 1) {
				movPP_y += 1.0f;
				movPg_x += 1.0f;
				if (movPP_y >= 180.0f && movPg_x >= 0.0f) {
					animacionP = false;
					stateP = 0;
				}
			}
		}
		//puertas de baños
		if (animacionB)
		{
			if (stateBa == 0) {
				movPba_y -= 1.0f;
				movPbaA_y += 1.0f;
				if (movPba_y <= -90.0 && movPbaA_y >= 90.0) {
					animacionB = false;
					stateBa = 1;
				}
			}
			if (stateBa == 1) {
				movPba_y += 1.0f;
				movPbaA_y -= 1.0f;
				if (movPba_y >= 0.0f && movPbaA_y <= 0.0f) {
					animacionB = false;
					stateBa = 0;
				}
			}

		}
		//animacion del carro con puerta de garage.
		if (animacionC == true && statec != 4)
		{
			if (statec == 0)
			{
				movc_x -= 2.0f;
				giro_llantas += 1.0;
				if (movc_x <= -150.0f) {
					statec = 1;
					giro_ca = 180.0f;
				}
			}
			if (statec == 1)
			{

				movc_z -= 1.6f;
				movPg_x -= 1.5f;
				giro_llantas -= 1.0;
				if (movPg_x <= -90.0) {
					statec = 2;
				}
			}
			if (statec == 2) {
				movc_z -= 1.6;
				if (movc_z <= -20.0f) {
					statec = 3;
				}

			}
			if (statec == 3) {
				movPg_x += 1.5f;
				if (movPg_x >= 0.0f) {
					statec = 4;
					animacionC = false;
				}
			}
		}
		//Animacion de la rata 
		if (animacion1)
		{
			if (stateRata == 0)
			{
				moveRatony += 1.0f;
				if (moveRatony >= +150.0f) {
					stateRata = 1;
					giroraton = 90.0f;
				}
			}
			if (stateRata == 1)
			{
				moveRatonx += 5.0f;
				if (moveRatonx >= +180.0f) {
					stateRata = 2;
					giroraton = 180.0f;
				}
			}
			if (stateRata == 2)
			{
				moveRatony -= 5.0f;
				if (moveRatony <= -180.0f) {
					stateRata = 3;
					giroraton = -90.0f;
				}
			}
			if (stateRata == 3)
			{
				moveRatonx -= 5.0f;
				if (moveRatonx <= -0.0f) {
					stateRata = 4;
					giroraton = 0.0f;
				}
			}
			if (stateRata == 4)
			{
				moveRatony += 5.0f;
				if (moveRatony >= 0.0f) {
					stateRata = 0;
					giroraton = 0.0f;
					animacion1 = false;
				}
			}


		}
	}


void getResolution()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}


int main()
{
	/////////////////////Musica de fondo///////////////////////////////////////////
	PlaySound(TEXT("up.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGeIHC", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");

	vector<std::string> faces
	{
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// ----------------------------------------------------
	//modelos estaticos
	Model piso("resources/objects/piso/piso.obj");
	//modelo de la casa
	Model casaU("resources/objects/CasaUp/CasaUp2.obj");
	//modelo de puerta para Anim
	Model puertaF("resources/objects/Puertas/puertaF.obj");
	//modelo de la puerta de bañosM
	Model puertaB("resources/objects/Puertas/PuertaB.obj");
	Model puertaBA("resources/objects/Puertas/PuertaBA.obj");
	//modelo de la puerta del garage
	Model puertaG("resources/objects/Puertas/puertaG.obj");
	//modelo por primitivas
	Model carrito("resources/objects/coche/carrito.obj");
	//llanta 
	Model llanta("resources/objects/coche/llanta.obj");
	//cocina 
	Model cocina("resources/objects/CasaUp/cocina.obj");
	//cuna 
	Model cuna("resources/objects/CasaUp/cuna.obj");
	//gallo
	Model gallo("resources/objects/CasaUp/gallo.obj");
	//Estufa
	Model estufa("resources/objects/CasaUp/stove.obj");
	//sillon
	Model sillon("resources/objects/CasaUp/sillon.obj");
	//silla arriba
	Model silla("resources/objects/CasaUp/SillaA.obj");
	//maquina
	Model machine("resources/objects/CasaUp/machine.obj");
	//bote
	Model bote("resources/objects/CasaUp/tash.obj");
	//sillon con almohada
	Model sillonP("resources/objects/CasaUp/sillonP.obj");
	//lamparas
	Model lamp("resources/objects/CasaUp/lamparas.obj");
	//mesaArriba
	Model table("resources/objects/CasaUp/mesa2.obj");
	//cabeza
	Model cabeza("resources/objects/cabeza/cabeza.obj");
	//torso
	Model torso("resources/objects/torso/torso.obj");
	//piernader
	Model piernader("resources/objects/piernader/piernader.obj");
	//piernaizq
	Model piernaizq("resources/objects/piernaizq/piernaizq.obj");
	//brazoder
	Model brazoder("resources/objects/brazoder/brazoder.obj");
	//brazoizq
	Model brazoizq("resources/objects/brazoizq/brazoizq.obj");
	//Globos
	Model globos("resources/objects/Globos/globito1.obj");
	//raton
	Model ratoncito("resources/objects/ratoncito/ratoncito.obj");
	//muebles
	Model sala("resources/objects/sala/sala.obj");
	Model banos("resources/objects/banos/banos.obj");
	Model banos2("resources/objects/banos2/banos2.obj");
	Model letrerosgara("resources/objects/letrerosgara/letrerosgara.obj");
	Model cuartobbyTapete("resources/objects/cuartobbyTapete/cuartobbyTapete.obj");

	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posz = 0;
		KeyFrame[i].posx = 0;
		KeyFrame[i].movCa_y = 0;
		KeyFrame[i].movTor = 0;
		KeyFrame[i].movPie = 0;
		KeyFrame[i].movbraz = 0;
	}


	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		//direccion de la luz direccional
		//lightDirection 0 en x, -1 en y por que va de arriba hacia abajo y -1 en z para dentro del monitor
		staticShader.setVec3("dirLight.direction", lightDirection);
		//si la apagamos y volvemos 0 la mayoria se ve negro y cambiando los parametros se ve mas luz o menos luz
		//luz que está en todo el ambiente y afecta a las caras menos iluminadas
		staticShader.setVec3("dirLight.ambient", glm::vec3(aR, aG , aB)); //componente ambiental
		//componente difusa
		//los rayos luminosos van desde la camara y hace como si estuviera pegando una luz en el escenario
		staticShader.setVec3("dirLight.diffuse", glm::vec3(dR, dG, dB));
		//componente especular
		//o en el programa de modelado podemos manipular las coordenadas de este comportamiento.
		//se recomienda trabajar con blanca
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));

		//fuente de luz posicional, tiene una pocision dentro del escenario. 
		//se mueve con teclas M y N por la variable lightPosition
		staticShader.setVec3("pointLight[0].position", glm::vec3 (-150.0f,0.0f,-20.f));
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.3f, 0.3f, 0.3f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		//parametros para afectar la luz porque son de atenuación.
		//las dos primeras lineas afectan la intensidad de la luz
		staticShader.setFloat("pointLight[0].constant", 0.08f);
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		//aplica una mayor diferencia visual en cuestion de la diatancia que abarca la lúz
		staticShader.setFloat("pointLight[0].quadratic", 0.00032f);

		//otra fuente de luz posicional en una posicion estatica
		staticShader.setVec3("pointLight[1].position", glm::vec3(-180.0, 0.0f, 50.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3 (0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 1.0f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.00032f);
		staticShader.setFloat("material_shininess", 32.0f);

		//si se agrega otra luz en el shader hay que poner el código de esta pero apagada si no se va usa

		staticShader.setVec3("pointLight[2].position", glm::vec3(150.0, 0.0f, 50.0f));
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 1.0f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.00032f);
		staticShader.setFloat("material_shininess", 32.0f);

		//agregando cuarta luz con cambio de color
		staticShader.setVec3("pointLight[3].position", glm::vec3(-180.0f, 0.0f, 50.0f));
		staticShader.setVec3("pointLight[3].ambient", glm::vec3(0.0f, 0.0f, .0f));
		staticShader.setVec3("pointLight[3].diffuse", glm::vec3(0.0f, 0.0f, .0f));
		staticShader.setVec3("pointLight[3].specular", glm::vec3(0.0f, 0.0f, .0f));
		staticShader.setFloat("pointLight[3].constant", 1.0f);
		staticShader.setFloat("pointLight[3].linear", 0.009f);
		staticShader.setFloat("pointLight[3].quadratic", 0.00032f);
		staticShader.setFloat("material_shininess", 32.0f);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		glm::mat4 tmp1 = glm::mat4(1.0f);
		glm::mat4 tmp2 = glm::mat4(1.0f);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//// Light
		glm::vec3 lightColor = glm::vec3(0.6f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.75f);


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projection);
		staticShader.setMat4("view", view);

		//dibujando piso
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f));
		staticShader.setMat4("model", model);
		piso.Draw(staticShader);

		//dibujando Casa
		tmp = model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); //temporal por si se mueve la casa
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		casaU.Draw(staticShader);

		//dibujando puerta de entrada
		model = glm::translate(tmp, glm::vec3(-14.0f, 42.0f, 15.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movP_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puertaF.Draw(staticShader);

		//dibujando puerta del cuarto bebé
		model = glm::translate(tmp, glm::vec3(79.0f, 117.0f, -66.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movPB_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puertaF.Draw(staticShader);

		//puerta del cuarto papás
		model = glm::translate(tmp, glm::vec3(56.0f, 117.0f, -45.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movPP_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puertaF.Draw(staticShader);

		//puertas de baños
		//baño abajo
		model = glm::translate(tmp, glm::vec3(-49.5f, 42.0f, -39.8f));
		model = glm::scale(model, glm::vec3(15.0));
		model = glm::rotate(model, glm::radians(movPba_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puertaB.Draw(staticShader);

		//baño de arriba
		model = glm::translate(tmp, glm::vec3(-23.5f, 117.0f, -42.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movPbaA_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		puertaBA.Draw(staticShader);

		//puerta garage
		model = glm::translate(tmp, glm::vec3(-150.0f, 67.0f, 63.5f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movPg_x), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		puertaG.Draw(staticShader);

		//coche por primitivas
		model = glm::translate(glm::mat4(1.0f), glm::vec3(movc_x, 23.0f, movc_z));
		tmp1 = model = glm::rotate(model, glm::radians(giro_ca), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(18.0f));
		staticShader.setMat4("model", model);
		carrito.Draw(staticShader);

		//llanta trasera derecha
		model = glm::translate(tmp1, glm::vec3(14.00f, -15.0f, 27.2f));
		model = glm::scale(model, glm::vec3(17.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);
		//llanta trasera izq
		model = glm::translate(tmp1, glm::vec3(-16.f, -15.0f, 27.2f));
		model = glm::scale(model, glm::vec3(17.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);

		//llanta delantera derecha
		model = glm::translate(tmp1, glm::vec3(14.0f, -15.0f, -20.0f));
		model = glm::scale(model, glm::vec3(17.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);
		//llanta trasera izq
		model = glm::translate(tmp1, glm::vec3(-16.f, -15.0f, -20.0f));
		model = glm::scale(model, glm::vec3(17.0f));
		model = glm::rotate(model, glm::radians(giro_llantas), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		llanta.Draw(staticShader);
		//cocina
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		cocina.Draw(staticShader);
		//cuna 
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		cuna.Draw(staticShader);
		//gallo
		model = glm::translate(tmp, glm::vec3(51.0f, 292.5f, 84.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movGall), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		gallo.Draw(staticShader);
		//estufa
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		estufa.Draw(staticShader);
		//sillon 1
		model = glm::translate(tmp, glm::vec3(5.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		sillon.Draw(staticShader);
		//lamparas 
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		lamp.Draw(staticShader);
		//globos
		model = glm::translate(tmp, glm::vec3(10.0f, movx_glob, -10.5f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movGlob), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		globos.Draw(staticShader);

		//dibujando raton
		model = glm::translate(glm::mat4(1.0f), glm::vec3(moveRatonx, 0.0f, moveRatony)); //temporal por si se mueve la casa
		model = glm::scale(model, glm::vec3(1.0f));
		model = glm::rotate(model, glm::radians(giroraton), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		ratoncito.Draw(staticShader);

		//sala
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		sala.Draw(staticShader);
		//baños
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		banos.Draw(staticShader);
		//baños2
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		banos2.Draw(staticShader);
		//Letreros
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		letrerosgara.Draw(staticShader);
		//cuartoBB
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		cuartobbyTapete.Draw(staticShader);


		///////////////////cuarto arriba//////////////////////
		// mesas
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		table.Draw(staticShader);
		//silla
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		silla.Draw(staticShader);
		//maquina
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		machine.Draw(staticShader);
		//bote basura
		model = glm::translate(tmp, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		bote.Draw(staticShader);
		//sillòn con almohada
		model = glm::translate(tmp, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		sillonP.Draw(staticShader);
		//dibujando niño
		
		//dibujando torso
		model = glm::translate(tmp, glm::vec3(posx, 116.0f, posz));
		tmp2 = model = glm::rotate(model, glm::radians(movTor), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(15.0f));
		staticShader.setMat4("model", model);
		torso.Draw(staticShader);

		//dibujando piernader
		model = glm::translate(tmp2, glm::vec3(1.0f, -7.5f, 0.3f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movPie), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernader.Draw(staticShader);


		//dibujando piernaizq
		model = glm::translate(tmp2, glm::vec3(-1.0f, -7.5f, 0.2f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(-movPie), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", model);
		piernaizq.Draw(staticShader);

		//dibujando brazoder
		model = glm::translate(tmp2, glm::vec3(2.0f, 2.2f, -1.2f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movbraz), glm::vec3(1.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		brazoder.Draw(staticShader);

		//dibujando brazoizq
		model = glm::translate(tmp2, glm::vec3(-2.6f, 2.2f, -1.1f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(-movbraz), glm::vec3(1.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		brazoizq.Draw(staticShader);

		//dibujando cabeza
		model = glm::translate(tmp2, glm::vec3(-0.5f, 12.0f, -0.2f));
		model = glm::scale(model, glm::vec3(15.0f));
		model = glm::rotate(model, glm::radians(movCa_y), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", model);
		cabeza.Draw(staticShader);

		// -------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario
		// -------------------------------------------------------------------------------------------------------------------------

		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, view, projection, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		//std::cout <<"frame time = " << frameTime << " milli sec"<< std::endl;
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	skybox.Terminate();

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	//movimiento de puerta de entrada y puerta del cuarto bebé
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion = true;
	//movimiento de puerta de los papás y garage
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		animacionP = true;
	//moviento puertas de baños
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		animacionB = true;
	//animación del coche con puerta de garage
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		animacionC = true;
	}
	//animación del raton
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		animacion1 = true;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS){
		aR = 0.1f;//ambiente Roja
		aG = 0.1f;//ambiente green
		aB = 0.1f; //ambiente blue.
		dR = 0.5f; //difusa Roja
		dG = 0.2; //difusa Green
			dB = 0.9; // difusa Blue
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		aR = 0.3f;//ambiente Roja
		aG = 0.3f; //ambiente green
		aB = 0.3f; // ambiente blue.
		dR = 0.6f; //difusa Roja
		dG = 0.7; //difusa Green
		dB = 0.3; // difusa Blue
	}
		




	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}