#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>

const float PI = 3.14159265358979323;

const float g = 9.80665;

GLuint VAO1, VAO2, VAO3;

GLuint shaderProgram;

const GLchar* vertexShaderSource = R"(

    #version 450 core

    layout (location = 0) in vec3 position;
	layout (location = 1) in vec3 color;

	out vec3 vertexColor;

    void main() {

		vertexColor = color;
        gl_Position = vec4(position.x, position.y, 0.0, 1.0);

    }

)";

const GLchar* fragmentShaderSource = R"(

    #version 450 core

	in vec3 vertexColor;
	out vec4 fragColor;

    void main() {

        fragColor = vec4(vertexColor, 1.0f);

    }

)";

class object {

	public:

		//Physical properties
		float radius;
		float mass;
		float weight = (mass) * (g);

		//Rendering properties
		int iterations;
		float alpha = 360 / 20 * (2 * PI / 360);
		float beta = alpha;
		/* const static int verticesArraySize = 6 * iterations; */
		/* const static int colorsArraySize = 9 * iterations; */
		GLfloat vertices[120];													
		GLfloat colors[180];

		//Goberning characteristics
		float coordinatesX;
		float coordinatesY;

		float velocityX;
		float velocityY;
		float velocityModule = sqrt((velocityX * velocityX) + (velocityY * velocityY));

		float accelerationX;
		float accelerationY;
		float accelerationModule = sqrt((accelerationX * accelerationX) + (accelerationY * accelerationY));

		float angularVelocityZ;
		float angularVelocityModule = sqrt(angularVelocityZ * angularVelocityZ);

		float angularAccelerationZ;
		float angularAcceleration = sqrt(angularAccelerationZ * angularAccelerationZ);

		float linearMomentumX = (mass) * (velocityX);
	   	float linearMomentumY = (mass) * (velocityY);
		float linearMomentumModule = sqrt((linearMomentumX * linearMomentumX) + (linearMomentumY * linearMomentumY));
 		float momentInertia = (PI * (radius * radius * radius * radius)) / (4);

		float kinetickEnergy = ((1/2) + (mass) + (velocityModule * velocityModule)) + ((1/2) + (momentInertia) + (angularVelocityZ * angularVelocityZ)); 
		float potentialEnergy = (mass) * (g) * (coordinatesY + 0.5);

		//Time characteristics
		float deltaTime;
		float frameTime;

};

class box {

	public:

		GLfloat verticesbox[16] {
		
			-0.99, -0.99,
			 0.99, -0.99,

			 0.99, -0.99,
			 0.99,  0.99,

			 0.99, 0.99,
			-0.99, 0.99,

			-0.99, 0.99,
			-0.99, -0.99
		};

		GLfloat colorbox[24] = {

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

		};

};

std::vector<object> init(std::vector<object> objects, box box1);

void display(int iterations, GLfloat vertices[], GLfloat verticesbox[]);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void rendersphere(int iterations, float radius, GLfloat vertices[], GLfloat colors[], float alpha, float beta);

void renderbox(GLfloat verticesbox[], GLfloat colorsbox[]);

void updateobjectposition(float radius, float mass, int iterations, float alpha, float beta, GLfloat vertices[],float coordinatesX, float coordinatesY, float velocityX, float velocityY, float accelerationX, float accelerationY, float deltaTime, float totalTime, GLfloat verticesbox[]);

bool colisionDetection(float ObjectPositionX1, float ObjectPositionY1, float radius1, float ObjectPositionX2, float ObjectPositionY2, float radius2);

bool colisionBorderDetection(float ObjectPositionX, float ObjectPositionY, float radius, GLfloat verticesbox[]);


int main() {

	std::vector<object> objects;

	box box1;

    if (!glfwInit()) {

        std::cout << "GLFW initialization failed!" << std::endl;
        return -1;
    
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(3840, 2160, "Dynamic Window", nullptr, nullptr);

    if (!window) {

        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;

    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
		 
        std::cout << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    
	}

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    objects = init(objects, box1);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

		for (int i = 0; i < 2; i++) {
	
			updateobjectposition(objects[i].radius, objects[i].mass, objects[i].iterations, objects[i].alpha, objects[i].beta, objects[i].vertices, objects[i].coordinatesX, objects[i].coordinatesY, objects[i].velocityX, objects[i].velocityY, objects[i].accelerationX, objects[i].accelerationY, objects[i].deltaTime, objects[i].frameTime, box1.verticesbox);
			display(objects[i].iterations, objects[i].vertices, box1.verticesbox);

		}

        glfwSwapBuffers(window);
    
	}

    glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

std::vector<object> init(std::vector<object> objects, box box1) {

	
	objects.push_back(object());

	objects[0].radius = 0.01;
	objects[0].mass = 5;

	objects[0].coordinatesX = 0.0;
	objects[0].coordinatesY = 0.0;

	objects[0].velocityX = 0.0;
	objects[0].velocityY = 0.0;

	objects[0].accelerationX = 0.0;
	objects[0].accelerationY = 0.0;

	objects[0].angularVelocityZ = 0.0;

	objects[0].angularAccelerationZ = 0.0;

	objects[0].deltaTime = 0.0;
	objects[0].frameTime = 0.0;


	objects.push_back(object());

	objects[1].radius = 0.01;
	objects[1].mass = 5;

	objects[1].coordinatesX = 0.0;
	objects[1].coordinatesY = 0.0;

	objects[1].velocityX = 0.0;
	objects[1].velocityY = 0.0;

	objects[1].accelerationX = 0.0;
	objects[1].accelerationY = 0.0;

	objects[1].angularVelocityZ = 0.0;

	objects[1].angularAccelerationZ = 0.0;

	objects[1].deltaTime = 0.0;
	objects[1].frameTime = 0.0;

	for (int i = 0; i < 2; i++) {

		rendersphere(objects[i].iterations, objects[i].radius, objects[i].vertices, objects[i].colors, objects[i].alpha, objects[i].beta);

	}


	renderbox(box1.verticesbox, box1.colorbox);

	return objects;

}

void display(int iterations, GLfloat vertices, GLfloat verticesbox) {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    glUseProgram(shaderProgram);

	glBindVertexArray(VAO1);
	GLuint vertexVBO1;
	glGenBuffers(1, &vertexVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3 * iterations);
	glBindVertexArray(0);


	glBindVertexArray(VAO3);
	GLuint vertexVBO3;
	glGenBuffers(1, &vertexVBO3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesbox), verticesbox, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, 4);
	glDrawArrays(GL_LINES, 4, 4);
	glDrawArrays(GL_LINES, 8, 4);
	glDrawArrays(GL_LINES, 12, 4);


	glBindVertexArray(0);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}

