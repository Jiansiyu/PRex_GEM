#!/bin/bash
#export DB_DIR=/home/newdriver/Storage/Research/PRex_Experiment/prex_analyzer/PRex_Database/DB_GEM_check
export DB_DIR=/home/newdriver/Storage/Research/PRex_GEM/PRex_Replay/PRex_Database/PRex_Database
export MPDGEM=/home/newdriver/Storage/Research/PRex_GEM/PRex_Replay/PRexAnalyzer
export LD_LIBRARY_PATH=$DB_DIR:$MPDGEM:$LD_LIBRARY_PATH
export PATH=$DB_DIR:$MPDGEM:$PATH
