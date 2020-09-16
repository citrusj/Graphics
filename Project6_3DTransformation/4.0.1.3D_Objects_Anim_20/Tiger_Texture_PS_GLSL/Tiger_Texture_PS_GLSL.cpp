#define _CRT_SECURE_NO_WARNINGS

#include "my-geom-objects.h"


glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewMatrix, ProjectionMatrix;

glm::mat4 Iron_ModelViewMatrix;
glm::mat3 Iron_ModelViewMatrixInvTrans;

glm::mat4 ModelMatrix_Iron_eye;
glm::mat4 Matrix_Iron_eye_inverse;
glm::mat4 ModelMatrix_Iron_to_eye;
glm::mat4 ModelMatrix_Iron;

//camera
typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move;
}Camera;

Camera camera;

void set_ViewMatrix_from_camera_frame(void) {
	ViewMatrix = glm::mat4(camera.uaxis[0], camera.vaxis[0], camera.naxis[0], 0.0f,
		camera.uaxis[1], camera.vaxis[1], camera.naxis[1], 0.0f,
		camera.uaxis[2], camera.vaxis[2], camera.naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ViewMatrix = glm::translate(ViewMatrix, glm::vec3(-camera.pos[0], -camera.pos[1], -camera.pos[2]));
}
void set_ViewMatrix_from_Iron(void) {
	
}

enum axes {U_AXIS, V_AXIS, N_AXIS};
int flag_rotate_axis, flag_translate_axis=0;

void initialize_camera(void) {
	camera.pos[0] = 300.0f; 
	camera.pos[1] = 400.0f;
	camera.pos[2] = 300.0f;

	//
	glm::vec3 n = glm::normalize(glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]) - glm::vec3(0.0f, 0.0f, 0.0f));
	glm::vec3 u = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f),n));
	glm::vec3 v = glm::cross(n, u);


	camera.uaxis[0] = u[0]; camera.uaxis[1] = u[1]; camera.uaxis[2] = u[2];
	camera.vaxis[0] = v[0]; camera.vaxis[1] = v[1]; camera.vaxis[2] = v[2];
	camera.naxis[0] = n[0]; camera.naxis[1] = n[1]; camera.naxis[2] = n[2];
	
	camera.move = 0;
	camera.fovy = 45.0f, camera.aspect_ratio = 1.0f; camera.near_c = 0.1f; camera.far_c = 1000.0f;

	set_ViewMatrix_from_camera_frame();
}
#define CAM_RSPEED 0.1f
void renew_cam_rotation_around_axis(int angle)
{
	glm::mat3 RotationMatrix;
	glm::vec3 direction;
	switch (flag_rotate_axis) {
	case V_AXIS:
		RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angle,
			glm::vec3(camera.vaxis[0], camera.vaxis[1], camera.vaxis[2])));

		direction = RotationMatrix * glm::vec3(camera.uaxis[0], camera.uaxis[1], camera.uaxis[2]);
		camera.uaxis[0] = direction.x; camera.uaxis[1] = direction.y; camera.uaxis[2] = direction.z;
		direction = RotationMatrix * glm::vec3(camera.naxis[0], camera.naxis[1], camera.naxis[2]);
		camera.naxis[0] = direction.x; camera.naxis[1] = direction.y; camera.naxis[2] = direction.z;
		break;
	case N_AXIS:
		RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angle,
			glm::vec3(camera.naxis[0], camera.naxis[1], camera.naxis[2])));

		direction = RotationMatrix * glm::vec3(camera.uaxis[0], camera.uaxis[1], camera.uaxis[2]);
		camera.uaxis[0] = direction.x; camera.uaxis[1] = direction.y; camera.uaxis[2] = direction.z;
		direction = RotationMatrix * glm::vec3(camera.vaxis[0], camera.vaxis[1], camera.vaxis[2]);
		camera.vaxis[0] = direction.x; camera.vaxis[1] = direction.y; camera.vaxis[2] = direction.z;
		break;
	case U_AXIS:
		RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angle,
			glm::vec3(camera.uaxis[0], camera.uaxis[1], camera.uaxis[2])));

		direction = RotationMatrix * glm::vec3(camera.naxis[0], camera.naxis[1], camera.naxis[2]);
		camera.naxis[0] = direction.x; camera.naxis[1] = direction.y; camera.naxis[2] = direction.z;
		direction = RotationMatrix * glm::vec3(camera.vaxis[0], camera.vaxis[1], camera.vaxis[2]);
		camera.vaxis[0] = direction.x; camera.vaxis[1] = direction.y; camera.vaxis[2] = direction.z;
		break;
	}
	set_ViewMatrix_from_camera_frame();
	glutPostRedisplay();
}
void renew_cam_zoom(int fovchange)
{
	if (camera.fovy + fovchange < 10.0f || camera.fovy + fovchange>150.0f) return;
	camera.fovy += fovchange;
	ProjectionMatrix = glm::perspective(camera.fovy * TO_RADIAN, camera.aspect_ratio, 100.0f, 20000.0f);
	glutPostRedisplay();

}



