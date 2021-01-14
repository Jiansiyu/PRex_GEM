# UVa decoder
A modified version of the UVa MPD4 decoder. 

## main change

* changed the bank number and the tag 
* change the data search structure of the 'inputhandler.cpp' so as to decode the PRex data 
* changed the grephic out style

## pedestal generator instructions

### 1. check the whether the mapping match the prex mapping
### 2. change the config/gem.cfg file 
    # runType
    RUNTYPE: CalPedestal
    # pedestal
    SAVEPED: ./Pedestal/     // this is the file name of the generated pedestal
    # # Input File for physics analysis; NOT FOR OTHER TYPE ANALYSIS

    INPUTFILE:   //  the pedestal raw run file name


### 3. generate the pedestal
    ./mpd4_decoder

### 4. save the file to the prex database
* it will generate a file named PRex_Pedestal.txt
* this file is conpatible with the standard prex pedestal, need to replace the pedestal in the prex database

## New features 
### 1. add function to take standalone configure file
Option 1.
```c++
./mpd4_decoder

# it will load the default configure file which located in folder ./config/gem.cfg
```
Option 2.
```c++
./mpd4_decoder config.cfg
# it will load config.cfg instead of using the default ./config/gem.cfg file
```



## RUN LIST

#### 16-Jun-2019
* Pedestal Run 1371 20513

#### 23-Aug-2019
* Pedestal Run 2155, 21297
   
#### 03-Sep-2019
* Pedestal Run 2303, 21425
* HV off
* pulse trigger



# Current Work
- [x] add the function to take configure file when run the scrips
- [ ] check whay some of then dataframe have missing channels

# issues
- [ ] missing channel for some frame 
- [ ] one HRS, the noise is much higher than the other