#include<stdio.h>
#include<inttypes.h>
#include<stdlib.h>
#include<time.h>

#include "mtwister.h"


//default block size 32
#define STARTING_BLOCK_SIZE (1<<4)
#define BASE_SEED 1000
//#define NUM_SHUFFLING 5

//[attention!] global variables
int BLOCK_SIZE = STARTING_BLOCK_SIZE ;
int num_message_alice2bob; //[TODO] bad practice. Later convert it to local variable
FILE * outfp;
int total_existing_errors;


typedef struct rawkey_buffer_type{
    int length;
    uint8_t * rk; //rawkey buffer array  
} rawkey;

typedef struct parity_message_type{
    int l; //low
    int h; //high
    uint8_t pl; //paritylow
    uint8_t ph; //parityhigh
} parity_message;

typedef struct response_message_type{
    int l;
    int h;
    uint8_t missmatch ; //missmatch ==0 if lower half missmatch and 1 if higher half. missmatch==3 indicates no missmatch.
}response_message;


void init_rawkey_buffer(rawkey* a , int n){
    a->length = n;
    a->rk = (uint8_t *) malloc(n*sizeof(uint8_t));

}



uint8_t get_parity( uint8_t* s, int l, int h){
    //This function needs to be updated to support bitwise operations. 
    int i, j;
    uint8_t p;
    p = 0;
    for (i = l;i<h;i++){
        p+= s[i];

    }
    p %= 2;
    return p;
}

void load_data(char* file_name, rawkey * a_key, rawkey* b_key){
    FILE * fp;
    int i, n;
    char c;
    fp = fopen(file_name,"r");
    
    fscanf(fp,"%d\n",&n);
    init_rawkey_buffer(a_key,n);
    init_rawkey_buffer(b_key,n);
    for(i=0;i<n;i++){
        fscanf(fp," %c",&c);
        a_key->rk[i] = (uint8_t)(c - '0');
    }
    for(i=0;i<n;i++){
        fscanf(fp," %c",&c);
        b_key->rk[i] = (uint8_t)(c - '0');
    }
    fclose(fp);

}

void print_buffer(rawkey *buff){
    int i;
    for (i=0; i<buff->length; i++){
        //printf("%d",(int)buff->rk[i]);
    }
    printf("\n");
}

void init_parity_message_bunch(rawkey* alice,parity_message** pmb){
    int block_count;
    //int i,l,h;
    block_count = alice->length / BLOCK_SIZE ;
    //printf("block_count=%d\n",block_count);
    *pmb = (parity_message * )malloc(sizeof(parity_message)*block_count);
    //printf("parity init\n");

}

void init_response_message_bunch(rawkey* bob, response_message ** rmb){
    int block_count;
    block_count = bob->length / BLOCK_SIZE ;
    *rmb = (response_message * )malloc(sizeof(response_message)* block_count);
    //printf("response init\n");
}

void print_bunched_error_bit(rawkey * buff, int block_index, int error_location){
    int i;
    if(error_location <0){
        
        for(i=block_index * BLOCK_SIZE ; i< (block_index+1) * BLOCK_SIZE;i++){
            //printf("%d",(int)buff->rk[i]);
        }
        printf("\n");
        return ;
    }
    for (i=block_index * BLOCK_SIZE; i<error_location; i++){
       // printf("%d",(int)buff->rk[i]);
    };
    //printf (" %d ",(int)buff->rk[error_location]);
    for (i=error_location+1; i<(block_index+1)*BLOCK_SIZE; i++){
        //printf("%d",(int)buff->rk[i]);
    };
    printf("\n");

}



void print_error_bit(rawkey * buff,int error_location){
    int i;
    
    for (i=0; i<error_location; i++){
        //printf("%d",(int)buff->rk[i]);
    };
    
    //printf (" %d ",(int)buff->rk[error_location]);
    
    for (i=error_location+1; i<buff->length; i++){
        //printf("%d",(int)buff->rk[i]);
    };
    //printf("\n");
    

}


void print_parity_message(parity_message* pm){
    //printf("parity: alice -> bob\n");
    //printf( "l=%d, h=%d, block_length=%d\npl=%d\tph=%d\n\n",pm->l,pm->h,pm->h-pm->l,pm->pl,pm->ph);
}

