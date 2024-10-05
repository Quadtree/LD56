std::shared_ptr<class SDL_Texture> LoadTexture(std::string filename);
void _AssertOnMainThread(int line, std::string file);

#ifdef _DEBUG
#define AssertOnMainThread() _AssertOnMainThread(__LINE__, __FILE__)
#else
#define AssertOnMainThread()
#endif

void SubmitToThreadPool(function<void()> func);