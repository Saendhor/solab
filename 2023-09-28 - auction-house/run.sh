print "[RUN] Checking for existing executable and deleting\n"
rm auction-house

print "[RUN] Compiling with parameters\n"
print "\tgcc -l pthread -o auction-house auction-house.c -g\n"
gcc -l pthread -o auction-house auction-house.c -g

print "[RUN] Running with parameters\n"
print "\tauction-house auctions.txt 4\n"
./auction-house auctions.txt 4