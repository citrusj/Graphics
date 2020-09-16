#include <stdio.h>
#include <stdlib.h>
#include <limits.h> //+
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;
float center_car_x = 0.0f;
int car_num = 1;
#include "objects.h"


//타이머 설정
unsigned int timestamp = 0;
void timer(int value) {
	timestamp = (timestamp + 1) % UINT_MAX;
	glutPostRedisplay();
	glutTimerFunc(5, timer, 0);
}
unsigned int scene_len = 8000;

void display(void) {
	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);

	//airplane in
	if ((timestamp % scene_len > 1080) && (timestamp % scene_len <= 2160)) {
		float tb_airplane[2][9] = {
			{500, -0.007f, 4, -300.0f, 200.0f, 90, 2.0f, 2.0f, 1.0f},
			{300, -0.01f, 3, -300.0f, 100.0f, 90, 1.5f, 3.0f, -1.0f}
		};
		int airplane_clock;
		float route_airplane;
		float rotate_airplane;
		if ((timestamp / 40) % 2 == 0) {
			for (int n = 0; n < 2; n++) {

				for (int i = 0; i < tb_airplane[n][2]; i++) {
					airplane_clock = (timestamp + 100 * i) % int(tb_airplane[n][0]) - int(tb_airplane[n][0] / 2);
					route_airplane = (airplane_clock * airplane_clock) * (tb_airplane[n][1]);
					rotate_airplane = atanf((tb_airplane[n][1]) * 2 * (airplane_clock));
					ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(tb_airplane[n][8], 1.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(airplane_clock+ tb_airplane[n][3], route_airplane+ tb_airplane[n][4], 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, rotate_airplane + 90 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(tb_airplane[n][6], tb_airplane[n][7], 1.0f));
					ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
					glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
					draw_airplane();
				}
			}
		}
		airplane_clock = (timestamp) % 361;
		if (airplane_clock < 200) {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, 1.0f));
		    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.9f, 1.50f, 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, airplane_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::translate(ModelMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
			ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0f, 2.0f, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_airplane();
		}

	}



	//house rain
	int house_show_clock;
	if (timestamp % scene_len > 2490 && timestamp % scene_len <= 3000) {
		float rotate_house = (timestamp / 2) % 360;
		for (int i = 0; i < 12; i++) {
			for (int n = 0; n < 5; n++) {
				house_show_clock = ((timestamp + 30 * n + ((i + 3) * (n + 2) * 11))) % 450;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((i - 6) * 35.0f, 390 - house_show_clock, 0.0f));
				ModelMatrix = glm::rotate(ModelMatrix, 90 * (i % 4) * TO_RADIAN + rotate_house * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.6f, 0.6f, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_house();
			}
		}
	}

	//cocktail
	int cocktail_clock = ((timestamp) % 361);
	int radius_cocktail = ((((timestamp / 200) % 10) * 10));

	////cocktail rotate
	//for (int i = 0; i < 5; i++) {
	//	radius_cocktail = (i + 1) * 40;
	//	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(300.0f, 0.0f, 0.0f));
	//	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(radius_cocktail * (-1), radius_cocktail * (-1), 0.0f)); //지우든지말든지
	//	ModelMatrix = glm::rotate(ModelMatrix, cocktail_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	//	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(radius_cocktail, radius_cocktail, 0.0f));
	//	if (i % 2 == 1) {
	//		ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	//	}
	//	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));

	//	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	//	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//	draw_cocktail();
	//}

	//cocktail in
	if ((timestamp % scene_len > 2160) && (timestamp % scene_len <= 2490)) {
		int cocktail_in_clock = ((timestamp) % 270 / 3);
		if (timestamp % scene_len >= 2430) cocktail_in_clock = 90;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(260.0f, 40.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, cocktail_in_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 60.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(7.0f, 6.0f, 1.0f));

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();

		ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(280.0f, 140.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, cocktail_in_clock * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 60.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(7.0f, 6.0f, 1.0f));

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cocktail();
	}


	//road

	if (timestamp % scene_len >= 4291) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -400.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(25.0f, 25.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_road();
	}
	//road small
	if (timestamp % scene_len >= 3890 && timestamp % scene_len < 4291)
	{
		float scale_road = (timestamp % scene_len - 3890) / 400.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -400.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(25.0f * scale_road, 25.0f * scale_road, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_road();
	}

	float tree_x, tree_y;
	//tree
	if (timestamp % scene_len >= 4320) {
		int tree_clock = (timestamp % 720) /2;
		if (tree_clock >= 51) tree_clock = 50;
		int tree_clock2 = timestamp % 720;
		float tree_y_scale = 5.0f *tree_clock / 50.0f;
		for (int i = 5; i >=0; i--) {
			for (int j = -1; j < 3; j += 2) {
				tree_x = (-560.0f + 90.0f * i) * j; tree_y = -230.0f + 120.0f * i;
				ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tree_x, tree_y, 0.0f));
				if (tree_clock2 <= 360) {
					int route_tree_factor = 1; 
					if (tree_clock2 < 180) route_tree_factor = -1;
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(20.0f, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, tree_clock2 * 1.05f * TO_RADIAN * route_tree_factor, glm::vec3(0.0f, 0.0f, 1.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-20.0f, 0.0f, 0.0f));
					ModelMatrix = glm::rotate(ModelMatrix, (-tree_clock2) * TO_RADIAN *route_tree_factor, glm::vec3(0.0f, 0.0f, 1.0f));
				}
				ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.5f, tree_y_scale, 1.0f));
				ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
				glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
				draw_tree();
			}
		}
	}
	//car
	if (timestamp % scene_len >= 4400) {
		int car_clock = ((timestamp % 1442) / 2 - 360) * -1;
		float route_width_car = abs(((car_clock - 540) / 180)) * 90;
		float rotation_angle_car = fabs(atanf(route_width_car * 2 * TO_RADIAN * cosf(car_clock * TO_RADIAN * 2))) * -1;
		float scailing_factor_car = (-0.01f) * (car_clock - 360)* 2.0f;

		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3( -route_width_car * sinf(2.0 * car_clock * TO_RADIAN)+ center_car_x,
			(float)car_clock, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scailing_factor_car, scailing_factor_car, 1.0f));

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (car_num == 1) {
			draw_car();
		}
		else draw_car2();
	}

	//cake

	//cake in
	int cake_clock;
	float route_cake, rotate_cake;
	float tb_cake[6][5] = { //dx, dy, rotate, scale, trans
		{1.0f, 1.0f, 0.5f, 8.0f, 150.0f},
		{-3.0f, 0.3f, 0.8f, 6.0f, -150.0f},
		{2.0f, 2.0f, 0.5f, 9.0f, 100.0f},
		{-2.0f, 5.0f, 1.6f, 8.0f, -130.0f}, 
		{-1.0f, 3.0f, 3.0f,7.0f, -120.4f},
		{2.5f, 0.9f, 0.3f, 10.0f, 30.0f}
	};
	if ((timestamp % scene_len) >= 360 && (timestamp % scene_len) <= 1080) {
		for (int i = 0; i < 6; i++) {
			cake_clock = (((timestamp - 360) % 721) / 2 - 360) * int(tb_cake[i][0]);
			route_cake = tb_cake[i][1] * fabs(cake_clock) - 150.0f;
			rotate_cake = cake_clock * TO_RADIAN * tb_cake[i][2];

			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cake_clock + tb_cake[i][4], route_cake, 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, rotate_cake, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(tb_cake[i][3], tb_cake[i][3], 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_cake();

		}
	}

	if ((timestamp % scene_len) >= 720 && (timestamp % scene_len) <= 1080) {
		for (int i = 0; i < 6; i++) {
			cake_clock = (timestamp % 360 - 360) * int(tb_cake[i][0]);
			route_cake = tb_cake[i][1] * fabs(cake_clock) - 150.0f;
			rotate_cake = cake_clock * TO_RADIAN * tb_cake[i][2];
			int scale_cake = tb_cake[i][3] * 0.4;
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cake_clock + tb_cake[i][4], route_cake, 1.0f));
			ModelMatrix = glm::rotate(ModelMatrix, rotate_cake, glm::vec3(0.0f, 0.0f, 1.0f));
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale_cake, scale_cake, 1.0f));
			ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_cake();

		}
	}
	//hat
	
	if (timestamp % scene_len < 3131) {
		if (timestamp % scene_len == 780) {
			hat_color[1][0] = 255 / 255.0f; hat_color[1][1] = 102 / 255.0f; hat_color[1][2] = 255 / 255.0f;
			hat_color[3][0] = 255 / 255.0f; hat_color[3][1] = 102 / 255.0f; hat_color[3][2] = 255 / 255.0f;
		}
		if (timestamp % scene_len == 1090) {
			hat_color[1][0] = 111 / 255.0f; hat_color[1][1] = 85 / 255.0f; hat_color[1][2] = 157 / 255.0f;
			hat_color[3][0] = 111 / 255.0f; hat_color[3][1] = 85 / 255.0f; hat_color[3][2] = 157 / 255.0f;
		}
		if (timestamp % scene_len == 2370) {
			hat_color[1][0] = 0 / 255.0f; hat_color[1][1] = 63 / 255.0f; hat_color[1][2] = 122 / 255.0f;
			hat_color[3][0] = 0 / 255.0f; hat_color[3][1] = 63 / 255.0f; hat_color[3][2] = 122 / 255.0f;
			cocktail_color[1][0] = 235 / 255.0f; cocktail_color[1][1] = 225 / 255.0f; cocktail_color[1][2] = 196 / 255.0f;
		}
		if (timestamp % scene_len == 2500) {
			hat_color[1][0] = 235 / 255.0f; hat_color[1][1] = 225 / 255.0f; hat_color[1][2] = 196 / 255.0f;
			hat_color[3][0] = 235 / 255.0f; hat_color[3][1] = 225 / 255.0f; hat_color[3][2] = 196 / 255.0f;
		}

		float scale_hat_factor = (timestamp % scene_len) * 0.04f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -100.0f, 0.0f));
		if ((timestamp % scene_len) < 300) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale_hat_factor, scale_hat_factor, 1.0f));
		}
		else if ((timestamp % scene_len) < 3131) {
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.0f, 12.0f, 1.0f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hat();
	}
	//진동
	if (timestamp % scene_len >= 3131 &&timestamp%scene_len< 3490) {
		float rotate_hat_factor = 3 * sinf((timestamp % 359) - 179);
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -100.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (180+ rotate_hat_factor) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.0f, 12.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hat();
	}
	//hat small
	if (timestamp % scene_len >= 3490 && timestamp % scene_len < 3890) {
		float scale_hat = (400 - (timestamp % scene_len - 3490)) / 400.0f;
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -100.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(12.0f*(scale_hat), 12.0f*(scale_hat), 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_hat();
	}

	
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

int leftbuttonpressed = 0;
void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

void motion(int x, int y) {
	if (leftbuttonpressed) {
		center_car_x = x - win_width / 2.0f;
		//centerx = x - win_width / 2.0f, centery = (win_height - y) - win_height / 2.0f;
		car_num = (car_num + 1) % 2;
		glutPostRedisplay();
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);

	glDeleteVertexArrays(1, &VAO_cake);
	glDeleteBuffers(1, &VBO_cake);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);

	glDeleteVertexArrays(1, &VAO_hat);
	glDeleteBuffers(1, &VBO_hat);

	glDeleteVertexArrays(1, &VAO_car);
	glDeleteBuffers(1, &VBO_car);

	glDeleteVertexArrays(1, &VAO_cocktail);
	glDeleteBuffers(1, &VBO_cocktail);

	glDeleteVertexArrays(1, &VAO_car2);
	glDeleteBuffers(1, &VBO_car2);

	glDeleteVertexArrays(1, &VAO_sword);
	glDeleteBuffers(1, &VBO_sword);

	glDeleteVertexArrays(1, &VAO_road);
	glDeleteBuffers(1, &VBO_road);

	glDeleteVertexArrays(1, &VAO_tree);
	glDeleteBuffers(1, &VBO_tree);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	//+
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	//
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	prepare_car2();
	prepare_hat();
	prepare_cake();
	prepare_sword();
	prepare_tree();
	prepare_road();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.0.1.3";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


