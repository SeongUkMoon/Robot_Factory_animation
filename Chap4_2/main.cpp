#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <windows.h>
//#include <GL/glut.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "OBJ_Converter.h" // obj 로더 헤더
#include "glaux.h" 

// 텍스쳐 로드 라이브러리
#define _CRT_SECURE_NO_DEPRECATE
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#include <mmsystem.h> // 사운드 재생 라이브러리
#pragma comment(lib, "winmm")

// 사운드 파일 저장 경로
#define SOUND_BEEP "./sound/beep.wav"
#define SOUND_ARM1 "./sound/arm1.wav"
#define SOUND_ARM2 "./sound/arm2.wav"
#define SOUND_DROP "./sound/drop.wav"
#define SOUND_SFX "./sound/sfx.wav"

// 배경 텍스쳐 파일 저장 경로
#define TEXTURE_BACKGROUND "./texture/background.bmp"

//텍스쳐 저장 공간을 가리키는 포인터
unsigned int BackgroundTextureObject[1];
AUX_RGBImageRec* pTextureImage[1];  

// 컨베이어 벨트 모델
CModel belt;
CModel gear;

// 로봇 스핀 팔 모델
CModel arm_body;
CModel arm_stick;
CModel arm_grab_middle;
CModel arm_grab_middle_bolt;
CModel arm_grab_left;
CModel arm_grab_right;

// 로봇 그랩 팔 모델
CModel arm2_body1;
CModel arm2_body2;
CModel arm2_body3;
CModel arm2_body1_bolt;
CModel arm2_body3_bolt;
CModel arm2_stick1;
CModel arm2_stick2;
CModel arm2_grab_middle;
CModel arm2_grab_left1;
CModel arm2_grab_left2;
CModel arm2_grab_right1;
CModel arm2_grab_right2;

// 로봇 머리, 몸통
CModel big_bot_body;
CModel big_bot_head;
CModel big_bot_interval_body; // 중간 임시 몸통
CModel big_bot_fullbody;

// 로봇 데코
CModel heart;
CModel mouth;
CModel closed_eye;
CModel opened_eye1;
CModel opened_eye2;
CModel opened_eye3;

int SceneNumber = 1;		// 장면 번호 (0 ~ 10)
int ToggleMaking = 0;		// 장면 7: 조립시 부분 동작 번호

// 컨베이어 벨트 및 딜레이 타임 변수
float start_delay = 0.0;		// 시작 전 딜레이
float delay = 0.0;				// 중간 딜레이
float gear_angle = 0.0;
float moving_first = 7.0;
float moving_second = 0;

// 로봇 회전팔 관련 타임 변수
float arm_ready = 0;
float arm_down = 0.;
float arm_up = 0;
float head_angle = 0;
float roll_down = 0;

// 로봇 그랩팔 관련 타임 변수
float arm2_body1_angle = 30;
float arm2_body2_angle = 60;
float arm2_grab_up = -0.5;
float arm2_grab_angle = 45.0;
float robot_down = 1.5;

// 기본 시점 (Perspective Projection)
GLfloat camx = 5, camy = 5, camz = 30;				// 시점
GLfloat cam2x = 5, cam2y = 5, cam2z = 0;				// 관찰자
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0; // 머리 벡터

// 이동 시점 토글
bool view_moving = false;
// 음향 시점 토글
bool playsound_toggle = true;

// 장면 초기화 함수
void InitScene() {
	SceneNumber = 0;
	ToggleMaking = 0;

	start_delay = 0.;
	delay = 0.0;
	moving_first = 7.0;
	moving_second = 0;
	arm_ready = 0;
	arm_down = 0.;
	arm_up = 0;
	head_angle = 0;
	roll_down = 0;

	arm2_body1_angle = 30;
	arm2_body2_angle = 60;
	arm2_grab_up = -0.5;
	arm2_grab_angle = 40.;
	robot_down = 1.5;
}

