#include "Camera.h"

Camera::Camera(uint32_t screenWidth, uint32_t screenHeight): Camera(screenHeight, screenWidth, 45.f, 0.1f, 1000.f){
    
}

Camera::Camera(uint32_t screenWidth, uint32_t screenHeight, float fov, float nearPlane, float farPlane){
    lastX = screenWidth/2.f;
    lastY = screenHeight/2.f;
    setUpProjection(glm::radians(fov), static_cast<float>(screenWidth)/static_cast<float>(screenHeight), nearPlane, farPlane);
    update();
}

void Camera::setUpProjection(float fov, float aspectRatio, float nearPlane, float farPlane){
    projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane); 
	this->fov = fov;
	this->aspectRatio = aspectRatio;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

void Camera::setScreenSize(uint32_t screenWidth, uint32_t screenHeight){
    setUpProjection(this->fov, static_cast<float>(screenWidth)/static_cast<float>(screenHeight), this->nearPlane, this->farPlane);
}

void Camera::setMovementSpeed(float speed){
    movementSpeed = speed;
}
float Camera::getMovementSpeed() const{
    return movementSpeed;
}
   
void Camera::setMouseSensitivity(float sensitivity){
    mouseSensitivity = sensitivity;
}
float Camera::getMouseSensitivity() const{
    return mouseSensitivity;
}

void Camera::update(){
    viewMatrix = glm::mat4(1.f);

    //setting proper rotation
    viewMatrix = glm::rotate(viewMatrix, roll, glm::vec3(0,0,1));
    viewMatrix = glm::rotate(viewMatrix, yaw, glm::vec3(0,1,0));
    viewMatrix = glm::rotate(viewMatrix, pitch, glm::vec3(1,0,0));

    //FOR NOW we move when in direction that we are facing, 
    //so if we look up and press W we won't go forward but up
    front = glm::vec3(viewMatrix * glm::vec4(0,0,1,0));
    up = glm::vec3(viewMatrix * glm::vec4(0,1,0,0));
    right = glm::cross(front, up);

    //normalize
    front = glm::normalize(front);
	up = glm::normalize(up);
	right = glm::normalize(right);


    glm::vec3 pointThatCameraIsAimedAt = position + front;

    viewMatrix = glm::lookAt(position, pointThatCameraIsAimedAt, up);
}

void Camera::processMouseMovement(float xPos, float yPos)
{
    if (firstMouseMove){
        lastX = xPos;
        lastY = yPos;
        firstMouseMove = false;
    }

    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    rotate(glm::radians(-xoffset), glm::radians(invertHorizontalMove ? yoffset : -yoffset), 0);
}

void Camera::rotate(float yaw, float pitch, float roll, GLboolean constrainPitch){
    this->yaw += yaw;
    this->pitch += pitch;
    this->roll += roll;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch){
        if (this->pitch > glm::radians(90.0f))
            this->pitch = glm::radians(90.0f);
        if (this->pitch < glm::radians(-90.0f))
            this->pitch = glm::radians(-90.0f);
    }

    update();
}

void Camera::move(Directions direction, float deltaTime){

    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == RIGHT)
        position += right * velocity;
    if (direction == UP)
        position += up * velocity;
    if (direction == DOWN)
        position -= up * velocity;

    update();
}

glm::mat4 Camera::getViewMatrix() const{
    return viewMatrix;
}
glm::mat4 Camera::getProjectionMatrix() const{
    return projectionMatrix;
}

void Camera::setPosition(glm::vec3 p){
    position = p;
    update();
}

void Camera::setRotation(float yaw, float pitch, float roll){
    this->yaw = yaw;
    this->pitch = pitch;
    this->roll = roll;
    update();
}

glm::vec3 Camera::getPosition() const{
    return position;
}