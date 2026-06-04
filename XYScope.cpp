#include "SC_PlugIn.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>

static InterfaceTable *ft;
#define BUFFER_SIZE 32768

struct SharedLayout {
    uint32_t write_ptr;
    float x[BUFFER_SIZE];
    float y[BUFFER_SIZE];
    float z[BUFFER_SIZE];
};

static SharedLayout* g_shared_mem = nullptr;
static int g_shm_fd = -1;

struct XYScope : public Unit {};

static void XYScope_next_a(XYScope* unit, int inNumSamples);
static void XYScope_Ctor(XYScope* unit);
static void XYScope_Dtor(XYScope* unit);

void XYScope_Ctor(XYScope* unit) {
    SETCALC(XYScope_next_a);
    ClearUnitOutputs(unit, unit->mNumOutputs); 
}

void XYScope_Dtor(XYScope*) {}

void XYScope_next_a(XYScope* unit, int inNumSamples) {
    float* inX = IN(0);
    float* inY = IN(1);
    
    bool hasZ = unit->mNumInputs > 2;
    bool isAudioZ = hasZ && (INRATE(2) == calc_FullRate);
    float* inZ = isAudioZ ? IN(2) : nullptr;
    float controlZ = (hasZ && !isAudioZ) ? IN0(2) : 0.0f;

    float* outX = (unit->mNumOutputs > 0) ? OUT(0) : nullptr;
    float* outY = (unit->mNumOutputs > 1) ? OUT(1) : nullptr; 

    for (int i = 0; i < inNumSamples; ++i) {
        float currentX = inX[i];
        float currentY = inY[i];
        float currentZ = isAudioZ ? inZ[i] : controlZ;

        if (!std::isfinite(currentX)) currentX = 0.f;
        if (!std::isfinite(currentY)) currentY = 0.f;
        if (!std::isfinite(currentZ)) currentZ = 0.f;

        if (outX) outX[i] = currentX;
        if (outY) outY[i] = currentY;

        if (g_shared_mem != nullptr) {
            uint32_t idx = g_shared_mem->write_ptr;
            g_shared_mem->x[idx] = currentX;
            g_shared_mem->y[idx] = currentY;
            g_shared_mem->z[idx] = currentZ;
            g_shared_mem->write_ptr = (idx + 1) % BUFFER_SIZE;
        }
    }
}

PluginLoad(XYScope) {
    ft = inTable; 
    if (g_shm_fd < 0) {
        shm_unlink("/sc_xy_scope");
        g_shm_fd = shm_open("/sc_xy_scope", O_CREAT | O_RDWR, 0666);
        if (g_shm_fd >= 0) {
            ftruncate(g_shm_fd, sizeof(SharedLayout));
            void* map_ptr = mmap(0, sizeof(SharedLayout), PROT_READ | PROT_WRITE, MAP_SHARED, g_shm_fd, 0);
            if (map_ptr != MAP_FAILED) {
                g_shared_mem = (SharedLayout*)map_ptr;
                g_shared_mem->write_ptr = 0;
            }
        }
    }
    DefineUnit("XYScope", sizeof(XYScope), (UnitCtorFunc)&XYScope_Ctor, (UnitDtorFunc)&XYScope_Dtor, 0);
}