// 컨베이어 벨트 작동 함수
void BeltRotating() {
	glPushMatrix();
	glTranslatef(-10, -1, 0);
	glColor3f(0.1, 0.1, 0.1);
	DrawObject(belt);
	glPushMatrix();
	glTranslatef(20.0,-1,0);
	glRotatef(gear_angle, 0, 0, 1);
	glTranslatef(-8.72, 1, 0);
	glColor3f(0.6, 0.6, 0.6);
	DrawObject(gear);
	glPopMatrix();
	glPopMatrix();
}// 

// 로봇 집게 운반 함수
void RobotDown() {
	glPushMatrix();
	glTranslatef(30, -3, 0);
	glRotatef(arm2_body1_angle, 0, 0, 1);
	glTranslatef(-30, 3, 0);
	glTranslatef(16,12,0);
	glRotatef(arm2_body2_angle,0,0,1);
	glTranslatef(-16, -12, 0);
	glTranslatef(7.0, -1.0 + robot_down, 0.);
	glPushMatrix();
	glColor3f(0.3, 0.3, 0.3);
	DrawObject(big_bot_head);
	glColor3f(0.7, 0.7, 0.7);
	DrawObject(closed_eye);
	glColor3f(0.1, 0.1, 0.1);
	DrawObject(mouth);
	glPopMatrix();
	glColor3f(0.3, 0.3, 0.3);
	DrawObject(big_bot_body);
	glColor3f(0.2, 0.2, 0.2);
	DrawObject(heart);
	glPopMatrix();
}

// 로봇 벨트 이동 함수
void RobotMoving() {
	glPushMatrix();
			glTranslatef(moving_first, -1.0, 0.);
			glPushMatrix();
				glColor3f(0.3, 0.3, 0.3);
				DrawObject(big_bot_head);
				glColor3f(0.7, 0.7, 0.7);
				DrawObject(closed_eye);
				glColor3f(0.1, 0.1, 0.1);
				DrawObject(mouth);
			glPopMatrix();
			glColor3f(0.3, 0.3, 0.3);
			DrawObject(big_bot_body);
			glColor3f(0.2, 0.2, 0.2);
			DrawObject(heart);
		glPopMatrix();
}

// 로봇 회전 함수
void RobotRotating() {
	glPushMatrix();
			glTranslatef(0., -1.0, 0.);
			glPushMatrix();
				glTranslatef(0., roll_down, 0.);
				glRotatef(head_angle, 0.0f, 1.0f, 0.0f);
				glColor3f(0.3, 0.3, 0.3);
				DrawObject(big_bot_head);
				glColor3f(0.7, 0.7, 0.7);
				DrawObject(closed_eye);
				glColor3f(0.1, 0.1, 0.1);
				DrawObject(mouth);
			glPopMatrix();
			glColor3f(0.3, 0.3, 0.3);
			DrawObject(big_bot_body);
			glColor3f(0.2, 0.2, 0.2);
			DrawObject(heart);
		glPopMatrix();
}

// 로봇 중간 과정 함수
void RobotInterval() {
	glPushMatrix();
			glTranslatef(0., -1.0, 0.);
			glPushMatrix();
				glColor3f(0.3, 0.3, 0.3);
				glColor3f(0.7, 0.7, 0.7);
				DrawObject(opened_eye1);
				glColor3f(1.0, 1.0, 1.0);
				DrawObject(opened_eye2);
				glTranslatef(0., roll_down - 0.1, 0.);
				glColor3f(0.1, 0.1, 0.1);
				DrawObject(mouth);
			glPopMatrix();
			glColor3f(0.3, 0.3, 0.3);
			DrawObject(big_bot_interval_body);
			glColor3f(0.8, 0.3, 0.5);
			DrawObject(heart);
		glPopMatrix();
}

