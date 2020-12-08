#!/bin/bash

mkdir -p $3 

#./rrswabian $1 $2 $3

#correction for Aaice ./crt [workDirectory] [H_Offset] [V_Offset] [A_Offset] [D_Offset]
./crt $3 alice 0 -750 -1250 -250


rm $3/alice.out

#correction for Bob ./crt [workDirectory] [H_Offset] [V_Offset] [A_Offset] [D_Offset]
./crt $3 bob 0 0 250 250



rm $3/bob.out

if [ -z "$6" ]
    then
        printf  "input_filename, alice_singles_rate, bob_singles_rate, coincidence_window(ps), coincidence_count_rate, sifted_key_length, num_error, QBER, hv_count,ad_count,alice_efficiency(%%), bob_effeciency(%%),duration(s),hv_QBER,ad_qber\n" > ./$3/$4
fi 


printf "$2," >> $3/$4

# ./cm [workDirectory] [coincidenceWindow] [matchingShift]
./cm $3 $5 -7500 >> $3/$4

# To create the raw key and the basis choices 
./grb $3 alice 
./grb $3 bob 

#If you want the sifted key to be written in a file uncomment the two line below. 
#./grb $3 alice 
#./grb $3 bob 

rm $3/alice_corrected.out
rm $3/bob_corrected.out
rm $3/alice_coin.out
rm $3/bob_coin.out
rm $3/basis_match_bitmask.out


