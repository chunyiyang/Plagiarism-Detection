//============================================================================
// Name        : Plagiarism.cpp
// Author      : Chunyi Yang
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++
//============================================================================

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>
#include <math.h>

#define K_LENGTH 9
//#define PER_NUM 3
#define D1  0.2
#define D2  0.8
#define P1  0.997
#define P2  0.003



using namespace std;

vector<vector<int> > shingleMatrix;
map<string, int>  indexTable;
vector<string> fileNames;
int g_fileNumber = 0;
int g_shingleNumber = 0;
int g_perNumber = 0;

// the bigger the better : P1
double falPos(int b, int r){
	double res =  pow((1 - pow(0.2, r)),b);
	cout << "P1 = " << res << " ";
	return res;
}

// the smaller the better: P2
double falNeg(int b, int r){
	double res = pow((1 - pow(0.8,r)), b);
	cout << "P2 = " << res << " ";
	return res;
}

bool bCheckbr(int b, int r){
	if ((falPos(b, r)>=P1)&&(falNeg(b, r)<=P2)) return true;
	else return false;
}

int getPernum(int &b, int &r){
	int pernum = sqrt(g_shingleNumber)+1;
	bool found = false;
	while(!found){
		for(int n=2;n<pernum;n++){
			if (pernum% n ==0){
				b = pernum/n;
				r = n;
				if (bCheckbr(b, r)){
					found = true;
					break;
				}
				cout << " permutation num = " << pernum << " b = " << b << " r = " << r << endl;
			}
		}

		pernum++;
	}
	return pernum;
}

void addShingle(string strshingle, vector<int> & fileShingle){
	map<string,int>::iterator  it;

	it = indexTable.find(strshingle);
	if (it!=indexTable.end()){
		fileShingle[it->second]+=1;
	}
	else{
		indexTable.insert(make_pair(strshingle,g_shingleNumber));
		g_shingleNumber++;
		fileShingle.push_back(1);
	}
}

int createShingle(ifstream & myfile){
	string strfile;
	string line;
	vector<int> fileShingle(g_shingleNumber, 0);
	bool bspace = false;
	int start=0;
	while(getline(myfile,line)){
		strfile.append(line);
	}
//	cout << strfile << endl;
	int i = 0;
	while(i<strfile.size()){
		if((strfile[i]>='a')&&(strfile[i]<='z')) {
			bspace = false;
			i++;
		}
		else if((strfile[i]>='A')&&(strfile[i])<='Z'){
			strfile[i] = strfile[i] - 'A' + 'a';
			bspace = false;
			i++;
		}
		else if((strfile[i]==' ') && (!bspace)){
			bspace = true;
			i++;
		}
		else if((strfile[i]>='0')&&(strfile[i]<='9')) {
			bspace = false;
			i++;
		}
		else strfile.erase(i,1);
	}
//	cout << strfile << endl;
	int n=strfile.size();
	while(start+8<n){
		addShingle(strfile.substr(start, K_LENGTH), fileShingle );
		start++;
	}
	shingleMatrix.push_back(fileShingle);
	return 0;
}

vector<string> getDoclists(){
	char c;
	vector<string> files;
	string name;
	while(cin.get(c)){
		if ((c==' '|| c== '\n') && name.size()>0){
//			cout << "filename = " << name << endl;
			files.push_back(name);
			name.clear();
		}
		else if((c==' '|| c== '\n') && name.size()==0){
		}
		else{
			name.push_back(c);
		}
	}
	if (name.size()>0){
		files.push_back(name);
	}
    return files;
}

bool readinfiles(vector<string> files){
    int size = files.size();
    string path = "/home/mwang2/test/coen281/";
    ifstream myfile;
    if(size<2){
        cout << "less than two files!"  << endl;
        return false;
    }
	for(int i=0; i<size; i++){
		myfile.open((path+files[i]).c_str());
		if(myfile.is_open()){
			fileNames.push_back(files[i]);
			createShingle(myfile);
			myfile.close();
		}
		else{
			cout << "error in filename!" << endl;
			return false;
		}
	}
	if(fileNames.size()<2){
		cout << "less than two files" << endl;
		return false;
	}
	return true;
}

bool appendShingleMatrix(){
	for(int i=0; i<shingleMatrix.size(); i++){
		int h=shingleMatrix[i].size();
		while(h<g_shingleNumber){
			shingleMatrix[i].push_back(0);
			h++;
		}
	}

	int lengthShingle = shingleMatrix.size();
	if (lengthShingle == 0){
		cout << "Error: no shingles!" << endl;
		return false;
	}
	int lengthFile = shingleMatrix[0].size();
	if (lengthFile == 0){
		cout << "Error: no docs!" << endl;
		return false;
	}
    cout << "******* Below are the original shingle matrix ******* "<< endl;
    cout << "              ";
	for(int i=0; i<fileNames.size(); i++){
		cout.width(20);
		cout << fileNames[i] ;
	}
	cout << endl;
	map<string,int>::iterator  it;
	it = indexTable.begin();

	for(int i=0; i<g_shingleNumber; i++){
		cout.width(10);
		cout << it->first ;
		for(int j=0; j<shingleMatrix.size(); j++){
			cout.width(20);
			cout << shingleMatrix[j][it->second] ;
		}
		cout << endl;
		it++;
	}
	return true;
}

// should I start from 0 or 1????
vector<vector<int> > createPermumatrix(){
	vector<vector<int> > permuMatrix;
	for (int i=0; i<g_perNumber; i++){
		vector<int> temp;
		for (int j=0; j<g_shingleNumber; j++){
			temp.push_back(j);  //
		}
		random_shuffle(temp.begin(), temp.end());
		permuMatrix.push_back(temp);
	}
	return permuMatrix;
}