// 로봇 완성 함수
void RobotComplete() {
	glPushMatrix();
			glTranslatef(moving_second, -1.0, 0.);
			glPushMatrix();
				glColor3f(0.4, 0.4, 0.4);
				DrawObject(opened_eye1);
				glColor3f(1.0, 1.0, 1.0);
				DrawObject(opened_eye2);
				glColor3f(0.1, 0.1, 0.1);
				DrawObject(opened_eye3);
				glTranslatef(0., roll_down - 0.1, 0.);
				glColor3f(0.1, 0.1, 0.1);
				DrawObject(mouth);
			glPopMatrix();
			glColor3f(0.3, 0.3, 0.3);
			DrawObject(big_bot_fullbody);
			glColor3f(0.8, 0.3, 0.5);
			DrawObject(heart);
		glPopMatrix();
}

// 로봇 회전 팔 함수
void RobotArm() {
	glPushMatrix();
		glColor3f(0.8, 0.8, 0.8);
		DrawObject(arm_body);
		glPushMatrix();
		glTranslatef(0, arm_ready + arm_down + arm_up, 0);
		glColor3f(0.1, 0.1, 0.1);
		DrawObject(arm_stick);
		glColor3f(0.9, 0.9, 0.9);
		DrawObject(arm_grab_middle);
		glColor3f(0.5, 0.5, 0.5);
		DrawObject(arm_grab_middle_bolt);
		glPushMatrix();
			glColor3f(0.35, 0.35, 0.35);
			DrawObject(arm_grab_left);
			DrawObject(arm_grab_right);
		glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

// 로봇 회전 팔 회전 함수
void RobotArmRotating() {
	glPushMatrix();
		glColor3f(0.8, 0.8, 0.8);
		DrawObject(arm_body);
		glPushMatrix();
		glTranslatef(0., roll_down - 3.0, 0.);
		glRotatef(head_angle, 0.0f, 1.0f, 0.0f);
		glColor3f(0.1, 0.1, 0.1);
		DrawObject(arm_stick);
		glColor3f(0.9, 0.9, 0.9);
		DrawObject(arm_grab_middle);
		glColor3f(0.5, 0.5, 0.5);
		DrawObject(arm_grab_middle_bolt);
		glPushMatrix();
			glColor3f(0.35, 0.35, 0.35);
			glPushMatrix();
			glTranslatef(0.22, 7.2, 0.);
			glRotatef(24.0, 0., 0., 1.);
			glTranslatef(0.22, -7., 0.);
			DrawObject(arm_grab_left);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(-0.22, 7.2, 0.);
			glRotatef(-24.0, 0., 0., 1.);
			glTranslatef(-0.22, -7., 0.);
			DrawObject(arm_grab_right);
			glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

// 로봇 집게 팔 함수
void RobotArm2() {
	glPushMatrix();
		glTranslatef(30, -3.0, 0);
		glRotatef(arm2_body1_angle, 0, 0, 1);
		glTranslatef(-30, +3.0, 0);
		glColor3f(0.8,0.8,0.8);
		DrawObject(arm2_body1);
		glColor3f(0.5, 0.5, 0.5);
		DrawObject(arm2_body1_bolt);
		glPushMatrix();
		glTranslatef(16.0,12.0,0);
		glRotatef(arm2_body2_angle,0,0,1);
		glTranslatef(-16.0,-12.0,0);
			glColor3f(0.8, 0.8, 0.8);
			DrawObject(arm2_body2);
			glColor3f(0.4, 0.4, 0.4);
			DrawObject(arm2_stick1);
			glPushMatrix();
				glColor3f(0.8,0.8,0.8);
				DrawObject(arm2_body3);
				glColor3f(0.5, 0.5, 0.5);
				DrawObject(arm2_body3_bolt);
				glPushMatrix();
					glTranslatef(0,arm2_grab_up,0);
					glColor3f(0.4, 0.4, 0.4);
					DrawObject(arm2_stick2);
					glColor3f(0.8, 0.8, 0.8);
					DrawObject(arm2_grab_middle);
					glPushMatrix();
						glTranslatef(6.1,7.0,0);
						glRotatef(arm2_grab_angle,0,0,1);
						glTranslatef(-6.1,-7.0,0);
						glColor3f(0.3, 0.3, 0.3);
						DrawObject(arm2_grab_left1);
						glColor3f(0.8, 0.8, 0.8);
						DrawObject(arm2_grab_left2);
					glPopMatrix();
					glPushMatrix();
						glTranslatef(7.7,7.0, 0.0);
						glRotatef(-arm2_grab_angle, 0, 0, 1);
						glTranslatef(-7.7,-7.0, 0.0);
						glColor3f(0.3, 0.3, 0.3);
						DrawObject(arm2_grab_right1);
						glColor3f(0.8, 0.8, 0.8);
						DrawObject(arm2_grab_right2);
					glPopMatrix();
				glPopMatrix();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}

// 뒷 배경 함수
void BackGround() {
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, BackgroundTextureObject[0]);
	glBegin(GL_POLYGON);
	glNormal3f(0,0,1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-20, -20, -10);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(20, -20, -10);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(20, 20, -10);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-20, 20, -10);
	glEnd();
	glPopMatrix();
}

