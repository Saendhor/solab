printf "[RUN] Compiling with following parameters:\n"
printf "\tgcc -l pthread -o file-size file-size.c -g\n"
gcc -l pthread -o file-size file-size.c -g

printf "[RUN] Executing command:\n"
printf "\tfile-size dir-1 dir-2 dir-3\n\n"
./file-size dir-1 dir-2 dir-3