void print_response_message(response_message* rm){
    //printf("response: bob -> alice\n");
    //printf( "l=%d, h=%d, block_length=%d\nmissmatch=%d\n\n",rm->l,rm->h,rm->h-rm->l,rm->missmatch);
}


void bob_cascade(rawkey* bob, parity_message * pm,response_message * rm){
    uint8_t bl, bh;
    int l = pm->l;
    int h = pm->h;
    bl = get_parity(bob->rk,l,(l+h)/2);
    bh = get_parity(bob->rk,(l+h)/2,h);
    rm->l = l;
    rm->h = h;
    //rm->missmatch = 3;
    if (bl!=pm->pl && bh != pm->ph){
        //printf("No error found! BOTH missmatch\n");
        //printf("bl=%d bh=%d\nal=%d ah=%d\n",bl,bh,pm->pl,pm->ph);
        //printf(" l=%d  h=%d\n",l,h);
        //printf("al=%d ah=%d\n",pm->l,pm->h);
        rm->missmatch = 3;
        //exit(0);
    }
    else if(bl!=pm->pl) {
        rm->missmatch = 0;
    }
    else if(bh != pm->ph){
        rm->missmatch = 1;
    }
    else{
        printf("No error found!\n");
        rm->missmatch = 3;
        //exit(0);
    }

}

void alice_cascade(rawkey * alice, parity_message* pm, response_message *rm){
    int l,h,m;
    //if (rm->missmatch ==3) return;
    m = (rm->l + rm->h)/2 ; 
    if (rm->missmatch == 3){
        return;
    }
    if (rm->missmatch == 0){
        l = rm->l;
        h = m ; 
    }
    else{
        l = m ;
        h = rm->h ; 
    }
    num_message_alice2bob ++; //a new parity message is computed by alice
    pm->l = l;
    pm->h = h; 
    //if (rm->missmatch ==3) return;
    pm->pl = get_parity(alice->rk,pm->l,(int)((pm->h+pm->l)/2) );
    pm->ph = get_parity(alice->rk,(int)((pm->h+pm->l)/2),pm->h );


}
void init_bunched_cascade(rawkey * alice, rawkey * bob, parity_message ** pmb, response_message ** rmb){

init_parity_message_bunch(alice,pmb);
init_response_message_bunch(bob,rmb);

}

void correct_bob(rawkey * buff, int block_index, int error_location){
    if(error_location<0){
        return;
    }
    else{
        buff->rk[error_location] = 1 - buff->rk[error_location] ;
    }
}

