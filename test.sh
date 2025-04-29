
#!/bin/sh
id=$RANDOM

echo $id env: $(env | grep test)

for arg in $@; do
	echo $id arg:">$arg<"
done

read -t 0.1 a
echo $id read: $a
