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

    for(int i{25} ; i<75 ; ++i){
        for(int y{25} ; y<75 ; ++y){
            data[ i*100*3 + y*3   ] = 0x00;
            data[ i*100*3 + y*3+1 ] = 0x00;
            data[ i*100*3 + y*3+2 ] = 0xff;
        }
    }

    int window_width, window_height;
    int tex_width  = 100;
    int tex_height = 100;

    glfwMakeContextCurrent(window);

    // create a handle and finally a texture
    GLuint tex_handle;               // texture id
    glGenTextures(1, &tex_handle);   // create 1 texture and return name of texture to tex_handle
    glBindTexture(GL_TEXTURE_2D, tex_handle); // i want to work with tex_handle as a 2D texture( bind texture as 2D)
    // the way pixels are stored:
    glPixelStorei(GL_PACK_ALIGNMENT, 1); // still white frame
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // load info into texture

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set up orphographic projection(2D)
        glfwGetFramebufferSize(window, &window_width, &window_height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity(); // reset matrix to an identity matrix
        glOrtho(0, window_width, 0, window_height,-1,1); // to fill matrix with orphographic projection

        glMatrixMode(GL_MODELVIEW);
        // now render texture 
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(200,200);
            glTexCoord2d(1,0); glVertex2i(200+tex_width*2 ,200);
            glTexCoord2d(1,1); glVertex2i(200+tex_width*2, 200+tex_height*2);
            glTexCoord2d(0,1); glVertex2i(200,200+tex_height*2);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    delete data;
    return 0;
}