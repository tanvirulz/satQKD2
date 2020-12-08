// writing on a text file
#include <iostream>
#include <fstream>


#include <stdio.h>
#include <inttypes.h>

#define BUFFER_SIZE (32*1024)
#define H 1
#define V 2
#define A 4
#define D 8

using namespace std;

int main(int argc, char * argv[]){
    char inBuffer[BUFFER_SIZE];
    char outBuffer[BUFFER_SIZE];

    char infile_name[256];
    char outfile_name[256];

    uint64_t raw_ts; //rawtimestamp
    uint64_t corrected_ts;
    uint64_t detector;

    int64_t cH,cV,cA,cD;
    int64_t tmp;
    int counter=0;  //delete it

    cH=0;
    cV=0;
    cA=0;
    cD=0;

    if (argc < 7) {
        printf("crt [work directory] [player name] [corrH] [corrV] [corrA] [corrD]\n");
        exit(0);
    }
    //8th july data
    //alice correction 
    //cH=0;cV=1*500;cA=2*500+5*500;cD=5*500;
    //bob correction 
    //cH=0;cV=0;cA=7*500;cD=0;

    //14th july data
    //alice correction

    //cH=2*500;cV=4*500;cA=6*500+3*500;cD=3*500+3*500;
    //bob correction
    //cH=0;cV=0;cA=0;cD=6*500;

    ifstream infile;
    ofstream outfile;


    infile.rdbuf()->pubsetbuf(inBuffer, BUFFER_SIZE);
    outfile.rdbuf()->pubsetbuf(outBuffer, BUFFER_SIZE);


    sprintf(infile_name,"%s/%s.out",argv[1],argv[2]);
    sprintf(outfile_name,"%s/%s_corrected.out",argv[1],argv[2]);
    
    sscanf(argv[3],"%" SCNd64,&cH);
    sscanf(argv[4],"%" SCNd64,&cV);
    sscanf(argv[5],"%" SCNd64,&cA);
    sscanf(argv[6],"%" SCNd64,&cD);
    //printf("infile: %s\n",infile_name);

    //infile.open("./output/alice.out", ios::in|ios::binary);
    //outfile.open("./output/alice_corrected.out",ios::out|ios::binary|ios::trunc);
    infile.open(infile_name, ios::in|ios::binary);
    outfile.open(outfile_name,ios::out|ios::binary|ios::trunc);

    while( 1 ){
        infile.read(reinterpret_cast<char *>(&raw_ts),sizeof(raw_ts));
        if (infile.eof()) break; // EOF can only be detected after the final *failed* read attempt. 


        detector = raw_ts & uint64_t(0xF);
        raw_ts = raw_ts >> 4;
        //printf("ts =%" PRIu64 "\n", raw_ts);
        //printf("detector =%" PRIu64 "\n", detector);
        
        switch (detector)
        {
        case H:
            tmp = int64_t( int64_t( raw_ts) + cH) ;
            break;
        case V:
            tmp = int64_t( int64_t( raw_ts) + cV);
            break;
        case A:
            tmp = int64_t( int64_t( raw_ts) + cA);
            break;
        case D:
            tmp = int64_t( int64_t( raw_ts) + cD);
            break;
        
        default:
            printf("error!\n");
            printf("at position %d\n",counter);
            printf("ts =%" PRIu64 "\n", raw_ts);
            printf("detector =%" PRIu64 "\n", detector);
            exit(1);
            break;
        }
        if (tmp <0){
            tmp = 0;
        }
        corrected_ts = uint64_t(tmp);
        corrected_ts = (corrected_ts <<4)|detector;
        outfile.write(reinterpret_cast<char *>(&corrected_ts),sizeof(corrected_ts));
        //outfile.write(reinterpret_cast<char *>(&raw_ts),sizeof(raw_ts));
        counter++;
    }
    
    infile.close();
    outfile.close();
    return 0;

}
