#!/bin/bash

filesPerDay=3
dst_files_gen1=""
dst_files_own=""


days="`seq 63 1 63`"
# hours="`seq 0 5 23`"
hours="0 5"
cur_run="mar19"

for day in $days; do
	ddd=`printf "%03d" $day`
	for hour in $hours; do
		yy=`echo $cur_run | cut -c4-5`
		hh=`printf "%02d" $hour`
		dst_files_own+=`find /lustre/nyx/hades/user/dborisen/dstProduction/dst_${cur_run}/$ddd/init -name "be${yy}${ddd}${hh}*" | tr "\n" ","`
		dst_files_gen1+=`find /lustre/hebe/hades/dst/mar19/gen1/$ddd/root/ -name "be${yy}${ddd}${hh}*" |sort | sed -n "1,${filesPerDay}p" | tr "\n" ","` 
	done
done
echo "dst_files_own: $dst_files_own"
echo "dst_files_gen1: $dst_files_gen1"

INIT_AND_FILL=""
WIDTH_FIT=""
ADC_CALIB=""
ADC_CALIB_QA=""
EP_CALIB=""
RDISTS_REFILL=""

# INIT_AND_FILL="on"
# WIDTH_FIT="on"
ADC_CALIB="on"
ADC_CALIB_QA="on"
# EP_CALIB="on"
# RDISTS_REFILL="on"

# INITIALIZATION AND HISTS FILLING
if [ ! -z $INIT_AND_FILL ]; then
	./test run=$cur_run -initCal 							&& 
	./test run=$cur_run in=${dst_files_own}  -fillCells  
fi

# WIDTH FIT
if [ ! -z $WIDTH_FIT ]; then
	./test run=$cur_run -fit 								&&
	for day in $days; do
		day_f=`printf "%03d" $day`
		if [ ! -d pdfs/$day_f ]; then 
			mkdir pdfs/$day_f
		fi
		./test run=$cur_run day=$day -printW				&&
		./test run=$cur_run day=$day -printZ1Z2
	done
fi   

# ADC calibration
if [[ $? -eq 0 && ! -z $ADC_CALIB ]]; then
	./test run=$cur_run -calibrate 						&&
	./test run=$cur_run -adc 								
fi

# VIEW OF ADC calibration RESULT 
if [[ $? -eq 0 && ! -z $ADC_CALIB_QA ]]; then
	 ./test run=$cur_run in=${dst_files_own} -recalibFill 	&&
	./test run=$cur_run  -printCh							&&
	./test run=$cur_run day=$day -panaramas 
fi


# EP CALIBRATION
if [[ $? -eq 0 && ! -z $EP_CALIB ]]; then
	./test run=$cur_run -genEPpars 
fi

# RDISTS refilling
if [[ $? -eq 0 && ! -z $RDISTS_REFILL ]]; then
	./test run=$cur_run in=${dst_files_gen1} -rRefil
fi


# VIEW OF EP CALIBRATION RESULT
# ./test run=$cur_run in=${dst_files_own}  -fillEp &&
# ./test run=$cur_run -writeEp



