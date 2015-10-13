#
# pull data from sql, plot using matplotlib
# see http://stackoverflow.com/questions/18663746/matplotlib-multiple-lines-with-common-date-on-x-axis-solved

import sys
import os
import time
import math
import datetime
import MySQLdb as mdb
import numpy

# so matplotlib has to have some of the setup parameters _before_ pyplot
import matplotlib
matplotlib.use('agg')

#matplotlib.rcParams['figure.dpi'] = 100
#matplotlib.rcParams['figure.figsize'] = [10.24, 7.68]
matplotlib.rcParams['lines.linewidth'] = .8
matplotlib.rcParams['axes.color_cycle'] = ['r','g','b','k']
matplotlib.rcParams['axes.labelsize'] = 'large'
matplotlib.rcParams['font.size'] = 12
matplotlib.rcParams['grid.linestyle']='-'

import matplotlib.pyplot as plt

anomalies = 0

print "GraphTH.py V1.69 12/04/2013 WPNS",time.asctime(),

# open the database connection, read the last <many> seconds of data, put them in a Numpy array called Raw
DBconn = mdb.connect('localhost', 'root', 'Pass', 'Test')
cursor = DBconn.cursor()
# sql = "select ComputerTime,Temperature,Humidity from TempHumid where ComputerTime >= (unix_timestamp(now())-(60*60*24))"
sql = "SELECT * FROM Table1"


cursor.execute(sql)
Raw = numpy.fromiter(cursor.fetchall(), count=-1, dtype=[('', numpy.float)]*3)
#Raw = numpy.fromiter(cursor.fetchall(), count=-1, dtype=[('', numpy.float), ('', numpy.float), ('',numpy.float)])

print "\n", Raw, "\n"

Raw = Raw.view(numpy.float).reshape(-1, 3)

print "\n", Raw, "\n"

(samples,ports)=Raw.shape
print 'Samples: {}, DataPoints: {}'.format(samples,ports),
plotme=numpy.zeros((samples,ports-1)) # make an array the same shape minus the epoch numbers

# Ports indicate number of columns, date is one so subtract 1
# Samples is number of rows

for y in range(ports-1):
#    print y
    for x in range(samples-1):  # can't do last one, there's no (time) delta from previous sample
        seconds = Raw[x+1,0]-Raw[x,0]
        # if the number didn't overflow the counter
        plotme[x,y] = Raw[x,y+1]

    plotme[samples-1,y] = None # set last sample to "do not plot"

    #for x in range(samples-1):                   # go thru the dataset again
        #if (Raw[x+1,1] == -0.1):                 # if values are "reading failed" flag
            #plotme[x+1,0] = plotme[x,0]          # copy current sample over it
            #plotme[x+1,1] = plotme[x,1]          # for temperature and humidity both
            #anomalies += 1

        #if (abs(Raw[x+1,1]-Raw[x,1]) > 10):      # if temperature jumps more than 10 degrees in a minute
            #plotme[x+1,0] = plotme[x,0]          # copy current sample over it
            #plotme[x+1,1] = plotme[x,1]          # for temperature and humidity both
            #anomalies += 1

print "Anomalies: ",anomalies,

#print "\n", Raw, "\n"



#print "\n"
#print plotme #Plot me [x][0] contains first column.. 
#print "\n"

# get an array of adatetime objects (askewchan from stackoverflow, above)
dts = map(datetime.datetime.fromtimestamp, Raw[:,0])
#print "\n", dts, "\n"

# set up the plot details we want
plt.grid(True)
plt.ylabel('Random, Static')
plt.axis(ymax=100,ymin=0)
plt.xlabel(time.asctime())
plt.title("Random Int (Red), Static Float (Green)")
plt.hold(True)

# and some fiddly bits around formatting the X (date) axis
plt.gca().xaxis.set_major_formatter(matplotlib.dates.DateFormatter('%m/%d %H:%M'))
plt.gca().xaxis.set_major_locator(matplotlib.dates.HourLocator())
lines = plt.plot(dts,plotme[:,0])   # Only Random. Not static [1]
plt.gcf().autofmt_xdate()

FileName = '../graphics/graphs/test.png'
plt.savefig(FileName)

print '\nCopy to Web Server...\n'
Destination = '/var/www/graphics/test'
CurrentDir = os.getcwd()
os.system("cp " + CurrentDir + "/" + FileName + " {}".format(Destination))
print 'Done at',time.asctime()
