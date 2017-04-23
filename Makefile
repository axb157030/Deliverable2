all:  echo_c echo_s log_s


echo_c:			echo_c.o echo_c_functions.o
			gcc -o echo_c echo_c.o echo_c_functions.o

echo_c.o:		echo_c.c echo_c_functions.h
			gcc -c -g echo_c.c
			
echo_c_functions.o:	echo_c_functions.c
			gcc -c -g echo_c_functions.c

echo_s:			echo_s.o echo_s_functions.o
			gcc -o echo_s echo_s.o echo_s_functions.o

echo_s.o: 		echo_s.c echo_s_functions.h
			gcc -c -g echo_s.c
			
echo_s_functions.o:	echo_s_functions.c
			gcc -c -g echo_s_functions.c

log_s:			log_s.o log_s_functions.o
			gcc -o log_s log_s.o log_s_functions.o

log_s.o: 		log_s.c log_s_functions.h
			gcc -c -g log_s.c
			
log_s_functions.o:	log_s_functions.c
			gcc -c -g log_s_functions.c

clean:  
			rm -f echo_c echo_c.o echo_c_functions.o echo_s echo_s.o echo_s_functions.o log_s log_s.o log_s_functions.o
