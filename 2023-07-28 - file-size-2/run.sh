printf "[RUN] Compiling with parameters\n"
printf "\t gcc -l pthread -o file-size-2 file-size.c -g \n"
gcc -l pthread -o file-size-2 file-size.c -g

printf "[RUN] Executing with parameters\n"
printf "\t file-size-2 dir-1 dir-2\n"
./file-size-2 dir-1 dir-2