// callbacks
float PRP_distance_scale[6] = { 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f };

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);

	if (shading_flag == 0) {
		glUseProgram(h_ShaderProgram_TXPS);
	}
	else {
		glUseProgram(h_ShaderProgram_GS);
	}

	set_material_floor();
	if (shading_flag == 0) {
		glUniform1i(loc_texture, TEXTURE_ID_FLOOR);
	}
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-500.0f, 0.0f, 500.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1000.0f, 1000.0f, 1000.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	draw_floor();
	if (shading_flag == 0) {
		//screen
		if (flag_screen_effect > 0) {
			set_material_screen();
			//	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-250.0f, 0.0f, 250.0f));
			//	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(500.0f, 500.0f, 500.0f));
			//	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
			ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(250.0f, 250.0f, 250.0f));
			ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
			ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

			glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
			glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

			glUniform1i(loc_screen_effect, flag_screen_effect);
			draw_floor();
			glUniform1i(loc_screen_effect, 0);
		}
	}
	set_material_tiger();
	if (shading_flag == 0) {
		glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	}
	ModelViewMatrix = glm::rotate(ViewMatrix, 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	int tiger_dir = rotation_angle_tiger / 90;
	float move_tiger = int(rotation_angle_tiger) % 90;
	if (tiger_dir == 0) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, move_tiger, -move_tiger * 2));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(90.0f, 0.0f, 90.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -180 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (tiger_dir == 1) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-move_tiger * 2, 90.0f - move_tiger, 0.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(90.0f, 0.0f, -90.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (tiger_dir == 2) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, move_tiger, move_tiger * 2));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-90.0f, 0.0f, -90.0f));
	}
	else if (tiger_dir == 3) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(move_tiger * 2, 90.0f - move_tiger, 0.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-90.0f, 00.0f, 90.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}

	draw_tiger();

	glUseProgram(h_ShaderProgram_simple);
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	if (shading_flag == 0) {
		glUseProgram(h_ShaderProgram_TXPS);
	}
	else {
		glUseProgram(h_ShaderProgram_GS);
	}


	set_material_ben(); //moving person
	float rotate_angle_person = int(move_distance_person) % 180;
	float trans_person = int(move_distance_person) % 360;
	if (shading_flag == 0) {
		glUniform1i(loc_texture, TEXTURE_ID_BEN);
	}
	ModelViewMatrix = glm::rotate(ViewMatrix, 45*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	if (move_distance_person >= 360.0f) {
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(-1.0f, 1.0f, 1.0f));
	}
	if (trans_person <= 180.0f) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(100.0f, 0.0f, 00.0f));
	}
	else {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(-100.0f, 0.0f, 00.0f));
	}
	ModelViewMatrix = glm::rotate(ModelViewMatrix, (rotate_angle_person) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(50.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, -100.0f, -100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	draw_ben();

	/*
	set_material_wolf();
	glUniform1i(loc_texture, TEXTURE_ID_WOLF);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(60.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, 100.0f,100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_wolf();
	*/

	set_material_tiger();
	if (shading_flag == 0) {
		glUniform1i(loc_texture, TEXTURE_ID_SPIDER);
	}
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -50.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, move_distance_spider * 0.1f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, move_distance_spider * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(50.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, -20.0f, 20.0f));

	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	draw_spider();
	
	/*if (camera_iron == 1) {
		ModelMatrix_Iron_to_eye = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 8.0f));
		ModelMatrix_Iron_to_eye = glm::rotate(ModelMatrix_Iron_to_eye, 180*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

		ModelMatrix_Iron = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f));
		ModelMatrix_Iron = glm::translate(ModelViewMatrix, glm::vec3(0.0f, move_distance_spider * 0.1f, 0.0f));
		ModelMatrix_Iron = glm::rotate(ModelViewMatrix, move_distance_spider * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_Iron = glm::translate(ModelViewMatrix, glm::vec3(50.0f, 0.0f, 0.0f));
		ModelMatrix_Iron = glm::scale(ModelViewMatrix, glm::vec3(20.0f, -20.0f, 20.0f));

		Matrix_Iron_eye_inverse = ModelMatrix_Iron * ModelMatrix_Iron_to_eye;
		ViewMatrix = glm::affineInverse(Matrix_Iron_eye_inverse);
	}*/



	set_material_tiger();
	if (shading_flag == 0) {
		if (flag_dragon_texture) {
			glUniform1i(loc_texture, TEXTURE_ID_DRAGON);
		}
		else {
			glUniform1i(loc_texture, TEXTURE_ID_TIGER);
		}
	}
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-50.0f, 50.0f, 20.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	draw_dragon();
	/*
	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(80.0f, 0.0f, -50.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_optimus();
	
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100.0f, 30.0f, 10.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(80.0f, 80.0f, 80.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_cow();
	*/
	//set_material_tiger();
	if (shading_flag == 0) {
		glUseProgram(h_ShaderProgram_TXPS);
	}
	else {
		glUseProgram(h_ShaderProgram_GS);
	}

	//glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-80.0f, 0.0f, 80.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	draw_bike();
	
	if (shading_flag == 0) {
		glUseProgram(h_ShaderProgram_TXPS);
	}
	else {
		glUseProgram(h_ShaderProgram_GS);
	}
	/*set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100.0f, 0.0f, 80.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(3.0f, 3.0f, 3.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_bus();*/
	
	/*set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(150.0f, 0.0f, 60.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_godzilla();
	*/
	//아이언맨
	set_material_tiger();
	if (shading_flag == 0) {
		glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	}
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 50.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -45.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, -1.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, move_distance_ironman * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, move_distance_ironman * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	//ModelViewMatrix = glm::rotate(ModelViewMatrix, 90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(25.0f, 25.0f, 25.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	Iron_ModelViewMatrix = ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	Iron_ModelViewMatrixInvTrans = ModelViewMatrixInvTrans;

	if (shading_flag == 0) {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	else {
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	}
	draw_ironman();
	draw_axes();
	glm::vec4 position_EC = Iron_ModelViewMatrix * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	if (shading_flag == 0) {
		glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	}
	else {
		glUniform4fv(loc_light_gs[3].position, 1, &position_EC[0]);
	}
	glm::vec3 direction_EC = glm::mat3(Iron_ModelViewMatrixInvTrans) * glm::vec3(light[3].spot_direction[0],
		light[3].spot_direction[1], light[3].spot_direction[2]);
	if (shading_flag == 0) {
		glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	}
	else {
		glUniform3fv(loc_light_gs[3].spot_direction, 1, &direction_EC[0]);
	}
	
	if (camera_iron == 1) {
		ModelMatrix_Iron_to_eye = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 8.0f));
		/*ModelMatrix_Iron_to_eye = glm::rotate(ModelMatrix_Iron_to_eye, -90 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_Iron_to_eye = glm::rotate(ModelMatrix_Iron_to_eye, 270 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_Iron_to_eye = glm::rotate(ModelMatrix_Iron_to_eye, 270*TO_RADIAN, glm::vec3(1.0f, .0f, 0.0f));*/

		ModelMatrix_Iron = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 50.0f, 0.0f));
		ModelMatrix_Iron = glm::rotate(ModelMatrix_Iron, -45.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, -1.0f));
		ModelMatrix_Iron = glm::rotate(ModelMatrix_Iron, move_distance_ironman * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_Iron = glm::translate(ModelMatrix_Iron, glm::vec3(200.0f, 0.0f, 0.0f));
		ModelMatrix_Iron = glm::rotate(ModelMatrix_Iron, -90 * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix_Iron = glm::rotate(ModelMatrix_Iron, move_distance_ironman * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_Iron = glm::scale(ModelMatrix_Iron, glm::vec3(25.0f, 25.0f, 25.0f));
		Matrix_Iron_eye_inverse = ModelMatrix_Iron * ModelMatrix_Iron_to_eye;
		ViewMatrix = glm::affineInverse(Matrix_Iron_eye_inverse);
	}
	


	/*
	set_material_tank();
	glUniform1i(loc_texture, TEXTURE_ID_TANK);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(50.0f, 0.0f, 230.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(10.0f, 10.0f, 10.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_tank();
	*/
	for (int i = 0; i < 2; i++) {
		position_EC = ViewMatrix * glm::vec4(light[i].position[0], light[i].position[1],
			light[i].position[2], light[i].position[3]);
		if (shading_flag == 0) {
			glUniform4fv(loc_light[i].position, 1, &position_EC[0]);
		}
		else {
			glUniform4fv(loc_light_gs[i].position, 1, &position_EC[0]);
		}
		direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[i].spot_direction[0],
			light[i].spot_direction[1], light[i].spot_direction[2]);
		if (shading_flag == 0) {
			glUniform3fv(loc_light[i].spot_direction, 1, &direction_EC[0]);
		}
		else {
			glUniform3fv(loc_light_gs[i].spot_direction, 1, &direction_EC[0]);
		}
	}
	
	if (shading_flag == 0) {
		glUniform1f(loc_blind_timer, light_timer);
	}
	glUseProgram(0);

	glutSwapBuffers();
}

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	cur_frame_tiger = timestamp_scene % N_TIGER_FRAMES;
	cur_frame_ben = timestamp_scene % N_BEN_FRAMES;
	cur_frame_wolf= timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;
	move_distance_ironman = timestamp_scene % 360;
	move_distance_person = timestamp_scene % 720;
	move_distance_spider = timestamp_scene % 1080;
	move_distance_tiger = timestamp_scene % 360;
	rotation_angle_tiger = (timestamp_scene % 360);
	light_timer = (timestamp_scene % 270);
	//

	glutPostRedisplay();
	if (flag_tiger_animation)
		glutTimerFunc(10, timer_scene, 0);
}