// 디스플레이 콜백 함수 + 효과음
void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camx, camy, camz, cam2x, cam2y, cam2z, cam_upx, cam_upy, cam_upz);
	
	glPushMatrix();
		BackGround();
		BeltRotating();
		RobotArm2();
		switch (SceneNumber) {
		case 0:// 초기 단계, 아무런 동작 없음. 딜레이 타임
			RobotArm();
			RobotDown();
			if (start_delay > 5.6) SceneNumber++;
			break;

		case 1: // 로봇 팔2 동작 1 수행
			if (playsound_toggle) { PlaySound(TEXT(SOUND_ARM2), NULL, SND_ASYNC); playsound_toggle = false; }
			RobotArm();
			RobotDown();
			if (arm2_body1_angle == 0) { SceneNumber++; playsound_toggle = true; }
			break;

		case 2: // 로봇 팔2 동작 2 수행
			RobotArm();
			RobotDown();
			if (arm2_body2_angle == 0) SceneNumber++;
			break;

		case 3:  // 로봇 팔2가 미완성 로봇을 든 상태로 대기
			RobotArm();
			RobotDown();
			if (delay >= 6.0) SceneNumber++;
			break;

		case 4:  // 로봇 떨어짐, 로봇 팔2 올라감
			if (playsound_toggle) { PlaySound(TEXT(SOUND_DROP), NULL, SND_ASYNC); playsound_toggle = false; }
			RobotArm();
			RobotDown();
			if (robot_down <= 0) { SceneNumber++; playsound_toggle = true; }
			break;

		case 5:  // 미완성 로봇 옆으로 이동, 로봇 팔 준비 자세
			RobotArm();
			RobotMoving();
			if (moving_first <= 0) SceneNumber++;
			break;

		case 6:  // 로봇 팔 내려와 잡음
			RobotArm();
			RobotMoving();
			if (arm_down <= -4.0) SceneNumber++;
			break;

		case 7:
			if (playsound_toggle) { PlaySound(TEXT(SOUND_ARM1), NULL, SND_ASYNC); playsound_toggle = false; }
			if (ToggleMaking == 0) { // 조립 중
				RobotArmRotating();
				RobotRotating();
			}
			else if (ToggleMaking == 1) { // 완성 직전
				RobotArm();
				RobotInterval();
			}
			else if (ToggleMaking == 2) { // 완성 후
				RobotArm();
				RobotComplete();
				SceneNumber++; 
				playsound_toggle = true;
			}
			break;

		case 8:
			RobotArm();
			RobotComplete();
			if (arm_up >= 2.8) SceneNumber++;
			if (arm_up >= 2.8) { arm_up = 0; arm_down = 0; arm_ready = 0; }
			break;

		case 9:
			RobotArm();
			RobotComplete();
			if (moving_second < -7.0) SceneNumber++;
			break;

		case 10:
			RobotArm();
			RobotComplete();
			InitScene();
			break;
		}
	glPopMatrix();

	//glFlush();
	glutSwapBuffers();
}

