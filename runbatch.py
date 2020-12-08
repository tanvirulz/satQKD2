import os
from os import walk
#input_directory = "./swabian/20200824_sample/33db"
#input_directory = "./test"
#input_directory = "/media/tanvir/QubeSat1_64/23_10_2020"
#input_directory = "/media/tanvir/SpooqyLab/Table_Top_Demo/30sep2020"
#input_directory = "/media/tanvir/QubeSat1_64/23_10_2020"
input_directory = "./dark"
coincidence_window = 1500 #pico-seconds


work_directory = "23Oct2020_dark"

log_file_name = "result_" + work_directory+"_cw"+str(coincidence_window)+"ps.csv"

input_files = []
for (dirpath, dirnames, filenames) in walk(input_directory):
    input_files.extend(filenames)
    break

os.system("mkdir -p "+work_directory)

print(input_directory)
print(input_files)

fp = open(work_directory+"/"+log_file_name,"w")
#print(work_directory+log_file_name)
fp.write("input_filename, alice_singles_rate, bob_singles_rate, coincidence_window(ps), coincidence_count_rate, sifted_key_length, num_error, QBER, hv_count,ad_count,alice_efficiency(%), bob_effeciency(%),duration(s),hv_QBER,ad_QBER\n")
fp.close()

file_count = 0
for file in input_files:
    if ("B.ttbin" in file):
        file_count +=1 
i = 0
for file in input_files:
    if ("B.ttbin" in file):
        print (file_count-i, "files to go")
        print ("working on: "+file)
        i+=1
        #./run.sh ./swabian/20200714_Swabian_Timestamp/data_2 4_1uW0dB.1.ttbin 14july0db 
        #os.system( "print '"+file+",'>> input_directory+"/"+log_file_name")
        os.system("./run.sh "+input_directory+" "+file+" "+work_directory+" "+log_file_name+" "+str(coincidence_window)+" "+"runbatch")
        print (file,"[Done]")
    
