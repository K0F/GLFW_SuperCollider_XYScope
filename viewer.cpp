#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#define BUFFER_SIZE 32768
#define TRAIL_SAMPLES 24000

#define AUTOROTATERATE 0.25f

struct SharedLayout {
  uint32_t write_ptr;
  float x[BUFFER_SIZE];
  float y[BUFFER_SIZE];
  float z[BUFFER_SIZE];
};

int main(int argc, char *argv[]) {
  double target_fps = 60.0;
  int width = 932;
  int height = 576;
  bool mode3D = false;
  bool autorotate = true;
  int frameCount = 0;
  
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-r" && i + 1 < argc) {
      target_fps = std::stod(argv[++i]);
    } else if (arg == "-s" && i + 1 < argc) {
      height = std::stoi(argv[++i]);
    } else if (arg == "-3d") {
      mode3D = true;
      autorotate = true;
    } else if (arg == "-a") {
      autorotate = false;
    } else {
      std::cerr << "Usage: " << argv[0]
                << " [-r fps_rate] [-s height of the window] [-3d XYZ] [-a turn OFF "
                   "AutoRotate]\n";
      return -1;
    }
  }

  int shm_fd = shm_open("/tmp/sc_xy_scope", O_RDONLY, 0444);
  if (shm_fd < 0) {
    std::cerr << "Error: Could not open shared memory." << std::endl;
    return -1;
  }

  struct stat shm_stat;
  if (fstat(shm_fd, &shm_stat) < 0 || shm_stat.st_size < sizeof(SharedLayout)) {
    std::cerr << "Error: Shared memory size mismatch. Run clear steps."
              << std::endl;
    close(shm_fd);
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

  std::string title = mode3D ? "XYZ 3D Scope Viewer" : "XY Scope Viewer";
  GLFWwindow *window =
      glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  if (!window) {
    std::cerr << "Error: Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);

  double time_per_frame = 1.0 / target_fps;
  bool is_dragging = false;
  double last_x = 0.0, last_y = 0.0;
  float rot_x = mode3D ? 20.0f : 0.0f;
  float rot_y = mode3D ? -30.0f : 0.0f;


  while (!glfwWindowShouldClose(window)) {
    double frame_start_time = glfwGetTime();

    if (mode3D) {
      double mouse_x, mouse_y;
      glfwGetCursorPos(window, &mouse_x, &mouse_y);
      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!is_dragging) {
          is_dragging = true;
        } else {
          rot_y += (mouse_x - last_x) * 0.3f;
          rot_x += (mouse_y - last_y) * 0.3f;
        }
        last_x = mouse_x;
        last_y = mouse_y;
      } else {
        is_dragging = false;
      }
    }

    
      if (autorotate)
        rot_y += AUTOROTATERATE;

    //int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.012f, 0.003f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (mode3D) {
      glScalef(0.55f, 0.55f, 0.55f);
      glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
      glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
    }

    uint32_t current_write_ptr = shm->write_ptr;

    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < TRAIL_SAMPLES; i++) {
      int idx =
          (current_write_ptr + BUFFER_SIZE - TRAIL_SAMPLES + i) % BUFFER_SIZE;
      float age_factor = (float)i / (float)TRAIL_SAMPLES;
      float alpha = age_factor * age_factor * age_factor;
      glColor4f(1.0f, 0.85f, 0.1f, alpha * 0.12f);
      glVertex3f(shm->x[idx], shm->y[idx], mode3D ? shm->z[idx] : 0.0f);
    }
    glEnd();

    glLineWidth(0.75f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < TRAIL_SAMPLES; i++) {
      int idx =
          (current_write_ptr + BUFFER_SIZE - TRAIL_SAMPLES + i) % BUFFER_SIZE;
      float age_factor = (float)i / (float)TRAIL_SAMPLES;
      float alpha = age_factor * age_factor * age_factor;
      glColor4f(1.0f, 0.95f, 0.7f, alpha * 0.12f);
      glVertex3f(shm->x[idx], shm->y[idx], mode3D ? shm->z[idx] : 0.0f);
    }
    glEnd();


    glfwSwapBuffers(window);
    glfwPollEvents();

    double elapsed_time = glfwGetTime() - frame_start_time;
    if (elapsed_time < time_per_frame) {
      double sleep_seconds = time_per_frame - elapsed_time;
      auto sleep_duration = std::chrono::microseconds(
          static_cast<int>(sleep_seconds * 1000000.0));
      std::this_thread::sleep_for(sleep_duration);
    }
    
    ++frameCount;

  }//end loop

  munmap(shm, sizeof(SharedLayout));
  close(shm_fd);
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
