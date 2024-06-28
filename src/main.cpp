#include <stdio.h>
#include <GLFW/glfw3.h>
#include "video_reader.hpp"




int main(int argc, char** argv){
    VideoReaderState vr_state;
    int window_width, window_height;
    //const int frame_width = vr_state.width;
    //const int frame_height = vr_state.height;
    //uint8_t* frame_data = new uint8_t[frame_width * frame_height * 4];

    
    // make a pointer to a window and initialize the window
    GLFWwindow* window;

    if(!glfwInit()){
        fprintf(stdout,"error: couldnt initialize window\n");
        return 1;
    }

    window = glfwCreateWindow(640, 480, "Window",NULL,NULL);
    if(!window){
        fprintf(stdout,"error: couldnt create window\n");
        return 1;
    }
    
    //VideoReaderState vr_state;
    if(!video_reader_open_file(&vr_state, "/home/igosh/Desktop/abc.mp4")){
        printf("error: couldnt open video\n");
        return 1;
    }

    glfwMakeContextCurrent(window);

    // create a handle and finally a texture
    GLuint tex_handle;               // texture id
    glGenTextures(1, &tex_handle);   // create 1 texture and return name of texture to tex_handle
    glBindTexture(GL_TEXTURE_2D, tex_handle); // i want to work with tex_handle as a 2D texture( bind texture as 2D)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);    // the way pixels are stored:
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    //glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    //glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    const int frame_width = vr_state.width;
    const int frame_height = vr_state.height;
    uint8_t* frame_data = new uint8_t[frame_width * frame_height * 4];


    while(!glfwWindowShouldClose(window)){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear entire screen
        // set up orphographic projection(2D)
        //int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity(); // reset matrix to an identity matrix
        glOrtho(0, window_width, window_height, 0, -1, 1); // to fill matrix with orphographic projection
        glMatrixMode(GL_MODELVIEW);
        // now render texture 
        if(!video_reader_read_frame(&vr_state, frame_data)){
            printf("error: couldnt load video frame\n");
            return 1;
        }
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_data); // load info into texture

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
        glfwPollEvents();
    }

    video_reader_close(&vr_state);

    return 0;
}


















/*
        if(!load_frame("/home/igosh/Desktop/abc.mp4", &frame_width, &frame_height, &frame_data)){
        printf("error: couldnt open video frame\n");
        return 1;
    };

*/