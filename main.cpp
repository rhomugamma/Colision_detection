#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <random>

const float PI = 3.14159265358979323;

const float g = 9.80665;

GLuint shaderProgram;

class box {

	public:

		GLfloat verticesbox[16] = {

			-0.99, -0.99,
			 0.99, -0.99,

			 0.99, -0.99,
			 0.99,  0.99,

			 0.99, 0.99,
			-0.99, 0.99,

			-0.99, 0.99,
			-0.99, -0.99

		};


		GLfloat colorbox[24] {

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
	
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,

		};

		GLuint VAO;

		void renderBoxObject() {

	        glGenVertexArrays(1, &VAO);
    	    glBindVertexArray(VAO);

	        GLuint vertexVBO;
        	glGenBuffers(1, &vertexVBO);
        	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesbox), verticesbox, GL_STATIC_DRAW);
        	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	        glEnableVertexAttribArray(0);

        	GLuint colorVBO;
        	glGenBuffers(1, &colorVBO);
        	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        	glBufferData(GL_ARRAY_BUFFER, sizeof(colorbox), colorbox, GL_STATIC_DRAW);
       		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
     	   	glEnableVertexAttribArray(1);

        	glBindVertexArray(0);
    	
		}

	    void cleanup() {

    	    glDeleteVertexArrays(1, &VAO);
    
		}

};

class object {

	public:

		//Physical properties
		float radius;
		float mass;
		float weight = (mass) * (g);
		float color1;
		float color2;
		float color3;

		//Rendering properties
		int iterations = 15;
		float alpha = (360 / 15) * (2 * PI / 360);
		float beta = alpha;
		/* const static int verticesArraySize = 6 * iterations; */
		/* const static int colorsArraySize = 9 * iterations; */
		GLfloat vertices[90];													
		GLfloat colors[135];
		GLuint VAO;

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
 		float momentInertia = (mass * (radius * radius)) / (2);

		float kinetickEnergy = ((0.5) + (mass) + (velocityModule * velocityModule)) + ((0.5) + (momentInertia) + (angularVelocityZ * angularVelocityZ)); 
		float potentialEnergy = (mass) * (g) * (coordinatesY + 0.5);

		//Time characteristics
		float deltaTime;
		float frameTime;

		void renderSphereObject() {

			GLfloat totalTime = glfwGetTime();
			deltaTime = totalTime - frameTime;
			frameTime = totalTime;


			for (int i = 0; i < 6 * iterations; i += 6) {

				vertices[i] = coordinatesX;
        		vertices[i + 1] = coordinatesY;

		        vertices[i + 2] = radius * cos(alpha) + coordinatesX;
 		       	vertices[i + 3] = radius * sin(alpha) + coordinatesY;

 		       	vertices[i + 4] = radius * cos(alpha + beta) + coordinatesX;
        		vertices[i + 5] = radius * sin(alpha + beta) + coordinatesY;

				alpha += beta;

			};

    		for (int i = 0; i < 9 * iterations; i += 3) {

		        colors[i] = color1;
       			colors[i + 1] = color2;
     		    colors[i + 2] = color3;
    
			};

		}


		void updateObjectPosition(std::vector<object>& objects, box box1) {

		GLfloat totalTime = glfwGetTime();
		deltaTime = totalTime - frameTime;
		frameTime = totalTime;

		borderCollision(box1);

		objectCollision(objects);

		coordinatesX = (coordinatesX) + (velocityX * deltaTime) + ((1/2) * (accelerationX) * (deltaTime * deltaTime));	
		coordinatesY = (coordinatesY) + (velocityY * deltaTime) + ((1/2) * (accelerationY) * (deltaTime * deltaTime));

		velocityX = (velocityX) + ((accelerationX) * (deltaTime));
		velocityY = (velocityY) + ((accelerationY) * (deltaTime));

		
		for (int i = 0; i < 6 * iterations; i += 6) {

				vertices[i] = coordinatesX;
	    	    vertices[i + 1] = coordinatesY;

				vertices[i + 2] = radius * cos(alpha) + coordinatesX;
    		    vertices[i + 3] = radius * sin(alpha) + coordinatesY;

 				vertices[i + 4] = radius * cos(alpha + beta) + coordinatesX;
	        	vertices[i + 5] = radius * sin(alpha + beta) + coordinatesY;

				alpha += beta;

			};
 
		}


		void renderObject() {

	        glGenVertexArrays(1, &VAO);
    	    glBindVertexArray(VAO);

	        GLuint vertexVBO;
        	glGenBuffers(1, &vertexVBO);
        	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	        glEnableVertexAttribArray(0);

        	GLuint colorVBO;
        	glGenBuffers(1, &colorVBO);
        	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
       		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
     	   	glEnableVertexAttribArray(1);

        	glBindVertexArray(0);
    	
		}