//static int flag_blind_effect = 0;
void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0;
	static int PRP_distance_level = 4;
	//
	
	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
		int light_ID = (int)(key - '0');

		if (shading_flag == 0) {
			glUseProgram(h_ShaderProgram_TXPS);
		}
		else {
			glUseProgram(h_ShaderProgram_GS);
		}
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		if (shading_flag == 0) {
			glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		}
		else {
			glUniform1i(loc_light_gs[light_ID].light_on, light[light_ID].light_on);
		}
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 'a': // toggle the animation effect.
		flag_tiger_animation = 1 - flag_tiger_animation;
		if (flag_tiger_animation) {
			glutTimerFunc(100, timer_scene, 0);
			fprintf(stdout, "^^^ Animation mode ON.\n");
		}
		else
			fprintf(stdout, "^^^ Animation mode OFF.\n");
		break;
	case 'f':
		flag_fog = 1 - flag_fog;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_fog, flag_fog);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 't':
		flag_dragon_texture = 1 - flag_dragon_texture;
		glutPostRedisplay();
		break;
	case 'k':
		flag_texture_mapping = 1 - flag_texture_mapping;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		}
		break;
	case 'd':
		PRP_distance_level = (PRP_distance_level + 1) % 6;
		fprintf(stdout, "^^^ Distance level = %d.\n", PRP_distance_level);

		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(500.0f, 300.0f, 500.0f),
			glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		if (shading_flag == 0) {
			glUseProgram(h_ShaderProgram_TXPS);
		}
		else {
			glUseProgram(h_ShaderProgram_GS);
		}
		
		// Must update the light 1's geometry in EC.
		position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
			light[1].position[2], light[1].position[3]);
		glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
		direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0],
			light[1].spot_direction[1], light[1].spot_direction[2]);
		glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'p':
		flag_polygon_fill = 1 - flag_polygon_fill;
		if (flag_polygon_fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups
		break;
	case 'u':
		flag_rotate_axis = U_AXIS;
		break;
	case 'v':
		flag_rotate_axis = V_AXIS;
		break;
	case 'n':
		flag_rotate_axis = N_AXIS;
		break;
	case 'i':
		flag_translate_axis = 1 - flag_translate_axis;
		break;
	case 'q':
		shading_flag = 1 - shading_flag;
		glutPostRedisplay();
		break;
	case 'w':
		flag_blind_effect = 1 - flag_blind_effect;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_blind_effect, flag_blind_effect);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	case 'e':
		if (screen_frequency > 12.0f) {
			flag_screen_effect = 0;
			screen_frequency = 1.0;

		}
		else {
			flag_screen_effect = 1;
			
			glUseProgram(h_ShaderProgram_TXPS);
			glUniform1f(loc_screen_frequency, screen_frequency);
			glUseProgram(0);
			glutPostRedisplay();
			screen_frequency += 1.0f;
		}
		break;
	case 'r':
		camera_iron = 1 - camera_iron;
		if (camera_iron == 0) {
			set_ViewMatrix_from_camera_frame();
		}
		glutPostRedisplay();
	}
}
int prevx, prevy, camera_move;
#define CAM_TSPEED 0.2f
void motion(int x, int y)
{
	if (!camera_move) return;

	int xchange = prevx - x, ychange = prevy - y;
	if (flag_translate_axis == 0) {
		camera.pos[0] += CAM_TSPEED * xchange * (camera.uaxis[0]);
		camera.pos[1] += CAM_TSPEED * xchange * (camera.uaxis[1]);
		camera.pos[2] += CAM_TSPEED * xchange * (camera.uaxis[2]);

		camera.pos[0] += CAM_TSPEED * ychange * (-camera.vaxis[0]);
		camera.pos[1] += CAM_TSPEED * ychange * (-camera.vaxis[1]);
		camera.pos[2] += CAM_TSPEED * ychange * (-camera.vaxis[2]);
	}
	else {
		camera.pos[0] += 0.5 * xchange * (-camera.naxis[0]);
		camera.pos[1] += 0.5 * xchange * (-camera.naxis[1]);
		camera.pos[2] += 0.5 * xchange * (-camera.naxis[2]);
	}
	prevx = x; prevy = y;
	set_ViewMatrix_from_camera_frame();
	
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			camera_move = 1;
			prevx = x; prevy = y;
		}
		else if (state == GLUT_UP) camera_move = 0;
	}
}
void special(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		renew_cam_rotation_around_axis(-10);
		break;
	case GLUT_KEY_LEFT:
		renew_cam_rotation_around_axis(10);
		break;
	case GLUT_KEY_UP:
		renew_cam_zoom(-3);
		break;
	case GLUT_KEY_DOWN:
		renew_cam_zoom(3);
		break;
	}
}
void reshape(int width, int height) {
	
	
	glViewport(0, 0, width, height);
	
	camera.aspect_ratio = (float) width / height;
	ProjectionMatrix = glm::perspective(camera.fovy*TO_RADIAN, camera.aspect_ratio, 100.0f, 20000.0f);

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO); 
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &rectangle_VAO);
	glDeleteBuffers(1, &rectangle_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_GS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
		{ GL_NONE, NULL }
	};
	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "u_blind_effect");
	loc_blind_timer = glGetUniformLocation(h_ShaderProgram_TXPS, "u_blind_timer");

	//screen
	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_effect");
	loc_screen_frequency = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_frequency");
	loc_screen_width = glGetUniformLocation(h_ShaderProgram_TXPS, "screen_width");


	//gouraud shading
	
	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color_gs = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light_gs[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light_gs[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light_gs[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light_gs[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light_gs[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light_gs[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light_gs[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light_gs[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light_gs[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}

	loc_material_gs.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material_gs.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material_gs.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material_gs.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material_gs.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");

	/*loc_texture = glGetUniformLocation(h_ShaderProgram_TXGS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXGS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXGS, "u_flag_fog");*/

	
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	//screen
	glUniform1i(loc_screen_effect, 0);
	glUniform1f(loc_screen_frequency, 1.0f);
	glUniform1f(loc_screen_width, 0.125f);

	//blind
	glUniform1i(loc_blind_effect, 0);

	//glUseProgram(0);


	//gouraud
	glUseProgram(h_ShaderProgram_GS);

	glUniform4f(loc_global_ambient_color_gs, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light_gs[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light_gs[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light_gs[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light_gs[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light_gs[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light_gs[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light_gs[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light_gs[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light_gs[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light_gs[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light_gs[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material_gs.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material_gs.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material_gs.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material_gs.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material_gs.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void initialize_flags(void) {
	flag_tiger_animation = 1;
	flag_polygon_fill = 1;
	flag_texture_mapping = 1;
	flag_fog = 0;
	shading_flag = 0;
	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_flag_fog, flag_fog);
	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {

	glEnable(GL_MULTISAMPLE);


  	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(500.0f, 300.0f, 500.0f),
	//	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ViewMatrix = glm::lookAt(glm::vec3(camera.pos[0], camera.pos[1], camera.pos[2]),
		glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	initialize_lights_and_material();
	initialize_flags();
	flag_rotate_axis = V_AXIS;
	glGenTextures(N_TEXTURES_USED, texture_names);
}

void set_up_scene_lights(void) {
	// point_light_WC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 100.0f; 	// point light position in WC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.7f; light[0].diffuse_color[1] = 0.7f;
	light[0].diffuse_color[2] = 0.7f; light[0].diffuse_color[3] = 1.5f;

	light[0].specular_color[0] = 0.9f; light[0].specular_color[1] = 0.9f;
	light[0].specular_color[2] = 0.9f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = -800.0f; light[1].position[1] = 500.0f; // spot light position in WC
	light[1].position[2] = -800.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.2125f; light[1].ambient_color[1] = 0.1275f;
	light[1].ambient_color[2] = 0.054f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.714f; light[1].diffuse_color[1] = 0.4284f;
	light[1].diffuse_color[2] = 0.181f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.3935f; light[1].specular_color[1] = 0.271f;
	light[1].specular_color[2] = 0.166f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 1.8f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 1.8f;
	light[1].spot_cutoff_angle = 5.0f;
	light[1].spot_exponent = 8.0f;

	//light 2
	// spot_light_EC: use light 2
	light[2].light_on = 0;
	light[2].position[0] = 0.0f; light[2].position[1] = 50.0f; // spot light position in EC
	light[2].position[2] = 0.0f; light[2].position[3] = 1.0f;
		  
	light[2].ambient_color[0] = 0.152f; light[2].ambient_color[1] = 0.152f;
	light[2].ambient_color[2] = 0.152f; light[2].ambient_color[3] = 1.0f;
		  								  
	light[2].diffuse_color[0] = 0.0f; light[2].diffuse_color[1] = 0.0f;
	light[2].diffuse_color[2] = 1.0f; light[2].diffuse_color[3] = 1.0f;
		 
	light[2].specular_color[0] = 0.772f; light[2].specular_color[1] = 0.772f;
	light[2].specular_color[2] = 0.772f; light[2].specular_color[3] = 1.0f;
		  
	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = -1.0f; // spot light direction in EC
	light[2].spot_direction[2] = -5.0f;
	light[2].spot_cutoff_angle = 10.0f;
	light[2].spot_exponent = 8.0f;

	//light 3
	// spot_light_MC: use light 3
	light[3].light_on = 0;
	light[3].position[0] = 0.0f; light[3].position[1] = 0.0f; // spot light position in WC
	light[3].position[2] = 0.0f; light[3].position[3] = 1.0f;
		  
	light[3].ambient_color[0] = 1.0f; light[3].ambient_color[1] = 0.0f;
	light[3].ambient_color[2] = 0.3f; light[3].ambient_color[3] = 1.0f;
		  
	light[3].diffuse_color[0] = 1.0f; light[3].diffuse_color[1] = 0.0f;
	light[3].diffuse_color[2] = 0.3f; light[3].diffuse_color[3] = 1.0f;
		  
	light[3].specular_color[0] = 1.0f; light[3].specular_color[1] = 0.0f;
	light[3].specular_color[2] = 0.3f; light[3].specular_color[3] = 1.0f;
		  
	light[3].spot_direction[0] = 0.0f; light[3].spot_direction[1] = 0.0f; // spot light direction in WC
	light[3].spot_direction[2] = -1.0f;
	light[3].spot_cutoff_angle = 20.0f;
	light[3].spot_exponent = 8.0f;

	glUseProgram(h_ShaderProgram_TXPS);

	//light 0
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],
		light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_light[0].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	//light 1
	glUniform1i(loc_light[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
												light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]); 
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1], 
																light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]); 
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);


	//light 2
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	// need to supply position in EC for shading
	/*glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);*/
	glUniform4fv(loc_light[2].position, 1, light[2].position);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	/*glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);*/
	glUniform3fv(loc_light[2].spot_direction, 1, light[2].spot_direction);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);

	//light 3
	glUniform1i(loc_light[3].light_on, light[3].light_on);
	// need to supply position in EC for shading
	position_EC = Iron_ModelViewMatrix * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[3].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC = glm::mat3(Iron_ModelViewMatrix) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1],
		light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light[3].spot_exponent, light[3].spot_exponent);

	//glUseProgram(0);



	//gouraud
	glUseProgram(h_ShaderProgram_GS);

	//light 0
	glUniform1i(loc_light_gs[0].light_on, light[0].light_on);
	position_EC = ViewMatrix * glm::vec4(light[0].position[0], light[0].position[1],
		light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_light_gs[0].position, 1, &position_EC[0]);
	glUniform4fv(loc_light_gs[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light_gs[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light_gs[0].specular_color, 1, light[0].specular_color);

	//light 1
	glUniform1i(loc_light_gs[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light_gs[1].position, 1, &position_EC[0]);
	glUniform4fv(loc_light_gs[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light_gs[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light_gs[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light_gs[1].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light_gs[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light_gs[1].spot_exponent, light[1].spot_exponent);


	//light 2
	glUniform1i(loc_light_gs[2].light_on, light[2].light_on);
	// need to supply position in EC for shading
	/*glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);*/
	glUniform4fv(loc_light_gs[2].position, 1, light[2].position);
	glUniform4fv(loc_light_gs[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light_gs[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light_gs[2].specular_color, 1, light[2].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	/*glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);*/
	glUniform3fv(loc_light_gs[2].spot_direction, 1, light[2].spot_direction);
	glUniform1f(loc_light_gs[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light_gs[2].spot_exponent, light[2].spot_exponent);

	//light 3
	glUniform1i(loc_light_gs[3].light_on, light[3].light_on);
	// need to supply position in EC for shading
	position_EC = Iron_ModelViewMatrix * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light_gs[3].position, 1, &position_EC[0]);
	glUniform4fv(loc_light_gs[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light_gs[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light_gs[3].specular_color, 1, light[3].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	direction_EC = glm::mat3(Iron_ModelViewMatrix) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1],
		light[3].spot_direction[2]);
	glUniform3fv(loc_light_gs[3].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light_gs[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light_gs[3].spot_exponent, light[3].spot_exponent);

	glUseProgram(0);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_floor();
	prepare_screen();
	//
	prepare_tiger();
	prepare_ben();
	prepare_wolf();
	prepare_spider();
	prepare_dragon();
	prepare_optimus();
	prepare_cow();
	prepare_bus();
	prepare_bike();
	prepare_godzilla();
	prepare_ironman();
	prepare_tank();
	set_up_scene_lights();
	initialize_screen();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	
	initialize_OpenGL();
	initialize_camera();
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

void greetings(char *program_name, char messages[][256], int n_message_lines) {
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
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '0', '1', 'a', 't', 'f', 'c', 'd', 'y', 'u', 'i', 'o', 'ESC'"  };

	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}