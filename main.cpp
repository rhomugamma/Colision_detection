#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

const float PI = 3.14159265358979323;

GLuint VAO1, VAO2;

GLuint shaderProgram;

float radius1 = 0.01;
const int iterations1 = 20;
float beta1 = 360.0f / iterations1 * (2 * PI / 360.0f);
float alpha1 = beta1;
GLfloat vertices1[6 * iterations1];													
GLfloat colors1[9 * iterations1];

float deltatime1 = 0.0;
float frametime1 = 0.0;

float ObjectPositionX1 = -0.97;
float ObjectPositionY1 = -0.97;
float ObjectPositionZ1 = 0.0;

float ObjectVelocityX1 = 0.7;
float ObjectVelocityY1 = 0.5;
float ObjectVelocityZ1 = 0.0;

float ObjectAccelerationX1 =  0.5;
float ObjectAccelerationY1 = -0.4905;


float radius2 = 0.01;
const int iterations2 = 20;
float beta2 = 360.0f / iterations2 * (2 * PI / 360.0f);
float alpha2 = beta2;
GLfloat vertices2[6 * iterations2];													
GLfloat colors2[9 * iterations2];

float deltatime2 = 0.0;
float frametime2 = 0.0;

float ObjectPositionX2 =  0.97;
float ObjectPositionY2 = -0.97;
float ObjectPositionZ2 =  0.0;

float ObjectVelocityX2 = -0.7;
float ObjectVelocityY2 =  0.5;
float ObjectVelocityZ2 =  0.0;

float ObjectAccelerationX2 = -0.5;
float ObjectAccelerationY2 = -0.4905;

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


void init();

void display();

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void rendersphere1(float radius1, int iterations1);

void rendersphere2(float radius2, int iterations2);

void updateobjectposition1();

void updateobjectposition2();

bool colisionDetection(float ObjectPositionX1, float ObjectPositionY1, float radius1, float ObjectPositionX2, float ObjectPositionY2, float radius2);


int main() {

    if (!glfwInit()) {

        std::cout << "GLFW initialization failed!" << std::endl;
        return -1;
    
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dynamic Window", nullptr, nullptr);

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

    init();

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
		updateobjectposition1();
		updateobjectposition2();
        display();
        glfwSwapBuffers(window);
    
	}

    glDeleteVertexArrays(1, &VAO1);
	glDeleteVertexArrays(1, &VAO2);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

void init() {

	rendersphere1(radius1, iterations1);
	rendersphere2(radius2, iterations2);

   }

void display() {

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO1);

    glUseProgram(shaderProgram);

	GLuint vertexVBO1;
	glGenBuffers(1, &vertexVBO1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO1);

    glDrawArrays(GL_TRIANGLES, 0, 3 * iterations1);


	glBindVertexArray(VAO2);

	GLuint vertexVBO2;
	glGenBuffers(1, &vertexVBO2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VAO2);

	glDrawArrays(GL_TRIANGLES, 0, 3 * iterations2);

	glBindVertexArray(0);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}

void rendersphere1(float radius1, int iterations1) {

	glGenVertexArrays(1, &VAO1);														
    glBindVertexArray(VAO1);

    for (int i = 0; i < 6 * iterations1; i++) {
        
		vertices1[i] = 0;
        vertices1[i + 1] = 0;

        vertices1[i + 2] = radius1 * cos(beta1);
        vertices1[i + 3] = radius1 * sin(beta1);

        vertices1[i + 4] = radius1 * cos(beta1 + alpha1);
        vertices1[i + 5] = radius1 * sin(beta1 + alpha1);

        i = i + 5;
        beta1 = beta1 + alpha1;
    
	};

    for (int i = 0; i < 9 * iterations1; i += 3) {

        colors1[i] = 1.0;
        colors1[i + 1] = 0.0;
        colors1[i + 2] = 0.0;
    
	}

    GLuint vertexVBO1;															
    glGenBuffers(1, &vertexVBO1);													
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO1);										
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    GLuint colorVBO1;
    glGenBuffers(1, &colorVBO1);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 
}

void rendersphere2(float radius2, int iterations2) {

	glGenVertexArrays(1, &VAO2);														
    glBindVertexArray(VAO2);

    for (int i = 0; i < 6 * iterations2; i++) {
        
		vertices2[i] = 0;
        vertices2[i + 1] = 0;

        vertices2[i + 2] = radius2 * cos(beta2);
        vertices2[i + 3] = radius2 * sin(beta2);

        vertices2[i + 4] = radius2 * cos(beta2 + alpha2);
        vertices2[i + 5] = radius2 * sin(beta2 + alpha2);

        i = i + 5;
        beta2 = beta2 + alpha2;
    
	};

    for (int i = 0; i < 9 * iterations2; i += 3) {

        colors2[i] = 0.0;
        colors2[i + 1] = 0.0;
        colors2[i + 2] = 1.0;
    
	}

    GLuint vertexVBO2;															
    glGenBuffers(1, &vertexVBO2);													
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO2);										
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    GLuint colorVBO2;
    glGenBuffers(1, &colorVBO2);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors2), colors2, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 
}

