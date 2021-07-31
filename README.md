COP6726 – Database System Implementation
Project 5 – Report

Team members:

1.	Akshay Ganapathy
2.	Riyaz Shaik

Project Demo video included inside the zip file.

Steps to execute project code:
The code was executed using Windows Subsystem for Linux (Ubuntu 18.04) environment.
1.	The project source code is in the folder “a5-test”. Its contents include the following:
	a.	Code needed to run main.cc.
	b.	All “.bin” and “.meta” files included in the “a5-test” folder.
	c.	Google test framework has been added in the directory “googletest”. This framework is needed to run gtest unit test cases.
	d.	All the gtest unit test cases have been added in “a5-test” folder. 
	e.	Makefile to compile main.cc and gtest unit test cases.
2.	To run main.cc, from project root execute command “make a5.out”. Run a5.out by command “./a5.out” followed by list of commands:
	1. Fireup the database
	2. Enter your CNF
	3. Close the database
3.	To run gtest unit test cases, from project root execute command “make gtest.out” and then execute “./gtest.out”.
