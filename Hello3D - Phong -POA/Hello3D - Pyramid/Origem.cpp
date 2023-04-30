/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico/Computação Gráfica - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 01/03/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Object.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	//glm::vec3 normal;
};


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// int loadSimpleOBJ(string filepath, int& nVerts);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX=false, rotateY=false, rotateZ=false;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);


bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

glm::mat4 projection;
glm::mat4 view = glm::mat4(1.0f);

GLuint VAO2;
GLuint VAO3;
Object* suzy;
int nVerts;

bool isObjASelected = false;
bool isObjBSelected = false;

vector <Vertex> vertices;
vector <GLuint> indices;
vector <glm::vec2> texCoords;
vector <glm::vec3> normals;

// Função MAIN
int main()
{
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	

	glfwSetCursorPos(window,WIDTH / 2, HEIGHT / 2);


	//Desabilita o desenho do cursor 
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	//GLuint shader.ID = setupShader();
	Shader shader("Phong.vs","Phong.fs");

	glUseProgram(shader.ID);

	

	glEnable(GL_DEPTH_TEST);
	glfwPollEvents();

	suzy = new Object;
	VAO2 = suzy->loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts, 0.0f, 0.5f);
	VAO3 = suzy->loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts, 5.0f, 1.0f);

	//Definindo as propriedades do material da superficie
	shader.setFloat("ka", 0.4);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10.0);

	//Definindo a fonte de luz pontual
	shader.setVec3("lightPos", -2.0, 10.0, 2.0);
	shader.setVec3("lightColor", 1.0, 1.0, 1.0);


	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		// Matriz de model -- transformações no objeto
		glm::mat4 model = glm::mat4(1); //matriz identidade;
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		//Matriz de view -- posição e orientação da câmera
		//glm::mat4 view = glm::mat4(1.0f);
		GLint viewLoc = glGetUniformLocation(shader.ID, "view");
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

		//Matriz de projeção perspectiva - definindo o volume de visualização (frustum)
		//glm::mat4 projection;
		projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
		GLint projLoc = glGetUniformLocation(shader.ID, "projection");
		glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

		model = glm::mat4(1); 
		// model = glm::translate(model, glm::vec3(0.0, 0.0, cos(angle)*10.0));

		if (rotateX) { model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f)); }
		else if (rotateY) { model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f)); }
		else if (rotateZ) { model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f)); }
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		
		//Atualizando a posição e orientação da câmera
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES
		
		glBindVertexArray(VAO2);
		glDrawArrays(GL_TRIANGLES, 0, nVerts);

		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO3);
		glDrawArrays(GL_TRIANGLES, 0, nVerts);

		// Chamada de desenho - drawcall
		// CONTORNO - GL_LINE_LOOP
		
		//glDrawArrays(GL_POINTS, 0, nVerts);
		//glBindVertexArray(0);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &VAO3);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

	float cameraSpeed = 0.05;

	if (key == GLFW_KEY_W) { cameraPos += cameraFront * cameraSpeed; }
	if (key == GLFW_KEY_S) { cameraPos -= cameraFront * cameraSpeed; }
	if (key == GLFW_KEY_A) { cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; }
	if (key == GLFW_KEY_D) { cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//unsigned char data[4];
		GLfloat depth;
		//glReadPixels(lastX , lastY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glReadPixels(lastX, lastY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
		//int pickedColor = data[0] + 256*data[1] + 256*256*data[2];
		glm::vec4 viewport = glm::vec4(0, 0, WIDTH, HEIGHT);
		glm::vec3 wincoord = glm::vec3(lastX, lastY, depth);
		glm::vec3 objcoord = glm::unProject(wincoord, view, projection, viewport);

		if (depth != 1.0) {
			if (objcoord.x < 2.0) {
				if (isObjASelected == true) {
					VAO2 = suzy->loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts, 0.0f, 0.5f);
					isObjASelected = false;
				}
				else {
					VAO2 = suzy->loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts, 0.0f, 0.2f);
					isObjASelected = true;
				}
			}
			else {
				if (isObjBSelected == true) {
					VAO3 = suzy->loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts, 5.0f, 1.0f);
					isObjBSelected = false;
				}
				else {
					VAO3 = suzy->loadSimpleOBJ("../../3D_models/Suzanne/suzanneTriLowPoly.obj", nVerts, 5.0f, 0.2f);
					isObjBSelected = true;
				}
			}
		}

	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f) {
		fov = 1.0f;
	}
	if (fov > 45.0f) {
		fov = 45.0f;
	}
}