void updateobjectposition1() {

	GLfloat totalTime1 = glfwGetTime();
	deltatime1 = totalTime1 - frametime1;
	frametime1 = totalTime1;

	ObjectPositionX1 = (ObjectPositionX1) + (ObjectVelocityX1 * deltatime1) + ((ObjectAccelerationX1 * deltatime1 * deltatime1)/2);
	ObjectPositionY1 = (ObjectPositionY1) + (ObjectVelocityY1 * deltatime1) + ((ObjectAccelerationY1 * deltatime1 * deltatime1)/2);

	ObjectVelocityX1 = (ObjectVelocityX1) + (ObjectAccelerationX1 * deltatime1);
	ObjectVelocityY1 = (ObjectVelocityY1) + (ObjectAccelerationY1 * deltatime1);

	if (ObjectPositionX1 + radius1 >= 1.0f || ObjectPositionX1 - radius1 <= -1.0f) {
   
   		ObjectVelocityX1 = -ObjectVelocityX1;
   
   	}

	else if (ObjectPositionY1 + radius1 >= 1.0f || ObjectPositionY1 - radius1 <= -1.0f) {
       
	   	ObjectVelocityY1 = -ObjectVelocityY1;
    
	}

	for(int i = 0; i < 6*iterations1; i++) {

		vertices1[i] = 0 + ObjectPositionX1;
        vertices1[i + 1] = 0 + ObjectPositionY1;

        vertices1[i + 2] = radius1 * cos(beta1) + ObjectPositionX1;
        vertices1[i + 3] = radius1 * sin(beta1) + ObjectPositionY1;

        vertices1[i + 4] = radius1 * cos(beta1 + alpha1) + ObjectPositionX1;
        vertices1[i + 5] = radius1 * sin(beta1 + alpha1) + ObjectPositionY1;

        i = i + 5;

        beta1 = beta1 + alpha1;
	
	}

	bool colision = colisionDetection(ObjectPositionX1, ObjectPositionY1, radius1, ObjectPositionX2, ObjectPositionY2, radius2);

	if (colision) {

		ObjectVelocityX1 = -ObjectVelocityX1;
		ObjectVelocityY1 = -ObjectVelocityY1;

		ObjectVelocityX2 = -ObjectVelocityX2;
		ObjectVelocityY2 = -ObjectVelocityY2;

	}	
}

void updateobjectposition2() {

	GLfloat totalTime2 = glfwGetTime();
	deltatime2 = totalTime2 - frametime2;
	frametime2 = totalTime2;

	ObjectPositionX2 = (ObjectPositionX2) + (ObjectVelocityX2 * deltatime2) + ((ObjectAccelerationX2 * deltatime2 * deltatime2)/2);
	ObjectPositionY2 = (ObjectPositionY2) + (ObjectVelocityY2 * deltatime2) + ((ObjectAccelerationY2 * deltatime2 * deltatime2)/2);

	ObjectVelocityX2 = (ObjectVelocityX2) + (ObjectAccelerationX2 * deltatime2);
	ObjectVelocityY2 = (ObjectVelocityY2) + (ObjectAccelerationY2 * deltatime2);


	if (ObjectPositionX1 + radius1 >= 1.0f || ObjectPositionX1 - radius1 <= -1.0f) {
   
   		ObjectVelocityX1 = -ObjectVelocityX1;
   
   	}

	else if (ObjectPositionY1 + radius1 >= 1.0f || ObjectPositionY1 - radius1 <= -1.0f) {
       
	   	ObjectVelocityY1 = -ObjectVelocityY1;
    
	}

	for(int i = 0; i < 6*iterations2; i++) {

		vertices2[i] = 0 + ObjectPositionX2;
        vertices2[i + 1] = 0 + ObjectPositionY2;

        vertices2[i + 2] = radius2 * cos(beta2) + ObjectPositionX2;
        vertices2[i + 3] = radius2 * sin(beta2) + ObjectPositionY2;

        vertices2[i + 4] = radius2 * cos(beta2 + alpha2) + ObjectPositionX2;
        vertices2[i + 5] = radius2 * sin(beta2 + alpha2) + ObjectPositionY2;

        i = i + 5;

        beta2 = beta2 + alpha2;
	
	}

	bool colision = colisionDetection(ObjectPositionX1, ObjectPositionY1, radius1, ObjectPositionX2, ObjectPositionY2, radius2);

	if (colision) {

		ObjectVelocityX1 = -ObjectVelocityX1;
		ObjectVelocityY1 = -ObjectVelocityY1;

		ObjectVelocityX2 = -ObjectVelocityX2;
		ObjectVelocityY2 = -ObjectVelocityY2;

	}
	
}

bool colisionDetection(float ObjectPositionX1, float ObjectPositionY1, float radius1, float ObjectPositionX2, float ObjectPositionY2, float radius2) {

	float dx = ObjectPositionX2 - ObjectPositionX1;
	float dy = ObjectPositionY2 - ObjectPositionY1;
	float distance = sqrt(pow(dx, 2) + pow(dy, 2));

	float limit = radius1 + radius2;

	if (distance <= limit) {

		return true;

	}

	else {

		return false;

	}


}
