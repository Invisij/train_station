#include "Angel.h"

void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

const int socanh = 36;
const int chiSoDinhTren = 8, chiSoDinhDuoi = chiSoDinhTren + socanh + 2;
const int NumPoints = 36;
const int NumPoint = NumPoints + socanh * 12;

const int NumPoints1 = 38; // Hinh tru tron
const int NumPoints2 = 37;// Hinh tru tron
const int NumPoints3 = 20;// Hinh mat cau
const int NumPoints4 = 38;// Hinh mat cau
const int NumPoints5 = 42;// Hinh 1/2 tru nghieng

point4 points[NumPoint];
color4 colors[NumPoint];
vec3 normals[NumPoint];
point4 vertices[8+(socanh+2)*2];

point4 points1[NumPoints1 * 2 + NumPoints2 * 2];
color4 colors1[NumPoints1 * 2 + NumPoints2 * 2];
vec3 normals1[NumPoints1 * 2 + NumPoints2 * 2];

point4 points2[NumPoints3 + NumPoints4 * 8];
color4 colors2[NumPoints3 + NumPoints4 * 8];
vec3 normals2[NumPoints3 + NumPoints4 * 8];

point4 points3[NumPoints5];
color4 colors3[NumPoints5];
vec3 normals3[NumPoints5];

GLuint program;
GLuint model_loc, view_loc, projection_loc;


mat4 Model_gaTauDien;
mat4 Model01;

// thông số nền nhà ga
mat4 Model_nenNhaGa;
GLfloat chieuNgang_nenNhaGa = 8, chieuDai_nenNhaGa = 44, chieuCao_nenNhaGa = 2;
GLfloat doDayTuong_nhaGa1 = 0.3, chieuCaoTuong_nhaGa1 = 5.5;
mat4 Model_tuongNhaGa2;
GLfloat chieuCaoCua = 3, chieuNgangCua = 1.2, doDayCua = 0.1;
GLfloat chieuCaoTuong2 = 4, doDayTuong2 = 0.3, chieuDaiTuong2 = 9;
GLfloat quay_cua_phai = 0, quay_cua_trai = 0;

// thông số phần mái
mat4 Model_maiChe;
GLfloat tiepDien_cot = 0.3, chieuCao_cot = 14;
GLfloat chieuDai_mai = 18.9, doDay_mai = 0.12;
GLfloat khoangCachGiuaCacCot = 3;

// thông số phần đường ray
mat4 Model_duongRay;
GLfloat chieuNgang_duongRay = 5;
GLfloat chieuNgang_phanLot = 0.3, chieuCao_phanLot = 0.1;
GLfloat chieuDai_moiNoi = 0.2, chieuRong_moiNoi = 0.1, chieuCao_moiNoi = 0.1;
GLfloat chieuNgang_phanDemSat = 0.06, chieuCao_phanDemSat = 0.22;

int stt = 0;
void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[stt++] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[stt++] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[stt++] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[stt++] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[stt++] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[stt++] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[stt++] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[stt++] = point4(0.5, -0.5, -0.5, 1.0);

	vertices[stt++] = point4(0.0, 0.5, 0.0, 1.0); // đỉnh trên chỉ số 8
	for (int i = 0; i < 360; i += (360/socanh))
	{
		vertices[stt++] = point4(0.5*cos(DegreesToRadians*i), 0.5, 0.5*sin(DegreesToRadians*i), 1.0);
	}
	vertices[stt++] = point4(0.5, 0.5, 0.0, 1.0);

	vertices[stt++] = point4(0.0, -0.5, 0.0, 1.0); // đỉnh dưới chỉ số 8 + 6 + 2 = 16 
	for (int i = 0; i < 360; i += 360 / socanh)
	{
		vertices[stt++] = point4(0.5 * cos(DegreesToRadians * i), -0.5, 0.5 * sin(DegreesToRadians * i), 1.0);
	}
	vertices[stt++] = point4(0.5, -0.5, 0.0, 1.0);
}

int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; points[Index] = vertices[d]; Index++;
}
void makeColorRGBCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void circle(int chiSoTam)
{
	vec4 u = vertices[chiSoTam+1] - vertices[chiSoTam];
	vec4 v = vertices[chiSoTam+2] - vertices[chiSoTam];
	vec3 normal = normalize(cross(u, v));

	for (int i = 1; i <=  socanh; i++)
	{
		normals[Index] = normal; points[Index] = vertices[chiSoTam]; Index++;
		normals[Index] = normal; points[Index] = vertices[chiSoTam+i]; Index++;
		normals[Index] = normal; points[Index] = vertices[chiSoTam+i+1]; Index++;
	}
}
void canhHinhTru()
{
	circle(8);
	circle(chiSoDinhDuoi);
	for (int i = 1; i <= socanh; i++)
	{
		quad(chiSoDinhTren + i, chiSoDinhTren + i + 1, chiSoDinhDuoi + i + 1, chiSoDinhDuoi + i);
	}
}
void initMatday(int i, GLfloat z, GLfloat dau)
{
	int goc = 0, j;
	j = i;
	points1[i] = point4(0, 0, z, 1);
	for (i += 1; i < NumPoints1 + j; i++) {
		points1[i] = point4(cos(goc * DegreesToRadians) / 2, sin(goc * DegreesToRadians) / 2, z, 1.0);
		goc += 10;
	}
	vec4 u = (points1[j + 1] - points1[j]);
	vec4 v = (points1[j + 2] - points1[j]);
	vec3 normal = dau * normalize(cross(u, v));
	for (i = j; i < NumPoints1 + j; i++) normals1[i] = normal;
}

void initMatbenvaday()
{
	initMatday(0, -0.5, -1.0);
	initMatday(NumPoints1, 0.5, 1.0);
	int goc = 0;
	for (int i = NumPoints1 * 2; i < NumPoints1 * 2 + NumPoints2 * 2 - 1; i += 2) {
		points1[i] = point4(cos(goc * DegreesToRadians) / 2, sin(goc * DegreesToRadians) / 2, -0.5, 1.0);
		points1[i + 1] = point4(cos(goc * DegreesToRadians) / 2, sin(goc * DegreesToRadians) / 2, 0.5, 1.0);
		goc += 10;
	}

	for (int i = NumPoints1 * 2 + 1; i < NumPoints1 * 2 + NumPoints2 * 2 - 1; i += 2) {
		vec4 u = points1[i - 1] - points1[i];
		vec4 v = points1[i + 1] - points1[i];
		vec3 normal = normalize(cross(u, v));
		normals1[i] = normal;
		normals1[i - 1] = normal;
		normals1[i + 1] = normal;
	}
	vec4 u = points1[NumPoints1 * 2] - points1[NumPoints1 * 2 + 1];
	vec4 v = points1[NumPoints1 * 2 + 2] - points1[NumPoints1 * 2 + 1];
	vec3 normal = normalize(cross(u, v));
	normals1[NumPoints1 * 2 + NumPoints2 * 2 - 1] = normal;
	normals1[NumPoints1 * 2 + NumPoints2 * 2 - 2] = normal;
}

//Xay dung dinh de tao 1 phan 4 mat cau
void initNon()
{
	int goc = 0;
	points2[0] = point4(0, 0.5, 0, 1);
	for (int i = 1; i <= NumPoints3; i++) {
		points2[i] = point4(0.5 * sin(10 * DegreesToRadians) * cos(goc * DegreesToRadians), 0.5 * cos(10 * DegreesToRadians), 0.5 * sin(10 * DegreesToRadians) * sin(goc * DegreesToRadians), 1.0);
		goc += 10;
	}

	for (int j = 0; j < NumPoints3 - 2; j += 2) {
		vec4 u = points2[j + 1] - points2[0];
		vec4 v = points2[j + 2] - points2[0];
		vec3 normal = -normalize(cross(u, v));
		normals2[0] = normal;
		normals2[j + 1] = normal;
		normals2[j + 2] = normal;
	}
	vec4 u = points2[18] - points2[0];
	vec4 v = points2[19] - points2[0];
	vec3 normal = -normalize(cross(u, v));
	normals2[19] = normal;
}

void init1Phan4MatCau()
{
	initNon();
	int goc1 = 0, goc2 = 0;
	for (int i = NumPoints3; i < NumPoints3 + NumPoints4 * 8; i += 2) {
		points2[i] = point4(0.5 * cos((goc2 + 10) * DegreesToRadians) * (cos(goc1 * DegreesToRadians)), 0.5 * sin((goc2 + 10) * DegreesToRadians), 0.5 * cos((goc2 + 10) * DegreesToRadians) * sin(goc1 * DegreesToRadians), 1.0);
		points2[i + 1] = point4(0.5 * cos(goc2 * DegreesToRadians) * (cos(goc1 * DegreesToRadians)), 0.5 * sin(goc2 * DegreesToRadians), 0.5 * cos(goc2 * DegreesToRadians) * sin(goc1 * DegreesToRadians), 1.0);
		goc1 += 10;
		if (goc1 > 180) {
			goc1 = 0;
			goc2 += 10;
		}
	}
	for (int j = NumPoints3; j < NumPoints3 + NumPoints4 * 8; j += NumPoints4) {
		int i;
		for (i = j + 1; i < j + NumPoints4 - 1; i += 2) {
			vec4 u = points2[i - 1] - points2[i];
			vec4 v = points2[i + 1] - points2[i];
			vec3 normal = normalize(cross(u, v));
			normals2[i] = normal;
			normals2[i - 1] = normal;
			normals2[i + 1] = normal;
		}
		vec4 u = points2[i] - points2[i - 1];
		vec4 v = points2[i - 2] - points2[i - 1];
		vec3 normal = normalize(cross(u, v));
		normals2[i] = normal;

	}
}
//Xay dung dinh de tao hinh 1/2 tru nghieng
void initTrunGhieng() {
	int goc = 0;
	for (int i = 2; i < NumPoints5 - 2; i += 2) {
		points3[i] = point4(cos(goc * DegreesToRadians) / 2, sin(goc * DegreesToRadians) / 2, -0.5, 1.0);
		points3[i + 1] = point4(cos(goc * DegreesToRadians) / 2, 0.5 + sin(goc * DegreesToRadians) / 2, 0.5, 1.0);
		goc += 10;
	}
	points3[0] = point4(0.5, 0, -0.5, 1.0);
	points3[1] = point4(0.5, 0, 0.5, 1.0);
	vec4 u1 = points3[1] - points3[0];
	vec4 v1 = points3[3] - points3[0];
	vec3 normal1 = -normalize(cross(u1, v1));
	normals3[0] = normal1;
	normals3[1] = normal1;
	points3[NumPoints5 - 1] = point4(-0.5, 0, -0.5, 1.0);
	points3[NumPoints5 - 2] = point4(-0.5, 0, 0.5, 1.0);
	vec4 u2 = points3[NumPoints5 - 2] - points3[NumPoints5 - 1];
	vec4 v2 = points3[NumPoints5 - 4] - points3[NumPoints5 - 1];
	vec3 normal2 = -normalize(cross(u2, v2));
	normals3[NumPoints5 - 2] = normal2;
	normals3[NumPoints5 - 1] = normal2;
	for (int i = 3; i < NumPoints5 - 1; i += 2) {
		vec4 u = points3[i - 1] - points3[i];
		vec4 v = points3[i + 1] - points3[i];
		vec3 normal = normalize(cross(u, v));
		normals3[i] = normal;
		normals3[i - 1] = normal;
		normals3[i + 1] = normal;
	}
}
void generateGeometry(void)
{
	initCube();
	makeColorRGBCube();
	canhHinhTru();
	initMatbenvaday();
	init1Phan4MatCau();
	initTrunGhieng();
}

GLuint vao[4];
GLuint buffer[4];
void initGPUBuffers(void)
{
	glGenVertexArrays(4, vao);
	glGenBuffers(4, buffer);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	// hình vuông + trụ tròn
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

	// hình trụ tròn
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points1) + sizeof(colors1) + sizeof(normals1), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points1), points1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points1), sizeof(colors1), colors1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points1) + sizeof(colors1), sizeof(normals1), normals1);

	// 1/4 mặt cầu
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2) + sizeof(colors2) + sizeof(normals2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points2), points2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points2), sizeof(colors2), colors2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points2) + sizeof(colors2), sizeof(normals2), normals2);

	// 1/4 trụ nghiêng
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points3) + sizeof(colors3) + sizeof(normals3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points3), points3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points3), sizeof(colors3), colors3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points3) + sizeof(colors3), sizeof(normals3), normals3);
}

/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(10.0, 25.0, 10.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);
void makeColorRGB(GLfloat red, GLfloat green, GLfloat blue, float shininess)
{
	color4 material_ambient(red / 510, green / 510, blue / 510, 1.0);
	color4 material_diffuse(red / 255.0, green / 255.0, blue / 255.0, 1.0);
	color4 material_specular(red / 510, green / 510, blue / 510.0, 1.0);
	float material_shininess = shininess;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
void makeColor(GLfloat red, GLfloat green, GLfloat blue, float shininess)
{
	color4 material_ambient(red / 2, green / 2, blue / 2, 1.0);
	color4 material_diffuse(red , green, blue, 1.0);
	color4 material_specular(1, 1, 1, 1.0);
	float material_shininess = shininess;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}
void MaterialSt(GLfloat mar, GLfloat mag, GLfloat mab,
	GLfloat mdr, GLfloat mdg, GLfloat mdb,
	GLfloat msr, GLfloat msg, GLfloat msb,
	GLfloat ms)
{
	color4 material_ambient(mar, mag, mab, 1);
	color4 material_diffuse(mdr / 255, mdg / 255, mdb / 255, 1);
	color4 material_specular(msr, msg, msb, 1);
	float material_shininess = ms;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);
}

// màu
void Cay()
{
	MaterialSt(1, 1, 1,
		24, 177, 76,
		0.6, 0.6, 0.6,
		10000);
}
void Be_tong()
{
	MaterialSt(1, 1, 1,
		160, 160, 160,
		0.6, 0.6, 0.6,
		10000);
}
void Tuong()
{
	MaterialSt(1, 1, 1,
		255, 255, 255,
		1, 1, 1,
		1000);
}
void Mai_nhua()
{
	MaterialSt(1, 1, 1,
		17, 51, 97,
		1, 1, 1,
		10);
}
void cot_sat()
{
	MaterialSt(1, 1, 1,
		230, 230, 230,
		1, 1, 1,
		10);
}
void thep()
{
	MaterialSt(1, 1, 1,
		127, 127, 127,
		1, 1, 1,
		10);
}
void San_den()
{
	MaterialSt(1, 1, 1,
		10, 10, 10,
		0.6, 0.6, 0.6,
		10);
}
void Son_satTrang()
{
	MaterialSt(1, 1, 1,
		230, 230, 230,
		1, 1, 1,
		10);
}

