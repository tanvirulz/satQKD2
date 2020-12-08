import ephem

import pandas as pd

import datetime
from matplotlib import pyplot as plt
import plotly.graph_objects as go

from plotly.subplots import make_subplots

from TimeTagger import FileReader
import numpy as np
import easygui
import numpy as np
import matplotlib.pyplot as plt

import os
from os import walk

def loadSwabianTimeStamps(fileIn, t = np.inf):
    
        file_reader = FileReader(fileIn)
        stamps = []
        channel = []
        
        
        while file_reader.hasData():
            n_events = 100000; # Number of events to read at once
            #n_events = 200000; # Number of events to read at once
            
            # Read at most n_events.
            data = file_reader.getData(n_events);  # data is an instance of TimeTagStreamBuffer

            channelTmp = data.getChannels()
            stampsTmp = data.getTimestamps()
          #  overflow_types = data.getEventTypes()   # TimeTag = 0, Error = 1, OverflowBegin = 2, OverflowEnd = 3, MissedEvents = 4
          #  missed_events = data.getMissedEvents()
            
            for i in range(len(channelTmp)):
                channel.append(channelTmp[i])
                stamps.append(stampsTmp[i])
            break
            if stamps[-1] - stamps[0] > t:
                break
        return stamps, channel

class TTfile:
    def __init__(self, fname ,first_time_entry=0,time_offset=0):
        
        self.fname = fname
        self.loss = int("-"+fname[6:8])
        self.first_time_entry = first_time_entry
        self.time_offset = first_time_entry

    def printdata(self):
        print (self.fname,self.loss,self.first_time_entry,self.time_offset)
        

def loadSwabianTimeStamps(fileIn, t = np.inf):
    
        file_reader = FileReader(fileIn)
        stamps = []
        channel = []
        
        
        while file_reader.hasData():
            n_events = 100; # Number of events to read at once
            #n_events = 200000; # Number of events to read at once
            
            # Read at most n_events.
            data = file_reader.getData(n_events);  # data is an instance of TimeTagStreamBuffer

            channelTmp = data.getChannels()
            stampsTmp = data.getTimestamps()
          #  overflow_types = data.getEventTypes()   # TimeTag = 0, Error = 1, OverflowBegin = 2, OverflowEnd = 3, MissedEvents = 4
          #  missed_events = data.getMissedEvents()
            
            for i in range(len(channelTmp)):
                channel.append(channelTmp[i])
                stamps.append(stampsTmp[i])
            break
            if stamps[-1] - stamps[0] > t:
                break
        return stamps, channel


input_directory = "/media/tanvir/SpooqyLab/Table_Top_Demo/23_10_2020"
coincidence_window = 1500 #pico-seconds

power_prefix = "3_88m"
work_directory = "23Oct2020_stitch"

#log_file_name = "result_" + work_directory+"_cw"+str(coincidence_window)+"ps.csv"

input_files = []
for (dirpath, dirnames, filenames) in walk(input_directory):
    input_files.extend(filenames)
    break

os.system("mkdir -p "+work_directory)


#fp = open(work_directory+"/"+log_file_name,"w")
#print(work_directory+log_file_name)
#fp.write("input_filename, alice_singles_rate, bob_singles_rate, coincidence_window(ps), coincidence_count_rate, sifted_key_length, num_error, QBER, hv_count,ad_count,alice_efficiency(%), bob_effeciency(%),duration(s),hv_QBER,ad_QBER\n")
#fp.close()

db = {}

for file in input_files:
    if ("B.ttbin" in file and power_prefix in file):
        #print (file_count-i, "files to go")
        print ("working on: "+file)
        stamps,channel = loadSwabianTimeStamps(input_directory+"/"+file)
        fileobj = TTfile(file,first_time_entry = stamps[0])
        db[fileobj.loss] = fileobj
        
        print (file,"[Done]")

df = pd.read_csv("stitched23Oct2020Data3_88mW.csv")

def stitch_half_second(input_directory,fileobj,write_at): #self, fname ,first_time_entry=0,time_offset=0):
    current_offset = fileobj.time_offset
    end_time = current_offset + int(0.5 * 10**12)
    #printf("srswabian [inputdir] [inputefile] [existing output directory name] [timeoffset] ");
    file = fileobj.fname
    
    print("./srswabian "+input_directory+" "+file+" "+work_directory + " " + str(int(current_offset)) +" "+ str(int(end_time)) +" "+ str(int(write_at)) )
    os.system("./srswabian "+input_directory+" "+file+" "+work_directory + " " + str(int(current_offset)) +" "+ str(int(end_time)) +" "+ str(int(write_at)) )
    #os.system("./a.out")
    fileobj.time_offset = end_time
    

write_at = int(1 * 10 ** 12)
for index,row in df.iterrows():
    
    loss = int(row["lossDB_Round"])
    fileobj = db[loss]
    stitch_half_second(input_directory,fileobj, write_at)
    write_at = write_at + int(0.5 * 10**12)


