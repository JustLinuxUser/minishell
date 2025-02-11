id=
while (true); do
	inotifywait -e modify,create,delete *.c
	kill -9 $id
	make run &
	id=$!
	sleep 3
done