void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	GLuint loc_vPosition0 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition0);
	glVertexAttribPointer(loc_vPosition0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint loc_vColor0 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor0);
	glVertexAttribPointer(loc_vColor0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	GLuint loc_vNormal0 = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal0);
	glVertexAttribPointer(loc_vNormal0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));


	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	GLuint loc_vPosition1 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition1);
	glVertexAttribPointer(loc_vPosition1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint loc_vColor1 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor1);
	glVertexAttribPointer(loc_vColor1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points1)));
	GLuint loc_vNormal1 = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal1);
	glVertexAttribPointer(loc_vNormal1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points1) + sizeof(colors1)));


	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	GLuint loc_vPosition2 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition2);
	glVertexAttribPointer(loc_vPosition2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint loc_vColor2 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor2);
	glVertexAttribPointer(loc_vColor2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points2)));
	GLuint loc_vNormal2 = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal2);
	glVertexAttribPointer(loc_vNormal2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points2) + sizeof(colors2)));


	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
	GLuint loc_vPosition3 = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition3);
	glVertexAttribPointer(loc_vPosition3, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint loc_vColor3 = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor3);
	glVertexAttribPointer(loc_vColor3, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points3)));
	GLuint loc_vNormal3 = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal3);
	glVertexAttribPointer(loc_vNormal3, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points3) + sizeof(colors3)));

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1, 1, 1, 1.0);
}

void lapPhuong()
{
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void truTron()
{
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glDrawArrays(GL_TRIANGLES, NumPoints, socanh * 12);
}

// Hưng
mat4 symbol, Model;
void Ground()
{
	makeColorRGB(255, 255, 255, 10);
	symbol = Scale(1000, 1.0, 1000);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * symbol);
	lapPhuong();

	Tuong();
	symbol = Scale(12,2,1000);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * symbol);
	lapPhuong();

	symbol = Scale(20, 3, 1000) * Translate(3.2,0,0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * symbol);
	lapPhuong();

	symbol = Scale(20, 3, 1000) * Translate(-3.2, 0, 0);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * symbol);
	lapPhuong();
}

// nền nhà ga
void tuongNhaGa1()
{
	symbol = Scale(3*chieuNgang_nenNhaGa/4, chieuCaoTuong_nhaGa1, doDayTuong_nhaGa1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * symbol);
	lapPhuong();
}
void tuong45()
{
	symbol = Scale(doDayTuong2, chieuCaoTuong2, 4.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * Model_tuongNhaGa2 * symbol);
	lapPhuong();
}
void tuong6()
{
	symbol = Scale(doDayTuong2, chieuCaoTuong2, 0.6);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * Model_tuongNhaGa2 * symbol);
	lapPhuong();
}
void tuong15()
{
	symbol = Scale(doDayTuong2, chieuCaoTuong2, 1.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * Model_tuongNhaGa2 * symbol);
	lapPhuong();
}
void tuong12()
{
	symbol = Scale(doDayTuong2, chieuCaoTuong2-chieuCaoCua, 1.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * Model_tuongNhaGa2 * symbol);
	lapPhuong();
}
void cuaPhai()
{
	symbol = Translate(doDayCua / 2, 0, -chieuNgangCua / 2) * RotateY(quay_cua_phai) * Translate(-doDayCua / 2, 0, chieuNgangCua / 2)
		* Scale(doDayCua, chieuCaoCua, chieuNgangCua);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * Model_tuongNhaGa2 * symbol);
	lapPhuong();
}
void cuaTrai()
{
	symbol = Translate(doDayCua / 2, 0, chieuNgangCua / 2) * RotateY(quay_cua_trai) * Translate(-doDayCua / 2, 0, -chieuNgangCua / 2)
		* Scale(doDayCua, chieuCaoCua, chieuNgangCua);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * Model_tuongNhaGa2 * symbol);
	lapPhuong();
}
void tuongNhaGa2()
{
	Tuong();
	Model_tuongNhaGa2 = Translate(0, 0, -chieuDaiTuong2/4);
	tuong45();
	Model_tuongNhaGa2 = Translate(0, 1.5, 0.6);
	tuong12();
	Model_tuongNhaGa2 = Translate(0, 0, 1.5);
	tuong6();
	Model_tuongNhaGa2 = Translate(0, 1.5, 2.4);
	tuong12();
	Model_tuongNhaGa2 = Translate(0, 0, 3.75);
	tuong15();

	Model_tuongNhaGa2 = Translate((doDayTuong2 / 2) - (doDayCua / 2), -(chieuCaoTuong2 / 2) + chieuCaoCua / 2, 0.6);
	cuaPhai();
	Model_tuongNhaGa2 = Translate((doDayTuong2 / 2) - (doDayCua / 2), -(chieuCaoTuong2 / 2) + chieuCaoCua / 2, 2.4);
	cuaTrai();

}
void nenNhaGa()
{
	Tuong();
	symbol = Scale(chieuNgang_nenNhaGa, chieuCao_nenNhaGa, chieuDai_nenNhaGa);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nenNhaGa * symbol);
	lapPhuong();
}
void nhaGa()
{
	Model_nenNhaGa = identity();
	nenNhaGa();

	Model_nenNhaGa = Translate(chieuNgang_nenNhaGa / 8, (chieuCao_nenNhaGa / 2) + (chieuCaoTuong_nhaGa1 / 2), -(chieuDai_nenNhaGa / 2) + (doDayTuong_nhaGa1 / 2));
	tuongNhaGa1();

	Model_nenNhaGa = Translate(chieuNgang_nenNhaGa / 8, (chieuCao_nenNhaGa / 2) + (chieuCaoTuong_nhaGa1 / 2), (chieuDai_nenNhaGa / 2) - (doDayTuong_nhaGa1 / 2));
	tuongNhaGa1();

	//Model_nenNhaGa = Translate((chieuNgang_nenNhaGa / 2) - (doDayTuong2 / 2), (chieuCao_nenNhaGa / 2) + (chieuCaoTuong2 / 2), (chieuDai_nenNhaGa / 2) - (chieuDaiTuong2 / 2) - 3);
	//tuongNhaGa2();
}

// phần mái
void cot(GLfloat x, GLfloat y, GLfloat z)
{
	cot_sat();
	symbol = Translate(x, y, z) * Scale(tiepDien_cot, chieuCao_cot, tiepDien_cot);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_maiChe * symbol);
	truTron();
}
void thanhdondoc(GLfloat x, GLfloat y, GLfloat z)
{
	cot_sat();
	symbol = Translate(x, y, z) * RotateZ(8) * Translate(-chieuDai_mai / 8, 0, 0) * Scale(chieuDai_mai, tiepDien_cot, tiepDien_cot);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_maiChe * symbol);
	lapPhuong();
}
void thanhdonngang(GLfloat x, GLfloat y, GLfloat z)
{
	cot_sat();
	symbol = Translate(x, y, z) * RotateZ(8) * Scale(tiepDien_cot, tiepDien_cot, khoangCachGiuaCacCot);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_maiChe * symbol);
	lapPhuong();
}
void mai(GLfloat x, GLfloat y, GLfloat z)
{
	Mai_nhua();
	symbol = Translate(x, y, z) * RotateZ(8) * Translate(-chieuDai_mai / 8, 0, 0) * Scale(chieuDai_mai, doDay_mai, khoangCachGiuaCacCot);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_maiChe * symbol);
	lapPhuong();
}
void maiChe()
{
	Model_maiChe = Translate(chieuNgang_nenNhaGa/3, chieuCao_nenNhaGa/2, 6 * khoangCachGiuaCacCot);

	thanhdonngang(0, chieuCao_cot, khoangCachGiuaCacCot);
	mai(0, chieuCao_cot + tiepDien_cot, khoangCachGiuaCacCot);
	for (int i = 0; i < 13; i++) {
		cot(0, chieuCao_cot/2, -i * khoangCachGiuaCacCot);
		thanhdondoc(0, chieuCao_cot, -i * khoangCachGiuaCacCot);
		thanhdonngang(0, chieuCao_cot, -i * khoangCachGiuaCacCot);
		mai(0, chieuCao_cot + tiepDien_cot, -i * khoangCachGiuaCacCot);
	}
	thanhdonngang(0, chieuCao_cot, -13 * khoangCachGiuaCacCot);
	mai(0, chieuCao_cot + tiepDien_cot, -13 * khoangCachGiuaCacCot);
}

// phần đường ray
void thanhRay()
{
	makeColorRGB(190, 192, 116, 100);
	symbol = Translate(0, 0, 0)
		* Scale(chieuNgang_phanLot, chieuCao_phanLot, chieuDai_nenNhaGa);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_duongRay * symbol);
	lapPhuong();

	makeColorRGB(80,40,10,10);

	for (int i = 0; i <= 40; i++) {
		symbol = Translate(0, chieuCao_phanLot/2 + chieuCao_moiNoi/2, -i + (chieuDai_nenNhaGa/2))
			* Scale(chieuDai_moiNoi, chieuCao_moiNoi, chieuRong_moiNoi);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_duongRay * symbol);
		lapPhuong();
	}

	symbol = Translate(0, chieuCao_phanLot/2 + chieuCao_moiNoi + chieuCao_phanDemSat/2, 0)
		* Scale(chieuNgang_phanDemSat, chieuCao_phanDemSat, chieuDai_nenNhaGa);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_duongRay * symbol);
	lapPhuong();

}
void nenThanhRay()
{
	//makeColorRGB(190, 192, 116, 100);
	Tuong();
	symbol = Scale(chieuNgang_duongRay, chieuCao_nenNhaGa-1, chieuDai_nenNhaGa);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_duongRay * symbol);
	lapPhuong();
}
void duongRay()
{
	// ray phải
	Model_duongRay = Translate(-(chieuNgang_duongRay / 2) - (chieuNgang_nenNhaGa / 2), (chieuCao_nenNhaGa / 2) - 1, 0)
		* Translate(chieuNgang_duongRay / 4, 0, 0);
	thanhRay();

	// ray trái
	Model_duongRay = Translate(-(chieuNgang_duongRay / 2) - (chieuNgang_nenNhaGa / 2), (chieuCao_nenNhaGa / 2) - 1, 0)
		* Translate(-chieuNgang_duongRay / 4, 0, 0);
	thanhRay();

	Model_duongRay = Translate(-(chieuNgang_duongRay / 2) - (chieuNgang_nenNhaGa / 2), -0.5, 0);
	nenThanhRay();
}

