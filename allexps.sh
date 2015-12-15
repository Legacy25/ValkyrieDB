echo "Setting up 100MB data"

./test/tpch-data-setup.py 0.1

echo "Data setup complete"

rm -rf small
mkdir small

echo "Running 1.sql [SMALL]"
./exp.sh test/sql/testcases/1.sql &>small/t1.out;

echo "Running 2.sql [SMALL]"
./exp.sh test/sql/testcases/2.sql &>small/t2.out;

echo "Running 3.sql [SMALL]"
./exp.sh test/sql/testcases/3.sql &>small/t3.out;

echo "Running 4.sql [SMALL]"
./exp.sh test/sql/testcases/4.sql &>small/t4.out;

echo "Running 5.sql [SMALL]"
./exp.sh test/sql/testcases/5.sql &>small/t5.out;

echo "Setting up 1GB data"

./test/tpch-data-setup.py 1

echo "Data setup complete"

rm -rf large
mkdir large


echo "Running 1.sql [LARGE]"
./exp.sh test/sql/testcases/1.sql &>large/t1.out;

echo "Running 2.sql [LARGE]"
./exp.sh test/sql/testcases/2.sql &>large/t2.out;

echo "Running 3.sql [LARGE]"
./exp.sh test/sql/testcases/3.sql &>large/t3.out;

echo "Running 4.sql [LARGE]"
./exp.sh test/sql/testcases/4.sql &>large/t4.out;

echo "Running 5.sql [LARGE]"
./exp.sh test/sql/testcases/5.sql &>large/t5.out;