#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <vector>
#include <deque>

#include <ctime>
#include <ratio>
#include <chrono>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 512
#define MAP_WIDTH 24
#define MAP_HEIGHT 24

#define PI 3.1415926535897932384626433832795f
#define RADPI 57.295779513082320876798154814105f
#define DEGPI 0.01745329251994329576923690768489f
#define DEG2RAD( x ) ( ( float )( x ) * DEGPI )
#define RAD2DEG( x ) ( ( float )( x ) * RADPI )

// 1 = Walls
// 2 = Player
std::vector<float> FovPoly;
static float playerDirection = 45.f;
static float playerPos[2] = { 11.5f, 11.5f };
static float sind = 0.f;
static float cosd = 0.f;
static float playerFov = 90.f / 2.f;
static bool pressedKeys[GLFW_KEY_LAST];

static float HalfWidth = SCREEN_WIDTH / 2.f;
static float HalfHeight = SCREEN_HEIGHT / 2.f;
static float Mult = SCREEN_HEIGHT / MAP_HEIGHT;
static float hMult = SCREEN_HEIGHT / MAP_HEIGHT / 2.f;
static float qMult = SCREEN_HEIGHT / MAP_HEIGHT / 4.f;

static std::chrono::high_resolution_clock::time_point PreviousTime;
static std::chrono::duration<double> FrameTime;

// Y first X second
static int WorldMap[MAP_HEIGHT][MAP_WIDTH] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
};

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);