int run_bunched_cascade(rawkey * alice, rawkey * bob, parity_message ** pmb, response_message ** rmb){
    int current_block_size;
    int num_blocks;
    int i;
    int error_locaiton;
    int step_count;
    //int block_count;
    int l,h;
    int total_error_found = 0 ;
    //block_count = alice->length / BLOCK_SIZE ;

    parity_message *pm;
    response_message *rm;
    //init_parity_message_bunch(alice,pmb);
    //init_response_message_bunch(bob,rmb);

    

    current_block_size = BLOCK_SIZE ; 
    num_blocks = alice->length / BLOCK_SIZE ;

     for (int i=0;i<num_blocks;i++){
        l = i*BLOCK_SIZE;
        h = (i+1)*BLOCK_SIZE;
        
        (*pmb)[i].l = l;
        (*pmb)[i].h = h;
        printf("here l=%d\n",l);
        (*pmb)[i].pl = get_parity(alice->rk,l,(int)((h+l)/2) );

        (*pmb)[i].ph = get_parity(alice->rk,(int)((h+l)/2),h );        
    }
    num_message_alice2bob += num_blocks;

    //printf("alice->length = %d\n",alice->length);
    //printf("block size= %d\n",BLOCK_SIZE);
    //printf("num_blocks= %d\n",num_blocks);
    step_count = 0;
    while(1){
        step_count++ ;
        //printf("step:%d\n",step_count);
        for (i=0;i<num_blocks;i++){
            //pm = &((*pmb)[i]);
            pm = (*pmb) +i;
            //rm = &((*rmb)[i]);
            rm = (*rmb) + i;
            print_parity_message(pm);
            bob_cascade(bob,pm,rm);
        }
        if (current_block_size == 2){
            for (i=0;i<num_blocks;i++){
                //rm = &((*rmb)[i]);
                rm = (*rmb)+i;
                //error_locaiton = rm->l + rm->missmatch;
                if (rm->missmatch == 3) {
                    error_locaiton = -1;
                    //printf("No error in block: %d\n",i);
                }
                else{ 
                    error_locaiton = rm->l + rm->missmatch;
                    //printf("error found at index = %d\n",error_locaiton);
                    total_error_found++ ;
                }
                //printf("Alice->\t");
                print_bunched_error_bit(alice,i,error_locaiton);
                //print_error_bit(alice,rm);
                //printf("Bob->\t");
                print_bunched_error_bit(bob,i,error_locaiton);
                //print_error_bit(bob,rm);


                correct_bob(bob,i,error_locaiton);

                printf("BobC->\t");
                print_bunched_error_bit(bob,i,error_locaiton);

            }
            //printf("Binary ran for %d steps\n",step_count);
            break;
        }
        for (i=0;i<num_blocks;i++){
            //pm = &((*pmb)[i]);
            pm = (*pmb) +i;
            //pm = pmb[i];
            //rm = rmb[i];
            //rm = &((*rmb)[i]);
            rm = (*rmb) + i;
            print_response_message(rm);
            //print_parity_message(pm);
            alice_cascade(alice,pm,rm);
        }
        current_block_size /=2;
       
    }
    return total_error_found;
}

int run_cascade(rawkey * alice, rawkey * bob, parity_message * pm, response_message * rm){
    //int current_block_length = 0;
    int error_lcation =0;
    int step_count = 0;
    int total_error_found = 0;

    pm->l=0;
    pm->h = alice->length;
    pm->pl = get_parity(alice->rk,pm->l,(int)((pm->h+pm->l)/2) );
    pm->ph = get_parity(alice->rk,(int)((pm->h+pm->l)/2),pm->h );
    //print_parity_message(pm);
    
    while(1){
        step_count++;
        print_parity_message(pm);
        bob_cascade(bob,pm,rm);
        print_response_message(rm);
        
        if ((rm->h-rm->l) == 2){

            error_lcation = rm->l + rm->missmatch;
            printf("error found at index = %d\n",error_lcation);
            
            total_error_found++ ;

            printf("Alice->\t");
            print_error_bit(alice,error_lcation);
            printf("Bob->\t");
            print_error_bit(bob,error_lcation);
            printf("Binary ran for %d steps\n",step_count);
            break;
        } 
        alice_cascade(alice,pm,rm);
        //print_parity_message(pm);
    }
    
    return 1;
}

/*
void shuffle (rawkey * buff,int seed ){
    //implements Knuth Shuffle 
    int i;
    int a,b;
    uint8_t tc;
    srand(seed);
    for (i=0 ; i < buff->length-1 ; i++){
        a = i;
        b = a+ rand() % (buff->length - i );
        
        //swap a b
        tc = buff->rk[a];
        buff->rk[a] = buff->rk[b];
        buff->rk[b] = tc;
    }

}
*/

void shuffle (rawkey * buff,int seed ){
    //implements Knuth Shuffle 
    int i;
    int a,b;
    uint8_t tc;
    //srand(seed);
    MTRand r = seedRand(seed);
    for (i=0 ; i < buff->length-1 ; i++){
        a = i;
        b = a+ genRandLong(&r) % (buff->length - i );
        
        //swap a b
        tc = buff->rk[a];
        buff->rk[a] = buff->rk[b];
        buff->rk[b] = tc;
    }

}

