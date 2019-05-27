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
echo "1/6"
$PROGRAM_NAME -V --suppress-signature -t 2 -o first_lookup_1 --mesh-min-area=1e-6 --static-refinment-steps 1 
echo "2/6"
$PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_2 --mesh-min-area=1e-6 --static-refinment-steps 2 
echo "3/6"
$PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_3 --mesh-min-area=1e-6 --static-refinment-steps 3 
echo "4/6"
$PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_4 --mesh-min-area=1e-6 --static-refinment-steps 4 
echo "5/6"
$PROGRAM_NAME    --suppress-signature -t 2 -o first_lookup_5 --mesh-min-area=1e-6 --static-refinment-steps 5 
echo "6/6"
$PROGRAM_NAME -V --suppress-signature -t 2 -o first_lookup_6 --mesh-min-area=1e-6 --static-refinment-steps 6 


echo ""
echo ""
echo ""
echo "SECOND EXPERIMENT: Mesh refinement radius"
echo "1/12"
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_1  --mesh-min-area=1e-6 --refinment-radius=0.02
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_2  --mesh-min-area=1e-6 --refinment-radius=0.05
echo "3/12"
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_3  --mesh-min-area=1e-6 --refinment-radius=0.08
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_4  --mesh-min-area=1e-6 --refinment-radius=0.11
echo "5/12"
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_5  --mesh-min-area=1e-6 --refinment-radius=0.14
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_6  --mesh-min-area=1e-6 --refinment-radius=0.17
echo "7/12"
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_7  --mesh-min-area=1e-6 --refinment-radius=0.20
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_8  --mesh-min-area=1e-6 --refinment-radius=0.23
echo "9/12"
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_9  --mesh-min-area=1e-6 --refinment-radius=0.26
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_10 --mesh-min-area=1e-6 --refinment-radius=0.29
echo "11/12"
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_11 --mesh-min-area=1e-6 --refinment-radius=0.32
$PROGRAM_NAME -V --suppress-signature -t 2 -o second_lookup_12 --mesh-min-area=1e-6 --refinment-radius=0.35

echo ""
echo ""
echo ""
echo "THIRD EXPERIMENT"
$PROGRAM_NAME -V --suppress-signature -t 2 -o third_lookup_1  --mesh-min-area=1e-6 --mesh-exp=2
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_2  --mesh-min-area=1e-6 --mesh-exp=3
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_3  --mesh-min-area=1e-6 --mesh-exp=4
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_4  --mesh-min-area=1e-6 --mesh-exp=5
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_5  --mesh-min-area=1e-6 --mesh-exp=6
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_6  --mesh-min-area=1e-6 --mesh-exp=7
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_7  --mesh-min-area=1e-6 --mesh-exp=8
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_8  --mesh-min-area=1e-6 --mesh-exp=9
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_9  --mesh-min-area=1e-6 --mesh-exp=10
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_10 --mesh-min-area=1e-6 --mesh-exp=11
$PROGRAM_NAME    --suppress-signature -t 2 -o third_lookup_11 --mesh-min-area=1e-6 --mesh-exp=12
$PROGRAM_NAME -V --suppress-signature -t 2 -o third_lookup_12 --mesh-min-area=1e-6 --mesh-exp=13


echo ""
echo ""
echo ""
echo "LAST EXPERIMENT"
$PROGRAM_NAME -V --suppress-signature -t 2 -o last_lookup_1  --mesh-min-area=1e-6     --mesh-exp=2  --refinment-radius=0.02 --static-refinment-steps 1  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_2  --mesh-min-area=1e-6     --mesh-exp=3  --refinment-radius=0.04 --static-refinment-steps 2  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_3  --mesh-min-area=1e-6     --mesh-exp=4  --refinment-radius=0.08 --static-refinment-steps 3  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_4  --mesh-min-area=1e-6     --mesh-exp=5  --refinment-radius=0.10 --static-refinment-steps 4  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_5  --mesh-min-area=1e-6     --mesh-exp=6  --refinment-radius=0.12 --static-refinment-steps 5  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_6  --mesh-min-area=1e-6     --mesh-exp=7  --refinment-radius=0.14 --static-refinment-steps 6  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_7  --mesh-min-area=1e-6     --mesh-exp=8  --refinment-radius=0.16 --static-refinment-steps 7  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_8  --mesh-min-area=1e-6     --mesh-exp=9  --refinment-radius=0.18 --static-refinment-steps 8  
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_9  --mesh-min-area=1e-6     --mesh-exp=10 --refinment-radius=0.20 --static-refinment-steps 8 
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_10 --mesh-min-area=1e-6     --mesh-exp=11 --refinment-radius=0.22 --static-refinment-steps 8 
$PROGRAM_NAME    --suppress-signature -t 2 -o last_lookup_11 --mesh-min-area=1e-6     --mesh-exp=12 --refinment-radius=0.24 --static-refinment-steps 8 
$PROGRAM_NAME -V --suppress-signature -t 2 -o last_lookup_12 --mesh-min-area=1e-6     --mesh-exp=13 --refinment-radius=0.26 --static-refinment-steps 8 