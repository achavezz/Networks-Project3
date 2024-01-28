all: Project3ClientA Project3ServerA

Project3ClientA: Project3Client.cpp NetworkHeader.h
	g++ -o Project3ClientA Project3Client.cpp

Project3ServerA: Project3ServerA.cpp NetworkHeader.h
	g++ -o Project3ServerA Project3ServerA.cpp