#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdint>
#include <chrono>
#include <thread>
#include <GLFW/glfw3.h>

#define BUFFER_SIZE 32768
#define TRAIL_SAMPLES 24000 

struct SharedLayout {
    uint32_t write_ptr;
    float x[BUFFER_SIZE];
    float y[BUFFER_SIZE];
};

int main() {
    int shm_fd = shm_open("/sc_xy_scope", O_RDONLY, 0444);
    if (shm_fd < 0) {
        std::cerr << "Error: Could not open shared memory." << std::endl;
        return -1;
    }

    SharedLayout* shm = (SharedLayout*)mmap(0, sizeof(SharedLayout), PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        std::cerr << "Error: Memory mapping failed." << std::endl;
        close(shm_fd);
        return -1;
    }

    if (!glfwInit()) {
        std::cerr << "Error: Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // SMOOTHING FIX 1: Request 8x Hardware Multisample Anti-Aliasing (MSAA)
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(800, 800, "XY Scope - Ultra-Smooth Vector Glow", NULL, NULL);
    if (!window) {
        std::cerr << "Error: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // 120 FPS target

    const double TARGET_FPS = 120.0;
    const double TIME_PER_FRAME = 1.0 / TARGET_FPS;

    while (!glfwWindowShouldClose(window)) {
        double frame_start_time = glfwGetTime();

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.0f, 0.012f, 0.003f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);

        // Global Blend Setup for glow synthesis
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending
        
        // SMOOTHING FIX 2: Explicitly enable hardware multisampling
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        uint32_t current_write_ptr = shm->write_ptr;

        // =================================================================
        // PASS 1: The Phosphor Bloom (Thick, soft background glow)
        // =================================================================
        glLineWidth(5.5f); // Make this line wide to create a fuzzy edge
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < TRAIL_SAMPLES; i++) {
            int idx = (current_write_ptr + BUFFER_SIZE - TRAIL_SAMPLES + i) % BUFFER_SIZE;
            float age_factor = (float)i / (float)TRAIL_SAMPLES;
            float alpha = age_factor * age_factor * age_factor; 

            // Darker green, heavily transparent (0.12 max alpha) to simulate a soft edge bleed
            glColor4f(0.0f, 0.7f, 0.15f, alpha * 0.12f); 
            glVertex2f(shm->x[idx], shm->y[idx]);
        }
        glEnd();

        // =================================================================
        // PASS 2: The Core Electron Beam (Thin, sharp, bright center)
        // =================================================================
        glLineWidth(1.5f); // Thin laser line right down the middle
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < TRAIL_SAMPLES; i++) {
            int idx = (current_write_ptr + BUFFER_SIZE - TRAIL_SAMPLES + i) % BUFFER_SIZE;
            float age_factor = (float)i / (float)TRAIL_SAMPLES;
            float alpha = age_factor * age_factor * age_factor; 

            // Striking, bright mint/cyan-green core for that authentic CRT energy beam look
            glColor4f(0.4f, 1.0f, 0.6f, alpha * 0.75f); 
            glVertex2f(shm->x[idx], shm->y[idx]);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();

        double elapsed_time = glfwGetTime() - frame_start_time;
        if (elapsed_time < TIME_PER_FRAME) {
            double sleep_seconds = TIME_PER_FRAME - elapsed_time;
            auto sleep_duration = std::chrono::microseconds(static_cast<int>(sleep_seconds * 1000000.0));
            std::this_thread::sleep_for(sleep_duration);
        }
    }

    munmap(shm, sizeof(SharedLayout));
    close(shm_fd);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}