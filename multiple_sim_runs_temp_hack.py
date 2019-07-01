#!/usr/bin/env python

# Workaround for running many OPS simulations with "Segmentation fault" error
from time import sleep
from subprocess import call
import datetime

def main():
    runs = 600
    for x in range (1201, 1201+runs):
        modify_seed(x)
        if (x != 0) and (x != 1) and (x != 12) and (x != 13) and (x != 665):  # Skip problematic seeds
            call(["./ops-simu-run.sh", "-m", "cmdenv"])
            print ("### Simulation run %s" %x + " done! ###")
            if (x+1) == runs:
                reset_file(x)


def modify_seed(seed):
    # Open INI file and replace the line corresponding to the seed number #
    ini_file = 'simulations/master_thesis_trends.ini'
    # Read in the file
    with open(ini_file, 'r') as file :
        filedata = file.read()

    # Replace the target string
    if seed == 1201:
        filedata = filedata.replace('seed-0-mt = ${{repetition}}'.format(42), 'seed-0-mt = ' + str(seed) )
    else:
        filedata = filedata.replace('seed-0-mt = ' + str((seed - 1)), 'seed-0-mt = ' + str(seed) )
  
    # Write the file out again
    with open(ini_file, 'w') as file:
        file.write(filedata)


def reset_file(seed):
    ini_file = 'simulations/master_thesis_trends.ini'
    # Read in the file
    with open(ini_file, 'r') as file :
        filedata = file.read()
        filedata =  filedata.replace('seed-0-mt = ' + str(seed), 'seed-0-mt = ${{repetition}}'.format(42))
    
    # Write the file out again
    with open(ini_file, 'w') as file:
        file.write(filedata)
        
        
if __name__ == '__main__':
    main()
