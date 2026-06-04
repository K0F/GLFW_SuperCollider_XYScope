#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <thread>
#include <unistd.h>

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

  SharedLayout *shm = (SharedLayout *)mmap(0, sizeof(SharedLayout), PROT_READ,
                                           MAP_SHARED, shm_fd, 0);
  if (shm == MAP_FAILED) {
    std::cerr << "Error: Memory mapping failed." << std::endl;
    close(shm_fd);
    return -1;
  }

  if (!glfwInit()) {
    std::cerr << "Error: Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 8);

  // golden PAL as default
  GLFWwindow *window =
      glfwCreateWindow(932, 576, "XY Scope - PAL@120", NULL, NULL);
  if (!window) {
    std::cerr << "Error: Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0); // 120 FPS target (60 sould be enought)

  const double TARGET_FPS = 120.0;
  const double TIME_PER_FRAME = 1.0 / TARGET_FPS;

  while (!glfwWindowShouldClose(window)) {
    double frame_start_time = glfwGetTime();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // color tranform dim screen
    glClearColor(0.012f, 0.0096f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    uint32_t current_write_ptr = shm->write_ptr;

    glLineWidth(1.5f); // 5.5?
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < TRAIL_SAMPLES; i++) {
      int idx =
          (current_write_ptr + BUFFER_SIZE - TRAIL_SAMPLES + i) % BUFFER_SIZE;
      float age_factor = (float)i / (float)TRAIL_SAMPLES;
      float alpha = age_factor * age_factor * age_factor;

      // #ffcc00 as target gradient
      glColor4f(1.0f, 0.85f, 0.1f, alpha * 0.12f);
      glVertex2f(shm->x[idx], shm->y[idx]);
    }
    glEnd();

    glLineWidth(0.75f); // Thin laser line right down the middle
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < TRAIL_SAMPLES; i++) {
      int idx =
          (current_write_ptr + BUFFER_SIZE - TRAIL_SAMPLES + i) % BUFFER_SIZE;
      float age_factor = (float)i / (float)TRAIL_SAMPLES;
      float alpha = age_factor * age_factor * age_factor;

      // white-hot orange
      glColor4f(1.0f, 0.95f, 0.7f, alpha * 0.12f);
      glVertex2f(shm->x[idx], shm->y[idx]);
    }
    glEnd();

    glfwSwapBuffers(window);
    glfwPollEvents();

    double elapsed_time = glfwGetTime() - frame_start_time;
    if (elapsed_time < TIME_PER_FRAME) {
      double sleep_seconds = TIME_PER_FRAME - elapsed_time;
      auto sleep_duration = std::chrono::microseconds(
          static_cast<int>(sleep_seconds * 1000000.0));
      std::this_thread::sleep_for(sleep_duration);
    }
  }

  munmap(shm, sizeof(SharedLayout));
  close(shm_fd);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
