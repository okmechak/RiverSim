echo "v2.4.1 test"
echo "currently next parameters has the biggest impact on simulation: mesh-exp, refinment-radius and stati-refinment-steps."
echo "these parameters handles mesh refinement aroud tips points."
echo "1 - lets at firts gradualy increase static-refinment-steps."
echo "2 - than gradualy increase refinment-radius"
echo "3 - than gradualy increase mesh-exp"
echo "and the last one is combination of all above"
echo "4 - all parameters increas"

echo "FIRST EXPERIMENT"
./source/riversim -V -t 2 -o first_lookup_1 --static-refinement-steps 1
./source/riversim -V -t 2 -o first_lookup_2 --static-refinement-steps 2
./source/riversim -V -t 2 -o first_lookup_3 --static-refinement-steps 3
./source/riversim -V -t 2 -o first_lookup_4 --static-refinement-steps 4
./source/riversim -V -t 2 -o first_lookup_5 --static-refinement-steps 5
./source/riversim -V -t 2 -o first_lookup_6 --static-refinement-steps 6
./source/riversim -V -t 2 -o first_lookup_7 --static-refinement-steps 7

echo "SECOND EXPERIMENT"
./source/riversim -V -t 2 -o second_lookup_1  --refinment-radius = 0.02
./source/riversim -V -t 2 -o second_lookup_2  --refinment-radius = 0.05
./source/riversim -V -t 2 -o second_lookup_3  --refinment-radius = 0.08
./source/riversim -V -t 2 -o second_lookup_4  --refinment-radius = 0.11
./source/riversim -V -t 2 -o second_lookup_5  --refinment-radius = 0.14
./source/riversim -V -t 2 -o second_lookup_6  --refinment-radius = 0.17
./source/riversim -V -t 2 -o second_lookup_7  --refinment-radius = 0.20
./source/riversim -V -t 2 -o second_lookup_8  --refinment-radius = 0.23
./source/riversim -V -t 2 -o second_lookup_9  --refinment-radius = 0.26
./source/riversim -V -t 2 -o second_lookup_10 --refinment-radius = 0.29
./source/riversim -V -t 2 -o second_lookup_11 --refinment-radius = 0.32
./source/riversim -V -t 2 -o second_lookup_12 --refinment-radius = 0.35

echo "THIRD EXPERIMENT"
./source/riversim -V -t 2 -o third_lookup_1  --mesh-exp = 2
./source/riversim -V -t 2 -o third_lookup_2  --mesh-exp = 3
./source/riversim -V -t 2 -o third_lookup_3  --mesh-exp = 4
./source/riversim -V -t 2 -o third_lookup_4  --mesh-exp = 5
./source/riversim -V -t 2 -o third_lookup_5  --mesh-exp = 6
./source/riversim -V -t 2 -o third_lookup_6  --mesh-exp = 7
./source/riversim -V -t 2 -o third_lookup_7  --mesh-exp = 8
./source/riversim -V -t 2 -o third_lookup_8  --mesh-exp = 9
./source/riversim -V -t 2 -o third_lookup_9  --mesh-exp = 10
./source/riversim -V -t 2 -o third_lookup_10 --mesh-exp = 11
./source/riversim -V -t 2 -o third_lookup_11 --mesh-exp = 12
./source/riversim -V -t 2 -o third_lookup_12 --mesh-exp = 13


echo "LAST EXPERIMENT"
./source/riversim -V -t 2 -o last_lookup_1  --mesh-exp = 2  --refinment-radius = 0.02 --static-refinement-steps 1  
./source/riversim -V -t 2 -o last_lookup_2  --mesh-exp = 3  --refinment-radius = 0.04 --static-refinement-steps 2  
./source/riversim -V -t 2 -o last_lookup_3  --mesh-exp = 4  --refinment-radius = 0.08 --static-refinement-steps 3  
./source/riversim -V -t 2 -o last_lookup_4  --mesh-exp = 5  --refinment-radius = 0.10 --static-refinement-steps 4  
./source/riversim -V -t 2 -o last_lookup_5  --mesh-exp = 6  --refinment-radius = 0.12 --static-refinement-steps 5  
./source/riversim -V -t 2 -o last_lookup_6  --mesh-exp = 7  --refinment-radius = 0.14 --static-refinement-steps 6  
./source/riversim -V -t 2 -o last_lookup_7  --mesh-exp = 8  --refinment-radius = 0.16 --static-refinement-steps 7  
./source/riversim -V -t 2 -o last_lookup_8  --mesh-exp = 9  --refinment-radius = 0.18 --static-refinement-steps 8  
./source/riversim -V -t 2 -o last_lookup_9  --mesh-exp = 10 --refinment-radius = 0.20 --static-refinement-steps 8 
./source/riversim -V -t 2 -o last_lookup_10 --mesh-exp = 11 --refinment-radius = 0.22 --static-refinement-steps 8 
./source/riversim -V -t 2 -o last_lookup_11 --mesh-exp = 12 --refinment-radius = 0.24 --static-refinement-steps 8 
./source/riversim -V -t 2 -o last_lookup_12 --mesh-exp = 13 --refinment-radius = 0.26 --static-refinement-steps 8 