void run_shuffled_bunched_cascade(rawkey * alice, rawkey * bob, parity_message ** pmb, response_message ** rmb, int number_of_shuffle){
    int i;
    int num_error_found = 0;
    num_message_alice2bob = 0;
    /*
    init_bunched_cascade(alice,bob,pmb,rmb);
    num_error_found += run_bunched_cascade(alice,bob,pmb, rmb);

    free(*pmb);
    free(*rmb);
    */
  
    for( i = 0;i<number_of_shuffle;i++){

        init_bunched_cascade(alice,bob,pmb,rmb);
        num_error_found += run_bunched_cascade(alice,bob,pmb, rmb);

        free(*pmb);
        free(*rmb);
        fprintf(outfp,"%d,%d,%0.2f,%d,%d,%d\n",i+1,num_error_found,(num_error_found *100.0)/total_existing_errors,num_message_alice2bob, num_message_alice2bob*9,num_message_alice2bob*2);
        if (num_error_found == total_existing_errors) break;

        printf("alice\n");
        print_buffer(alice);
            shuffle(alice,BASE_SEED+i);
        print_buffer(alice);

        printf("bob\n");
        print_buffer(bob);
            shuffle(bob,BASE_SEED+i);
        print_buffer(bob);
        
        if (i < 2){
            BLOCK_SIZE = BLOCK_SIZE * 2;

        }
        
        
    }
    printf("Total Number or errors found: %d\n",num_error_found);
    printf("number of messages sent from Alice to Bob: %d\n",num_message_alice2bob);
    printf("Total Bytes sent from Alice to Bob: %d\n",num_message_alice2bob*9); //[todo] encode the parities in the same byte
    printf("Total Bytes sent from Bob to Alice: %d\n",num_message_alice2bob*9); //[todo] see above
    printf("number of bits exposed: %d\n",num_message_alice2bob*2);
    //fprintf(outfp,"num_round,num_errors_detected,num_msg_a2b,bytes_a2b,bits_exposed\n")
    
}

int comparekey(rawkey * alice, rawkey *bob){
    int i;
    int mismatch = 0;
    for (i=0;i<alice->length;i++){

        if (alice->rk[i]!=bob->rk[i]) {
            mismatch ++;
        }
    }
    printf("key mismatch in %d positions\n",mismatch);
    return mismatch;
}


int main(int argc, char ** argv){
    rawkey alice;
    rawkey bob;
    parity_message pm;
    response_message rm;

    parity_message * parity_message_bunch;
    response_message * response_message_bunch;
    int number_of_shuffle = 0;
    int cascade_run_round = 0;
    int pa, pb;

    if (argc !=4 ){
        printf("Please enter [file name] [number of rounds] [outfilename] \n");
        return 0;
    }
    else{
        load_data(argv[1],&alice,&bob);
        sscanf(argv[2],"%d",&cascade_run_round);
    }
    if (cascade_run_round<=0){
        printf("You have to run the cascade for at least 1 round.\n");
        return 0;
    }

    number_of_shuffle = cascade_run_round; 

    outfp = fopen(argv[3],"w");
    fprintf(outfp,"num_round,num_errors_detected,percent_error_found,num_msg_a2b,bytes_a2b,bits_exposed\n");
    
    //load_data("input2.txt",&alice,&bob);

    print_buffer(&alice);
    print_buffer(&bob);
    total_existing_errors =  comparekey(&alice,&bob);

    //pa = get_parity(alice.rk,0,alice.length);
    //pb = get_parity(bob.rk,0,bob.length);
    //printf("pa = %d \npb =%d\n",pa,pb);

    //init_parity_message_bunch(&alice, &parity_message_bunch);

    //run_cascade(&alice,&bob,&pm,&rm);
    
    
    //init_bunched_cascade(&alice,&bob,&parity_message_bunch,&response_message_bunch);
    run_shuffled_bunched_cascade (&alice,&bob,&parity_message_bunch,&response_message_bunch, number_of_shuffle);
    
    comparekey(&alice,&bob);
    fclose(outfp);
    //run_bunched_cascade(&alice,&bob,&parity_message_bunch,&response_message_bunch);
    /*
    print_buffer(&alice);
    shuffle(&alice,BASE_SEED);
    print_buffer(&alice);
    printf("bob\n");
    print_buffer(&bob);
    shuffle(&bob,BASE_SEED);
    print_buffer(&bob);
    */
    //free(parity_message_bunch);
    //free(response_message_bunch);
    return 0;
}
