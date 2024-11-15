# Program-4
i. Which parts of the assignment were completed: Program 1, 2 and 3 

ii. Any bugs that you have encountered
My program 3 has been full of bugs, and I could not exactly figure out why. 

iii. Complete instructions of how to run our program and iv. The input file (if any) and the output files (if any) 

g++ s1.cc image.cc -o s1
./s1 sphere0.pgm 200 output.txt

g++ s2.cc image.cc -o s2
./s2 parameters.txt sphere1.pgm sphere2.pgm sphere3.pgm output_directions.txt           

g++ s3.cc image.cc -o s3
./s3 output_directions.txt object1.pgm object2.pgm object3.pgm 10 50 output_normals.pgm output_albedo.pgm
