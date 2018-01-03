#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
#include <GL/freeglut.h>
#include <cstring>
#include <stdlib.h>		  // srand, rand
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include "math.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Geometry/TriangularSurface/StaticTriangularSurface.h"
#include "GL2/GL2_Object.h"
#include "GL2/GL2_World.h"
#include "GL2/GL2_Light.h"
#include "GL2/GL2_Material.h"
#include "BMPImage.h"
#include <Windows.h>
#include "MMSystem.h"
#include "ParticleSystem.h"
int width_window = 1040;
int height_window = 880;

int step[5] = { 0, 0, 0, 0, 0 };
bool step1 = false;
bool step2 = false;
bool step3 = false;
bool step4 = false;
bool step5 = false;
bool cond[6] = { false };
bool lose = false;

GL2_World gl_world;

float my_round(float x, unsigned int digits) {
	float fac = pow(10, digits);
	return round(x*fac) / fac;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		if(step[0]==1){ step[1] = 1; }

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		if (step[0] == 1) {
			step[2] = 1;
		}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		if (step[0] == 1) {
			step[3] = 1;
		}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		if (step[0] == 1) {
			step[4] = 1;
		}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		step[0] = 1;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	gl_world.camera_.UpdateDolly(yoffset);

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		//std::cout << xpos << " " << ypos << std::endl;
		gl_world.camera_.StartMouseRotation(xpos, ypos);


	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		//std::cout << xpos << " " << ypos << std::endl;
		gl_world.camera_.EndMouseRotation(xpos, ypos);

	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{


	gl_world.camera_.ProcessMouseMotion(xpos, ypos);
	//std::cout << xpos << " " << ypos << std::endl;
}

bool winCheck(bool cond[]) {
	for (int i = 0; i < 6; ++i) {
		if (cond[i] == false) return false;
		else continue;
	}
	return true;
}

int main(void)
{
	GLFWwindow *window = nullptr;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_SAMPLES, 32);

	// window resolution
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width_window = mode->width * 0.8f;
	height_window = mode->height * 0.8f;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width_window, height_window, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	// callbacks here
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glClearColor(1, 1, 1, 1); // while background

	printf("%s\n", glGetString(GL_VERSION));

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	const float aspect = (float)width / (float)height;
	glViewport(0, 0, width, height);
	glOrtho(-1.2*aspect, 1.2*aspect, -1.2, 1.2, -100.0, 100.0);
	gluLookAt(0.1, 0.1, 0.1, 0.0, 0.0, 0.0, 0, 1, 0);//camera takes care of it

	const float zNear = 0.001, zFar = 100.0, fov = 45.0;			// UI
	gl_world.camera_.Resize(width, height, fov, zNear, zFar);
	gl_world.initShaders();

	//	glEnable(GL_MULTISAMPLE);
	enum { num_surface = 22 };

	//StaticTriangularSurface surface,surface2;
	ParticleSystem ps;
	StaticTriangularSurface surface[num_surface];
	//surface.readObj("BuddhaSculpture.obj", true, true);
	surface[0].readObj("model/dungeon.obj", true, true); // scaled our polygons to fit in box(-1,-1,-1)x(1,1,1)
	surface[1].readObj("model/red.obj", true, true);
	surface[2].readObj("model/blue.obj", true, true);
	surface[3].readObj("model/skull.obj", true, true);
	surface[4].readObj("model/green.obj", true, true);
	surface[5].readObj("model/sun.obj", true, true);
	surface[6].readObj("model/snow.obj", true, true);
	surface[7].readObj("model/death_block.obj", true, true);
	surface[8].readObj("model/tree_block.obj", true, true);
	surface[9].readObj("model/fire_block.obj", true, true);
	surface[10].readObj("model/snow_block.obj", true, true);
	surface[11].readObj("model/hat.obj", true, true);
	surface[12].readObj("model/tombstone.obj", true, true);
	surface[13].readObj("model/boat1.obj", true, true);
	surface[14].readObj("model/right_hand.obj", true, true);
	surface[15].readObj("model/hero.obj", true, true);
	surface[16].readObj("model/scroll.obj", true, true);
	surface[17].readObj("model/text.obj", true, true);
	surface[18].readObj("model/skull_normal.obj", true, true);
	surface[19].readObj("model/text2.obj", true, true);
	surface[20].readObj("model/party_ball.obj", true, true);
	surface[21].readObj("model/text3.obj", true, true);
	/*surface2.scale(0.3, 0.3, 0.3);
	surface2.move(-0.03, 0.05, -0.03);*/
	for (int j = 1; j < 7; ++j) {
		surface[j].scale(0.1, 0.01, 0.1);

	}
	surface[7].scale(0.15, 0.15, 0.15);
	surface[8].scale(0.14, 0.14, 0.14);
	surface[9].scale(0.14, 0.8, 0.14);
	surface[10].scale(0.14, 0.14, 0.14);
	surface[11].scale(0.14, 0.17, 0.14);
	surface[12].scale(0.15, 0.15, 0.1);
	surface[13].scale(0.14, 0.25, 0.14);
	surface[14].scale(0.2, 0.2, 0.2);
	surface[15].scale(0.15, 0.15, 0.15);
	surface[16].scale(0.48, 0.4, 0.4);
	surface[17].scale(0.3, 0.3, 0.3);
	surface[18].scale(0.25, 0.25, 0.25);
	surface[19].scale(0.3, 0.3, 0.3);
	surface[20].scale(0.3, 0.3, 0.3);
	surface[21].scale(0.3, 0.3, 0.3);

	//move
	surface[1].move(-0.07, -0.08, 0.0);
	surface[2].move(0.07, -0.08, 0.0);
	surface[3].move(0.07, -0.08, 0.14);
	surface[4].move(-0.07, -0.08, -0.14);
	surface[5].move(0.07, -0.08, -0.14);
	surface[6].move(-0.07, -0.08, 0.14);
	surface[7].move(-0.07, 0.2, -0.32);
	surface[8].move(-0.07*3, -0.01, -0.14);
	surface[9].move(-0.07 * 3, -0.08, 0.14);
	surface[10].move(0.07*3, -0.01, 0.14);
	surface[11].move(0.07*3, -0.05, -0.14);
	surface[12].move(0.07, -0.04, 0.0);
	surface[13].move(-0.07*3, -0.05, 0.0);
	surface[14].move(0.15, 0.0, 1.2);
	surface[15].move(-0.07*5, 0.0, 0.0);
	surface[16].move(0.05, 0.0, 1.0);
	surface[17].move(0.08, 0.02, 1.01);
	surface[18].move(-0.35, 0.0, 1.0);
	surface[19].move(0.08, -0.1, 1.01);
	surface[20].move(0.1, 0.3, 1.0);
	surface[21].move(0.08, -0.015, 1.0);
	glEnable(GL_TEXTURE_2D);


	surface[14].rotate(80, { 0.0,1.0,0.0 });
	surface[18].rotate(120, { 0.0,1.0,0.0 });

	for (int k = 0; k < num_surface; ++k) {
		surface[k].vertex_uv_.init(surface[k].vertex_positions_.num_elements_);
	}
	glm::vec4 uPlane = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 vPlane = { 0.0f, 1.0f, 0.0f, 0.0f };
	glm::vec4 zPlane = { 1.0f, 1.0f, 0.0f, 0.0f };
	glm::vec4 tPlane = { 1.0f, 0.0f, 0.0f, 0.0f };

	//shoot ray from z axis to cylinder surface
	for (int j = 0; j < num_surface; ++j) {
		for (int i = 0; i < surface[j].vertex_positions_.num_elements_; ++i)
		{
			if (j == 1 || j == 2 || j == 5 || j == 6 || j == 8 || j == 9) {
				glm::vec2 ray_xy(surface[j].vertex_positions_[i].x_, surface[j].vertex_positions_[i].z_);
				glm::vec2 x_axis(1.0f, 0.0f);
				float theta = acos(dot(glm::normalize(ray_xy), x_axis));

				if (ray_xy.y < 0.0) theta *= -1;

				surface[j].vertex_uv_[i].x_ = theta / 3.141592f + 0.5f;
				surface[j].vertex_uv_[i].y_ = surface[j].vertex_positions_[i].y_ + 0.5f;
			}
			else {
				glm::vec4 ray_xy(surface[j].vertex_positions_[i].x_, surface[j].vertex_positions_[i].y_, surface[j].vertex_positions_[i].z_, 0.0);
				glm::vec4 plane;
				if (i % 4 == 0) plane = uPlane; else if (i % 4 == 1) plane = vPlane; else if (i % 4 == 2) plane = zPlane;
				else plane = tPlane;
				surface[j].vertex_uv_[i].x_ = dot(ray_xy, plane);
				surface[j].vertex_uv_[i].y_ = dot(ray_xy, plane);
			}

		}
	}

	/*surface.vertex_positions_.init(4);
	surface.vertex_positions_[0] = TV(-1.0, -1.0, 0.0);
	surface.vertex_positions_[1] = TV(1.0, -1.0, 0.0);
	surface.vertex_positions_[2] = TV(1.0, 1.0, 0.0);
	surface.vertex_positions_[3] = TV(-1.0, 1.0, 0.0);
	surface.vertex_normals_.init(4);
	surface.vertex_normals_[0] = TV(0.0, 0.0, 1.0);
	surface.vertex_normals_[1] = TV(0.0, 0.0, 1.0);
	surface.vertex_normals_[2] = TV(0.0, 0.0, 1.0);
	surface.vertex_normals_[3] = TV(0.0, 0.0, 1.0);
	surface.vertex_uv_.init(4);
	surface.vertex_uv_[0] = TV2(0.0, 0.0);
	surface.vertex_uv_[1] = TV2(1.0, 0.0);
	surface.vertex_uv_[2] = TV2(1.0, 1.0);
	surface.vertex_uv_[3] = TV2(0.0, 1.0);
	surface.triangles_.init(2);
	surface.triangles_[0] = TV_INT(0, 1, 2);
	surface.triangles_[1] = TV_INT(0, 2, 3);*/

	GL2_Object gl_obj[num_surface];
	for (int n = 0; n < num_surface; ++n) {
		gl_obj[n].initPhongSurfaceWithTexture(surface[n]);
	}
	gl_obj[20].mat_.setRed();
	//gl_obj2.mat_.setGold();

	// Simple texture
	glGenerateMipmap(GL_TEXTURE_2D);
	GLuint textures[num_surface];
	glGenTextures(num_surface, textures);


	// Texture for wall
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	int texture_width, texture_height;
	unsigned char *rgb_array;
	readBMP24("texture/stone.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	// do not forget these options!
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexParameter.xhtml
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for fire symbol

	glBindTexture(GL_TEXTURE_2D, textures[1]);
	readBMP24("texture/fire.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for water symbol
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	readBMP24("texture/water.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for skull symbol
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	readBMP24("texture/skull.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for tree symbol
	glBindTexture(GL_TEXTURE_2D, textures[4]);
	readBMP24("texture/green.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for sun symbol
	glBindTexture(GL_TEXTURE_2D, textures[5]);
	readBMP24("texture/sun.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for snow symbol
	glBindTexture(GL_TEXTURE_2D, textures[6]);
	readBMP24("texture/snow.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//Texture hell skull
	glBindTexture(GL_TEXTURE_2D, textures[7]);
	readBMP24("texture/hell.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for tree block
	glBindTexture(GL_TEXTURE_2D, textures[8]);
	readBMP24("texture/leave.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for fire block
	glBindTexture(GL_TEXTURE_2D, textures[9]);
	readBMP24("texture/fire.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for ice
	glBindTexture(GL_TEXTURE_2D, textures[10]);
	readBMP24("texture/ice3.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Texture for ice
	glBindTexture(GL_TEXTURE_2D, textures[11]);
	readBMP24("texture/leather.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Texture for tombstone
	glBindTexture(GL_TEXTURE_2D, textures[12]);
	readBMP24("texture/tombstone.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Texture for wood boat
	glBindTexture(GL_TEXTURE_2D, textures[13]);
	readBMP24("texture/wood2.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Texture for bone
	glBindTexture(GL_TEXTURE_2D, textures[14]);
	readBMP24("texture/bone.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Texture for bone
	glBindTexture(GL_TEXTURE_2D, textures[15]);
	readBMP24("texture/paper4.bmp", &texture_width, &texture_height, &rgb_array);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb_array);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);

	glLoadIdentity();

	GL2_Light light;
	//sound
	Sleep(10);
	PlaySound("evil_laugh.wav", NULL, SND_FILENAME);
	Sleep(10);
	PlaySound("theme_sound.wav", NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		gl_world.camera_.ContinueRotation();

		glm::mat4 vp = gl_world.camera_.GetWorldViewMatrix();

		for (int j = 8; j < 14; ++j) {
			if (my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.35, 2) || my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(-0.35, 2) || my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.28, 2) || my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(-0.28, 2))
			{
				lose = true;
			}
		}
		//tree
		if (my_round(surface[8].getAABB().getCenter().x_, 2) == my_round(surface[4].getAABB().getCenter().x_, 2) && my_round(surface[8].getAABB().getCenter().z_, 2) == my_round(surface[4].getAABB().getCenter().z_, 2)) { cond[0] = true;  }
		//ice
		if (my_round(surface[10].getAABB().getCenter().x_, 2) == my_round(surface[6].getAABB().getCenter().x_, 2) && my_round(surface[10].getAABB().getCenter().z_, 2) == my_round(surface[6].getAABB().getCenter().z_, 2)) { cond[1] = true; }
		//sun
		if (my_round(surface[11].getAABB().getCenter().x_, 2) == my_round(surface[5].getAABB().getCenter().x_, 2) && my_round(surface[11].getAABB().getCenter().z_, 2) == my_round(surface[5].getAABB().getCenter().z_, 2)) { cond[2] = true;  }
		//tomb
		if (my_round(surface[12].getAABB().getCenter().x_, 2) == my_round(surface[3].getAABB().getCenter().x_, 2) && my_round(surface[12].getAABB().getCenter().z_, 2) == my_round(surface[3].getAABB().getCenter().z_, 2)) { cond[3] = true;  }
		//water
		if (my_round(surface[13].getAABB().getCenter().x_, 2) == my_round(surface[2].getAABB().getCenter().x_, 2) && my_round(surface[13].getAABB().getCenter().z_, 2) == my_round(surface[2].getAABB().getCenter().z_, 2)) { cond[4] = true;  }
		//fire
		if (my_round(surface[9].getAABB().getCenter().x_, 2) == my_round(surface[1].getAABB().getCenter().x_, 2) && my_round(surface[9].getAABB().getCenter().z_, 2) == my_round(surface[1].getAABB().getCenter().z_, 2)) { cond[5] = true; }

		if (step[1] == 1)
		{
			//if hero in maximum range (wall - 3 pillars)
			if ((my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(-0.28,2)) || (my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(-0.14, 2) && my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(-0.07, 2)) || (my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(0.35, 2) && my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(0.14,2))) {}
			else {

			for (int j = 8; j < 14; ++j) {
			// if there is model to push
			if (my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(surface[j].getAABB().getCenter().x_, 2) && my_round((surface[15].getAABB().getCenter().z_ - 0.14), 2) == my_round(surface[j].getAABB().getCenter().z_, 2))
			{
				//checking obstacles
				//If the model is in the maximum range (wall - 3 pillars)
				if ((my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(-0.28, 2)) || (my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(-0.14, 2) && my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(-0.07, 2)) || (my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.35, 2) && my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.14, 2)))
				{
					step1 = true;
					break;
				}
				//if there is a block behind
				for (int n = 8; n < 14; ++n) {

					if (n == j) continue;
					else {
						if (my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(surface[n].getAABB().getCenter().x_, 2) && my_round((surface[j].getAABB().getCenter().z_ - 0.14), 2) == my_round(surface[n].getAABB().getCenter().z_, 2))
						{
							step1 = true; //a block behind found
							break;
						}
						else continue;
					}
				}
				//moving
				if (step1 == false) {
					for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
					{
						surface[15].vertex_positions_[i].z_ -= 0.14;
					}
					gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);

					for (int k = 0; k < surface[j].vertex_positions_.num_elements_; ++k)
					{
						surface[j].vertex_positions_[k].z_ -= 0.14;
					}
					gl_obj[j].updatePhongSurfaceWithTexture(surface[j]);

					break;
				}
				else break;
			}
			//nothing to push
			else if (j == 13) {
				for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
				{
					surface[15].vertex_positions_[i].z_ -= 0.14;
				}
				gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);
			}
			else continue;
		}
			}
			step[1] --;
			step1 = false;
		}

		if (step[2] == 1)
		{
			//if hero in maximum range
			if ((my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(0.28, 2)) || (my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(0.14, 2) && my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(-0.07, 2)) || (my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(0.35, 2) && my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(-0.14, 2))) {}
			else {
				for (int j = 8; j < 14; ++j) {

					if (my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(surface[j].getAABB().getCenter().x_, 2) && my_round((surface[15].getAABB().getCenter().z_ + 0.14), 2) == my_round(surface[j].getAABB().getCenter().z_, 2))
					{
						if ((my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.28, 2)) || (my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.14, 2) && my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(-0.07, 2)) || (my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.35, 2) && my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(-0.14, 2)))
						{
							step2 = true;
							break;
						}
						for (int n = 8; n < 14; ++n) {

							if (n == j) continue;
							else {
								if (my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(surface[n].getAABB().getCenter().x_, 2) && my_round((surface[j].getAABB().getCenter().z_ + 0.14), 2) == my_round(surface[n].getAABB().getCenter().z_, 2))
								{
									step2 = true; //if there is a block behind
									break;
								}
								else continue;
							}
						}
						if (step2 == false) {
							for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
							{
								surface[15].vertex_positions_[i].z_ += 0.14;
							}
							gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);

							for (int k = 0; k < surface[j].vertex_positions_.num_elements_; ++k)
							{
								surface[j].vertex_positions_[k].z_ += 0.14;
							}
							gl_obj[j].updatePhongSurfaceWithTexture(surface[j]);

							break;
						}
						else break;
					}
					else if (j == 13) {
						for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
						{
							surface[15].vertex_positions_[i].z_ += 0.14;
						}
						gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);
					}
					else continue;
				}
			}
			step[2] -- ;
			step2 = false;
		}


		if (step[3] == 1)
		{
			//check if hero in maximum range (wall - 2 pillars)
			if ((my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(-0.35, 2)) || (my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(0.28, 2) && my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(0.07, 2)) || (my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(-0.28, 2) && my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(0.07, 2))) {}
			else {
				for (int j = 8; j < 14; ++j) {
					//check if there is an model need to push
					if (my_round((surface[15].getAABB().getCenter().x_ - 0.14), 2) == my_round(surface[j].getAABB().getCenter().x_, 2) && my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(surface[j].getAABB().getCenter().z_, 2))
					{
						if ((my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(-0.35, 2)) || (my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.28, 2) && my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.07, 2)) || (my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(-0.28, 2) && my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.07, 2)))
						{
							step3 = true;
							break;
						}
						for (int n = 8; n < 14; ++n) {
							if (n == j) continue;
							else {
								if (my_round((surface[j].getAABB().getCenter().x_ - 0.14), 2) == my_round(surface[n].getAABB().getCenter().x_, 2) && my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(surface[n].getAABB().getCenter().z_, 2))
								{
									step3 = true; //check if it's blocked behind the model
									break;
								}
								else continue;
							}
						}
						if (step3 == false) {
							for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
							{
								surface[15].vertex_positions_[i].x_ -= 0.14;
							}
							gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);

							for (int k = 0; k < surface[j].vertex_positions_.num_elements_; ++k)
							{
								surface[j].vertex_positions_[k].x_ -= 0.14;
							}
							gl_obj[j].updatePhongSurfaceWithTexture(surface[j]);

							break;
						}
						else break;
					}
					else if (j == 13) {
						for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
						{
							surface[15].vertex_positions_[i].x_ -= 0.14;
						}
						gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);
					}
					else continue;
				}
			}

			step[3] --;
			step3 = false;
		}

		if (step[4] == 1)
		{
			//check if hero in maximum range (wall - 3 pillar)
			if ((my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(0.35, 2)) || (my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(0.21, 2) && my_round(surface[15].getAABB().getCenter().z_, 2) == 0) || (my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(0.28, 2) && my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(-0.21, 2)) || (my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(-0.28, 2) && my_round(surface[15].getAABB().getCenter().x_, 2) == my_round(-0.21, 2))) {}
			else {
				for (int j = 8; j < 14; ++j) {
					//check if there is an model need to push
					if (my_round((surface[15].getAABB().getCenter().x_ + 0.14), 2) == my_round(surface[j].getAABB().getCenter().x_, 2) && my_round(surface[15].getAABB().getCenter().z_, 2) == my_round(surface[j].getAABB().getCenter().z_, 2))
					{
						if ((my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.35, 2)) || (my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(0.21, 2) && my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.00,2)) || (my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(0.28, 2) && my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(-0.21, 2)) || (my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(-0.28, 2) && my_round(surface[j].getAABB().getCenter().x_, 2) == my_round(-0.21, 2)))
						{
							step4 = true;
							break;
						}
						for (int n = 8; n < 14; ++n) {

							if (n == j) continue;
							else {
								if (my_round((surface[j].getAABB().getCenter().x_ + 0.14), 2) == my_round(surface[n].getAABB().getCenter().x_, 2) && my_round(surface[j].getAABB().getCenter().z_, 2) == my_round(surface[n].getAABB().getCenter().z_, 2))
								{
									step4 = true; //check if it's blocked behind the model
									break;
								}
								else continue;
							}
						}
						if (step4 == false) {
							for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
							{
								surface[15].vertex_positions_[i].x_ += 0.14;
							}
							gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);

							for (int k = 0; k < surface[j].vertex_positions_.num_elements_; ++k)
							{
								surface[j].vertex_positions_[k].x_ += 0.14;
							}
							gl_obj[j].updatePhongSurfaceWithTexture(surface[j]);

							break;
						}
						else break;
					}
					else if (j == 13) {
						for (int i = 0; i < surface[15].vertex_positions_.num_elements_; ++i)
						{
							surface[15].vertex_positions_[i].x_ += 0.14;
						}
						gl_obj[15].updatePhongSurfaceWithTexture(surface[15]);
					}
					else continue;
				}
			}
			step[4] --;
			step4 = false;
		}



			for (int j = 1; j < 7; ++j) {
				for (int i = 0; i < surface[j].vertex_positions_.num_elements_; ++i)
				{
					if (j == 2) surface[j].vertex_uv_[i].y_ -= 0.0005f;
					else if (j == 4) break;
					else
						surface[j].vertex_uv_[i].y_ -= 0.005f;
				}
				// don't need to update if there is no change
				gl_obj[j].updatePhongSurfaceWithTexture(surface[j]);
			}
			for (int i = 0; i < surface[7].vertex_positions_.num_elements_; ++i)
			{

				surface[7].vertex_uv_[i].x_ -= 0.005f;
			}
			// don't need to update if there is no change
			gl_obj[7].updatePhongSurfaceWithTexture(surface[7]);

		// don't need to update if there is no change
		// gl_obj.updatePhongSurface(surface);
		// gl_obj.updatePhongSurfaceWithTexture(surface);
		if(winCheck(cond) == true) ps.advanceOneTimeStep(0.005f);
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(gl_world.shaders_.program_id_);

		gl_world.shaders_.sendUniform(vp, "mvp");

		// uniform to send texture image data
		GLint loc = glGetUniformLocation(gl_world.shaders_.program_id_, "my_texture");
		if (loc != -1)
		{
			glUniform1i(loc, 0); // 0 is index of our first texture
		}

		if (step[0] == 1 && winCheck(cond) == false && lose == false) {
				if (step5 == false) {
				const float asp = (float)width_window / (float)height_window;
				gl_world.camera_.SetProjection(0.75, asp, 0.001, 100.0);
				gl_world.camera_.rotate(45.2);
				step5 = true;

			}
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[0]);
			glUniform1i(loc, 0);
			gl_obj[0].applyLighting(light);
			gl_obj[0].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[1]);
			glUniform1i(loc, 0);
			gl_obj[1].applyLighting(light);
			gl_obj[1].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[2]);
			glUniform1i(loc, 0);
			gl_obj[2].applyLighting(light);
			gl_obj[2].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[3]);
			glUniform1i(loc, 0);
			gl_obj[3].applyLighting(light);
			gl_obj[3].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[4]);
			glUniform1i(loc, 0);
			gl_obj[4].applyLighting(light);
			gl_obj[4].drawTextureWithShader(gl_world.shaders_);


			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[5]);
			glUniform1i(loc, 0);
			gl_obj[5].applyLighting(light);
			gl_obj[5].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[6]);
			glUniform1i(loc, 0);
			gl_obj[6].applyLighting(light);
			gl_obj[6].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			glUniform1i(loc, 0);
			gl_obj[7].applyLighting(light);
			gl_obj[7].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[8]);
			glUniform1i(loc, 0);
			gl_obj[8].applyLighting(light);
			gl_obj[8].drawTextureWithShader(gl_world.shaders_);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[9]);
			glUniform1i(loc, 0);
			gl_obj[9].applyLighting(light);
			gl_obj[9].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[6]);
			glUniform1i(loc, 0);
			gl_obj[10].applyLighting(light);
			gl_obj[10].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[11]);
			glUniform1i(loc, 0);
			gl_obj[11].applyLighting(light);
			gl_obj[11].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[12]);
			glUniform1i(loc, 0);
			gl_obj[12].applyLighting(light);
			gl_obj[12].drawTextureWithShader(gl_world.shaders_);


			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[13]);
			glUniform1i(loc, 0);
			gl_obj[13].applyLighting(light);
			gl_obj[13].drawTextureWithShader(gl_world.shaders_);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[14]);
			glUniform1i(loc, 0);
			gl_obj[15].applyLighting(light);
			gl_obj[15].drawTextureWithShader(gl_world.shaders_);
		}

		if (step[0] == 0) {
			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[14]);
			glUniform1i(loc, 0);
			gl_obj[14].applyLighting(light);
			gl_obj[14].drawTextureWithShader(gl_world.shaders_);

			//scroll
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[15]);
			glUniform1i(loc, 0);
			gl_obj[16].applyLighting(light);
			gl_obj[16].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			glUniform1i(loc, 0);
			gl_obj[17].applyLighting(light);
			gl_obj[17].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[14]);
			glUniform1i(loc, 0);
			gl_obj[18].applyLighting(light);
			gl_obj[18].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			glUniform1i(loc, 0);
			gl_obj[19].applyLighting(light);
			gl_obj[19].drawTextureWithShader(gl_world.shaders_);
		}
		if (winCheck(cond) == true) {
			if (step5 == true) {
				const float zNear = 0.001, zFar = 100.0, fov = 45.0;
				gl_world.camera_.Resize(width, height, fov, zNear, zFar);
				gl_world.camera_.rotate(-45.2);
				step5 = false;
			}


			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			glUniform1i(loc, 0);
			gl_obj[21].applyLighting(light);
			gl_obj[21].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[15]);
			glUniform1i(loc, 0);
			gl_obj[16].applyLighting(light);
			gl_obj[16].drawTextureWithShader(gl_world.shaders_);

			//gl_obj.drawPhongSurface();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[14]);
			glUniform1i(loc, 0);
			gl_obj[18].applyLighting(light);
			gl_obj[18].drawTextureWithShader(gl_world.shaders_);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[14]);
			glUniform1i(loc, 0);
			gl_obj[20].applyLighting(light);
			gl_obj[20].drawTextureWithShader(gl_world.shaders_);
		}

		if (lose == true) {
			if (step5 == true) {
				const float zNear = 0.001, zFar = 100.0, fov = 45.0;
				gl_world.camera_.Resize(width, height, fov, zNear, zFar);
				gl_world.camera_.rotate(-45.2);
				step5 = false;
			}

			surface[7].scale(1.1, 1.1, 1.1);
			gl_obj[7].updatePhongSurfaceWithTexture(surface[7]);
			Sleep(10);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[7]);
			glUniform1i(loc, 0);
			gl_obj[7].applyLighting(light);
			gl_obj[7].drawTextureWithShader(gl_world.shaders_);

		}
		glUseProgram(0);

		// Old-style rendering

		glLoadMatrixf(&vp[0][0]);

		glLineWidth(2);
		//gl_world.drawAxes();

		// draw particles
		if (winCheck(cond) == true) {
			glDisable(GL_LIGHTING);

			glPointSize(10.0f);
			glBegin(GL_POINTS);

			for (int p = 0; p < ps.particles_.size(); ++p)
			{
				const TV3 color1 = TV3(0.0f, 1.0f, 1.0f), color2 = TV3(1.0, 0.0, 1.0f);
				const float alpha = ps.particles_[p].vel_.getMagnitude() * 0.5f;
				const TV3 blenced_color = alpha * color1 + (1.0f - alpha) * color2;
				if (ps.particles_[p].pos_.y_ < 0.15 && ps.particles_[p].pos_.x_ < 0.15) {
					glColor3fv(blenced_color.values_);
					glVertex3fv(ps.particles_[p].pos_.values_);
				}
			}


			/*	for (int p = 0; p < ps.particles_.size(); ++p)
				{
					const TV3 color1 = TV3(1.0f, 0.0f, 1.0f), color2 = TV3(1.0, 1.0, 0.0f);

					if (const float alpha = ps.particles_[p].pos_.y_ * 0.5f > 0.0) {
						const TV3 blenced_color = alpha * color1 + (1.0f - alpha) * color2;
						glColor3fv(blenced_color.values_);
						glVertex3fv(ps.particles_[p].pos_.values_);
					}

				}*/

			glEnd();
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	glfwTerminate();

	return 0;
}