// reshape 콜백 함수, 뷰 변환 과정 포함
void Reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = (float)width / height;
	gluPerspective(40, ratio, 0.1, 1000);
}

// keyboard 콜백 함수 ( 시점 변환 )
void Keyboard(unsigned char KeyPressed, int x, int y) {
	switch (KeyPressed) {
	case '1': // 기본 시점 (Perspective Projection)
		PlaySound(TEXT(SOUND_BEEP), NULL, SND_ASYNC);
		view_moving = false;
		camx = 5; camy = 5; camz = 30;
		cam2x = 5; cam2y = 5; cam2z = 0;				
		cam_upx = 0; cam_upy = 1; cam_upz = 0;
		break;
	
	case'2': // 확대 시점 (Perspective Projection)
		PlaySound(TEXT(SOUND_BEEP), NULL, SND_ASYNC);
		camx = 0; camy = 1; camz = 8;
		cam2x = 0; cam2y = 1; cam2z = 0;  
		cam_upx = 0; cam_upy = 1; cam_upz = 0;
		break;
	
	case'3': // 측면 시점 (Perspective Projection)
		PlaySound(TEXT(SOUND_BEEP), NULL, SND_ASYNC);
		camx = -15; camy = 12; camz = 8;
		cam2x = 7; cam2y = 2; cam2z = 0;
		cam_upx = 0; cam_upy = 1; cam_upz = 0;
		break;

	case'4': // 이동 시점 (Perspective Projection)
		PlaySound(TEXT(SOUND_BEEP), NULL, SND_ASYNC);
		camx = 10; camy = 3; camz = 12;
		cam2x = 10; cam2y = 3; cam2z = 0;
		cam_upx = 0; cam_upy = 1; cam_upz = 0;
		view_moving = true;
		break;
	}
}

// 타이머 콜백 함수
void TimerFunction(int value) {
	switch (SceneNumber) {
	
	case 0: // 초기 단계, 아무런 동작 없음. 딜레이 타임
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		if (start_delay < 5.6) start_delay += 0.1;
		break;

	case 1: // 로봇 팔2 동작 1 수행
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		if (arm2_body1_angle > 0) arm2_body1_angle -= 5.;
		else arm2_body1_angle = 0;
		break;

	case 2: // 로봇 팔2 동작 2 수행
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		if (arm2_body2_angle > 0) arm2_body2_angle -= 8.;
		else arm2_body2_angle = 0;
		break;

	case 3: // 로봇 팔2가 미완성 로봇을 든 상태로 대기
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		if (delay < 6.0) delay += 0.1;
		break;

	case 4: // 로봇 떨어짐, 로봇 팔2 올라감
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		if (arm2_grab_angle > 0) arm2_grab_angle -= 10.;
		if (robot_down > 0) robot_down -= 0.6;
		if (arm2_grab_up < 0) arm2_grab_up += 0.15;
		break;

	case 5: // 미완성 로봇 옆으로 이동, 로봇 팔 준비 자세
		if (arm_ready < 1.0) arm_ready += 0.2;
		if (moving_first > 0) moving_first -= 0.25;
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		break;

	case 6: // 로봇 팔 내려와 잡음
		if (arm_down > -4.0) arm_down -= 1.0;
		break;

	case 7: // 로봇 팔, 로봇 머리 회전
		if (roll_down > -1.35) {
			roll_down -= 0.02; // 0.018
			head_angle += 22.6;
			if (head_angle > 360) {
				head_angle = 0;
			}
		}
		else {
			if (ToggleMaking == 0) ToggleMaking = 1; // 조립 전 >>  조립 중
			else if (ToggleMaking == 1) ToggleMaking = 2; // 조립 중 >> 조립 후
		}
		break;

	case 8: // 완성 후 로봇 팔 올라감
		if (arm_up < 2.8) arm_up += 0.7;
		break;
		
	case 9: // 조립 후 이동
		if (arm2_body2_angle < 60) arm2_body2_angle += 5.;
		else {
			arm2_body2_angle = 60;
			if (arm2_body1_angle < 30) arm2_body1_angle += 4.;
			else arm2_body1_angle = 30;
		}
		gear_angle += 5;
		if (gear_angle > 360) gear_angle = 0;
		if (moving_second > -7.0) moving_second -= 0.18;
		break;
	}

	if (view_moving) {
		cam2x -= 0.07;
		camx -= 0.07;
		if (camx <= 0) view_moving = false;
	}
	
	glutPostRedisplay();
	glutTimerFunc(1, TimerFunction, 1);
}

