#include <stdio.h>
#include <GLFW/glfw3.h>



int main(int argc, char** argv){
    // make a pointer to a window and initialize the window
    GLFWwindow* window;

    if(!glfwInit()){ 
        printf("error: initializing window failed\n");
        return 1;
    }
    printf("window initialized\n");


    window = glfwCreateWindow(614,614,"GLFW Window", NULL, NULL);
    if(!window){
        printf("error: couldnt open window\n");
        return 1;
    }

    unsigned char* data = new unsigned char[100 * 100 * 3];
    for(int x=0 ; x<100 ; ++x){
        for(int y=0 ; y<100 ; ++y){
            data[x*100*3 + y*3  ] = 0xff; //RED
            data[x*100*3 + y*3+1] = 0x00; //GREEN
            data[x*100*3 + y*3+2] = 0x00; //BLUE
        }
    }

    glfwMakeContextCurrent(window);

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawPixels(100,100,GL_RGB,GL_UNSIGNED_BYTE,data);
        glfwSwapBuffers(window);
        glfwWaitEvents();
    }


    return 0;
}