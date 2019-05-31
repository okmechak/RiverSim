#!/bin/bash

PROGRAM_NAME=none
if [ -z "$1" ]; then
    echo "Please specifie as first parameter path to the riversim program."
    exit 1
else
    PROGRAM_NAME=$1
fi
echo "$PROGRAM_NAME"

version=$($PROGRAM_NAME -v)
required_version="v2.4.1"
index=$(expr index "$version" "$required_version")
if [ "$index" -eq "0" ];then
    echo "Your program version is"  "$version" ", but required version is" "$required_version" 
    read -p "Do you want to continue?Y/N " -n 1 -r
    echo    # (optional) move to a new line
    if [[ $REPLY =~ ^[Nn]$ ]];then
        echo Exit
        exit 1
    fi
    echo Continue
fi


echo "v2.4.1 test"
echo "currently next parameters has the biggest impact on simulation: mesh-exp, refinment-radius and stati-refinment-steps."
echo "these parameters handles mesh refinement aroud tips points."
echo "1 - lets at firts gradualy increase static-refinment-steps."
echo "2 - than gradualy increase refinment-radius"
echo "3 - than gradualy increase mesh-exp"
echo "and the last one is combination of all above"
echo "4 - all parameters increas"

echo ""
echo ""
echo ""
echo "FIRST EXPERIMENT"
echo "1/7"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o first_lookup_1 --mesh-min-area=1e-6 --static-refinment-steps 1 &
echo "2/7"
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_2 --mesh-min-area=1e-6 --static-refinment-steps 2 &
echo "3/7"
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_3 --mesh-min-area=1e-6 --static-refinment-steps 3 &
echo "4/7"
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_4 --mesh-min-area=1e-6 --static-refinment-steps 4 &
echo "5/7"
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_5 --mesh-min-area=1e-6 --static-refinment-steps 5 &
echo "6/7"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o first_lookup_6 --mesh-min-area=1e-6 --static-refinment-steps 6 &
echo "6/7"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o first_lookup_6 --mesh-min-area=1e-6 --static-refinment-steps 7 &

echo ""
echo ""
echo ""
echo "SECOND EXPERIMENT: Mesh refinement radius"
echo "1/12"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_1  --mesh-min-area=1e-6 --refinment-radius=0.02 &
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_2  --mesh-min-area=1e-6 --refinment-radius=0.05 &
echo "3/12"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_3  --mesh-min-area=1e-6 --refinment-radius=0.10 &
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_4  --mesh-min-area=1e-6 --refinment-radius=0.16 &
echo "5/12"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_5  --mesh-min-area=1e-6 --refinment-radius=0.14 &
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_6  --mesh-min-area=1e-6 --refinment-radius=0.25 &
echo "7/12"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_7  --mesh-min-area=1e-6 --refinment-radius=0.35 &
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_8  --mesh-min-area=1e-6 --refinment-radius=0.40 &
echo "9/12"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_9  --mesh-min-area=1e-6 --refinment-radius=0.5 &
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_10 --mesh-min-area=1e-6 --refinment-radius=0.6 &
echo "11/12"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_11 --mesh-min-area=1e-6 --refinment-radius=0.7 &
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_12 --mesh-min-area=1e-6 --refinment-radius=0.8 &


echo ""
echo ""
echo ""
echo "LAST EXPERIMENT"
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o last_lookup_1  --mesh-min-area=1e-6  --refinment-radius=0.02 --static-refinment-steps 1 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_2  --mesh-min-area=1e-6  --refinment-radius=0.04 --static-refinment-steps 2 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_3  --mesh-min-area=1e-6  --refinment-radius=0.08 --static-refinment-steps 3 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_4  --mesh-min-area=1e-6  --refinment-radius=0.10 --static-refinment-steps 4 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_5  --mesh-min-area=1e-6  --refinment-radius=0.12 --static-refinment-steps 5 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_6  --mesh-min-area=1e-6  --refinment-radius=0.14 --static-refinment-steps 6 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_7  --mesh-min-area=1e-6  --refinment-radius=0.16 --static-refinment-steps 7 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_8  --mesh-min-area=1e-6  --refinment-radius=0.18 --static-refinment-steps 8 &    
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_9  --mesh-min-area=1e-6  --refinment-radius=0.20 --static-refinment-steps 9 &   
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_10 --mesh-min-area=1e-6  --refinment-radius=0.32 --static-refinment-steps 9 &   
nohup $PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_11 --mesh-min-area=1e-6  --refinment-radius=0.44 --static-refinment-steps 9 &  
nohup $PROGRAM_NAME -V --suppress-signature -t 2 -o last_lookup_12 --mesh-min-area=1e-6  --refinment-radius=0.56 --static-refinment-steps 9 &   