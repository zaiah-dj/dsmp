#include <inttypes.h>
#define AMT 4096
#if 0
#include "vendor/stb_vorbis.c"
#include <inttypes.h>
#define AMT 4096
#define peek() fprintf(stderr, "%d %d %d %d %d %d %d %d\n", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
void vvvv_ (const char *filename) {
	//Open the first 2046 bytes of an ogg file named 'filename'
	FILE *f = fopen(filename, "r");
	int err, mem;
	uint8_t buf[AMT]={0};
	stb_vorbis *N=NULL;
	fread(&buf, 1, AMT, f);

	/*Seems like we can use memory jumps...*/
	if (!(N = stb_vorbis_open_pushdata((unsigned char *)buf, AMT, &mem, &err, NULL)))
		/*Need to get more data.*/fprintf(stderr, "need more vorbis data to decode.\n");
	else
		fprintf(stderr, "Vorbis read successful");

	/*Dump the vorbis header*/
	fprintf(stderr, "Sample Rate:         %d\n", N->sample_rate); 
	fprintf(stderr, "Channels:            %d\n", N->channels); 
	fprintf(stderr, "           :         %d\n", N->setup_memory_required); 
	fprintf(stderr, "           :         %d\n", N->setup_temp_memory_required); 
	fprintf(stderr, "Temporary Mem Reqd:  %d\n", N->temp_memory_required); 

	/*Read that mug...*/
	int j=0;
	while ((j = fread(&buf, 1, AMT, f)) > EOF/*Not at end of vorbis file*/) {
		fprintf(stderr, "%d\n", j);
		peek();
	}	
}
#endif
