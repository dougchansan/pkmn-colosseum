#ifndef PCPORT_WINDOW_H
#define PCPORT_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GLFWwindow GLFWwindow;

void PCPort_SetHostWindow(GLFWwindow* window);
GLFWwindow* PCPort_GetHostWindow(void);
int PCPort_GetVideoWidth(void);
int PCPort_GetVideoHeight(void);
int PCPort_IsVideoBlack(void);
void PCPort_RunPreRetraceCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* PCPORT_WINDOW_H */
