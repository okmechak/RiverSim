#!/bin/bash

#----------------------------
#function definitions section
#----------------------------

function display_help()
{
    echo "Usage: $0 [option...] /riversim/program/path(mandatory) /output/path/of/simulation=plookup_{unix_time} number-of-threads=3" >&2
    echo
    echo "OPTIONS:"
    echo "   -h, --help           prints this help"
    echo
    echo "POSITIONAL PARAMETERS:"
    echo "   /riversim/program/path(mandatory) - you should specify location of RiverSim program in file system."
    echo "   /output/path/of/simulation - output path of simulation result. It also will append unix time in secods."
    echo "                                by default it is plookup_{unix_time}."
    echo "   number-of-threads - number of simultaneous simulation running. Default is 3."
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

function get_number_of_threads()
{
    if [ -z $1 ]
    then
        echo "3"
    else
        echo "$1"
    fi
}

function check_output_folder()
{
    if [[ ! -d $1 ]]
    then
        echo "There is no such folder: $1."
        read -p "Do you want to create it? Y/N " -n 1 -r
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


#MULTITHREADING
# example from stackoverflow
# https://stackoverflow.com/questions/6593531/running-a-limited-number-of-child-processes-in-parallel-in-bash

# means: run background processes in a separate processes...
set -m 
# execute add_next_task when we receive a child complete signal
trap add_next_task CHLD 

function add_next_task 
{
    # if still tasks to do then add one
    if [[ $index -lt ${#task_array[*]} ]]
    # apparently stackoverflow doesn't like bash syntax
    # the hash in the if is not a comment - rather it's bash awkward way of getting its length
    then
        echo "adding task $index"
        do_task "${task_array[$index]}" "$index" & 
        # replace the line above with the command you want
        index=$(($index+1))
    fi
}

function do_task {
    echo -e "\tstarting task index: $2"
    echo -e "\t\t eval $1"
    sleep 2
    echo -e "\tend of task index: $2"
}

#----------------------------
#program section
#----------------------------

#IITIALIZATION

echo "Precision parameter lookup."
handle_help_option $@
check_program_location_from_first_param $1
PROGRAM_NAME=$1 #first parameter should be program path

full_program_version=$($PROGRAM_NAME -v)
echo $full_program_version
required_version_str="2.4.1"

minimal_version_check "$full_program_version" "$required_version_str"

if [[ -z "$2" ]] 
then
    echo "No output folder specified. Current($PWD) will be used."
    output_folder="plookup"
else
    output_folder=$2
fi
output_folder="${output_folder}_$(date +%s)"
check_output_folder $output_folder



# Currently next parameters has the biggest impact on simulation accuracy(imho): mesh-min-area, mesh-exp, refinment-radius and stati-refinment-steps.
# these parameters handles mesh refinement aroud tips points.
# 1 - lets at firts gradualy increase static-refinment-steps.
# 2 - than gradualy increase refinment-radius
# 3 - than gradualy increase mesh-min-area
# and the last one is combination of all above
# 4 - all parameters increas


#MAIN CYCLE
# creation of task array

#some default values
simulation_type=2 #development case only values around tip
mesh_min_area_value=1e-6 
file_prefix=${output_folder}"/simdata"

task_index=0


#static-refinment-steps cycle
for st_ref_step_val in $(seq 1 7) #static-refinment-steps cycle
do
    task_array[$task_index]="$PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_st_ref_steps_${st_ref_step_val} --mesh-min-area=$mesh_min_area_value --static-refinment-steps $st_ref_step_val"
    task_index=$(($task_index+1))
done

#refinment-radius cycle
for ref_rad_value in $(seq 0.02 0.02 0.8) #static-refinment-steps cycle
do
    task_array[$task_index]="$PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_ref_rad_value_${ref_rad_value} --mesh-min-area=$mesh_min_area_value --refinment-radius=$ref_rad_value"
    task_index=$(($task_index+1))
done

#mesh-min-area cycle
for mesh_min_area_val in 1e-6 3e-7 1e-7 3e-8 1e-8 3e-9 1e-9 3e-10 1e-10  #static-refinment-steps cycle
do
    task_array[$task_index]="$PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_mesh_min_area_val_${mesh_min_area_val} --mesh-min-area=$mesh_min_area_val"
    task_index=$(($task_index+1))
done

#mesh-exp cycle
for mesh_exp_val in $(seq 1 2 30)  #static-refinment-steps cycle
do
    task_array[$task_index]="$PROGRAM_NAME -V --suppress-signature -t $simulation_type -o ${file_prefix}_mesh_min_area_val_${mesh_min_area_val} --mesh-min-area=$mesh_min_area_val"
    task_index=$(($task_index+1))
done

# end of creation task_array
# now we have run all those tasks

if [[ ! ${#task_array[*]} -eq 0 ]]
then
    index=0
    max_tasks=$(get_number_of_threads $3)
    echo "max tasks count: $max_tasks"

    # add initial set of tasks
    echo "Initial add"
    while [[ $index -lt $max_tasks ]]
    do
        add_next_task
    done
    wait # wait for all tasks to complete
else
    echo "Task array: $task_array - is empty"
    exit 1
fi

echo "Done."