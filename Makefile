all:
	gcc hard_drive.c main.c -o main
run:
	sudo ./main;google-chrome output.html
clean:
	rm output.html main
