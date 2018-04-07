# SOFE3950U-Lab5
Submission of Lab 5 for Operating Systems (SOFE3950U)

#Overview
Our program creates threads for each of the 5 customers. Once the threads are created, each thread
generates a random number of resources needed and requests for it. 

#Resources
The getResources function evaluates whether a certain resource demand is valid or not and whether it is safe. Once it is safe, the
resources are allocated and then they are released. 

#Mutex Locks
In order to prevent threads from obtaining the same resources at the same time we utilized mutex locks to avoid the race conditions.
