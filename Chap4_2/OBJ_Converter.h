#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

//#include <GL/glut.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

class CPoint2f { // 2���� �Ǽ� ���� ����
public:
	vector <float> d{ 0, 0 };
};

class CPoint3f { // 3���� �Ǽ� ���� ����
public:
	vector <float> d{ 0, 0, 0 };
};

class CPoint3i { // 3���� ���� ���� ����
public:
	vector <int> d{ 0, 0, 0 };
};

class CFace { // ��� ���� ���� ����
public:
	vector <CPoint3i> v_pairs;
};

class CObj { // obj ������ ���� ����
public:
	string name;
	vector <CPoint3f> v;
	vector <CPoint2f> vt;
	vector <CPoint3f> vn;
	vector <CFace> f;
};

class CModel { // obj �� Ŭ����
public:
	vector <CObj> objs;

	// �Ǽ� ��ū �и� 
	vector<float> my_strtok_f(char* str, char* delimeter) {
		vector <float> v;
		char* context = NULL;
		char* tok = strtok_s(str, delimeter, &context);

		while (tok != NULL) {
			v.push_back(atof(tok));
			tok = strtok_s(context, delimeter, &context);
		}
		return v;
	}

	// ���ڿ� ��ū �и�
	vector<string> my_strtok_s(char* str, char* delimeter) {
		vector <string> v;
		char* context = NULL;
		char* tok = strtok_s(str, delimeter, &context);

		while (tok != NULL) {
			v.push_back(tok);
			tok = strtok_s(context, delimeter, &context);
		}
		return v;
	}

	// ���� ��ū �и�
	vector<int> my_strtok_i(char* str, char* delimeter) {
		vector <int> v;
		char* context = NULL;
		char* tok = strtok_s(str, delimeter, &context);

		while (tok != NULL) {
			v.push_back(atoi(tok));
			tok = strtok_s(context, delimeter, &context);
		}
		return v;
	}

	void loadObj(ifstream& fin) {
		string line;
		CObj obj_tmp;
		int cnt = 0;

		//���� obj������ ���� ���ͼ�
		int cnt_prev_vertex = 0;
		int cnt_prev_texture = 0;
		int cnt_prev_normal = 0;

		while (getline(fin, line)) {
			int len = line.length();
			vector<float> vf;
			vector<string> s;
			vector<int> vi;
			CPoint3f p3;
			CPoint2f p2;
			CPoint3i p3i;
			if (line[0] == 'o' && line[1] == ' ') {
				obj_tmp.name = line.substr(2, len - 2);
				objs.push_back(obj_tmp);
				if (cnt > 0) {
					cnt_prev_vertex += objs[cnt - 1].v.size();
					cnt_prev_texture += objs[cnt - 1].vt.size();
					cnt_prev_normal += objs[cnt - 1].vn.size();
				}
				cnt += 1;
			}

			if (line[0] == 'v' && line[1] == ' ') {
				vf = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1], vf[2] };
				objs[cnt - 1].v.push_back(p3);
			}
			else if (line[0] == 'v' && line[1] == 't') {
				vf = my_strtok_f((char*)line.substr(3, len - 3).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1] };
				objs[cnt - 1].vt.push_back(p2);
			}
			else if (line[0] == 'v' && line[1] == 'n') {
				vf = my_strtok_f((char*)line.substr(3, len - 3).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1], vf[2] };
				objs[cnt - 1].vn.push_back(p3);
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
				int nVertexes = s.size();
				CFace face_tmp;
				for (unsigned int i = 0; i < nVertexes; i++) {
					vi = my_strtok_i((char*)s[i].c_str(), (char*)"/");
					p3i.d = { vi[0] - cnt_prev_vertex, -1, vi[1] - cnt_prev_normal }; // vt texture ������
					//p3i.d = { vi[0] - cnt_prev_vertex, vi[1] - cnt_prev_texture, vi[2] - cnt_prev_normal}; // vt texture ����
					face_tmp.v_pairs.push_back(p3i);
				}
				objs[cnt - 1].f.push_back(face_tmp);
			}
		}
	}
};

// �ﰢ�� ������ ������ obj �׸�
void DrawObject(CModel& m) {
	GLfloat x, y, z, nx, ny, nz;
	int v_id, vt_id, vn_id;
	for (int o = 0; o < m.objs.size(); o++) {
		int nFaces = m.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {

			int nPoints = m.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			// �ﰢ�� ������ �׸�
			for (int i = 0; i < nPoints; i++) {
				v_id = m.objs[o].f[k].v_pairs[i].d[0];
				vt_id = m.objs[o].f[k].v_pairs[i].d[1];
				vn_id = m.objs[o].f[k].v_pairs[i].d[2];
				
				x = m.objs[o].v[v_id - 1].d[0];
				y = m.objs[o].v[v_id - 1].d[1];
				z = m.objs[o].v[v_id - 1].d[2];

				nx = m.objs[o].vn[vn_id - 1].d[0];
				ny = m.objs[o].vn[vn_id - 1].d[1];
				nz = m.objs[o].vn[vn_id - 1].d[2];
				
				glNormal3f(nx, ny, nz);
				glVertex3f(x, y, z);
			}
			glEnd();
		}
	}
}