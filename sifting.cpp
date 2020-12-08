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


#define AD_BASIS 3
#define HV_BASIS 12

#define FILE_NAME_SIZE 256

using namespace std;

int main(int argc, char* argv[]){
    char stampsBuffer[BUFFER_SIZE];
    char coin_indexBuffer[BUFFER_SIZE];
    char bimask_Buffer[BUFFER_SIZE];

    char rawkey_Buffer[BUFFER_SIZE];

    uint64_t ts;
    uint64_t detector;
    
    
    uint32_t matched_index;
    uint32_t ts_index; 
    //uint32_t ib;
    uint8_t raw_bit;
    uint8_t basis_matched; 

    int raw_bits_count;


    ifstream stamps_infile;
    ifstream coin_index_infile;
    ifstream bitmap_infile;

    ofstream rawkey_outfile;
    
    char stamps_infile_name[FILE_NAME_SIZE];
    char coin_index_infile_name[FILE_NAME_SIZE];
    char bitmap_infile_name[FILE_NAME_SIZE];
    char rawkey_outfile_name[FILE_NAME_SIZE];



    if(argc<3){
        printf("sift [workfolder] [player name]");
        exit(0);
    }

    sprintf(stamps_infile_name,"%s/%s_corrected.out",argv[1],argv[2]);
    sprintf(coin_index_infile_name,"%s/%s_coin.out",argv[1],argv[2]);
    sprintf(bitmap_infile_name,"%s/%s",argv[1],"basis_match_bitmask.out");
    sprintf(rawkey_outfile_name,"%s/%s_sifted.txt",argv[1],argv[2]);
    
    
    stamps_infile.rdbuf()->pubsetbuf(stampsBuffer, BUFFER_SIZE);
    coin_index_infile.rdbuf()->pubsetbuf(coin_indexBuffer, BUFFER_SIZE);
    bitmap_infile.rdbuf()->pubsetbuf(bimask_Buffer, BUFFER_SIZE);

    rawkey_outfile.rdbuf()->pubsetbuf(rawkey_Buffer, BUFFER_SIZE);

    /*
    stamps_infile.open("./output/bob_corrected.out", ios::in|ios::binary);
    coin_index_infile.open("./output/bob_coin.out", ios::in|ios::binary);
    bitmap_infile.open("./output/basis_match_bitmask.out",ios::in|ios::binary);

    rawkey_outfile.open("./output/bob_rawkey.txt",ios::out|ios::binary|ios::trunc);
    */
    stamps_infile.open(stamps_infile_name, ios::in|ios::binary);
    coin_index_infile.open(coin_index_infile_name, ios::in|ios::binary);
    bitmap_infile.open(bitmap_infile_name,ios::in|ios::binary);

    rawkey_outfile.open(rawkey_outfile_name,ios::out|ios::binary|ios::trunc);
    
    matched_index = 0;
    ts_index = 0;
    raw_bit = 0;

    raw_bits_count = 0;
    basis_matched = 0;
    //ib = 0;
    //ainfile.read(reinterpret_cast<char *>(&a_ts),sizeof(a_ts));
    //if (ainfile.eof()) exit(0); // EOF can only be detected after the final *failed* read attempt. 
    //a_ts = a_ts>>4;

    //binfile.read(reinterpret_cast<char *>(&b_ts),sizeof(b_ts));
    //if (binfile.eof()) exit(0);
    //b_ts = b_ts >>4;

    //int debugloopcount = 0;
    while( 1 ){
        coin_index_infile.read(reinterpret_cast<char *>(&matched_index),sizeof(matched_index));
        if (coin_index_infile.eof()) break;
        bitmap_infile.read(reinterpret_cast<char *> (&basis_matched),sizeof(basis_matched));
        if (bitmap_infile.eof()) break;

        while (ts_index <= matched_index){
            stamps_infile.read(reinterpret_cast<char *>(&ts),sizeof(ts));
            if (stamps_infile.eof()) break; // EOF can only be detected after the final *failed* read attempt. 
            detector = ts & uint64_t(0xF);
            ts_index++;
            
        }

        if(basis_matched){    
            switch (detector)
            {
                case H:
                    raw_bit = 0; //perviously used '0' character encoding. 
                    break;
                case V:
                    raw_bit = 1;
                    break;
                case A:
                    raw_bit = 0;
                    break;
                case D:
                    raw_bit = 1;
                    break;
                
                default:
                    printf("error!\n");
                    printf("at position %d\n",(int)ts_index);
                    //printf("ts =%" PRIu64 "\n", raw_ts);
                    printf("detector =%" PRIu64 "\n", detector);
                    exit(1);
                    break;
            }
            rawkey_outfile.write(reinterpret_cast<char *> (&raw_bit), sizeof(raw_bit) );
            raw_bits_count ++;

        }
        
        
        //debugloopcount ++;
    }
    printf("Total number of sifted_key_bits = %d\n",raw_bits_count);


    return 0;
}