void DrawBox(int X, int Y, int Width, int Height, float* color) {
	float vertices[] =
	{
		X, Y,
		X + Width, Y,
		X + Width, Y + Height,
		X, Y + Height
	};

	if (color) {
		glColor3f(color[0], color[1], color[2]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.f, 1.f, 1.f);
	}
	else {
		glColor3f(1.f, 1.f, 1.f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void DrawBox2(int X, int Y, int Width, int Height, float* color) {
	float HWidth = Width / 2.f, HHeight = Height / 2.f;
	float vertices[] =
	{
		X - HWidth, Y - HHeight,
		X + HWidth, Y - HHeight,
		X + HWidth, Y + HHeight,
		X - HWidth, Y + HHeight,
	};

	if (color) {
		glColor3f(color[0], color[1], color[2]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.f, 1.f, 1.f);
	}
	else {
		glColor3f(1.f, 1.f, 1.f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_QUADS, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void DrawBoxOutline(int X, int Y, int Width, int Height, float* color) {
	float vertices[] =
	{
		X + 1, Y,
		X + Width, Y,
		X + Width, Y + Height - 1,
		X, Y + Height
	};

	if (color) {
		glColor3f(color[0], color[1], color[2]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.f, 1.f, 1.f);
	}
	else {
		glColor3f(1.f, 1.f, 1.f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, vertices);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void DrawLine(int X1, int Y1, int X2, int Y2, float* color) {
	float lineVertices[] =
	{
		X1, Y1,
		X2, Y2
	};

	if (color) {
		glColor3f(color[0], color[1], color[2]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lineVertices);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.f, 1.f, 1.f);
	}
	else {
		glColor3f(1.f, 1.f, 1.f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lineVertices);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void DrawCross(int X, int Y, int Leng, float* color) {
	int Length = Leng * 0.5f;

	float lineVertices[] =
	{
		X + Length, Y + Length,
		X - Length, Y - Length
	};

	float lineVertices2[] =
	{
		X + Length, Y - Length,
		X - Length, Y + Length
	};

	if (color) {
		glColor3f(color[0], color[1], color[2]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lineVertices);
		glDrawArrays(GL_LINES, 0, 2);
		glVertexPointer(2, GL_FLOAT, 0, lineVertices2);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.f, 1.f, 1.f);
	}
	else {
		glColor3f(1.f, 1.f, 1.f);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, lineVertices);
		glDrawArrays(GL_LINES, 0, 2);
		glVertexPointer(2, GL_FLOAT, 0, lineVertices2);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

float Distance(float Point1X, float Point1Y, float Point2X, float Point2Y) {
	float DeltaX = Point2X - Point1X;
	float DeltaY = Point2Y - Point1Y;
	return std::sqrtf(DeltaX * DeltaX + DeltaY * DeltaY);
}

void RayCast() {
	float CrossR[] = { 1.f, 0.f, 0.f };
	float CrossG[] = { 0.f, 1.f, 0.f };

	FovPoly.clear();
	FovPoly.push_back(HalfWidth + playerPos[0] * Mult);
	FovPoly.push_back(playerPos[1] * Mult);

	float HalfScreen = SCREEN_WIDTH * 0.25f;
	float DeltaX = 1.f - (playerPos[0] - std::floorf(playerPos[0]));
	float DeltaY = 1.f - (playerPos[1] - std::floorf(playerPos[1]));
	for (float W = -HalfScreen; W < HalfScreen; W++) {
		float playerDirectionFOV = playerDirection + W / HalfScreen * playerFov;
		float TanDir = std::tanf(DEG2RAD(playerDirectionFOV - 90.f));
		bool FlipX = false, FlipY = false;

		if ((playerDirectionFOV < 0.f && playerDirectionFOV > -180.f) ||
			(playerDirectionFOV > 180.f && playerDirectionFOV < 360.f)) {
			FlipX = true;
		}

		if ((playerDirectionFOV > 90.f && playerDirectionFOV < 270.f) ||
			(playerDirectionFOV < -90.f && playerDirectionFOV > -270.f)) {
			FlipY = true;
		}

		float HitX = -1.f, HitY = -1.f, HitX2 = -1.f, HitY2 = -1.f;

		if (FlipX) {
			for (int i = 1; i < 65; i++) {
				float ArrayX = (playerPos[0] + (DeltaX - i));
				float ArrayY = (playerPos[1] + (DeltaX - i) * -TanDir);
				if (!(ArrayX < 0.f || ArrayY < 0.f || ArrayX > MAP_WIDTH || ArrayY > MAP_HEIGHT)) {
					if (WorldMap[int(ArrayY)][int(ArrayX)] || WorldMap[int(ArrayY)][int(ArrayX - 1.f)]) {
						HitX = ArrayX;
						HitY = ArrayY;
						break;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 64; i++) {
				float ArrayX = (playerPos[0] + (DeltaX + i));
				float ArrayY = (playerPos[1] + (DeltaX + i) * -TanDir);
				if (!(ArrayX < 0.f || ArrayY < 0.f || ArrayX > MAP_WIDTH || ArrayY > MAP_HEIGHT)) {
					if (WorldMap[int(ArrayY)][int(ArrayX)] || WorldMap[int(ArrayY)][int(ArrayX - 1.f)]) {
						HitX = ArrayX;
						HitY = ArrayY;
						break;
					}
				}
			}
		}

		TanDir = std::tanf(DEG2RAD(playerDirectionFOV));
		if (FlipY) {
			for (int i = 1; i < 65; i++) {
				float ArrayX = (playerPos[0] + (DeltaY - i) * TanDir);
				float ArrayY = (playerPos[1] + DeltaY - i);
				if (!(ArrayX < 0.f || ArrayY < 0.f || ArrayX > MAP_WIDTH || ArrayY > MAP_HEIGHT)) {
					if (WorldMap[int(ArrayY)][int(ArrayX)] || WorldMap[int(ArrayY - 1.f)][int(ArrayX)]) {
						HitX2 = ArrayX;
						HitY2 = ArrayY;
						break;
					}
				}
			}
		}
		else {
			for (int i = 0; i < 64; i++) {
				float ArrayX = (playerPos[0] + (DeltaY + i) * TanDir);
				float ArrayY = (playerPos[1] + DeltaY + i);
				if (!(ArrayX < 0.f || ArrayY < 0.f || ArrayX > MAP_WIDTH || ArrayY > MAP_HEIGHT)) {
					if (WorldMap[int(ArrayY)][int(ArrayX)] || WorldMap[int(ArrayY - 1.f)][int(ArrayX)]) {
						HitX2 = ArrayX;
						HitY2 = ArrayY;
						break;
					}
				}
			}
		}

		if ((HitX > 0.f && HitY > 0.f) || (HitX2 > 0.f && HitY2 > 0.f)) {
			float RHitX = HitX, RHitY = HitY;
			if (HitX < 0.f || HitY < 0.f || HitX2 < 0.f || HitY2 < 0.f) {
				RHitX = std::fmaxf(HitX, HitX2);
				RHitY = std::fmaxf(HitY, HitY2);
			}
			else {
				if (Distance(playerPos[0], playerPos[1], HitX2, HitY2) < Distance(playerPos[0], playerPos[1], HitX, HitY)) {
					RHitX = HitX2;
					RHitY = HitY2;
				}
			}

			FovPoly.push_back(HalfWidth + RHitX * Mult);
			FovPoly.push_back(RHitY * Mult);

			DrawBox2(HalfWidth + RHitX * Mult, RHitY * Mult, 10.f, 10.f, CrossG);

			float DeltaX = RHitX - playerPos[0], DeltaY = RHitY - playerPos[1];
			float Distance2 = DeltaX * std::cosf(DEG2RAD(playerDirection - 90.f)) + DeltaY * std::sinf(DEG2RAD(playerDirection + 90.f));
			if (Distance2 > 0.f) {
				float Colors = 4.f / Distance2;
				float Color[] = { Colors, Colors, Colors };
				DrawBox2(W + HalfScreen, HalfHeight, 1.f, SCREEN_HEIGHT / Distance2, Color);
			}
		}
		else {
			float D2GPD = DEG2RAD(playerDirectionFOV);
			FovPoly.push_back(HalfWidth + playerPos[0] * Mult + std::sinf(D2GPD) * 1024.f);
			FovPoly.push_back(playerPos[1] * Mult + std::cosf(D2GPD) * 1024.f);
		}
	}
}

#include <Windows.h>
const char* NumberToHex() {
	switch (rand() % 0x10) {
	case 0x0:
		return "0";
	case 0x1:
		return "1";
	case 0x2:
		return "2";
	case 0x3:
		return "3";
	case 0x4:
		return "4";
	case 0x5:
		return "5";
	case 0x6:
		return "6";
	case 0x7:
		return "7";
	case 0x8:
		return "8";
	case 0x9:
		return "9";
	case 0xA:
		return "A";
	case 0xB:
		return "B";
	case 0xC:
		return "C";
	case 0xD:
		return "D";
	case 0xE:
		return "E";
	case 0xF:
		return "F";
	}
	return "F";
}

void MakeNewJunkFile() {
	std::ofstream File(".\\JunkAsm.txt", std::ofstream::binary);
	std::string JunkCode = "#define JUNKS \\";

	srand((UINT)time(NULL));
	for (INT i = 0x0; i < 0x3E8; i++) {
		JunkCode.append("\n__asm _emit 0x");
		JunkCode.append(NumberToHex());
		JunkCode.append(NumberToHex());
		JunkCode.append(" \\");
	}

	JunkCode.erase(JunkCode.end() - 0x2, JunkCode.end());
	File.write(JunkCode.c_str(), JunkCode.size());
	File.close();
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);

	/* Keyboard interaction */
	glfwSetKeyCallback(window, keyCallBack);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Set Viewport etc. */
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	FovPoly.reserve(SCREEN_WIDTH * 2 + 128);
	std::reverse(WorldMap, WorldMap + MAP_WIDTH);

	float gridColor[] = { 0.2f, 0.2f, 0.2f };
	float playerColor[] = { 1.f, 0.f, 0.f };
	float bgUpColor[] = { 0.4f, 0.4f, 0.4f };
	float bgDownColor[] = { 0.15f, 0.15f, 0.15f };

	MakeNewJunkFile();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window) && !pressedKeys[GLFW_KEY_ESCAPE])
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		FrameTime = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - PreviousTime);
		PreviousTime = std::chrono::high_resolution_clock::now();
		float FT = FrameTime.count();

		float PlayerX = HalfWidth + playerPos[0] * Mult;
		float PlayerY = playerPos[1] * Mult;

		for (int Y = 0; Y < MAP_HEIGHT; Y++) {
			for (int X = 0; X < MAP_WIDTH; X++) {
				switch (WorldMap[Y][X]) {
				case 1: { // Walls
					DrawBox(HalfWidth + (X * Mult), Y * Mult, Mult, Mult, nullptr);
					break;
				}
				}
				DrawBoxOutline(HalfWidth + (X * Mult), Y * Mult, Mult, Mult, gridColor);
			}
		}

		if (FovPoly.size() > 0) {
			glColor3f(0.5f, 0.5f, 0.5f);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, &FovPoly[0]);
			glDrawArrays(GL_POLYGON, 0, FovPoly.size() * 0.5f);
			glDisableClientState(GL_VERTEX_ARRAY);
		}

		float D2RPD = DEG2RAD(playerDirection);
		sind = std::sinf(D2RPD);
		cosd = std::cosf(D2RPD);

		if (pressedKeys[GLFW_KEY_W]) {
			playerPos[0] -= -sind * FT * 2.f;
			playerPos[1] += cosd * FT * 2.f;
		}
		if (pressedKeys[GLFW_KEY_S]) {
			playerPos[0] += -sind * FT * 2.f;
			playerPos[1] -= cosd * FT * 2.f;
		}

		if (pressedKeys[GLFW_KEY_A]) {
			playerDirection -= FT * 50.f;
		}
		if (pressedKeys[GLFW_KEY_D]) {
			playerDirection += FT * 50.f;
		}

		playerPos[0] = std::clamp(playerPos[0], 0.f, float(MAP_WIDTH));
		playerPos[1] = std::clamp(playerPos[1], 0.f, float(MAP_HEIGHT));
		playerDirection = std::fmodf(playerDirection, 360.f);

		DrawBox2(PlayerX, PlayerY, hMult + 2.f, hMult + 1.f, playerColor);

		D2RPD = DEG2RAD(playerDirection - playerFov);
		sind = std::sinf(D2RPD);
		cosd = std::cosf(D2RPD);
		DrawLine(PlayerX, PlayerY, PlayerX + sind * 1024.f, PlayerY + cosd * 1024.f, nullptr);

		D2RPD = DEG2RAD(playerDirection + playerFov);
		sind = std::sinf(D2RPD);
		cosd = std::cosf(D2RPD);
		DrawLine(PlayerX, PlayerY, PlayerX + sind * 1024.f, PlayerY + cosd * 1024.f, nullptr);

		DrawBox(0.f, HalfHeight, HalfWidth, HalfHeight, bgUpColor);
		DrawBox(0.f, 0.f, HalfWidth, HalfHeight, bgDownColor);
		RayCast();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	printf("Key:%i Scan:%i Action:%i Mod:%i FPS:%f PD:%f\n", key, scancode, action, mods, 1.0 / FrameTime.count(), playerDirection);

	switch (action) {
	case GLFW_PRESS: {
		pressedKeys[key] = true;
		break;
	}
	case GLFW_RELEASE: {
		pressedKeys[key] = false;
		break;
	}
	}
}

/*
std::string readShaderCode(const char* filename) {
	std::ifstream meInput(filename);
	if (!meInput.good()) {
		fprintf(stderr, "Error reading %s", filename);
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(meInput), std::istreambuf_iterator<char>());
}

GLuint create_quad_vao() {
	GLuint vao = 0, vbo = 0;
	float verts[] = { -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), verts, GL_STATIC_DRAW);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	GLintptr stride = 4 * sizeof(float);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(1);
	GLintptr offset = 2 * sizeof(float);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)offset);
	return vao;
}

GLuint create_quad_program() {
	GLuint program = glCreateProgram();
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string Temp = readShaderCode("GLSL\\vert_shader_str.glsl");
	const char* Shader = Temp.c_str();
	glShaderSource(vert_shader, 1, &Shader, NULL);
	glCompileShader(vert_shader);
	glAttachShader(program, vert_shader);
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string Temp2 = readShaderCode("GLSL\\frag_shader_str.glsl");
	const char* Shader2 = Temp2.c_str();
	glShaderSource(frag_shader, 1, &Shader2, NULL);
	glCompileShader(frag_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);
	return program;
}

int main(void)
{
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
		return -1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	GLuint quad_vao = create_quad_vao();
	GLuint quad_program = create_quad_program();

	GLuint ray_program = 0;
	// create the compute shader
	GLuint ray_shader = glCreateShader(GL_COMPUTE_SHADER);
	std::string Temp = readShaderCode("GLSL\\compute_shader_str.glsl");
	const char* Shader = Temp.c_str();
	glShaderSource(ray_shader, 1, &Shader, NULL);
	glCompileShader(ray_shader);
	ray_program = glCreateProgram();
	glAttachShader(ray_program, ray_shader);
	glLinkProgram(ray_program);

	// texture handle and dimensions
	GLuint tex_output = 0;
	int tex_w = 1024, tex_h = 1024;
	// create the texture
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// linear allows us to scale the window up retaining reasonable quality
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// same internal format as compute shader input
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT,
		NULL);
	// bind to image unit so can write to specific pixels from the shader
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// query up the workgroups
	int work_grp_size[3], work_grp_inv;
	// maximum global work group (total work in a dispatch)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_size[2]);
	printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_size[0],
		work_grp_size[1], work_grp_size[2]);
	// maximum local work group (one shader's slice)
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);
	// maximum compute shader invocations (x * y * z)
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max computer shader invocations %i\n", work_grp_inv);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		glUseProgram(ray_program);
		glDispatchCompute(tex_w, tex_h, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(quad_program);
		glBindVertexArray(quad_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_output);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
*/