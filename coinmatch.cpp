// writing on a text file
#include <iostream>
#include <fstream>


#include <stdio.h>
#include <inttypes.h>
#include <math.h>
//#include <stdlib.h> 

#define BUFFER_SIZE (32*1024)
#define H 1
#define V 2
#define A 4
#define D 8

#define AD_BASIS 3
#define HV_BASIS 12

#define FILE_NAME_SIZE 256


using namespace std;

int main(int argc, char * argv[]){
    char ainBuffer[BUFFER_SIZE];
    char binBuffer[BUFFER_SIZE];
    char aoutBuffer[BUFFER_SIZE];
    char boutBuffer[BUFFER_SIZE];
    char basismatchBuffer[BUFFER_SIZE];

    
    uint64_t a_ts;
    uint64_t b_ts;
    uint64_t a_det;
    uint64_t b_det;
    uint64_t begin_time;
    uint64_t end_time;
    
    double duration;
    
    int64_t cwindow;
    int64_t shift;
    int64_t diff;


    uint32_t ia;
    uint32_t ib;


    int coincount;
    int basis_match_count;
    int error_count;
    uint8_t basis_match_flag;

    
    ifstream ainfile;
    ifstream binfile;
    ofstream aoutfile;
    ofstream boutfile;
    ofstream basis_match_outfile;

    char ainfile_name[FILE_NAME_SIZE];
    char binfile_name[FILE_NAME_SIZE];
    char aoutfile_name[FILE_NAME_SIZE];
    char boutfile_name[FILE_NAME_SIZE];
    char basis_match_oufile_name[FILE_NAME_SIZE];

    if (argc<4){
        printf("cm [workdirectory] [coincidence windo in ps] [crosscorrelation shift]\n");
        exit(0);
    }
    ainfile.rdbuf()->pubsetbuf(ainBuffer, BUFFER_SIZE);
    binfile.rdbuf()->pubsetbuf(binBuffer, BUFFER_SIZE);
    aoutfile.rdbuf()->pubsetbuf(aoutBuffer, BUFFER_SIZE);
    boutfile.rdbuf()->pubsetbuf(boutBuffer, BUFFER_SIZE);
    basis_match_outfile.rdbuf()->pubsetbuf(basismatchBuffer, BUFFER_SIZE);

    sprintf(ainfile_name,"%s/%s",argv[1],"alice_corrected.out");
    sprintf(binfile_name,"%s/%s",argv[1],"bob_corrected.out");

    sprintf(aoutfile_name,"%s/%s",argv[1],"alice_coin.out");
    sprintf(boutfile_name,"%s/%s",argv[1],"bob_coin.out");
    sprintf(basis_match_oufile_name,"%s/%s",argv[1],"basis_match_bitmask.out");

    
    /*
    ainfile.open("./14july0db/alice_corrected.out", ios::in|ios::binary);
    binfile.open("./14july0db/bob_corrected.out", ios::in|ios::binary);

    aoutfile.open("./14july0db/alice_coin.out",ios::out|ios::binary|ios::trunc);
    boutfile.open("./14july0db/bob_coin.out",ios::out|ios::binary|ios::trunc);
    basis_match_outfile.open("./14july0db/basis_match_bitmask.out",ios::out|ios::binary|ios::trunc);
    */
   
    ainfile.open(ainfile_name, ios::in|ios::binary);
    binfile.open(binfile_name, ios::in|ios::binary);

    aoutfile.open(aoutfile_name,ios::out|ios::binary|ios::trunc);
    boutfile.open(boutfile_name,ios::out|ios::binary|ios::trunc);
    basis_match_outfile.open(basis_match_oufile_name,ios::out|ios::binary|ios::trunc);
    

    basis_match_flag =  0;
    basis_match_count = 0;
    ia = 0;
    ib = 0;

    //8th july data
    //shift = -9*500;
    //cwindow = 500;

    //14th july data
    //shift = -7*500;
    sscanf(argv[3],"%" SCNd64,&shift);
    //cwindow = 1500;
    sscanf(argv[2],"%" SCNd64,&cwindow);
    coincount = 0;
    error_count = 0;
   

    ainfile.read(reinterpret_cast<char *>(&a_ts),sizeof(a_ts));
    if (ainfile.eof()) exit(0); // EOF can only be detected after the final *failed* read attempt. 
    a_det  = a_ts & uint64_t(0xF);
    a_ts = a_ts>>4;
    begin_time = a_ts; 

    binfile.read(reinterpret_cast<char *>(&b_ts),sizeof(b_ts));
    if (binfile.eof()) exit(0);
    b_det = b_ts & uint64_t (0xF);
    b_ts = b_ts >>4;

    int debugloopcount = 0;
    int hv_count =0;
    int ad_count =0;
    int hv_error = 0;
    int ad_error = 0;
    //printf ("cwindow=%d\tshift=%d\n",(int)cwindow,(int)shift);
    while( 1 ){
        //if(debugloopcount ==34342903) break;
        //if(debugloopcount%10240 ==0) printf("debugcount at %d, coincidence count = %d\n",debugloopcount,coincount);
        //a_det = a_ts & uint64_t(0xF);
        //b_det = b_ts & uint64_t(0xF);

        //printf("ia= %d, ib=%d\n",(int)ia,(int)ib);
        //printf("a_ts =%" PRIu64 "\n", a_ts);
        //printf("b_ts =%" PRIu64 "\n", b_ts);

        diff = b_ts - a_ts + shift; 
        //printf("diff =%" PRId64 "\n\n", diff);
        
        if (abs(diff)<=cwindow/2){
            
            

            coincount +=1;
            aoutfile.write(reinterpret_cast<char *>(&ia),sizeof(ia));
            boutfile.write(reinterpret_cast<char *>(&ib),sizeof(ib));         
            //check basis match
            if ( (a_det & uint64_t(AD_BASIS)) && (b_det & uint16_t(AD_BASIS) )){
                basis_match_flag = 2;
                basis_match_count ++;

                 
                ad_count ++;
                if (a_det != b_det) {
                    error_count ++;
                    ad_error ++;
                }

            }
            else if( (a_det & uint64_t(HV_BASIS)) && (b_det & uint16_t(HV_BASIS)) ){
                basis_match_flag = 1;
                basis_match_count ++;
                hv_count ++;
                if (a_det != b_det ){
                    error_count ++;
                    hv_error ++;
                }  

            }
            else {
                basis_match_flag = 0;
            }

            // [ToDO] this is a characterwise bitmap. I should convert it into a bitwise bitmap
            basis_match_outfile.write(reinterpret_cast<char *>(&basis_match_flag),sizeof(basis_match_flag)); 

            //writebasis info



            ia+=1;
            ib+=1;
            
            ainfile.read(reinterpret_cast<char *>(&a_ts),sizeof(a_ts));
            if (ainfile.eof()) break; // EOF can only be detected after the final *failed* read attempt. 
            a_det = a_ts & uint64_t (0xF);
            a_ts = a_ts>>4;
            end_time = a_ts; 

            binfile.read(reinterpret_cast<char *>(&b_ts),sizeof(b_ts));
            if (binfile.eof()) break;
            b_det = b_ts & uint64_t (0xF);
            b_ts = b_ts >>4;
            
            //debug purpose
          
        }
        else if (diff>cwindow/2){
            //printf("here!\n");
            //printf("cwindow =%" PRId64 "\n\n", cwindow);
            ia+=1;
            ainfile.read(reinterpret_cast<char *>(&a_ts),sizeof(a_ts));
            if (ainfile.eof()) break; // EOF can only be detected after the final *failed* read attempt. 
            a_det = a_ts & uint64_t (0xF);
            a_ts = a_ts>>4;
            end_time = a_ts;
        }
        else if ( diff < -cwindow/2){
            ib+=1;
            binfile.read(reinterpret_cast<char *>(&b_ts),sizeof(b_ts));
            if (binfile.eof()) break;
            b_det = b_ts & uint64_t (0xF);
            b_ts = b_ts>>4;
        }
        else{
            printf ("ERROR! I should not be here.\n");

            break;
        }
        //debugloopcount ++;
    
    }
    basis_match_outfile.close();
    ainfile.close();
    binfile.close();
    aoutfile.close();
    boutfile.close();
    
    duration = (end_time  - begin_time) * pow(10.0,-12);

    

    //input_filename,// alice_singles_rate, bob_singles_rate, coincidence_window(ps), coincidence_count_rate, sifted_key_length, num_error, QBER, hv_count,ad_count,alice_efficiency(%), bob_effeciency(%), duration 
    //Alice's detection effeciency = coincidencerate / Bob's singles rate. 
    
    // As of August 7, 2020 the hv detectors are connected to ad line and vice versa. So I swap the hv count and ad count order in the line of code below. 
    printf("%f,%f,%d,%f,%d,%d,%f,%d,%d,%f,%f,%f,%f,%f\n",double(ia)*1.0/duration,int(ib)*1.0/duration,(int)cwindow,double(coincount)*1.0/duration,basis_match_count,error_count,(error_count *1.0)/basis_match_count,ad_count,hv_count,(coincount*100.0)/ib,(coincount*100.0)/ia,duration, (ad_error*1.0)/ad_count, (hv_error*1.0)/hv_count );
    
    /*
    printf("Total coincidences found = %d\n",coincount);
    printf("Coincidence Windowv(ns) = %f\n\n",cwindow/1000.0);
    printf("Total Basis matched coincidence count = %d\n",basis_match_count);
    printf("Error count on matched basis = %d\n\n",error_count);
    printf("QBER \t= %f%%\n\n",(error_count *100.0)/basis_match_count);
    printf("hv_count \t= %d\n",hv_count);
    printf("ad_count \t= %d\n",ad_count);
    printf("hv+ad counts \t= %d\n\n",hv_count+ad_count);
    printf("Item scanned \tia=%d, ib=%d\n",(int)ia,(int)ib );
    printf("Alice effeciency \t= %f%%\n",(coincount*100.0)/ia);
    printf("Bob effeciency \t= %f%%\n",(coincount*100.0)/ib);
    */
    return 0;
}