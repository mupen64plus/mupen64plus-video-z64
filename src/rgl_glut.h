typedef void (* rglGlutCommand_f)();

void rglGlutCreateThread(int recreatewindows);

void rglGlutPostCommand(rglGlutCommand_f c);

void rglSwapBuffers();

void rglGlutMinimizeWindow();
