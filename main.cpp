#include <map>
#include <deque>
#include <fstream>
#include <iostream>
#include <openssl/md5.h>

using namespace std;

struct Signature
{
	__int128 hash;
	string name;
};

map <size_t, deque <Signature>> signs;

bool loadSignatures()
{
	fstream file("main.mdb", ios::in);
	if(!file.good())
	{
		cout<<"Unable to open file: main.mdb"<<endl;
		return false;
	}

	while(1)
	{
		string str[3];
		getline(file, str[0], ':');
		getline(file, str[1], ':');
		getline(file, str[2]);
		if(file.eof())
			break;

		__int128 ret = 0;

		for(int n = str[1].size()-1; n >= 0; n--)
		{
			if(n % 2 != 0)
				swap(str[1][n], str[1][n-1]);

			if(str[1][n] >= '0' && str[1][n] <= '9')
				ret = (ret<<4)+(str[1][n] - 48);
			else if(str[1][n] >= 'a' && str[1][n] <= 'f')
				ret = (ret<<4)+(str[1][n] - 87);
		}

		signs[stoul(str[0])].push_back({ret, str[2]});
	}

	file.close();
	return true;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		cout<<"Syntax: "<<argv[0]<<" [file]"<<endl;
		return 1;
	}

	if(!loadSignatures())
		return 2;


	fstream file;
	file.open(argv[1], ios::in);
	if(!file.good())
	{
		cout<<"Unable to open file: "<<argv[1]<<endl;
		return 3;
	}

	file.seekg(0, file.end);
	size_t size = file.tellg();
	file.seekg (0, file.beg);
	char *buf = new char[size];
	file.read(buf, size);

	file.close();

	string found = "";
	for(auto const& s : signs)
	{
		#pragma omp parallel for
		for(size_t n = 0; n < size-s.first; n++)
		{
			__int128 result;
			MD5((const unsigned char*)&buf[n], s.first, (unsigned char*)&result);
			for(auto ss : s.second)
				if(result == ss.hash)
					found = ss.name;
		}
		if(found != "")
		{
			cout<<"Found "<<found<<endl;
			return 0;
		}
	}
	return 0;
}