void rendersphere(int iterations, float radius, GLfloat vertices[], GLfloat colors[], float alpha, float beta) {

	glGenVertexArrays(1, &VAO1);														
    glBindVertexArray(VAO1);

    for (int i = 0; i < 6 * iterations; i++) {
        
		vertices[i] = 0;
        vertices[i + 1] = 0;

        vertices[i + 2] = radius * cos(alpha);
        vertices[i + 3] = radius * sin(alpha);

        vertices[i + 4] = radius * cos(alpha + beta);
        vertices[i + 5] = radius * sin(alpha + beta);

        i = i + 5;

		alpha = alpha + beta;
    
	};

    for (int i = 0; i < 9 * iterations; i += 3) {

        colors[i] = 1.0;
        colors[i + 1] = 0.0;
        colors[i + 2] = 0.0;
    
	}

    GLuint vertexVBO1;															
    glGenBuffers(1, &vertexVBO1);													
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO1);										
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    GLuint colorVBO1;
    glGenBuffers(1, &colorVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 
}


void renderbox(GLfloat verticesbox[], GLfloat colorbox[]) {

	glGenVertexArrays(1, &VAO3);
	glBindVertexArray(VAO3);

	GLuint vertexVBO3;
	glGenBuffers(1, &vertexVBO3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesbox[]), verticesbox[], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);	

	GLuint colorVBO3;
	glGenBuffers(1, &colorVBO3);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorbox), colorbox, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void updateobjectposition(float radius, float mass, int iterations, float alpha, float beta, GLfloat vertices[],float coordinatesX, float coordinatesY, float velocityX, float velocityY, float accelerationX, float accelerationY, float deltaTime, float frameTime, GLfloat verticesbox[]) {

	GLfloat totalTime = glfwGetTime();
	deltaTime = totalTime - frameTime;
	frameTime = totalTime;

	coordinatesX = (coordinatesX) + (velocityX * deltaTime) + ((accelerationX * deltaTime * deltaTime)/2);
	coordinatesY = (coordinatesY) + (velocityY * deltaTime) + ((accelerationX * deltaTime * deltaTime)/2);

	velocityX = (velocityX) + (accelerationX * deltaTime);
	velocityY = (velocityY) + (accelerationY * deltaTime);


	for(int i = 0; i < 6*iterations; i++) {

		vertices[i] = 0 + coordinatesX;
        vertices[i + 1] = 0 + coordinatesY;

        vertices[i + 2] = radius * cos(alpha) + coordinatesX;
        vertices[i + 3] = radius * sin(alpha) + coordinatesY;

        vertices[i + 4] = radius * cos(alpha + beta) + coordinatesX;
        vertices[i + 5] = radius * sin(alpha + beta) + coordinatesY;

        i = i + 5;

        alpha = alpha + beta;
	
	}

	
	bool colisionBorder = colisionBorderDetection(coordinatesX, coordinatesY, radius, verticesbox);


	if (colisionBorder) {

		if (coordinatesX - radius < verticesbox[0] || coordinatesX + radius > verticesbox[2]) {

			coordinatesX = -coordinatesX;

		}

		else if (coordinatesY - radius < verticesbox[1] || coordinatesY + radius > verticesbox[7]) {

			coordinatesY = -coordinatesY;

		}

	}

}

bool colisionBorderDetection(float coordinatesX, float coordinatesY, float radius, GLfloat verticesbox[]) {

	if (coordinatesX + radius > verticesbox[2] || coordinatesX - radius < verticesbox[0]) {

		coordinatesX = -coordinatesX;
		return true;

	}	
	
	else if (coordinatesY + radius > verticesbox[7] || coordinatesY - radius < verticesbox[1]) {
    
		coordinatesY = -coordinatesY;	
		return true;
    
	} 
	
	else {

        return false;
    
	}

}

bool colisionDetection(float ObjectPositionX1, float ObjectPositionY1, float radius1, float ObjectPositionX2, float ObjectPositionY2, float radius2) {

	float dx = ObjectPositionX2 - ObjectPositionX1 - (radius2 - radius1);
	float dy = ObjectPositionY2 - ObjectPositionY1 - (radius2 - radius1);
	float distance = sqrt(pow(dx, 2) + pow(dy, 2));

	float limit = radius1 + radius2;

	if (distance < limit) {

		return true;

	}

	else {

		return false;

	}


}
