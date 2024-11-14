# To add a new algorithm in PgRouting

## Installation

Requirements for `cmake` :
```shell
sudo apt update
sudo apt install build-essential
sudo apt install libboost-graph-dev
sudo apt install postgresql-server-dev-14
sudo apt install sphinx
```

Install `cmake` and `g++` :
```shell
sudo apt install cmake
sudo apt install g++
```

Compile :
```shell
mkdir build
cd build
cmake -DWITH_DOC=ON ..
make
sudo make install
```

Pour installer l'extension : 
```sql
dropdb --if-exists -h localhost -U postgres -p 5432 tests_pgrouting
createdb -h localhost -U postgres -p 5432 tests_pgrouting
psql -h localhost -U postgres -p 5432 -d tests_pgrouting 
> CREATE EXTENSION pgrouting CASCADE;
> SELECT pgr_version();
```

## How modules are structured?

A PgRouting function is part of a module. For example, if we work in the module called `contraction` and want to add inside two functions:
- one to build a hierarchy contracted graph;
- one to apply a bidirectional Dijkstra shortest path algorithm.

Creating the first function will actually consist in adding a parameter to the existing `pgr_contraction` function, for which there are two already implemented contraction methods:
- dead-ends contraction (value 1);
- linear contraction (value 2).
We will add a third method, corresponding to contractions hierarchy.

Creating the second function will consist in creating a new set of files:
1. `src/<module>/<function>.c` which contains:
	- a `_<function>` method, having the same parameters as the SQL function (`PG_FUNCTION_ARGS`), that converts entries and exits between SQL and C and calls the `process` function;
	- a `process` method, that calls a `do_pgr_<function>` C++ function, declared in the `include/drivers/<module>/<function>_driver.h` file and implemented in the `src/<module>/<function>_driver.cpp`;

2. `src/<module>/<function>_driver.cpp` which contains:
	- a `pgr_<function>` method;
	- a `do_pgr_<function>` method;
This file can reference one or several `.hpp` files of the `include/<module>` folder. These files can contain additional functions, associated to their `.cpp` file in the `src/<module>` folder.

3. a `sql/<module>/_<function>.sql` and a `sql/<module>/<function>.sql` file;

4. a `drivers/<module>/<function>_driver.h` file.

## How to run tests?

There are two types of tests: the ones driven by `pgtap` and the ones made for the documentation.

First, install `pgtap`:
```shell
sudo apt install pgtap
sudo apt install postgresql-14-pgtap
```

Then, export the environment variables corresponding to your PostgreSQL connection:
```shell
export PGUSER="postgres"
export PGPASSWORD="postgres"
export PGHOST="localhost"
```
It will avoid that you have to type your password before the database creation and dropping operations. These variables are only valid for the session. To make them persistant, you must put them in the `.bashrc` file.

Copy the test script `tools/developer/run.sh`:

```shell
cp tools/developer/run.sh ./build
bash build/run.sh
```


