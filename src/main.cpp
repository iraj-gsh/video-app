#include <stdio.h>
#include <GLFW/glfw3.h>

bool load_frame(const char* filename, int* width_out, int* height_out, unsigned char** data_out);

int main(int argc, char** argv){
    int window_width, window_height;
    int frame_width, frame_height;
    unsigned char* frame_data;

    // make a pointer to a window and initialize the window
    GLFWwindow* window;
    if(!glfwInit()){
        fprintf(stdout,"error: couldnt initialize window\n");
        return 1;
    }
    window = glfwCreateWindow(1410,680,"Window",NULL,NULL);
    if(!window){
        fprintf(stdout,"error: couldnt create window\n");
        return 1;
    }
    
    glfwMakeContextCurrent(window);



    if(!load_frame("/home/igosh/Desktop/abc.mp4", &frame_width, &frame_height, &frame_data)){
        printf("error: couldnt open video frame\n");
        return 1;
    };

    
    

    glfwMakeContextCurrent(window);

    // create a handle and finally a texture
    GLuint tex_handle;               // texture id
    glGenTextures(1, &tex_handle);   // create 1 texture and return name of texture to tex_handle
    glBindTexture(GL_TEXTURE_2D, tex_handle); // i want to work with tex_handle as a 2D texture( bind texture as 2D)
    // the way pixels are stored:
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // still white frame
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data); // load info into texture

    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set up orphographic projection(2D)
        glfwGetFramebufferSize(window, &window_width, &window_height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity(); // reset matrix to an identity matrix
        glOrtho(0, window_width, window_height, 0, -1, 1); // to fill matrix with orphographic projection

        glMatrixMode(GL_MODELVIEW);
        // now render texture 
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(0,0);
            glTexCoord2d(1,0); glVertex2i(frame_width, 0);
            glTexCoord2d(1,1); glVertex2i(frame_width, frame_height);
            glTexCoord2d(0,1); glVertex2i(0, frame_height);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    //glfwDestroyWindow(window);
    return 0;
}