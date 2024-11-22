printf "[RUN] Compiling with parameters\n"
printf "\tgcc -l pthread -o calc-verifier calc-verifier.c -g\n"
gcc -l pthread -o calc-verifier calc-verifier.c -g


printf "[RUN] Running with parameters\n"
printf "\tcalc-verifier calc1.txt calc2.txt calc3.txt\n"
printf "\n"
./calc-verifier calc1.txt calc2.txt calc3.txt