// phần cầu thang nối đường lên ga với ga
mat4 Model_cauThang;
GLfloat chieuNgang_CauThang = 4, chieuCao_CauThang = 8;
GLfloat chieuDai_HanhLang = 7.5, chieuDaiDay_HanhLang = 32;
GLfloat chieuCaoCauThang_hanhLang = 0.5, chieuDaiCauThang_hanhLang = 0.5;
void hanhLang()
{
	// nâu đất
	makeColorRGB(190, 192, 116, 100);

	// đáy
	symbol = Scale(chieuNgang_CauThang * 3 / 4, chieuCao_nenNhaGa, chieuDai_HanhLang);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_cauThang * symbol);
	lapPhuong();

	// tường hướng Z+
	symbol = Translate(0, (chieuCao_nenNhaGa / 2) + (chieuCaoTuong_nhaGa1 / 2), (chieuDai_HanhLang / 2) - (doDayTuong_nhaGa1 / 2))
		* Scale(chieuNgang_CauThang * 3 / 4, chieuCaoTuong_nhaGa1, doDayTuong_nhaGa1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_cauThang * symbol);
	lapPhuong();

	// tường hướng X+
	symbol = Translate((chieuNgang_CauThang * 3 / 8)- (doDayTuong_nhaGa1/2), (chieuCao_nenNhaGa / 2) + (chieuCaoTuong_nhaGa1 / 2), 0)
		* Scale(doDayTuong_nhaGa1, chieuCaoTuong_nhaGa1, chieuDai_HanhLang);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_cauThang * symbol);
	lapPhuong();

	// tường hướng Z-
	symbol = Translate(chieuNgang_CauThang / 4, (chieuCao_nenNhaGa / 2) + (chieuCaoTuong_nhaGa1 / 2), -(chieuDai_HanhLang / 2) + (doDayTuong_nhaGa1 / 2))
		* Scale(chieuNgang_CauThang / 4, chieuCaoTuong_nhaGa1, doDayTuong_nhaGa1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_cauThang * symbol);
	lapPhuong();

	// cầu thang
	for (int i = 0; i < 9; i++)
	{
		symbol = Translate(-chieuNgang_CauThang / 8,
			(chieuCao_nenNhaGa / 2) - (chieuCaoCauThang_hanhLang / 2) - chieuCaoCauThang_hanhLang * i,
			-(chieuDai_HanhLang / 2) - (chieuDaiCauThang_hanhLang / 2) - chieuDaiCauThang_hanhLang * i)
			* Scale(chieuNgang_CauThang / 2, chieuCaoCauThang_hanhLang, chieuDaiCauThang_hanhLang);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_cauThang * symbol);
		lapPhuong();
	}
}
void dayCauThang()
{
	makeColorRGB(190, 192, 116, 0);
	symbol = Scale(chieuNgang_CauThang, chieuCao_nenNhaGa / 2, chieuDaiDay_HanhLang);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_cauThang * symbol);
	lapPhuong();
}
void cauThang()
{
	Model_cauThang = Translate((chieuNgang_nenNhaGa / 2) + chieuNgang_CauThang / 2, 0, 0)
		* Translate(-0.5, 0, (chieuDai_nenNhaGa / 2) - (chieuDai_HanhLang / 2));
	hanhLang();
	Model_cauThang = Translate((chieuNgang_nenNhaGa / 2) + chieuNgang_CauThang / 2, 0, 0)
		* Translate(0, -chieuCao_nenNhaGa / 4, -(chieuDai_nenNhaGa / 2) + (chieuDaiDay_HanhLang / 2));
	dayCauThang();
}
mat4 Model_nguoi, Model_than, Model_ngoi;
GLfloat chieuCaoDau = 0.2, chieuNgangDau = 0.12, chieuSauDau = 0.15;
GLfloat quay_nguoi = 90, nguoi_ngoi = 90;
GLfloat nguoi_tien = 0, nguoi_phai = 0;
void Dau() {
	makeColorRGB(252, 224, 166, 10000);
	symbol = Scale(chieuNgangDau, chieuCaoDau, chieuSauDau);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * Model_than * symbol);
	lapPhuong();
}
void Co() {
	makeColorRGB(252, 224, 166, 10000);
	symbol = Scale(chieuNgangDau / 1.1, chieuCaoDau / 2, chieuSauDau / 1.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * Model_than * symbol);
	truTron();
}
void Than() {
	makeColorRGB(0, 178, 191, 10000);
	symbol = Scale(chieuNgangDau * 2.5, chieuCaoDau * 3, chieuSauDau * 1.5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * Model_than * symbol);
	lapPhuong();
}
void TayPhai() {
	makeColorRGB(0, 178, 191, 10000);
	symbol = Translate(chieuCaoDau, 0, 0) * Scale(chieuNgangDau, chieuCaoDau * 3, chieuSauDau / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * Model_than * symbol);
	truTron();
}
void TayTrai() {
	makeColorRGB(0, 178, 191, 10000);
	symbol = Translate(-chieuCaoDau, 0, 0) * Scale(chieuNgangDau, chieuCaoDau * 3, chieuSauDau / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * Model_than * symbol);
	truTron();
}
void BapChanPhai() {
	makeColorRGB(54, 54, 54, 10000);
	symbol = Translate(chieuCaoDau / 2, 0, 0) * Scale(chieuNgangDau, chieuCaoDau * 2, chieuSauDau / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * symbol);
	truTron();
}
void CangChanPhai() {
	makeColorRGB(255, 255, 255, 10000);
	symbol = Translate(chieuCaoDau / 2, chieuCaoDau * 1, 0) * Scale(chieuNgangDau / 1.1, chieuCaoDau * 2, chieuSauDau / 2.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * symbol);
	truTron();
}
void BapChanTrai() {
	makeColorRGB(54, 54, 54, 10000);
	symbol = Translate(-chieuCaoDau / 2, 0, 0) * Scale(chieuNgangDau, chieuCaoDau * 2, chieuSauDau / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * symbol);
	truTron();
}
void CangChanTrai() {
	makeColorRGB(255, 255, 255, 10000);
	symbol = Translate(-chieuCaoDau / 2, chieuCaoDau * 1, 0) * Scale(chieuNgangDau / 1.1, chieuCaoDau * 2, chieuSauDau / 2.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_nguoi * symbol);
	truTron();
}
void Nguoi()
{
	Model_nguoi = Translate(nguoi_phai, 1, -nguoi_tien) * RotateY(quay_nguoi) * Scale(3, 3, 3);
	CangChanPhai();
	CangChanTrai();

	Model_nguoi = Model_nguoi * Translate(0, chieuCaoDau * 3, 0) * Translate(0, -chieuCaoDau, 0) * RotateX(nguoi_ngoi) * Translate(0, chieuCaoDau, 0);
	BapChanPhai();
	BapChanTrai();

	Model_nguoi = Model_nguoi * Translate(0, chieuCaoDau * 2.5, 0) * Translate(0, -chieuCaoDau * 1.5, 0) * RotateX(-nguoi_ngoi) * Translate(0, chieuCaoDau * 1.5, 0);
	Than();

	TayPhai();
	TayTrai();

	Model_nguoi = Model_nguoi * Translate(0, chieuCaoDau * 1.75, 0);
	Co();

	Model_nguoi = Model_nguoi * Translate(0, chieuCaoDau * 0.75, 0);
	Dau();
}
mat4 Model_gheNgoiGaTauDien;
void gheNgoiGaTauDien() {
	makeColorRGB(100, 149, 237, 10);
	symbol = Scale(chieuCaoDau, chieuCaoDau * 2, 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model * Model_gaTauDien * Model_gheNgoiGaTauDien * symbol);
	lapPhuong();
}
void gaTauDien()
{
	Model_gaTauDien = Translate((chieuNgang_duongRay)+(chieuNgang_nenNhaGa / 2), 0, 0);
	nhaGa();
	maiChe();
	Model_gheNgoiGaTauDien = Translate(1, 1.5, 0) * Scale(8.5, 3, 1);
	gheNgoiGaTauDien();
	//cauThang();
	duongRay();
}

