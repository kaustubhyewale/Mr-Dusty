'''	
	Group Name:- Rutherford @Core2Web Technology
	Project Name:- The Cleaner-Mr Dusty

'''

#import libraries
import sys
import json
import urllib.request
from time import sleep
import datetimefrom win10toast import ToastNotifier



toaster = ToastNotifier()

# Enter Your API key here
writeAPI = '7NBQ69K8AQU59B4I'

# URL where we will send the data, Don't change it
baseURL = 'https://api.thingspeak.com/update?api_key=%s' % writeAPI


#Automatic cleaning 
def automatic():
    while True:
        setTime = input("set Daily Cleaning Time(hh:mm) : ")

        setTime = setTime.split(':')
        print(setTime)

        if int(setTime[0]) <= 24 and int(setTime[1]) <= 60:
            break

        else:
            print("Time Format is Wrong ...Please try Again.")

     

    with urllib.request.urlopen(baseURL + '&field1=%d&field2=%d&field3=%d&field4=%d' % (int(setTime[0]),int(setTime[1]),0,0)) as url:     
        s = url.read()
        print(s)

    
    entry_id = [int(i) for i in s.split() if i.isdigit()] 
    print(entry_id[0])

    return entry_id[0],setTime
        


#For manual cleani
def manual():
    
    with urllib.request.urlopen(baseURL + '&field1=%d&field2=%d&field3=%d&field4=%d' % (-1,-1,1,0)) as url:     
        s = url.read()
        print(s)

    entry_id = [int(i) for i in s.split() if i.isdigit()] 
    print(entry_id[0])

    return entry_id[0]
        


def read(old_id):

    while True:

        with urllib.request.urlopen("https://api.thingspeak.com/channels/844803/feeds.json?api_key=MQ5JN9YWFXAV61D9&results=1") as url:
            response = url.read()

        data = json.loads(response)

            
        subData=data['feeds']

        for val in subData:
            entry_id = val['entry_id']
            dateVal = val['created_at']
            status = int(val['field4'])

            
        if entry_id == old_id:
            sleep(5)
            continue
        
        
        if status == 1:

            print("status = ",status)
    

            date=''
            time=''
            set = 0
            for i in dateVal:

                if i != 'T' and set == 0 :
                    date+=i

                elif i == 'T':
                    set=1

                elif set==1 and i != 'Z':
                    time+=i

            time = time.split(':')

            if(int(time[1])+30 > 59):
                time[0] = int(time[0])+1
                

            timeInd = (str(int(time[0])+5-24) if(int(time[0])+5 > 24) else str(int(time[0])+5)) +":"+ (str(int(time[1])+30-60) if(int(time[1])+30 > 59) else str(int(time[1])+30)) +":"+ time[2]            

            print("Cleaning is Done")
            print("At Ind Time = ",timeInd)
                    
            print("Date = ",date)

            toaster.show_toast("Mr. Cleaner-The DUSTY...",
                   "Cleaning is Done on "+date+" at "+timeInd+"'...",
                   icon_path=r"rutherford.ico",
                   duration=10)

            
            break



#main function
def main():

    print("Menu\n 1: Automatic \n 2: Manual \n")
    n=int(input("Enter Choice: "))

    if n==1:
        E_id,time = automatic()      
        read(E_id)
        sleep(5)

    elif n==2:
       E_id = manual()
       read(E_id)
       sleep(5)


#call the main function
main()