// 로봇 obj 로드 함수
void LoadRobotObjects() {
	string filepath = "./resource/big_bot_body.obj";
	ifstream fin(filepath);
	big_bot_body.loadObj(fin);
	fin.close();

	filepath = "./resource/heart.obj";
	ifstream fin2(filepath);
	heart.loadObj(fin2);
	fin2.close();

	filepath = "./resource/big_bot_head.obj";
	ifstream fin3(filepath);
	big_bot_head.loadObj(fin3);
	fin3.close();

	filepath = "./resource/closed_eye.obj";
	ifstream fin4(filepath);
	closed_eye.loadObj(fin4);
	fin4.close();

	filepath = "./resource/big_bot_fullbody.obj";
	ifstream fin5(filepath);
	big_bot_fullbody.loadObj(fin5);
	fin5.close();

	filepath = "./resource/opened_eye1.obj";
	ifstream fin6(filepath);
	opened_eye1.loadObj(fin6);
	fin6.close();

	filepath = "./resource/opened_eye2.obj";
	ifstream fin7(filepath);
	opened_eye2.loadObj(fin7);
	fin7.close();

	filepath = "./resource/opened_eye3.obj";
	ifstream fin8(filepath);
	opened_eye3.loadObj(fin8);
	fin8.close();

	filepath = "./resource/mouth.obj";
	ifstream fin9(filepath);
	mouth.loadObj(fin9);
	fin9.close();

	filepath = "./resource/big_bot_interval_body.obj";
	ifstream fin10(filepath);
	big_bot_interval_body.loadObj(fin10);
	fin10.close();
}

// 로봇 회전 팔 obj 로드 함수
void LoadSpinArmObjects() {
	string filepath = "./resource/arm_body.obj";
	ifstream fin(filepath);
	arm_body.loadObj(fin);
	fin.close();

	filepath = "./resource/arm_stick.obj";
	ifstream fin2(filepath);
	arm_stick.loadObj(fin2);
	fin2.close();

	filepath = "./resource/arm_grab_middle.obj";
	ifstream fin3(filepath);
	arm_grab_middle.loadObj(fin3);
	fin3.close();

	filepath = "C:./resource/arm_grab_left.obj";
	ifstream fin4(filepath);
	arm_grab_left.loadObj(fin4);
	fin4.close();

	filepath = "C:./resource/arm_grab_right.obj";
	ifstream fin5(filepath);
	arm_grab_right.loadObj(fin5);
	fin5.close();

	filepath = "C:./resource/arm_grab_middle_bolt.obj";
	ifstream fin6(filepath);
	arm_grab_middle_bolt.loadObj(fin6);
	fin6.close();
}