// Vinh
GLfloat TruY = 1.6, uTh = 0, vCuaT = 0, vCuaT1 = 0.09, vCuaT2 = 0.09;
mat4 ModeTru, ModelThangMay;
void ThangMay(GLfloat vCua) //trụ thang máy
{
	thep();

	GLfloat TruYTahngMay = TruY / 3.5;
	GLfloat TruX = TruY / 4 - 0.01, TruZ = TruY / 4 - 0.01; //ScaleController

	//Ground
	mat4 instance = Translate(0.0, 0.5 * TruYTahngMay / 80, 0.0)
		* Scale(TruX, TruYTahngMay / 80, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Celling
	instance = Translate(0, TruYTahngMay - TruYTahngMay / 80, 0.0) * Translate(0.0, 0.5 * TruYTahngMay / 80, 0.0)
		* Scale(TruX, TruYTahngMay / 80, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Rwall
	instance = Translate((0.5 * TruX) - TruX / 40, TruYTahngMay / 80, 0.0)
		* Translate(0.0, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX / 20, TruYTahngMay - TruYTahngMay / 40, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Lwall
	instance = Translate(-(0.5 * TruX) + TruX / 40, TruYTahngMay / 80, 0.0)
		* Translate(0.0, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX / 20, TruYTahngMay - TruYTahngMay / 40, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Bwall
	instance = Translate(0.0, TruYTahngMay / 80, -(0.5 * TruZ) + TruZ / 40)
		* Translate(0.0, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX, TruYTahngMay - TruYTahngMay / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Fwall
	instance = Translate(-TruX / 2, TruYTahngMay / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(0.5 * TruX / 3.5, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX / 3.5, TruYTahngMay - TruYTahngMay / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(TruX / 2, TruYTahngMay / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(-0.5 * TruX / 3.5, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX / 3.5, TruYTahngMay - TruYTahngMay / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	thep();
	/*Door*/
	instance = Translate(-TruX / 2 + vCua, TruYTahngMay / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(0.5 * TruX / 3.5, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX / 3.5, TruYTahngMay - TruYTahngMay / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(TruX / 2 - vCua, TruYTahngMay / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(-0.5 * TruX / 3.5, 0.5 * TruYTahngMay - TruYTahngMay / 80, 0.0)
		* Scale(TruX / 3.5, TruYTahngMay - TruYTahngMay / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModelThangMay * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);



}
void Tru(GLfloat u) //trụ thang máy
{
	Be_tong();
	GLfloat TruYVo = TruY;
	GLfloat TruX = TruYVo / 4, TruZ = TruYVo / 4; //ScaleController

	//Ground
	mat4 instance = Translate(0.0, 0.5 * TruYVo / 80, 0.0)
		* Scale(TruX, TruYVo / 80, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Celling
	instance = Translate(0, TruYVo - TruYVo / 80, 0.0) * Translate(0.0, 0.5 * TruYVo / 80, 0.0)
		* Scale(TruX, TruYVo / 80, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Rwall
	instance = Translate((0.5 * TruX) - TruX / 40, TruYVo / 80, 0.0)
		* Translate(0.0, 0.5 * TruYVo - TruYVo / 80, 0.0)
		* Scale(TruX / 20, TruYVo - TruYVo / 40, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Lwall
	instance = Translate(-(0.5 * TruX) + TruX / 40, TruYVo / 80, 0.0)
		* Translate(0.0, 0.5 * TruYVo - TruYVo / 80, 0.0)
		* Scale(TruX / 20, TruYVo - TruYVo / 40, TruZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Bwall
	instance = Translate(0.0, TruYVo / 80, -(0.5 * TruZ) + TruZ / 40)
		* Translate(0.0, 0.5 * TruYVo - TruYVo / 80, 0.0)
		* Scale(TruX, TruYVo - TruYVo / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Fwall
	instance = Translate(-TruX / 2, TruYVo / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(0.5 * TruX / 3.5, 0.5 * TruYVo - TruYVo / 80, 0.0)
		* Scale(TruX / 3.5, TruYVo - TruYVo / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(TruX / 2, TruYVo / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(-0.5 * TruX / 3.5, 0.5 * TruYVo - TruYVo / 80, 0.0)
		* Scale(TruX / 3.5, TruYVo - TruYVo / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	float a = TruX - 2 * (TruX / 3.5), b = a * 7 / 3, c = 0.75 * TruYVo - TruYVo / 40 - b;
	instance = Translate(0, TruYVo / 80 + b, (0.5 * TruZ) - TruZ / 40)
		* Translate(0, 0.5 * c - TruYVo / 80, 0.0)
		* Scale(a, c, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruYVo / 80 + (2 * b + c), (0.5 * TruZ) - TruZ / 40)
		* Translate(0, 0.5 * (TruYVo - 2 * b - c) - TruYVo / 80, 0.0)
		* Scale(a, TruYVo - 2 * b - c, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	thep();

	//Door
	instance = Translate(-TruX / 2 + vCuaT1, TruY / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(0.5 * TruX / 3.5, 0.5 * TruY / 3.5 - TruY / 80, -0.01)
		* Scale(TruX / 3.5, TruY / 3.5 - TruY / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(TruX / 2 - vCuaT1, TruY / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(-0.5 * TruX / 3.5, 0.5 * TruY / 3.5 - TruY / 80, -0.01)
		* Scale(TruX / 3.5, TruY / 3.5 - TruY / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-TruX / 2 + vCuaT2, TruY / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(0.5 * TruX / 3.5, 0.5 * TruY / 3.5 - TruY / 80 + TruY * 0.7, -0.01)
		* Scale(TruX / 3.5, TruY / 3.5 - TruY / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(TruX / 2 - vCuaT2, TruY / 80, (0.5 * TruZ) - TruZ / 40)
		* Translate(-0.5 * TruX / 3.5, 0.5 * TruY / 3.5 - TruY / 80 + TruY * 0.7, -0.01)
		* Scale(TruX / 3.5, TruY / 3.5 - TruY / 40, TruZ / 20);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, ModeTru * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	ModelThangMay = ModeTru * Translate(0, u, 0);
	ThangMay(vCuaT);
}

mat4 Model02;
GLfloat SanY = 0.1, SanX = 2.8, SanZ = 0.6;
void san() //sàn vào ga
{
	Be_tong();
	mat4 instance1 = Translate(-SanX / 2 + (SanZ + 0.2) / 2, 0, 0.0)
		* Scale(SanZ + 0.2, SanY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(0.0, 0, 0.0)
		* Scale(SanX, SanY, SanZ - 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

void LanCanSan()
{
	//lancan
	Son_satTrang();
	mat4 instance1 = Translate(-SanX / 2, SanY, SanZ / 4)
		* Scale(0.02, 2 * SanY, 3 * SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2), SanY, SanZ / 2 - 0.05)
		* Scale(0.02, 2 * SanY, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2), SanY, -SanZ / 2 + 0.05)
		* Scale(0.02, 2 * SanY, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2) / 2, SanY, -SanZ / 2)
		* Scale(SanZ + 0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2, SanY, SanZ / 4)
		* Scale(0.02, 2 * SanY, 3 * SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(SanX / 2 - (SanZ + 0.4), SanY, (SanZ - 0.2) / 2)
		* Scale(SanX - SanZ - 0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(SanX / 2 - (SanZ + 0.4), SanY, -(SanZ - 0.2) / 2)
		* Scale(SanX - SanZ - 0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void LanCanSanCoThangMay()
{

	//lancan
	Son_satTrang();
	mat4 instance1 = Translate(-SanX / 2, SanY, 0)
		* Scale(0.02, 2 * SanY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2), SanY, SanZ / 2 - 0.05)
		* Scale(0.02, 2 * SanY, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2), SanY, -SanZ / 2 + 0.05)
		* Scale(0.02, 2 * SanY, 0.1);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2) - 0.05, SanY, SanZ / 2)
		* Scale(0.1, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + 0.15, SanY, SanZ / 2)
		* Scale(0.3, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + (SanZ + 0.2) - 0.1, SanY, -SanZ / 2)
		* Scale(0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(-SanX / 2 + 0.1, SanY, -SanZ / 2)
		* Scale(0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(SanX / 2 - (SanZ + 0.4), SanY, (SanZ - 0.2) / 2)
		* Scale(SanX - SanZ - 0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(SanX / 2 - (SanZ + 0.4), SanY, -(SanZ - 0.2) / 2)
		* Scale(SanX - SanZ - 0.2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model02 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
mat4 Model_truTron;
void Tru_tron(GLfloat CotY, GLfloat CotX, GLfloat  x, GLfloat  y, GLfloat  z) //cột chống 
{
	GLfloat CotZ = CotX / 2.5;
	mat4 instance3 = Translate(x, y, z)
		* Scale(CotX, CotY, CotZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_truTron * instance3);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance3 = Translate(x, y, z) * RotateY(90)
		* Scale(CotX, CotY, CotZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_truTron * instance3);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance3 = Translate(x, y, z) * RotateY(45)
		* Scale(CotX, CotY, CotZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_truTron * instance3);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance3 = Translate(x, y, z) * RotateY(-45)
		* Scale(CotX, CotY, CotZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_truTron * instance3);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}

mat4 Model_mai;
void cot_mai(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c)
{
	mat4 instance = Translate(x, y, z)
		* Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void Khung_mai(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c)
{
	mat4 instance = Translate(x, y, z)
		* Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void mai_tron(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 instance = Translate(0, 0, 0.0)
		* Scale(x, y, z / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, z / 10) * RotateX(30) * Translate(0, 0, z / 10)
		* Scale(x, y, z / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -(z / 5) * abs(sin(3.14 / 6)), z / 10 + (z / 5) * abs(cos(3.14 / 6)))
		* RotateX(60) * Translate(0, 0, z / 10) * Scale(x, y, z / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, 0, -z / 10) * RotateX(-30) * Translate(0, 0, -z / 10)
		* Scale(x, y, z / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -(z / 5) * abs(sin(3.14 / 6)), -z / 10 - (z / 5) * abs(cos(3.14 / 6)))
		* RotateX(-60) * Translate(0, 0, -z / 10) * Scale(x, y, z / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void mai() //mái vào ga
{

	//mai1
	Mai_nhua();

	mat4 instance = Translate(0.0, TruY * 0.3, 0.0)
		* Scale(SanZ + 0.2, SanY / 10, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Son_satTrang();
	cot_mai(SanZ / 2 + 0.1 - SanZ / 30, TruY * 0.3 / 2, SanZ / 2 - SanZ / 30 - 0.01,
		SanZ / 20, TruY * 0.3, SanZ / 20);
	cot_mai(-SanZ / 2 - 0.1 + SanZ / 30, TruY * 0.3 / 2, SanZ / 2 - SanZ / 30 - 0.01,
		SanZ / 20, TruY * 0.3, SanZ / 20);
	cot_mai(SanZ / 2 + 0.1 - SanZ / 30, TruY * 0.3 / 2, -SanZ / 2 + SanZ / 30 + 0.01,
		SanZ / 20, TruY * 0.3, SanZ / 20);
	cot_mai(-SanZ / 2 - 0.1 + SanZ / 30, TruY * 0.3 / 2, -SanZ / 2 + SanZ / 30 + 0.01,
		SanZ / 20, TruY * 0.3, SanZ / 20);

	//mai2
	Mai_nhua();
	instance = Translate(SanZ / 2 + 0.1 + (SanX - SanZ - 0.2) / 2, TruY * 0.25, 0.0)
		* Scale(SanX - SanZ - 0.2, SanY / 10, SanZ / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanZ / 2 + 0.1 + (SanX - SanZ - 0.2) / 2, TruY * 0.25, SanZ / 10) * RotateX(30) * Translate(0, 0, SanZ / 10)
		* Scale(SanX - SanZ - 0.2, SanY / 10, SanZ / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanZ / 2 + 0.1 + (SanX - SanZ - 0.2) / 2, TruY * 0.25 - (SanZ / 5) * abs(sin(3.14 / 6)), SanZ / 10 + (SanZ / 5) * abs(cos(3.14 / 6)))
		* RotateX(60) * Translate(0, 0, SanZ / 10) * Scale(SanX - SanZ - 0.2, SanY / 10, SanZ / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanZ / 2 + 0.1 + (SanX - SanZ - 0.2) / 2, TruY * 0.25, -SanZ / 10) * RotateX(-30) * Translate(0, 0, -SanZ / 10)
		* Scale(SanX - SanZ - 0.2, SanY / 10, SanZ / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanZ / 2 + 0.1 + (SanX - SanZ - 0.2) / 2, TruY * 0.25 - (SanZ / 5) * abs(sin(3.14 / 6)), -SanZ / 10 - (SanZ / 5) * abs(cos(3.14 / 6)))
		* RotateX(-60) * Translate(0, 0, -SanZ / 10) * Scale(SanX - SanZ - 0.2, SanY / 10, SanZ / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_mai * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);



	Son_satTrang();

	cot_mai(-SanZ + 0.25 + SanZ + 0.18, TruY * 0.2 / 2, SanZ / 2 - SanZ / 30 - 0.01 - 0.1,
		SanZ / 20, TruY * 0.23, SanZ / 20);
	cot_mai(-SanZ + 0.25 + SanZ + 0.18, TruY * 0.2 / 2, -SanZ / 2 + SanZ / 30 + 0.01 + 0.1,
		SanZ / 20, TruY * 0.23, SanZ / 20);

	for (GLfloat i = SanZ; i < SanX - SanZ - 0.2 - SanZ / 8; i = i + SanZ)
	{
		cot_mai(-SanZ + 0.25 + SanZ + 0.18 + i, TruY * 0.2 / 2, SanZ / 2 - SanZ / 30 - 0.01 - 0.1,
			SanZ / 20, TruY * 0.23, SanZ / 20);
		cot_mai(-SanZ + 0.25 + SanZ + 0.18 + i, TruY * 0.2 / 2, -SanZ / 2 + SanZ / 30 + 0.01 + 0.1,
			SanZ / 20, TruY * 0.23, SanZ / 20);
	}
}
mat4 Model_bac_thang;
void Bac_thang(GLfloat r, GLfloat d, GLfloat n)
{

	GLfloat a = 1;
	for (GLfloat i = 0; i <= d * abs(cos(3.14 / 6)); i = i + d * abs(cos(3.14 / 6)) / n)
	{
		a++;
		mat4 instance = Translate(0.0, a * -d * abs(sin(3.14 / 6)) / n, i)
			* Scale(r, d * abs(sin(3.14 / 6)) / n, d * abs(cos(3.14 / 6)) / n);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_bac_thang * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
mat4 Model_thang_chuyen;
GLfloat vthg = 0;

void Bac_thang_chuyen_len(GLfloat r, GLfloat d, GLfloat n, GLfloat vth, GLfloat c)
{

	GLfloat a = 1, y, z, b = 0;
	thep();
	mat4 instance = Translate(0, -SanY / 2, 0)
		* Scale(r, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -d * abs(sin(3.14 * c / 180)), d * abs(cos(3.14 * c / 180)) + SanZ / 4)
		* Scale(r, SanY, SanZ / 2 + 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//lotduoi
	instance = Translate(0, -SanY / 2, 0) * RotateX(c) * Translate(0, 0, d * abs(cos(3.14 * c / 180)) / 2 + 0.1)
		* Scale(r, SanY, d * abs(cos(3.14 * c / 180)) + 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//lancanthgchuyen
	instance = Translate(r / 2, SanY, 0) * RotateX(c) * Translate(0, 0, d * abs(cos(3.14 * c / 180)) / 2 + 0.2)
		* Scale(0.02, 2 * SanY, d * abs(cos(3.14 * c / 180)) + 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-r / 2, SanY, 0) * RotateX(c) * Translate(0, 0, d * abs(cos(3.14 * c / 180)) / 2 + 0.2)
		* Scale(0.02, 2 * SanY, d * abs(cos(3.14 * c / 180)) + 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//
	instance = Translate(r / 2, SanY - 0.05, 0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-r / 2, SanY - 0.05, 0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(r / 2, SanY - d * abs(sin(3.14 * c / 180)), d * abs(cos(3.14 * c / 180)) + SanZ / 8)
		* Scale(0.02, 2 * SanY, SanZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate(-r / 2, SanY - d * abs(sin(3.14 * c / 180)), d * abs(cos(3.14 * c / 180)) + SanZ / 8)
		* Scale(0.02, 2 * SanY, SanZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	for (GLfloat i = 0; i <= d * abs(cos(3.14 * c / 180)) + 0.1; i = i + d * abs(cos(3.14 * c / 180)) / n)
	{
		a++;
		y = a * -d * abs(sin(3.14 * c / 180)) / n + vth * abs(sin(3.14 * c / 180)) / n + SanZ / 4;
		z = i - vth * abs(cos(3.14 * c / 180)) / n;

		if (z < -d * abs(cos(3.14 * c / 180)) / n || y >0)
		{
			z += d * abs(cos(3.14 * c / 180));
			y -= d * abs(sin(3.14 * c / 180));
			b += 1;
		}

		if (b == n)
		{
			vthg = 0;

		}

		San_den();
		mat4 instance = Translate(0, y - 0.03, z + 0.05)
			* Scale(r, d * abs(sin(3.14 * c / 180)) / n, d * abs(cos(3.14 * c / 180)) / n);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}


}
void Bac_thang_chuyen_xuong(GLfloat r, GLfloat d, GLfloat n, GLfloat vth, GLfloat c)
{

	GLfloat a = 1, y, z, b = 0;
	thep();
	mat4 instance = Translate(0, -SanY / 2, 0)
		* Scale(r, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -d * abs(sin(3.14 * c / 180)), d * abs(cos(3.14 * c / 180)) + SanZ / 4)
		* Scale(r, SanY, SanZ / 2 + 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//lotduoi
	instance = Translate(0, -SanY / 2, 0) * RotateX(c) * Translate(0, 0, d * abs(cos(3.14 * c / 180)) / 2 + 0.1)
		* Scale(r, SanY, d * abs(cos(3.14 * c / 180)) + 0.2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	//lancanthgchuyen
	instance = Translate(r / 2, SanY, 0) * RotateX(c) * Translate(0, 0, d * abs(cos(3.14 * c / 180)) / 2 + 0.2)
		* Scale(0.02, 2 * SanY, d * abs(cos(3.14 * c / 180)) + 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-r / 2, SanY, 0) * RotateX(c) * Translate(0, 0, d * abs(cos(3.14 * c / 180)) / 2 + 0.2)
		* Scale(0.02, 2 * SanY, d * abs(cos(3.14 * c / 180)) + 0.3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//
	instance = Translate(r / 2, SanY, 0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-r / 2, SanY, 0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(r / 2, SanY - d * abs(sin(3.14 * c / 180)), d * abs(cos(3.14 * c / 180)) + SanZ / 8)
		* Scale(0.02, 2 * SanY, SanZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-r / 2, SanY - d * abs(sin(3.14 * c / 180)), d * abs(cos(3.14 * c / 180)) + SanZ / 8)
		* Scale(0.02, 2 * SanY, SanZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	for (GLfloat i = 0; i <= d * abs(cos(3.14 * c / 180)) + 0.1; i = i + d * abs(cos(3.14 * c / 180)) / n)
	{
		a++;
		y = a * -d * abs(sin(3.14 * c / 180)) / n + vth * abs(sin(3.14 * c / 180)) / n + SanZ / 4;
		z = i - vth * abs(cos(3.14 * c / 180)) / n;

		if (z > d * abs(cos(3.14 * c / 180)) || y < -d * abs(sin(3.14 * c / 180)))
		{
			z -= d * abs(cos(3.14 * c / 180));
			y += d * abs(sin(3.14 * c / 180));
			b += 1;
		}

		if (b == n)
		{
			vthg = 0;

		}

		San_den();
		mat4 instance = Translate(0, y - 0.03, z + 0.05)
			* Scale(r, d * abs(sin(3.14 * c / 180)) / n, d * abs(cos(3.14 * c / 180)) / n);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang_chuyen * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	}


}
mat4 Model_truVuong;
void Tru_vuong(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c)
{
	mat4 instance = Translate(x, y, z)
		* Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_truVuong * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
mat4 Model_thang;
GLfloat a = (0.75 * TruY) / 3;
GLfloat b = SanZ / 5 + SanZ / 5 * abs(cos(3.14 / 6)) + SanZ / 5 * abs(cos(3.14 / 3));
void ThangCoThangChuyen() //thang vào ga
{

	Be_tong();

	mat4 instance = Translate(-0.1, 0, 0.0)
		* Scale(SanZ + 0.2, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate((SanZ - b / 2) / 4, 0.5 * SanY, 0.5 * SanZ / 2) * RotateX(30) * Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(SanZ - b / 2, SanY, a / abs(sin(3.14 / 6)));
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4)
		* Scale(SanZ - b / 2, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -a + 0.5 * SanY, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(SanZ - b / 2, SanY, a / abs(sin(3.14 / 6)));
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(SanZ - b / 2, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -2 * a + 0.5 * SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(SanZ - b / 2, SanY, 0.9 * a / abs(sin(3.14 / 6)));
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//
	Son_satTrang();
	instance = Translate(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4, 0.0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-0.1 + SanZ / 2 + 0.1 - (SanZ - b / 2), 3 * SanY / 4, 0.0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, -SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, -SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), SanY, 0.5 * SanZ / 2) * RotateX(30) * Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4 - 0.01)
		* Scale(0.02, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, SanY, 0.5 * SanZ / 2) * RotateX(30) * Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4 - 0.01)
		* Scale(0.02, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 - a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + -a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);




	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - a, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.02)
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - 2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - a, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.02)
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - 2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 - 2 * a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + 2 * -a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - 2 * a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - 2 * a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), -2 * a + SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02 / 2, 2 * SanY, 0.9 * a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - 3 * a + 0.02, 3 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, -2 * a + SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02 / 2, 2 * SanY, 0.9 * a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - 3 * a + 0.02, 3 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, 0);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 - 3 * a + (TruY * 0.11) / 2, 4 * SanZ / 2 + 3 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, 0);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - 3 * a + (TruY * 0.11) / 2, 4 * SanZ / 2 + 3 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);


	//



	//
	Be_tong();

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 - SanZ / 12, 0, 0);
	Tru_vuong(SanZ / 2 - SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 3 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 + SanZ / 12, 0, 0);
	Tru_vuong(-SanZ / 2 + SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 3 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 - SanZ / 12, -a / 2, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4);
	Tru_vuong(SanZ / 2 - SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 2 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 + SanZ / 12, -a / 2, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4);
	Tru_vuong(-SanZ / 2 + SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 2 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 - SanZ / 12, -a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4);
	Tru_vuong(SanZ / 2 - SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 + SanZ / 12, -a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4);
	Tru_vuong(-SanZ / 2 + SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, a - SanY, SanZ / 6);



	Model_bac_thang = Model_thang * Translate(b / 4, 1.5 * SanY, 0.75 * SanZ / 2);
	Bac_thang(b + 0.1, a / abs(sin(3.14 / 6)), 6);

	Model_bac_thang = Model_thang * Translate(0.0, -a, a / abs(sin(3.14 / 6)) * abs(cos(3.14 / 6)) + SanZ / 3)
		* Translate(b / 4, 1.5 * SanY, 0.75 * SanZ / 2);
	Bac_thang(b + 0.1, a / abs(sin(3.14 / 6)), 6);

	Model_bac_thang = Model_thang * Translate(0.0, -2 * a, 2 * a / abs(sin(3.14 / 6)) * abs(cos(3.14 / 6)) + 2 * SanZ / 3)
		* Translate(b / 4, 1.5 * SanY, 0.75 * SanZ / 2);
	Bac_thang(b + 0.1, a / abs(sin(3.14 / 6)), 6);


	Mai_nhua();

	Model_mai = Model_thang * Translate(b / 4 + 0.02, TruY * 0.25, 0.05) * RotateY(90);
	mai_tron(SanZ / 2 + 0.1, SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, 0.5 * SanY, 0.5 * SanZ / 2) * RotateX(30)
		* Translate(0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(a / abs(sin(3.14 / 6)), SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4 + 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(SanZ / 2, SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -a + 0.5 * SanY, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(a / abs(sin(3.14 / 6)), SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4 + 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(SanZ / 2, SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -2 * a + 0.5 * SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)) - 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(0.75 * a / abs(sin(3.14 / 6)), SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -2.75 * a, 3 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 1.5 * SanZ / 4 + 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(SanZ / 4, SanY / 10, SanZ);

	Model = Model_thang * Translate(0, -2.75 * a, 0);
	/*Nen(100, 0.01, 100);*/
}
void Thang() //thang vào ga
{

	Be_tong();

	mat4 instance = Translate((SanZ - b / 2) / 4, 0, 0)
		* Scale(SanZ - b / 4, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate((SanZ - b / 2) / 4, 0.5 * SanY, 0.5 * SanZ / 2) * RotateX(30) * Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(SanZ - b / 2, SanY, a / abs(sin(3.14 / 6)));
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4)
		* Scale(SanZ - b / 2, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -a + 0.5 * SanY, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(SanZ - b / 2, SanY, a / abs(sin(3.14 / 6)));
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(SanZ - b / 2, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate((SanZ - b / 2) / 4, -2 * a + 0.5 * SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(SanZ - b / 2, SanY, 0.9 * a / abs(sin(3.14 / 6)));
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//
	Son_satTrang();
	instance = Translate(-0.1 + 0.05 + SanZ / 2 + 0.1, 3 * SanY / 4, 0.0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-0.1 + 0.05 + SanZ / 2 + 0.1 - (SanZ - b / 2), 3 * SanY / 4, 0.0)
		* Scale(0.02, 2 * SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, -SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, -SanZ / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 + (TruY * 0.11) / 2, 0.0,
		SanZ / 20, TruY * 0.11, SanZ / 20);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), SanY, 0.5 * SanZ / 2) * RotateX(30) * Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4 - 0.01)
		* Scale(0.02, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, SanY, 0.5 * SanZ / 2) * RotateX(30) * Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4 - 0.01)
		* Scale(0.02, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 - a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + -a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - a + (TruY * 0.11) / 2, SanZ / 2 + a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);




	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - a, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.02)
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - 2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - a, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.02)
		* Scale(0.02, 2 * SanY, a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - 2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 - 2 * a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 + 2 * -a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);

	Model_mai = Model_thang * Translate(0, 0, SanZ / 2);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - 2 * a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, SanY / 4);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - 2 * a + (TruY * 0.11) / 2, SanZ + 2 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), -2 * a + SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02 / 2, 2 * SanY, 0.9 * a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4 - (SanZ - b / 2), 3 * SanY / 4 - 3 * a + 0.02, 3 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, -2 * a + SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)))
		* Scale(0.02 / 2, 2 * SanY, 0.9 * a / abs(sin(3.14 / 6)) + 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(3 * (SanZ - b / 2) / 4, 3 * SanY / 4 - 3 * a + 0.02, 3 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4)
		* Scale(0.02 / 2, 2 * SanY, SanZ / 2 - 0.05);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_thang * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_mai = Model_thang * Translate(0, 0, 0);
	cot_mai(-0.1 + SanZ / 2 + 0.2 - (SanZ - b / 2), 3 * SanY / 4 - 3 * a + (TruY * 0.11) / 2, 4 * SanZ / 2 + 3 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);
	Model_mai = Model_thang * Translate(0, 0, 0);
	cot_mai(-0.1 + SanZ / 2 + 0.1, 3 * SanY / 4 - 3 * a + (TruY * 0.11) / 2, 4 * SanZ / 2 + 3 * a / abs(cos(3.14 / 6)),
		SanZ / 20, TruY * 0.21, SanZ / 20);


	//



	//
	Be_tong();

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 - SanZ / 12, 0, 0);
	Tru_vuong(SanZ / 2 - SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 3 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 + SanZ / 12, 0, 0);
	Tru_vuong(-SanZ / 2 + SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 3 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 - SanZ / 12, -a / 2, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4);
	Tru_vuong(SanZ / 2 - SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 2 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 + SanZ / 12, -a / 2, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4);
	Tru_vuong(-SanZ / 2 + SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, 2 * a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 - SanZ / 12, -a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4);
	Tru_vuong(SanZ / 2 - SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, a - SanY, SanZ / 6);

	Model_truVuong = Model_thang * Translate((SanZ - b / 2) / 4 + SanZ / 12, -a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4);
	Tru_vuong(-SanZ / 2 + SanZ / 8, -(3 * a - SanY) / 2, 0, SanZ / 6, a - SanY, SanZ / 6);



	Model_bac_thang = Model_thang * Translate(b / 4, 1.5 * SanY, 0.75 * SanZ / 2);
	Bac_thang(b + 0.1, a / abs(sin(3.14 / 6)), 6);

	Model_bac_thang = Model_thang * Translate(0.0, -a, a / abs(sin(3.14 / 6)) * abs(cos(3.14 / 6)) + SanZ / 3)
		* Translate(b / 4, 1.5 * SanY, 0.75 * SanZ / 2);
	Bac_thang(b + 0.1, a / abs(sin(3.14 / 6)), 6);

	Model_bac_thang = Model_thang * Translate(0.0, -2 * a, 2 * a / abs(sin(3.14 / 6)) * abs(cos(3.14 / 6)) + 2 * SanZ / 3)
		* Translate(b / 4, 1.5 * SanY, 0.75 * SanZ / 2);
	Bac_thang(b + 0.1, a / abs(sin(3.14 / 6)), 6);


	Mai_nhua();

	Model_mai = Model_thang * Translate(b / 4 + 0.02, TruY * 0.25, 0.05) * RotateY(90);
	mai_tron(SanZ / 2 + 0.1, SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, 0.5 * SanY, 0.5 * SanZ / 2) * RotateX(30)
		* Translate(0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(a / abs(sin(3.14 / 6)), SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -a, SanZ / 2 + a / abs(cos(3.14 / 6))) * Translate(0.0, 0, SanZ / 4 + 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(SanZ / 2, SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -a + 0.5 * SanY, SanZ / 2 + SanZ / 2 + a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.5 * a / abs(sin(3.14 / 6)) - 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(a / abs(sin(3.14 / 6)), SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -2 * a, 2 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 2 * SanZ / 4 + 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(SanZ / 2, SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -2 * a + 0.5 * SanY, SanZ + 3 * SanZ / 4 + 2 * a / abs(cos(3.14 / 6))) * RotateX(30)
		* Translate(0.0, -0.5 * SanY, 0.75 * 0.5 * a / abs(sin(3.14 / 6)) - 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(0.75 * a / abs(sin(3.14 / 6)), SanY / 10, SanZ);

	Model_mai = Model_thang * Translate(0.0, -2.75 * a, 3 * (SanZ / 2 + a / abs(cos(3.14 / 6)))) * Translate(0.0, 0, 1.5 * SanZ / 4 + 0.1)
		* Translate(b / 4 + 0.02, TruY * 0.25, 0) * RotateY(90);
	mai_tron(SanZ / 4, SanY / 10, SanZ);

	Model = Model_thang * Translate(0, -2.75 * a, 0);

}

void LoiVaoCoThangMay()
{

	Model02 = Model01 * Translate(0, 0, 0);
	san();
	LanCanSanCoThangMay();
	ModeTru = Model02 * Translate(-0.5 * SanX + 0.2 + 0.5 * SanZ, -SanY / 2 - 0.75 * TruY + SanY, 3 * SanZ / 4) * RotateY(180);
	Tru(uTh);
	Model_thang = Model02 * Translate(-0.5 * SanX + 0.2 + 0.5 * SanZ, 0, -3 * SanZ / 4) * RotateY(180);
	Thang();
	Model_mai = Model01 * Translate(-SanX / 2 + (SanZ + 0.2) / 2, 0, 0);
	mai();
}
void LoiVaoKoThangMay()
{

	Model02 = Model01 * Translate(0, 0, 0);
	san();
	LanCanSan();
	Model_thang = Model02 * Translate(-0.5 * SanX + 0.2 + 0.5 * SanZ, 0, 3 * SanZ / 4);
	ThangCoThangChuyen();
	Model_thang_chuyen = Model_thang * Translate(-SanZ / 2, 1.5 * SanY - 0.1, 0.75 * SanZ / 2);
	Bac_thang_chuyen_len(SanZ - b, 3 * a / abs(sin(3.14 / 6)), 24, vthg, 30);
	Model_truTron = Model02 * Translate(0, 0, 0);
	Be_tong();
	Tru_tron(TruY * 0.70, SanZ / 3, -SanX / 2 + 0.2 + SanZ / 2, -0.5 * TruY * 0.70 - SanY / 2, 0);
	Model_mai = Model01 * Translate(-SanX / 2 + (SanZ + 0.2) / 2, 0, 0);
	mai();

}
mat4 Model_Ga, Model_Ga01, Model_Ga02;
GLfloat SanGaY = 0.2, SanGaX = 2.8, SanGaZ = SanGaX * 3, TruGaY = 1.6;
void TuongSanThangLenGa()
{
	Tuong();
	mat4 instance = Translate(0, (TruGaY) / 2, 0)
		* Scale(SanGaX / 50, TruGaY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	for (GLfloat i = 0; i <= TruGaY; i = i + 0.05)
	{

		instance = Translate(-0.05, i, 0) * RotateZ(135)
			* Scale(0.05, 0.02, SanZ - 0.01);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}
}
void TuongSanThangLenGaCua()
{
	Tuong();
	mat4 instance = Translate(0, TruGaY / 2, 0)
		* Scale(SanGaX / 50, TruGaY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	for (GLfloat i = 0; i <= TruGaY - (SanZ)+SanY; i = i + 0.05)
	{

		instance = Translate(-0.05, i + (SanZ), 0) * RotateZ(135)
			* Scale(0.05, 0.02, SanZ - 0.01);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}
}
void TuongRaoCayPhai()
{

	GLint X = 0, Y = 0, Z = 0;
	Tuong();
	mat4 instance = Translate(0, TruGaY / 2, 0)
		* Scale(SanGaX / 50, TruGaY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	for (GLfloat i = 0; i <= TruGaY; i = i + 0.05)
	{

		instance = Translate(-0.05, i, -SanZ / 4) * RotateZ(135)
			* Scale(0.05, 0.02, SanZ / 2 - 0.005);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}

	for (GLfloat i = 0; i <= TruGaY; i = i + 0.15)
	{
		for (GLfloat j = 0; j <= (SanZ / 2 - 0.005); j = j + 0.1)
		{

			instance = Translate(-0.05, i, SanZ / 4)
				* Scale(0.02, 0.02, SanZ / 2 - 0.005);
			glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
			glDrawArrays(GL_TRIANGLES, 0, NumPoints);

			instance = Translate(-0.05, TruGaY / 2, j)
				* Scale(0.02, TruGaY, 0.02);
			glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
			glDrawArrays(GL_TRIANGLES, 0, NumPoints);

		}
	}


	Cay();
	for (GLfloat i = 0; i <= TruGaY / 3; i = i + 0.03)
	{
		Y += 30;
		for (GLfloat j = 0; j <= (SanZ / 2 - 0.005); j = j + 0.06)
		{
			Z += 40;
			for (GLfloat k = -0.2; k <= (SanZ / 3 - 0.2); k = k + 0.08)
			{
				X += 50;

				instance = Translate(k, i, j) * RotateX(X) * RotateY(Y) * RotateZ(Z)
					* Scale(0.1, 0.001, 0.02);
				glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
				glDrawArrays(GL_TRIANGLES, 0, NumPoints);

			}

		}


	}
	for (GLfloat i = 2 * TruGaY / 3; i <= 3 * TruGaY / 3; i = i + 0.03)
	{
		Y += 30;
		for (GLfloat j = 0; j <= (SanZ / 2 - 0.005); j = j + 0.06)
		{
			Z += 40;
			for (GLfloat k = -0.2; k <= (SanZ / 3 - 0.2); k = k + 0.08)
			{
				X += 50;

				instance = Translate(k, i, j) * RotateX(X) * RotateY(Y) * RotateZ(Z)
					* Scale(0.1, 0.001, 0.02);
				glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
				glDrawArrays(GL_TRIANGLES, 0, NumPoints);

			}

		}


	}

}

void TuongRaoCayTrai()
{

	GLint X = 0, Y = 0, Z = 0;
	Tuong();
	mat4 instance = Translate(0, TruGaY / 2, 0)
		* Scale(SanGaX / 50, TruGaY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	for (GLfloat i = 0; i <= TruGaY; i = i + 0.05)
	{

		instance = Translate(-0.05, i, SanZ / 4) * RotateZ(135)
			* Scale(0.05, 0.02, SanZ / 2 - 0.005);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}

	for (GLfloat i = 0; i <= TruGaY; i = i + 0.15)
	{
		for (GLfloat j = 0; j <= (SanZ / 2 - 0.005); j = j + 0.1)
		{

			instance = Translate(-0.05, i, -SanZ / 4)
				* Scale(0.02, 0.02, SanZ / 2 - 0.005);
			glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
			glDrawArrays(GL_TRIANGLES, 0, NumPoints);

			instance = Translate(-0.05, TruGaY / 2, j - SanZ / 2)
				* Scale(0.02, TruGaY, 0.02);
			glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
			glDrawArrays(GL_TRIANGLES, 0, NumPoints);

		}
	}


	Cay();
	for (GLfloat i = 0; i <= TruGaY / 3; i = i + 0.03)
	{
		Y += 30;
		for (GLfloat j = 0; j <= (SanZ / 2 - 0.005); j = j + 0.06)
		{
			Z += 40;
			for (GLfloat k = -0.2; k <= (SanZ / 3 - 0.2); k = k + 0.08)
			{
				X += 50;

				instance = Translate(k, i, j - SanZ / 2) * RotateX(X) * RotateY(Y) * RotateZ(Z)
					* Scale(0.1, 0.001, 0.02);
				glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
				glDrawArrays(GL_TRIANGLES, 0, NumPoints);

			}

		}


	}
	for (GLfloat i = 2 * TruGaY / 3; i <= 3 * TruGaY / 3; i = i + 0.03)
	{
		Y += 30;
		for (GLfloat j = 0; j <= (SanZ / 2 - 0.005); j = j + 0.06)
		{
			Z += 40;
			for (GLfloat k = -0.2; k <= (SanZ / 3 - 0.2); k = k + 0.08)
			{
				X += 50;

				instance = Translate(k, i, j - SanZ / 2) * RotateX(X) * RotateY(Y) * RotateZ(Z)
					* Scale(0.1, 0.001, 0.02);
				glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
				glDrawArrays(GL_TRIANGLES, 0, NumPoints);

			}

		}


	}

}
void TuongRaoSat()
{
	Be_tong();
	mat4 instance;
	for (GLfloat i = 0; i <= TruGaY; i = i + 0.05)
	{

		instance = Translate(-0.05, i, 0) * RotateZ(135)
			* Scale(0.05, 0.02, SanZ - 0.01);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga02 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	}
}
void SanThangLenGa()
{
	Be_tong();
	mat4 instance = Translate(0, 0, 0)
		* Scale(SanZ, SanGaY, SanGaZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//wall
	Tuong();
	instance = Translate(0, TruGaY / 2, SanGaZ / 4 - SanGaX / 100 + 0.01)
		* Scale(SanZ, (TruGaY), SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY / 2, -SanGaZ / 4 + SanGaX / 100 - 0.01)
		* Scale(SanZ, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//wall2
	instance = Translate(SanZ / 2, TruGaY / 2, SanGaZ / 4 - SanZ / 2)
		* Scale(SanGaX / 50, TruGaY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanZ / 2, TruGaY / 2, -SanGaZ / 4 + SanZ / 2)
		* Scale(SanGaX / 50, TruGaY, SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//wall3
	instance = Translate(-SanZ / 2, TruGaY / 2, SanGaZ / 4 - SanZ / 6)
		* Scale(SanGaX / 50, TruGaY, SanZ / 3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanZ / 2, TruGaY / 2, -SanGaZ / 4 + SanZ / 6)
		* Scale(SanGaX / 50, TruGaY, SanZ / 3);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Mai_nhua();
	Model_thang_chuyen = Model_Ga01 * Translate(-SanZ / 4, TruGaY / 2, -SanGaZ / 4 + (TruGaY) / 2 / abs(cos(3.14 / 6)) * 0.5);
	Bac_thang_chuyen_len(SanZ / 2 - 0.02, (TruGaY / 2) / abs(sin(3.14 / 6)), 24, vthg, 30);
	Model_thang_chuyen = Model_Ga01 * Translate(-SanZ / 4, (TruGaY / 2) / abs(sin(3.14 / 6)), -SanGaZ / 4 + (TruGaY) / 2 / abs(cos(3.14 / 6)) * 0.5);
	Bac_thang_chuyen_len(SanZ / 2 - 0.02, (TruGaY / 2) / abs(sin(3.14 / 6)), 24, vthg, 30);

	Model_thang_chuyen = Model_Ga01 * Translate(-SanZ / 4, TruGaY / 2, SanGaZ / 4 - (TruGaY) / 2 / abs(cos(3.14 / 6)) * 0.5) * RotateY(180);
	Bac_thang_chuyen_xuong(SanZ / 2 - 0.02, (TruGaY / 2) / abs(sin(3.14 / 6)), 24, -vthg, 30);
	Model_thang_chuyen = Model_Ga01 * Translate(-SanZ / 4, (TruGaY / 2) / abs(sin(3.14 / 6)), SanGaZ / 4 - (TruGaY) / 2 / abs(cos(3.14 / 6)) * 0.5) * RotateY(180);
	Bac_thang_chuyen_xuong(SanZ / 2 - 0.02, (TruGaY / 2) / abs(sin(3.14 / 6)), 24, -vthg, 30);


	//
	Be_tong();
	instance = Translate(SanZ / 4, TruGaY / 2 - SanY / 2, 0)
		* Scale(SanZ / 2, SanY, SanGaZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate(0, TruGaY / 2 - SanY / 2, SanGaZ / 4 - SanZ / 4)
		* Scale(SanZ, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY / 2 - SanY / 2, -SanGaZ / 4 + SanZ / 4)
		* Scale(SanZ, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Son_satTrang();
	instance = Translate(SanZ / 2, TruGaY / 2 + SanY, 0)
		* Scale(0.02, 2 * SanY, SanGaZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);




	//

	Be_tong();
	instance = Translate(SanZ / 4, TruGaY - SanY / 2, 0)
		* Scale(SanZ / 2, SanY, SanGaZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	instance = Translate(0, TruGaY - SanY / 2, 0)
		* Scale(SanZ, SanY, 2 * SanZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY - SanY / 2, SanGaZ / 4 - SanZ / 4)
		* Scale(SanZ, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY - SanY / 2, -SanGaZ / 4 + SanZ / 4)
		* Scale(SanZ, SanY, SanZ / 2);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Son_satTrang();

	instance = Translate(-SanZ / 4, TruGaY + SanY, SanZ)
		* Scale(SanZ / 2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(-SanZ / 4, TruGaY + SanY, -SanZ)
		* Scale(SanZ / 2, 2 * SanY, 0.02);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY + SanY, SanGaZ / 4 - SanGaZ / 8 + 0.1)
		* Scale(0.02, 2 * SanY, SanGaZ / 8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY + SanY, -SanGaZ / 4 + SanGaZ / 8 - 0.1)
		* Scale(0.02, 2 * SanY, SanGaZ / 8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga01 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, 0);
	TuongSanThangLenGa();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, SanZ);
	TuongRaoCayPhai();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, -SanZ);
	TuongRaoCayTrai();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, 3 * SanZ - SanZ / 3 - 0.02);
	TuongRaoCayPhai();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, 0, 2 * SanZ);
	TuongSanThangLenGaCua();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, 0, -2 * SanZ);
	TuongSanThangLenGaCua();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, -3 * SanZ + SanZ / 3 + 0.02);
	TuongRaoCayTrai();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, 4 * SanZ - SanZ / 3 - 0.02);
	TuongRaoSat();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, 5 * SanZ - SanZ / 3 - 0.02);
	TuongRaoSat();
	Model_Ga02 = Model_Ga01 * Translate(-SanZ / 2, SanY, 6 * SanZ - SanZ / 3 - 0.02);
	TuongRaoSat();



	Model01 = Model_Ga01 * Translate(-(SanX + SanZ) / 2, SanGaY / 2 - SanY / 2, SanGaZ / 8);
	LoiVaoKoThangMay();
	Model01 = Model_Ga01 * Translate(-(SanX + SanZ) / 2, SanGaY / 2 - SanY / 2, -SanGaZ / 8);
	LoiVaoCoThangMay();
}

void TruGa()
{
	GLfloat TruGaY = 0.75 * TruY;
	Be_tong();
	mat4 instance = Translate(0, -TruGaY / 2 - SanGaY / 2, SanGaZ / 12)
		* Scale(SanGaX / 5, TruGaY, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0, -0.15 - SanGaY / 2, SanGaZ / 12)
		* Scale(SanGaX, 0.3, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -TruGaY / 2 - SanGaY / 2, -SanGaZ / 12)
		* Scale(SanGaX / 5, TruGaY, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0, -0.15 - SanGaY / 2, -SanGaZ / 12)
		* Scale(SanGaX, 0.3, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//

	instance = Translate(0, -TruGaY / 2 - SanGaY / 2, 3 * SanGaZ / 12)
		* Scale(SanGaX / 5, TruGaY, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0, -0.15 - SanGaY / 2, 3 * SanGaZ / 12)
		* Scale(SanGaX, 0.3, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -TruGaY / 2 - SanGaY / 2, 3 * -SanGaZ / 12)
		* Scale(SanGaX / 5, TruGaY, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0, -0.15 - SanGaY / 2, 3 * -SanGaZ / 12)
		* Scale(SanGaX, 0.3, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//

	instance = Translate(0, -TruGaY / 2 - SanGaY / 2, 5 * SanGaZ / 12)
		* Scale(SanGaX / 5, TruGaY, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0, -0.15 - SanGaY / 2, 5 * SanGaZ / 12)
		* Scale(SanGaX, 0.3, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -TruGaY / 2 - SanGaY / 2, 5 * -SanGaZ / 12)
		* Scale(SanGaX / 5, TruGaY, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	instance = Translate(0, -0.15 - SanGaY / 2, 5 * -SanGaZ / 12)
		* Scale(SanGaX, 0.3, SanGaX / 5);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void SanGa()
{
	Be_tong();

	mat4 instance = Translate(0, 0, 0)
		* Scale(SanGaX, SanGaY, SanGaZ);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Tuong();
	//Wall
	instance = Translate(SanGaX / 2 - SanGaX / 100 + 0.01, TruGaY / 2, SanGaZ / 2 - SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanGaX / 2 - SanGaX / 100 + 0.01, TruGaY / 2, -SanGaZ / 2 + SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 2 + SanGaX / 100 - 0.01, TruGaY / 2, SanGaZ / 2 - SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 2 + SanGaX / 100 - 0.01, TruGaY / 2, -SanGaZ / 2 + SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Wall2
	instance = Translate(SanGaX / 4 - SanGaX / 100 + 0.01, TruGaY / 2, SanGaZ / 2 - SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanGaX / 4 - SanGaX / 100 + 0.01, TruGaY / 2, -SanGaZ / 2 + SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 4 + SanGaX / 100 - 0.01, TruGaY / 2, SanGaZ / 2 - SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 4 + SanGaX / 100 - 0.01, TruGaY / 2, -SanGaZ / 2 + SanGaZ / 8)
		* Scale(SanGaX / 50, TruGaY, SanGaZ / 4);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	//Wall3
	Be_tong();
	instance = Translate(0, TruGaY / 2, SanGaZ / 2 - SanGaX / 100 + 0.01)
		* Scale(SanGaX, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, TruGaY / 2, -SanGaZ / 2 + SanGaX / 100 - 0.01)
		* Scale(SanGaX, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


	Son_satTrang();
	instance = Translate(SanGaX / 2 - SanGaX / 8 - SanGaX / 100, TruGaY / 2, SanGaZ / 4 - SanGaX / 100 + 0.01)
		* Scale(SanGaX / 4, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanGaX / 2 - SanGaX / 8 - SanGaX / 100, TruGaY / 2, -SanGaZ / 4 + SanGaX / 100 - 0.01)
		* Scale(SanGaX / 4, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 2 + SanGaX / 8 + SanGaX / 100, TruGaY / 2, SanGaZ / 4 - SanGaX / 100 + 0.01)
		* Scale(SanGaX / 4, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 2 + SanGaX / 8 + SanGaX / 100, TruGaY / 2, -SanGaZ / 4 + SanGaX / 100 - 0.01)
		* Scale(SanGaX / 4, TruGaY, SanGaX / 50);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	Be_tong();
	instance = Translate(SanGaX / 4, TruGaY / 2, -SanGaZ / 8)
		* Scale(SanGaX / 8, TruGaY, SanGaX / 8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 4, TruGaY / 2, -SanGaZ / 8)
		* Scale(SanGaX / 8, TruGaY, SanGaX / 8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(SanGaX / 4, TruGaY / 2, SanGaZ / 8)
		* Scale(SanGaX / 8, TruGaY, SanGaX / 8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(-SanGaX / 4, TruGaY / 2, SanGaZ / 8)
		* Scale(SanGaX / 8, TruGaY, SanGaX / 8);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, Model_Ga * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	TruGa();
	Model_Ga01 = Model_Ga * Translate(-SanGaX / 2 - SanZ / 2, 0, 0);
	SanThangLenGa();
	Model_Ga01 = Model_Ga * Translate(+SanGaX / 2 + SanZ / 2, 0, 0) * RotateY(180);
	SanThangLenGa();
}

// hoàng
//ve hinh khoi so cap don vi
//tru tron 
void TruTron(mat4 model) {
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLE_STRIP, NumPoints1 * 2, NumPoints2 * 2);
	glDrawArrays(GL_TRIANGLE_FAN, 0, NumPoints1);
	glDrawArrays(GL_TRIANGLE_FAN, NumPoints1, NumPoints1);
}
//Quat
void Quat(bool ok, mat4 model) {
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * Scale(2, 2, 1));
	glDrawArrays(GL_TRIANGLE_STRIP, NumPoints1 * 2, 20);
	if (ok == true) {
		glDrawArrays(GL_TRIANGLE_FAN, 0, 11);
		glDrawArrays(GL_TRIANGLE_FAN, NumPoints1, 11);
	}
}
//Ban nguyet
void BanNguyet(bool ok, mat4 model) {
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * Scale(1, 2, 1));
	glDrawArrays(GL_TRIANGLE_STRIP, NumPoints1 * 2, 38);
	if (ok == true) {
		glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
		glDrawArrays(GL_TRIANGLE_FAN, NumPoints1, 20);
	}
}
//Hop chu nhat
void hinhHopCN(mat4 model) {
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
//Mot phan tu mat cau
void MotPhanMatCau(int sl, mat4 model) {
	glBindVertexArray(vao[2]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 1 + sl / 2);
	for (int i = 0; i < 8; i++)
		glDrawArrays(GL_TRIANGLE_STRIP, NumPoints3 + NumPoints4 * i, sl);
}
//mot phan tru nghieng
void MotPhanTruNghieng(int vt, int sl, mat4 model) {
	glBindVertexArray(vao[3]);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * Scale(2, 1, 1));
	glDrawArrays(GL_TRIANGLE_STRIP, vt, sl);
}
//ve hinh cac bo phan cua tau, tau dien co kich thuoc la Ox(rong)=0.6, Oy(cao)=1.05, Oz(dai)=2.8;
//mat vuong bo cong
void matVuongCong2Phia(GLfloat yhcn, bool ok, mat4 model, int vt1, int vt2) {
	if (vt1 == 1)	makeColorRGB(232, 86, 108, 100);
	if (vt2 == 1) 		makeColorRGB(233, 243, 55, 100);
	Quat(ok, model * Translate(0.6 / 2, 0, 0) * Scale(0.2, 1, 1));
	Quat(ok, model * Translate(-0.6 / 2, 0, 0) * RotateZ(90) * Scale(1, 0.2, 1));
	if (vt1 == 1) makeColorRGB(225, 226, 231, 100);
	if (vt2 == 1) makeColorRGB(232, 86, 108, 100);
	hinhHopCN(model * Translate(0, 1 - yhcn / 2, 0) * Scale(0.6, yhcn, 1));
}
void matVuongCongPhiaTruoc(mat4 model, int vt) {
	if (vt == 1)	makeColorRGB(232, 86, 108, 100);
	else 	makeColorRGB(233, 243, 55, 100);
	MotPhanMatCau(20, model * Translate(-0.3, 0, 0) * RotateY(180) * Scale(0.2 / 0.5, 2, 2));
	MotPhanMatCau(20, model * Translate(0.3, 0, 0) * RotateY(90) * Scale(2, 2, 0.2 / 0.5));
	if (vt == 1) makeColorRGB(225, 226, 231, 100);
	Quat(false, model * RotateY(90) * Scale(1, 1, 0.6));
}
//khung mat ben tau
void khungMatBen(GLfloat x, GLfloat z, mat4 model, int ok = 0) {
	//phan tren
	makeColorRGB(217, 217, 217, 100);
	hinhHopCN(model * Translate(-(x / 4), 0.25, 0) * Scale(x / 2, 0.5, z));
	if (ok) makeColorRGB(255, 174, 114, 100);
	else  makeColorRGB(255, 161, 78, 100);
	hinhHopCN(model * Translate(x / 4, 0.25, z / 2 - 0.02) * Scale(x / 2, 0.5, 0.04));
	hinhHopCN(model * Translate(x / 4, 0.25, -z / 2 + 0.02) * Scale(x / 2, 0.5, 0.04));

	hinhHopCN(model * Translate(x / 4, 0.48, 0) * Scale(x / 2, 0.04, z - 0.08));
	hinhHopCN(model * Translate(x / 4, 0.02, 0) * Scale(x / 2, 0.04, z - 0.08));

	//phan duoi
	if (ok)	makeColorRGB(198, 207, 47, 90);
	else  	makeColorRGB(233, 243, 55, 100);
	hinhHopCN(model * Translate(0, -0.25, 0) * Scale(x, 0.5, z));
}

//mat tren
void matTrenTau(mat4 model) {
	matVuongCong2Phia(1, true, model * Scale(0.6, 0.1, 2.8), 1, 0);
	makeColorRGB(242, 255, 151, 100);
	hinhHopCN(model * Scale(0.6, 0.001, 2.8));
}
//mat ben tau
void matBenTau(mat4 model) {
	GLfloat x = 0.03, y = 1, z = 2;
	//mat ben 1
	khungMatBen(0.05, 0.8, model * Translate(-0.275, 0, 0));
	khungMatBen(0.05, 0.4, model * Translate(-0.275, 0, 1.2));
	khungMatBen(0.05, 0.4, model * Translate(-0.275, 0, -1.2));
	//mat ben 2
	khungMatBen(0.05, 2.8, model * Translate(0.275, 0, 0) * RotateY(180));
	hinhHopCN(model * Translate(0.16, -0.25, 0) * Scale(0.18, 0.04, 2));
}

//mat duoi tau
void matDuoiTau(mat4 model) {
	hinhHopCN(model * Scale(0.6, 0.05, 2.8));
	matVuongCong2Phia(1, true, model * RotateZ(180) * Scale(0.6, 0.17, 0.8), 0, 0);
}

//mat truoc sau tau
void matTruocSauTau(mat4 model) {
	GLfloat xs = 0.05, ys = 1, zs = 0.05;
	makeColorRGB(195, 203, 209, 100);
	matVuongCong2Phia(1, true, model * Translate(0, 0.5, 0) * Scale(0.55, 0.05, zs * 2), 0, 0);
	hinhHopCN(model * Translate(0, 0.5, 0) * Scale(0.55, 0.001, zs * 2));
	matVuongCong2Phia(1, true, model * Translate(0, -0.5, 0) * RotateZ(180) * Scale(0.55, 0.05, zs * 2), 0, 0);
	hinhHopCN(model * Translate(0, -0.5, 0) * Scale(0.55, 0.001, zs * 2));
	hinhHopCN(model * Translate(0.3 - xs, 0, 0) * Scale(xs, ys, zs * 2));
	hinhHopCN(model * Translate(-0.3 + xs, 0, 0) * Scale(xs, ys, zs * 2));
}

//bo phan dau tau kich thuoc x(rong)=0.6,y(cao)= 1.27,z(dai)=0.8
void bophanDauTau(bool ok, mat4 model) {
	//phan khung chua cua ra vao
	if (ok == true)
	khungMatBen(0.05, 0.3, model * Translate(-0.275, 0, -0.15));
	khungMatBen(0.05, 0.3, model * Translate(0.275, 0, -0.15) * RotateY(180));
	matVuongCong2Phia(1, false, model * Translate(0, 0.5, -0.15) * Scale(0.6, 0.1, 0.3), 1, 0);
	makeColorRGB(233, 243, 55, 100);
	hinhHopCN(model * Translate(0, 0.5, -0.15) * Scale(0.6, 0.001, 0.3));
	//phan mat dau tau
	matVuongCongPhiaTruoc(model * Translate(0, 0.6 - 0.1, -0.3) * Scale(0.6, 0.1, 0.25), 1);
	matVuongCong2Phia(1, true, model * Translate(0, 0.5, -0.3) * RotateX(-90) * Scale(0.6, 0.25, 0.001), 0, 0);
	//kinh
	mat4 modelk = model * Translate(0, 0.25, -0.8 + 0.5) * RotateX(-90) * RotateY(180);
	GLfloat y = 0.5;
	makeColorRGB(217, 217, 217, 90);
	hinhHopCN(modelk * Translate(0, (0.5 + 0.25) * y, 0) * RotateX(-26.56505) * Scale(0.36, 0.001, y * sqrt(5) / 2));
	MotPhanTruNghieng(0, 22, modelk * Translate(0.18, 0, 0) * Scale(0.12, y, 0.5));
	MotPhanTruNghieng(20, 22, modelk * Translate(-0.18, 0, 0) * Scale(0.12, y, 0.5));
	//phan duoi
	matVuongCong2Phia(1, true, model * Translate(0, -0.25, -0.55) * RotateX(-90) * Scale(0.6, 0.25, 0.5), 0, 1);
	makeColorRGB(233, 243, 55, 100);
	hinhHopCN(model * Translate(-0.275, -0.25, -0.425) * Scale(0.05, 0.5, 0.25));
	hinhHopCN(model * Translate(0.275, -0.25, -0.425) * Scale(0.05, 0.5, 0.25));
	hinhHopCN(model * Translate(0, 0, -0.3) * Scale(0.5, 1, 0.001));
	//phan mat duoi
	hinhHopCN(model * Translate(0, -0.5 - 0.025, -0.275) * Scale(0.6, 0.05, 0.55));
	matVuongCong2Phia(1, false, model * Translate(0, -0.5 - 0.025, -0.55) * RotateX(-90) * Scale(0.6, 0.25, 0.05), 0, 1);//nen
	matVuongCongPhiaTruoc(model * Translate(0, -0.55, -0.55) * RotateZ(180) * Scale(0.6, 0.17, 0.25), 0);
	matVuongCong2Phia(1, true, model * Translate(0, -0.55, -0.275) * RotateZ(180) * Scale(0.6, 0.17, 0.55), 0, 0);
}

//Bo phan banh xe tau
//banh tau
void banhxetau(mat4 model) {
	GLfloat z1 = 0.025, z2 = 0.03;
	//banh lon
	makeColorRGB(140, 140, 140, 100);
	TruTron(model * Scale(0.28, 0.28, z1));
	//banh nho
	model *= Translate(0, 0, -z1 / 2 - z2 / 2) * Scale(0.25, 0.25, z2);
	TruTron(model);
}
int qb = 0;
//truc hai ben banh de bam vao banh
void trucBamBanhxe(GLfloat dau, mat4 model) {
	makeColorRGB(0, 155, 154, 100);
	Quat(true, model * Translate(-0.12, 0.05, dau * 0.27) * Scale(0.045, 0.06, 0.06));
	hinhHopCN(model * Translate(-0.2075, 0.0725, dau * 0.27) * Scale(0.175, 0.075, 0.06));
	Quat(true, model * Translate(-0.295, 0.035, dau * 0.27) * RotateY(180) * Scale(0.075, 0.075, 0.06));

	Quat(true, model * Translate(0.12, 0.05, dau * 0.27) * RotateY(180) * Scale(0.045, 0.06, 0.06));
	hinhHopCN(model * Translate(0.2075, 0.0725, dau * 0.27) * Scale(0.175, 0.075, 0.06));
	Quat(true, model * Translate(0.295, 0.035, dau * 0.27) * Scale(0.075, 0.075, 0.06));

	Quat(true, model * Translate(-0.295, 0.1 / 2 - 0.015, dau * 0.27) * RotateZ(-180) * Scale(0.075, 0.085, 0.06));
	Quat(true, model * Translate(-0.245, 0.1 / 2 - 0.015, dau * 0.27) * RotateZ(180) * RotateY(180) * Scale(0.075, 0.085, 0.06));

	Quat(true, model * Translate(0.245, 0.1 / 2 - 0.015, dau * 0.27) * RotateZ(-180) * Scale(0.075, 0.085, 0.06));
	Quat(true, model * Translate(0.295, 0.1 / 2 - 0.015, dau * 0.27) * RotateZ(180) * RotateY(180) * Scale(0.075, 0.085, 0.06));
}

void banhxeToa(mat4 model) {
	//cap 2 banh
	GLfloat cd = 0.15, dcx = 0.15 + 0.12;

	//truc va hai banh
	makeColorRGB(225, 226, 231, 100);
	TruTron(model * Translate(dcx, 0, 0) * Scale(0.05, 0.05, 0.6));
	banhxetau(model * Translate(dcx, 0, -0.15) * RotateZ(a));
	banhxetau(model * Translate(dcx, 0, 0.15) * RotateY(180) * RotateZ(qb));
	makeColorRGB(225, 226, 231, 100);
	TruTron(model * Translate(-dcx, 0, 0) * Scale(0.05, 0.05, 0.6));
	banhxetau(model * Translate(-dcx, 0, -0.15) * RotateZ(a));
	banhxetau(model * Translate(-dcx, 0, 0.15) * RotateY(180) * RotateZ(qb));

	makeColorRGB(0, 155, 154, 100);
	hinhHopCN(model * Scale(cd, 0.1, 0.6));
	//ghep vao  HCN tren
	hinhHopCN(model * Translate(0, 0.145, 0) * Scale(0.18, 0.03, 0.18));
	Quat(true, model * Translate(-0.15 / 2, 0.1 / 2, -0.26) * RotateZ(-180) * Scale(0.045, 0.1, 0.08));
	Quat(true, model * Translate(0.15 / 2, 0.1 / 2, -0.26) * RotateZ(180) * RotateY(180) * Scale(0.045, 0.1, 0.08)); // ben kia tuong tu
	Quat(true, model * Translate(-0.15 / 2, 0.1 / 2, 0.26) * RotateZ(-180) * Scale(0.045, 0.1, 0.08));
	Quat(true, model * Translate(0.15 / 2, 0.1 / 2, 0.26) * RotateZ(180) * RotateY(180) * Scale(0.045, 0.1, 0.08));
	makeColorRGB(197, 194, 190, 100);
	TruTron(model * Translate(0, 0.08 / 2 + 0.05, 0) * RotateX(90) * Scale(0.15, 0.15, 0.08));//dieu chinh z
	//truc 2 ben
	trucBamBanhxe(-1, model);
	trucBamBanhxe(1, model);

}

void bophanbanhxeToa(mat4 model) {
	//chieu dai banh xe toa la 0.84 
	banhxeToa(model * Translate(0.84 / 2 + 0.5, -0.71, 0));
	banhxeToa(model * Translate(-0.84 / 2 - 0.45, -0.71, 0));
}

//Bo phan toa tau
//toa 1
void bophanToaTau1(mat4 model) {
	bophanbanhxeToa(model * RotateY(90));
	matTrenTau(model * Translate(0, 0.5, 0));
	matBenTau(model);
	matDuoiTau(model * Translate(0, -(0.5 - 0.05 / 2 + 0.05), 0));
	matTruocSauTau(model * Translate(0, 0, 1.45));
	bophanDauTau(false, model * Translate(0, 0, -1.4));
}
//toa2
void bophanToaTau2(mat4 model) {
	bophanbanhxeToa(model * RotateY(90));
	matTrenTau(model * Translate(0, 0.5, 0));
	matBenTau(model);
	matDuoiTau(model * Translate(0, -(0.5 - 0.05 / 2 + 0.05), 0));
	matTruocSauTau(model * Translate(0, 0, 1.45));
	matTruocSauTau(model * Translate(0, 0, -1.45));
}
//toa3
void bophanToaTau3(mat4 model) {
	bophanbanhxeToa(model * RotateY(90));
	matTrenTau(model * Translate(0, 0.5, 0));
	matBenTau(model);
	matDuoiTau(model * Translate(0, -(0.5 - 0.05 / 2 + 0.05), 0));
	matTruocSauTau(model * Translate(0, 0, -1.45));
	bophanDauTau(true, model * Translate(0, 0, 1.4) * RotateY(180));
}
//cua ra vao
void canhCua(mat4 model) {
	khungMatBen(0.05, 0.3, model,1);
}
//num cua
void numCua(mat4 model) {
	makeColorRGB(150, 226, 231, 100);
	BanNguyet(true, model * Scale(0.06, 0.03, 0.02));
}
//Toa tau
GLfloat zc1 = 0, zc2 = 0, xc1 = 0;
GLfloat qn = 0;
GLfloat xa = 0;
void ToaTau(int vt, mat4 model) {
	mat4 modeltoatau = model * RotateY(-90);
	if (vt == 1) bophanToaTau1(modeltoatau);
	else if (vt == 2) bophanToaTau2(modeltoatau);
	else bophanToaTau3(modeltoatau);
	//cua ra vao toa tau
	//cua so 1
	mat4 modelcanhcua1toa = modeltoatau * Translate(-0.275 + xc1, 0, 0.85 + zc1);
	canhCua(modelcanhcua1toa);
	mat4 modelcanhcua2toa = modeltoatau * Translate(-0.275 + xc1, 0, 0.55 + zc2);
	canhCua(modelcanhcua2toa);
	//num cua ra vao 1
	mat4 modelnumxoay1 = modelcanhcua1toa * Translate(-0.035, 0, -0.08) * RotateX(-90 + qn) * RotateY(90);
	numCua(modelnumxoay1);
	mat4 modelnumxoay2 = modelcanhcua2toa * Translate(-0.035, 0, 0.08) * RotateX(90 - qn) * RotateY(90);
	numCua(modelnumxoay2);

	//cua so 2
	modelcanhcua1toa = modeltoatau * Translate(-0.275 + xc1, 0, -0.55 + zc1);
	canhCua(modelcanhcua1toa);
	modelcanhcua2toa = modeltoatau * Translate(-0.275 + xc1, 0, -0.85 + zc2);
	canhCua(modelcanhcua2toa);
	//num cua ra vao 2
	modelnumxoay1 = modelcanhcua1toa * Translate(-0.035, 0, -0.08) * RotateX(-90 + qn) * RotateY(90);
	numCua(modelnumxoay1);
	modelnumxoay2 = modelcanhcua2toa * Translate(-0.035, 0, 0.08) * RotateX(90 - qn) * RotateY(90);
	numCua(modelnumxoay2);
	if (vt == 1) {
		//cua ra vao toa dau
		mat4 modelcanhcuatoadau = modeltoatau * Translate(-0.275 + xc1, 0, -1.55 + zc2);
		canhCua(modelcanhcuatoadau);
		//num cua ra vao toa dau
		modelnumxoay2 = modelcanhcuatoadau * Translate(-0.035, 0, -0.1) * RotateX(-90 + qn) * RotateY(90);
		numCua(modelnumxoay2);
	}
}

void DoanTau1() {
	mat4 modeltoa1 = RotateY(-90) * Translate(xa, 24.2, -3.9) * Scale(5,9,10);
	ToaTau(1, modeltoa1);
	mat4 modeltoa2 = modeltoa1 * Translate(-3, 0, 0);
	ToaTau(2, modeltoa2);
	mat4 modeltoa3 = modeltoa2 * Translate(-3, 0, 0);
	ToaTau(3, modeltoa3);
}

void DoanTau2() {
	mat4 modeltoa1 = RotateY(90) * Translate(xa, 24.2, -3.9) * Scale(5, 9, 10);
	ToaTau(1, modeltoa1);
	mat4 modeltoa2 = modeltoa1 * Translate(-3, 0, 0);
	ToaTau(2, modeltoa2);
	mat4 modeltoa3 = modeltoa2 * Translate(-3, 0, 0);
	ToaTau(3, modeltoa3);
}

// Minh
mat4 model_oto, model_cuaxe;
mat4 model_oto_cuoi;
GLfloat cua = 180, q = 0, copxe = 0.0;
mat4 symbol_oto;

void oto2_thanhxe(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c) {
	makeColorRGB(190, 192, 116, 100);
	symbol_oto = Translate(x, y, z) * Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_oto_cuoi * model_oto * symbol_oto);
	lapPhuong();

}
void oto2_thanhxe2(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c) {
	makeColorRGB(190, 192, 116, 100);
	symbol_oto = Translate(x, y, z) * Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_oto_cuoi * model_oto * symbol_oto);
	lapPhuong();

}
void oto2_cuasau(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c) {
	makeColorRGB(190, 192, 116, 100);
	symbol_oto = Translate(x, y, z) * Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_oto_cuoi * model_oto * model_cuaxe * symbol_oto);
	lapPhuong();

}
void oto2_kinhxe(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c) {
	makeColorRGB(190, 192, 116, 100);
	symbol_oto = Translate(x, y, z) * RotateZ(30) * Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_oto_cuoi * model_oto * symbol_oto);
	lapPhuong();

}
void oto2_canhcua(GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c) {
	makeColorRGB(190, 192, 116, 100);
	symbol_oto = Translate(x, y, z) * RotateY(cua) * Translate(0.075, 0, 0) * Scale(a, b, c);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_oto_cuoi * model_oto * symbol_oto);
	lapPhuong();

}
void banhxe(GLfloat x, GLfloat y, GLfloat z) {
	makeColorRGB(1, 1, 1, 100);
	for (int i = 0; i < 180; i++) {
		symbol_oto = Translate(x, y, z) * RotateZ(i) * Scale(0.05, 0.075, 0.01);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model_oto_cuoi * model_oto * symbol_oto);
		lapPhuong();
	}
}
void oto2_dauxe() {
	oto2_thanhxe2(0.325, 0.5, 0, 0.2, 0.01, 0.25);
	oto2_thanhxe2(0.3, 0.7, 0, 0.15, 0.01, 0.25);
	oto2_kinhxe(0.225 + 0.1 + pow(3, 1 / 2) / 20 + 0.015, 0.65, 0, 0.01, 0.1 / (sqrt(3) / 2), 0.25);
	oto2_thanhxe(0.425, 0.55, 0, 0.01, 0.1, 0.25);
	oto2_thanhxe(0.225 + 0.075, 0.6, 0.125, 0.15, 0.2, 0.01);
	oto2_canhcua(0.225 + 0.15, 0.6, -0.125, 0.15, 0.2, 0.01);
	oto2_thanhxe(0.025 + 0.225 + 0.145, 0.55, -0.125, 0.05, 0.1, 0.01);
	oto2_thanhxe(0.025 + 0.225 + 0.145, 0.55, 0.125, 0.05, 0.1, 0.01);
	banhxe(-0.1, 0.5, -0.126);
	banhxe(-0.1, 0.5, -0.126);
	banhxe(0.3, 0.5, -0.126);
	banhxe(0.3, 0.5, -0.126);
}
void oto2_cuoixe() {
	oto2_thanhxe(0, 0.5, 0, 0.45, 0.01, 0.25);
	oto2_thanhxe(0, 0.75, 0, 0.45, 0.01, 0.25);
	oto2_thanhxe(0, 0.625, -0.125, 0.45, 0.25, 0.01);
	oto2_thanhxe(0, 0.625, 0.125, 0.45, 0.25, 0.01);
	oto2_thanhxe(0.225, 0.625, 0, 0.01, 0.25, 0.25);
	oto2_cuasau(-0.225, 0.625, 0, 0.01, 0.25, 0.25);
}
void oto2() {
	model_oto = Translate(q, -0.5 - 0.4 + 0.075 / 2 + 0.01, -0.7);
	model_cuaxe = Translate(0, copxe, 0);
	oto2_cuoixe();
	oto2_dauxe();
}

void theGioi()
{
	Model = Translate(0, -12, 0);
	Ground();

	// ga
	Model_Ga = Scale(40 / 2.8, 10, 10);
	SanGa();

	// nắp
	Model = Scale(1.55, 1, 1.95) * Translate(0, 16.5, 0);
	gaTauDien();
	Nguoi();
	Model = Scale(1.55, 1, 1.95) * Translate(0, 16.5, 0) * Scale(-1, 1, 1);
	gaTauDien();

	// tàu
	DoanTau1();
	DoanTau2();

	// ô tô
	model_oto_cuoi = RotateY(180) * Translate(0, -3.5, 0) * RotateY(90) * Scale(20, 20, 20);
	oto2();

	model_oto_cuoi = RotateY(180) * Translate(0, -3.5, 20) * RotateY(90) * Scale(20, 20, 20);
	oto2();

	model_oto_cuoi = RotateY(180) * Translate(-20, -3.5, 20) * RotateY(90) * Scale(20, 20, 20);
	oto2();

	model_oto_cuoi = RotateY(90) * Translate(0, -3.5, -10) * Scale(20, 20, 20);
	oto2();
}

// thông số của view
vec4 eye = vec4(70, -8, 40, 1.0);
vec4 at = vec4(70, -8, 35, 1.0);
vec4 up = vec4(0.0, 0.1, 0.0, 1.0);
float speed = 1;
float rotation_speed = DegreesToRadians * 3;
// thông số của projection
GLfloat l = -1.0, r = 1.0;
GLfloat bottom = -1.0, top = 1.0;
GLfloat zNear = 0.8, zFar = 100.0;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mat4 view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	mat4 projection = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);

	theGioi();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	vec4 direction = normalize(at - eye);
	vec4 right = normalize( cross(up, direction) );
	vec4 fly = normalize(vec4(0, 1, 0, 1));

	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case '`':
		// trời sáng
		light_diffuse = color4(1.0, 1.0, 1.0, 1.0);
		light_specular = color4(1.0, 1.0, 1.0, 1.0);
		glClearColor(1, 1, 1, 1.0);
		break;
	case'~':
		// trời tối
		light_diffuse = color4(0.2, 0.2, 0.2, 0);
		light_specular = color4(0.5, 0.5, 0.5, 0);
		glClearColor(0, 0, 0, 1.0);
		break;
	case 'q':
		at = at - eye;
		at = vec4(at.x * cos(-rotation_speed) - at.z * sin(-rotation_speed), at.y, at.x * sin(-rotation_speed) + at.z * cos(-rotation_speed), 1.0);
		at = at + eye;
		break;
	case 'e':
		at = at - eye;
		at = vec4(at.x * cos(rotation_speed) - at.z * sin(rotation_speed), at.y, at.x * sin(rotation_speed) + at.z * cos(rotation_speed), 1.0);
		at = at + eye;
		break;
	case 'w':
		eye = eye + direction * speed * 3;
		at = at + direction * speed * 3;
		break;
	case 'd':
		eye = eye - right * speed;
		at = at - right * speed;
		break;
	case 's':
		eye = eye - direction * speed * 3;
		at = at - direction * speed * 3;
		break;
	case 'a':
		eye = eye + right * speed;
		at = at + right * speed;
		break;
	case ' ':
		eye = eye + fly * speed;
		at = at + fly * speed;
		break;
	case 'c':
		eye = eye - fly * speed;
		at = at - fly * speed;
		break;
	case 'g':
		nguoi_ngoi = nguoi_ngoi + 5;
		if (nguoi_ngoi > 90)
			nguoi_ngoi = 90;
		break;
	case 'G':
		nguoi_ngoi = nguoi_ngoi - 5;
		if (nguoi_ngoi < 0)
			nguoi_ngoi = 0;
		break;
	case 'h':
		if (nguoi_ngoi > 0)
			break;
		quay_nguoi = 0;
		nguoi_tien = nguoi_tien + 0.1;
		break;
	case 'n':
		if (nguoi_ngoi > 0)
			break;
		quay_nguoi = 180;
		nguoi_tien = nguoi_tien - 0.1;
		break;
	case 'b':
		if (nguoi_ngoi > 0)
			break;
		quay_nguoi = 90;
		nguoi_phai = nguoi_phai - 0.1;
		break;
	case 'm':
		if (nguoi_ngoi > 0)
			break;
		quay_nguoi = -90;
		nguoi_phai = nguoi_phai + 0.1;
		break;
	case 'x':
		if(xa>-5)	xa -= 0.5;
	  	qb -= 5;
		if (qb < 0) qb += 360;
		break;
	case 'z':
		if(xa< 30)xa += 0.5;
		qb += 5;
		if (qb > 360) qb -= 360;
		break;
	case 'j':
		copxe = copxe + 0.005;
		if (copxe > 0.25) copxe = 0.25;
		glutPostRedisplay();
		break;
	case 'J':
		copxe = copxe - 0.005;
		if (copxe < 0.0) copxe = 0;
		glutPostRedisplay();
		break;
	case 'k':
		cua = cua - 1;
		if (cua < 90) cua = 90;
		glutPostRedisplay();
		break;
	case 'K':
		cua = cua + 1;
		if (cua > 180) cua = 180;
		glutPostRedisplay();
		break;
	case 'l':
		q = q + 0.01;
		if (q > 1 - 0.65 / 2) q = 1 - 0.65 / 2;
		glutPostRedisplay();
		break;
	case 'L':
		q = q - 0.01;
		if (q < -1 + 0.65 / 2) q = -1 + 0.65 / 2;
		glutPostRedisplay();
		break;
	default:
		break;
	case 't':
		if (qn > -90) qn -= 5;
		if (qn == -90) {
			if (xc1 > -0.05) xc1 -= 0.05;
			if (xc1 < -0.05) {
				if (zc1 < 0.3)	zc1 += 0.05;
				if (zc2 > -0.3) zc2 -= 0.05;
			}
		}
		break;
	case 'T':
		if (qn < 0) qn += 5;
		if (qn == 0) {
			if (zc1 > 0.01)	zc1 -= 0.05;
			if (zc2 < -0.01) zc2 += 0.05;
			if (xc1 < -0.01 && zc1 <= 0.01 && zc2 >= -0.01) xc1 += 0.05;
		}
		break;
	case 'u':
		vthg += 0.1;
		break;
	/*case 'U':
		vthg -= 0.1;
		break;*/
	case 'i':
		uTh += 0.02;
		vCuaT1 += 0.01;
		if (vCuaT1 >= 0.09) vCuaT1 -= 0.01;
		if (uTh >= 1.12)
		{
			uTh -= 0.02;
			vCuaT -= 0.01;
			vCuaT2 -= 0.01;
			if (vCuaT <= 0) vCuaT += 0.01;
			if (vCuaT2 <= 0) vCuaT2 += 0.01;
		}
		else
		{
			vCuaT += 0.01;
			if (vCuaT >= 0.09) vCuaT -= 0.01;
		}
		break;
	case 'I':
		uTh -= 0.02;
		vCuaT2 += 0.01;
		if (vCuaT2 >= 0.09) vCuaT2 -= 0.01;
		if (uTh <= 0)
		{
			uTh += 0.02;
			vCuaT -= 0.01;
			vCuaT1 -= 0.01;
			if (vCuaT <= 0) vCuaT += 0.01;
			if (vCuaT1 <= 0) vCuaT1 += 0.01;;
		}
		else
		{
			vCuaT += 0.01;
			if (vCuaT >= 0.09) vCuaT -= 0.01;
		}
		break;
	}
	glutPostRedisplay();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(300, 100);
	glutCreateWindow( "Mo Hinh Ga Tau Dien" );

	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}
