#ifndef MP3_HPP_
#define MP3_HPP_
#ifdef __cplusplus
extern "C" {
#endif

void mp3Task(void* p);
void mp3_controls(void *p);
int getSong(char c, char *fileName, int current);
#ifdef __cplusplus
}
#endif
#endif /* MP3_HPP_ */