// 로봇 그랩 팔 obj 로드 함수
void LoadGrabArmObjects() {
	string filepath = "./resource/arm2_body1.obj";
	ifstream fin(filepath);
	arm2_body1.loadObj(fin);
	fin.close();

	filepath = "./resource/arm2_body1_bolt.obj";
	ifstream fin2(filepath);
	arm2_body1_bolt.loadObj(fin2);
	fin2.close();

	filepath = "./resource/arm2_body2.obj";
	ifstream fin3(filepath);
	arm2_body2.loadObj(fin3);
	fin3.close();

	filepath = "C:./resource/arm2_body3.obj";
	ifstream fin4(filepath);
	arm2_body3.loadObj(fin4);
	fin4.close();

	filepath = "C:./resource/arm2_body3_bolt.obj";
	ifstream fin5(filepath);
	arm2_body3_bolt.loadObj(fin5);
	fin5.close();

	filepath = "C:./resource/arm2_grab_middle.obj";
	ifstream fin6(filepath);
	arm2_grab_middle.loadObj(fin6);
	fin6.close();

	filepath = "C:./resource/arm2_grab_left1.obj";
	ifstream fin7(filepath);
	arm2_grab_left1.loadObj(fin7);
	fin7.close();

	filepath = "C:./resource/arm2_grab_left2.obj";
	ifstream fin8(filepath);
	arm2_grab_left2.loadObj(fin8);
	fin8.close();

	filepath = "C:./resource/arm2_grab_right1.obj";
	ifstream fin9(filepath);
	arm2_grab_right1.loadObj(fin9);
	fin9.close();

	filepath = "C:./resource/arm2_grab_right2.obj";
	ifstream fin10(filepath);
	arm2_grab_right2.loadObj(fin10);
	fin10.close();

	filepath = "C:./resource/arm2_stick1.obj";
	ifstream fin11(filepath);
	arm2_stick1.loadObj(fin11);
	fin11.close();

	filepath = "C:./resource/arm2_stick2.obj";
	ifstream fin12(filepath);
	arm2_stick2.loadObj(fin12);
	fin12.close();
}

// 컨베이어 벨트 obj 로드 함수
void LoadBelt() {
	string filepath = "./resource/belt.obj";
	ifstream fin(filepath);
	belt.loadObj(fin);
	fin.close();

	filepath = "./resource/gear.obj";
	ifstream fin2(filepath);
	gear.loadObj(fin2);
	fin2.close();
}

// 광원, 음영 초기화 함수
void InitLight() {
	GLfloat globalAmbientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat specref[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat shininess[] = { 25.0 };
	GLfloat LightPos[] = { 0.0, 30.0, 80, 1.0 };

	//GLfloat	 LightPos[] = { 5.0f, 5.0f, -50.0f, 1.0f };
	//GLfloat	 LightPos[] = { 0.0f, 0.0f, 100.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 512);
}

AUX_RGBImageRec* LoadBMP(const char* szFilename) {
	FILE* pFile = NULL;
	if (!szFilename) {
		return NULL;
	}
	fopen_s(&pFile, szFilename, "r");
	if (pFile) {
		fclose(pFile);
		return auxDIBImageLoad(szFilename);     //파일로부터 메모리로
	}
	return NULL;
}

int LoadGLTextures(const char* szFilePath) {       //파일을 로드하고 텍스쳐로 변환
	int Status = FALSE;
	glClearColor(0.0, 0.0, 0.0, 0.5);
	memset(pTextureImage, 0, sizeof(void*) * 1);    //포인터를 널로

	if (pTextureImage[0] = LoadBMP(szFilePath)) {   //비트맵을 로드하고 오류확인
		Status = TRUE;                              //상태 플랙을 True로
		glGenTextures(1, &BackgroundTextureObject[0]);      //텍스쳐 생성
		glBindTexture(GL_TEXTURE_2D, BackgroundTextureObject[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, 3,
			pTextureImage[0]->sizeX, pTextureImage[0]->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, pTextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	if (pTextureImage[0]) {                 //텍스쳐가 존재하면
		if (pTextureImage[0]->data) {       //텍스쳐 영상이 존재하면
			free(pTextureImage[0]->data);   //텍스쳐 영상공간 반납
		}
		free(pTextureImage[0]);             //텍스쳐 반납
	}
	return Status;
}

int main(int argc, char** argv) {

	LoadRobotObjects();
	LoadSpinArmObjects();
	LoadGrabArmObjects();
	LoadBelt();
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(300, 200);
	glClearColor(0.0, 0.0, 0.0, 0.5);

	glutCreateWindow("Robot Factory");
	
	LoadGLTextures(TEXTURE_BACKGROUND);

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutTimerFunc(1000, TimerFunction, 1);
	glutKeyboardFunc(Keyboard);
	InitLight();
	
	glutMainLoop();

	return 0;
}