		void borderCollision(box box1) {

			if (coordinatesX + radius > box1.verticesbox[2] || coordinatesX - radius < box1.verticesbox[0]) {

				velocityX = -velocityX;

			}
				
				
			else if (coordinatesY + radius > box1.verticesbox[7] || coordinatesY - radius < box1.verticesbox[1]) {

				velocityY = -velocityY;

			}

		}	


		void objectCollision(std::vector<object>& objects) {

			for (int i = 0; i < objects.size(); i++) {

				for (int j = i + 1; j < objects.size(); j++) {

						float dx = objects[j].coordinatesX - objects[i].coordinatesX - (objects[j].radius - objects[i].radius);
						float dy = objects[j].coordinatesY - objects[i].coordinatesY - (objects[j].radius - objects[i].radius);
						float distance = sqrt((dx * dx) + (dy * dy));

						float limit = objects[i].radius + objects[j].radius;

						if (distance <= limit) {

							/* float velX = objects[i].velocityX; */

							/* objects[i].velocityX = ((objects[i].mass * objects[i].velocityX) + (objects[j].mass * objects[j].velocityX) - (objects[j].mass * 1 * (objects[i].velocityX - objects[j].velocityX))) / (objects[i].mass + objects[j].mass); */
							/* objects[j].velocityX = ((objects[i].mass * velX) + (objects[j].mass * objects[j].velocityX) + (objects[i].mass * 1 * (velX - objects[j].velocityX))) / (objects[i].mass + objects[j].mass); */
							float nx = dx / distance;
                float ny = dy / distance;

                // Calculate the relative velocity along the normal vector
                float relativeVelocityX = objects[j].velocityX - objects[i].velocityX;
                float relativeVelocityY = objects[j].velocityY - objects[i].velocityY;
                float dotProduct = relativeVelocityX * nx + relativeVelocityY * ny;

                // Update the velocities of the colliding objects based on the collision response
                objects[i].velocityX += nx * dotProduct;
                objects[i].velocityY += ny * dotProduct;
                objects[j].velocityX -= nx * dotProduct;
                objects[j].velocityY -= ny * dotProduct;
	
						}	

				}

			}

		}


	    void cleanup() {

    	    glDeleteVertexArrays(1, &VAO);
    
		}

};


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


void init(std::vector<object>& objects, box box1);

void display(std::vector<object>& objects, box box1, GLFWwindow* window);

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void rendersphere(object& obt);

void renderbox(box box1);

void updateObjectPosition(object& obt);


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

    init(objects, box1);

    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
		display(objects, box1, window);
    
	}

    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}

void init(std::vector<object>& objects, box box1) {

	float xposition = -0.9;
	float yposition = -0.9;
	float numberObjects = 100;
	float height = 1.75;
	float increase = height / numberObjects;

	

	for (int i = 0; i < numberObjects; i++) {


		objects.push_back(object());

		objects[i].radius = 0.001538;						//0.001538
		objects[i].mass = 5;
		objects[i].color1 = 1.0;
		objects[i].color2 = 0.0;
		objects[i].color3 = 0.0;

		objects[i].coordinatesX = xposition;
		objects[i].coordinatesY = yposition;
	
		objects[i].velocityX =  0.2;
		objects[i].velocityY =  0.2;

		objects[i].accelerationX = 0.0;
		objects[i].accelerationY = 0.0;

		objects[i].angularVelocityZ = 0.0;

		objects[i].angularAccelerationZ = 0.0;

		objects[i].deltaTime = 0.0;
		objects[i].frameTime = 0.0;

		objects[i].renderSphereObject();
					  
		yposition += increase;

	}
	
}

void display(std::vector<object>& objects, box box1, GLFWwindow* window) {


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    glUseProgram(shaderProgram);


	for (int i = 0; i < objects.size(); i++) {

		objects[i].updateObjectPosition(objects, box1);
		rendersphere(objects[i]);

	}

	renderbox(box1);

	glfwSwapBuffers(window);

}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}

void rendersphere(object& obt) {

	obt.renderObject();
	glBindVertexArray(obt.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obt.iterations);
	glBindVertexArray(0);
 
}

void renderbox(box box1) {

	box1.renderBoxObject();
	glBindVertexArray(box1.VAO);
	glDrawArrays(GL_LINES, 0, 4);
	glDrawArrays(GL_LINES, 4, 4);
	glDrawArrays(GL_LINES, 8, 4);
	glDrawArrays(GL_LINES, 12, 4);
	glBindVertexArray(0);

}