vector<vector<int> > createSigmatrix(vector<vector<int> > & permu_matrix){
	vector<vector<int> > sigMatrix;
	int nsize = fileNames.size();
	
	for(int i=0; i<nsize; i++){
		cout.width(20);
		cout << fileNames[i] ;
	}
	cout << endl;

	for(int i=0; i<g_perNumber; i++){
		vector<int> temp;
		for (int j=0; j< nsize ;j++){
			int index = 0;
			while(shingleMatrix[j][permu_matrix[i][index]]==0 && index<g_shingleNumber){
				index++;
			}
			temp.push_back(permu_matrix[i][index]);
			cout.width(20);
			cout << permu_matrix[i][index] << " ";
		}
		sigMatrix.push_back(temp);
		cout << endl;
	}
	return sigMatrix;
}


map<string, vector<bool> >  mergeSigmatrix( vector<vector<int> > &sig_matrix, int b, int r){
	map<string, vector<bool> > minHashmap;
	map<string, vector<bool> >::iterator  it;

	while (b==0 || r==0) {
		cout << "Erro input!" << endl;
		return minHashmap;
	}
	int filesize = sig_matrix[0].size();

	cout << "lines = " << sig_matrix.size()  << " b = " << b << " r " << r << " filesize = " << sig_matrix[0].size() << endl;
	for (int i=0; i<b; i++){
		for (int j=0; j<filesize; j++){
			string strrowcombine;
			for(int k=0; k<r; k++){
				char buffer[20];
				sprintf(buffer, "%d,", sig_matrix[i*r+k][j]);
				strrowcombine.append(buffer);
			}
			it = minHashmap.find(strrowcombine);
			if (it != minHashmap.end()){
				it->second[j]=true;
			}
			else{
				vector<bool> mapvalue (filesize,false);
				mapvalue[j]=true;
				minHashmap.insert(make_pair(strrowcombine,mapvalue ));
			}
		}
	}
	it = minHashmap.begin();
	cout << "******* Below is the min-hash table*******" << endl;
	while (it!=minHashmap.end()){
	    cout.width(25);
		cout << it->first << "   ";
		for (int j=0; j<fileNames.size(); j++){
		    cout.width(15);
			cout << it->second[j] << "   ";
		}
		cout<<endl;
		it++;
	}
	return minHashmap;
}

vector<pair<int, int> >  getCandidates( map<string, vector<bool> > & minHashmap){
	vector<pair<int, int> >  candipairs;
	map<string, vector<bool> >::iterator  it;
	for(int i=0;i<fileNames.size();i++){
		for (int j=i+1; j<fileNames.size(); j++){
			bool found = false;
			it = minHashmap.begin();
			while(it!= minHashmap.end()&&!found){
				if(it->second[i]==1 && it->second[j]==1){
					found = true;
					candipairs.push_back(make_pair(i,j));
					cout << "******* Below is the candidate pairs ******* " << endl;
					cout.width(20);
					cout << fileNames[i] << "       and" ;
					cout.width(20);
					cout << fileNames[j] << endl;
				}
				it++;
			}
		}
	}
	return candipairs;
}

vector<pair<int, int> >  finalCheck(const vector<pair<int, int> > & candi_pairs ){
	vector<pair<int, int> > sim_lists;
	int nsize = candi_pairs.size();
	for (int i=0; i<nsize; i++){
		int count = 0;
		int totalnum = 0;
		int j=0;
		while(j<g_shingleNumber){
			if(shingleMatrix[candi_pairs[i].first][j] == shingleMatrix[candi_pairs[i].second][j]){
				count ++;
			}
			if(shingleMatrix[candi_pairs[i].first][j] >0  ||  shingleMatrix[candi_pairs[i].second][j]>0){
				totalnum ++;
			}
			j++;
		}
		if(count>totalnum*D2){
			sim_lists.push_back(candi_pairs[i]);
		}
	}

	return sim_lists;
}

void printResult(const vector<pair<int, int> > & sim_lists){
	int nsize = sim_lists.size();
	if (nsize == 0){
		cout << " no similar files" << endl;
		return;
	}
	else{
		cout << "******* Below are the similar file pairs ******* "<< endl;
		for(int i=0; i<nsize; i++){
			cout.width(20);
			cout << fileNames[sim_lists[i].first] << "       and";
			cout.width(20);
			cout << fileNames[sim_lists[i].second] << endl;
		}
	}
}

int main() {
	vector<vector<int> > permu_matrix;
	vector<vector<int> > sig_matrix;
	map<string, vector<bool> > minHashmap;
	vector<pair<int, int> > candi_pairs;
	vector<pair<int, int> > sim_lists;
	vector<string>  doclists;
	int b, r;
    doclists = getDoclists();
    if(!readinfiles(doclists)){
        cout << "error in file names!" << endl;
        return -1;
    }
	
	appendShingleMatrix();

	getPernum(b, r);
	g_perNumber = b*r;
	cout << endl;
	cout << "******** permutation = " << g_perNumber  << " b = " << b << " r " << r << endl;
	cout << "******** P1: "<< falPos(b,r) << endl;
	cout << "******** P2: "<< falNeg(b,r) << endl;

	// get permutation matrix
	permu_matrix = createPermumatrix();

	// use permutation matrix to create signature matrix
	sig_matrix = createSigmatrix(permu_matrix);

	// use r and b to rearrange signature matrix
	minHashmap = mergeSigmatrix(sig_matrix, b, r);

	// get all candidate pairs candi_pairs
	candi_pairs = getCandidates(minHashmap);
	// for each candidate pairs, run Jaccord similarity check to make sure similarity is about 0.8
	sim_lists = finalCheck(candi_pairs);

	printResult(sim_lists);

	return 0;
}

	
