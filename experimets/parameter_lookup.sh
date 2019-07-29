#!/bin/bash

#----------------------------
#function definitions section
#----------------------------

function display_help()
{
    echo "Usage: $0 [option...] /riversim/program/path " >&2
    echo
    echo "   -h, --help           prints this help"
}

function handle_help_option()
{
    for param in $@
    do
        if [[ "$param" == "--help" || "$param" == "-h" ]]
        then
            display_help
            exit 0
        fi
    done
}

function check_program_location_from_first_param()
{
    if [[ ! -x $1 ]]
    then
        echo "Please specifie as first parameter path to the riversim program. Currently it is equal to: $1"
        exit 1
    fi
}

function check_output_folder_from_second_param()
{
    if [[ ! -d $1 ]]
    then
        echo "There is no such folder: $1."
        read -p "Do you want to create it?Y/N " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]
        then
            if ! mkdir $1
            then
                echo "Folder $1 can't be created."
                exit 1
            else 
                echo "Folder $1 created successfully."
            fi
        else
            exit 0
        fi
    fi
}

function minimal_version_check()
{
    #handling of program version
    
    version_str=$1
    version_str=${version_str:23:5} #select only version numbers
    
    if [[ $version_str < $2 ]]
    then
        echo "Your program version is $version_str, but required version is $2" 
        read -p "Do you want to continue?Y/N " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Nn]$ ]]
        then
            exit 0
        fi
        echo Continue...
    fi
}

#----------------------------
#program section
#----------------------------

#IITIALIZATION

echo "Precision parameter lookup."

handle_help_option $@
check_program_location_from_first_param $1
PROGRAM_NAME=$1 #first parameter should be program path
if [[ -z "$2" ]] 
then
    echo "no folder"
    output_folder="."
else
    echo "folder" $#
    check_output_folder_from_second_param $2
    output_folder=$2
fi

full_program_version=$($PROGRAM_NAME -v)
echo $full_program_version
required_version_str="2.4.1"

minimal_version_check "$full_program_version" "$required_version_str"



echo "Currently next parameters has the biggest impact on simulation accuracy(imho): mesh-exp, refinment-radius and stati-refinment-steps."
echo "these parameters handles mesh refinement aroud tips points."
echo "1 - lets at firts gradualy increase static-refinment-steps."
echo "2 - than gradualy increase refinment-radius"
echo "3 - than gradualy increase mesh-exp"
echo "and the last one is combination of all above"
echo "4 - all parameters increas"


#MAIN CYCLE

#some default values
simulation_type=2 #development case only values around tip
mesh_min_area_value=1e-6 
file_prefix=${output_folder}"/lookup"

echo
echo "static-refinment-steps cycle"
for st_ref_step_val in $(seq 1 7) #static-refinment-steps cycle
do
    $PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_st_ref_steps_${st_ref_step_val} --mesh-min-area=$mesh_min_area_value --static-refinment-steps $st_ref_step_val
done

echo
echo "refinment-radius cycle"
for ref_rad_value in $(seq 0.02 0.02 0.8) #static-refinment-steps cycle
do
    $PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_ref_rad_value_${ref_rad_value} --mesh-min-area=$mesh_min_area_value --refinment-radius=$ref_rad_value
done

echo
echo "mesh-min-area cycle"
for mesh_min_area_val in 1e-6 3e-7 1e-7 3e-8 1e-8 3e-9 1e-9 3e-10 1e-10  #static-refinment-steps cycle
do
    $PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_mesh_min_area_val_${mesh_min_area_val} --mesh-min-area=$mesh_min_area_val
done


# TODO make array and paralellization quene