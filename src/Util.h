std::shared_ptr<class SDL_Texture> LoadTexture(std::string filename);
void _AssertOnMainThread(int line, std::string file);

#define AssertOnMainThread() _AssertOnMainThread(__LINE__, __FILE__)