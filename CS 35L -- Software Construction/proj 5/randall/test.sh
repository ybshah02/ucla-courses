test1=$(./randall 50 | wc -c); 
if [ $test1 -eq 50 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test2=$(./randall -o 11 60 | wc -c );
 if [ $test2 -eq 60 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test3=$(./randall -i rdrand 70 | wc -c); 
if [ $test3 -eq 70 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test4=$(./randall -i ~/testdoc.txt 50 | wc -c);
 if [ $test4 -eq 50 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test5=$(./randall -i mrand48_r 50 | wc -c);
if [ $test5 -eq 50 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test6=$(./randall -i ~/testdoc.txt -o 5 10 | wc -c);
 if [ $test6 -eq 10 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test7=$(./randall -i ~/testdoc.txt -o stdio 50 | wc -c);
 if [ $test7 -eq 50 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test8=$(./randall -i ~/testdoc.txt 80 | wc -c );
 if [ $test8 -eq 80 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi
test9=$(./randall -i ~/testdoc.txt -o 10 80 | wc -c );
if [ $test9 -eq 80 ];
 then 
 	echo "Success!"; 
 else 
 	echo "Failure.";
fi