#!/bin/bash

#----------------------------
#function definitions section
#----------------------------

function display_help()
{
    echo "Usage: $0 [option...] number-of-threads <array of tasks to run in parallel threads>" >&2
    echo
    echo "OPTIONS:"
    echo "   -h, --help           prints this help"
    echo
    echo "POSITIONAL PARAMETERS:"
    echo "   number-of-threads - number of parallel threads"
    echo "   <array of tasks> - arrau of task to run in parallel, for example it can be ../riversim -n 10   ../riversim -n 11 etc."
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

function check_number_of_threads()
{
    if ! [[ "$1" =~ ^[0-9]+$ ]]
    then
        echo "Sorry integers only for number of threads"
        exit 0
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
    echo $1
    eval $1
    echo -e "\tend of task index: $2"
}

#----------------------------
#program section
#----------------------------

#IITIALIZATION

echo "Parallel run."
handle_help_option $@


#MAIN CYCLE
# creation of task array
task_array=(${@:2})
echo ${task_array[*]}


# now we have run all those tasks

if [[ ! ${#task_array[*]} -eq 0 ]]
then
    index=0
    check_number_of_threads $1
    max_tasks=$1
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