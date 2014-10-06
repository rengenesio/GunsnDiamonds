#define DATASZ  (1<<15) /* (32768) amount of data to read from disk each time */
#define BUFSZ   (1<<16) /* (65536) size of audiostream buffer */

typedef struct MP3FILE
{
  PACKFILE *f;
  ALMP3_MP3STREAM *s;
} MP3FILE;

int play_mp3_file(MP3FILE *mp3, int buflen, int vol, int pan);
void close_mp3_file(MP3FILE *mp3);
MP3FILE *open_mp3_file(char *filename);
int poll_mp3_file(MP3FILE *mp3);

