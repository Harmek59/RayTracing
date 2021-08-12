#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
    enum Directions{
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    Camera(uint32_t screenWidth, uint32_t screenHeight);

    //fov in degrees
    Camera(uint32_t screenWidth, uint32_t screenHeight, float fov, float nearPlane, float farPlane);

    //fov in radians
    void setUpProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

    void setScreenSize(uint32_t screenWidth, uint32_t screenHeight);

    void setMovementSpeed(float speed);
    float getMovementSpeed() const;
   
    void setMouseSensitivity(float sensitivity);
    float getMouseSensitivity() const;

    void processMouseMovement(float xPos, float yPos);


    void update();

    // yaw, pitch, roll in radians
	void rotate(float yaw, float pitch, float roll, GLboolean constrainPitch = true);
    void move(Directions direction, float deltaTime);

    glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

    void setPosition(glm::vec3 p);
    void setRotation(float yaw, float pitch, float roll);

	glm::vec3 getPosition() const;

    void invertHorizontalMovement(){
        invertHorizontalMove = !invertHorizontalMove;
    }

//private:
    bool invertHorizontalMove = false;

    //projection attributes
    float fov, aspectRatio, nearPlane, farPlane;

    // camera attributes
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float lastX;
    float lastY;
    bool firstMouseMove = true; //without this there is strange camera move at first mouse move

    // euler angles
    float yaw = 0.;
    float pitch = 0.;
    float roll = 0.;

    // camera options
    float movementSpeed = 100.;
    float mouseSensitivity = 0.05f;
    
    //matrixes
    glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

   
    
};