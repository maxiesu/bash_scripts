import datetime
import sys

f = open("execution_times_for_5_10_20_30_50_wells_definitive.txt", "r")
a = []
sec = []
mins = []
hrs = []
averages = []
test_labels = []
for line in f:
    if line != "\n":
        if "test number" in line:
            test_names = line.split('of')[1]
            test_names = test_names.split('\n')[0]
        if "Solve time" in line: 
            characters = line.split(' ')[2:5]
            hrs.append(float(characters[0].split('h')[0]))
            mins.append(float(characters[1].split('m')[0]))
            sec.append(float(characters[2].split('s')[0]))
    else:
        if hrs != []:
            averages.append(sum([sum(sec)/len(sec), 60* sum(mins)/len(mins), 60*60*sum(hrs)/len(hrs)]))
            hrs = []
            mins = []
            sec = []
            test_labels.append(test_names)
#print(test_labels)

calculated_averages = []
for m in averages :
    if  "." in str(datetime.timedelta(seconds=m)) : 
        dt = str(datetime.timedelta(seconds=m)).split(".")[0] + "." + str(datetime.timedelta(seconds=m)).split(".")[1][0:2]
        #print(dt)
        calculated_averages.append(dt)
    else : 
        dt = str(datetime.timedelta(seconds=m)) + ".00"
        #print(dt)
        calculated_averages.append(dt)

labeled_averages = []
for i in range(0,len(calculated_averages)):
    s = test_labels[i] + ": " + calculated_averages[i]
    labeled_averages.append(s)
    if " 50_wells" in s:
        labeled_averages.append(" ")
    
with open('averages_of_performed_tests.txt', 'w') as f:
    print(*labeled_averages, sep = "\n", file=f)
