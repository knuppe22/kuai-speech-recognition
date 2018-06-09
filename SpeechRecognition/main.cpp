#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include "hmm.h"

using namespace std;

typedef struct {
	string name;
	int head;
	int tail;
	vector<int> phones;
} wordType;

vector<wordType> dic;	// (name, vector of phones)
int nstate = 1;		// Initial state
double** t;			// Transition probability matrix

int phone2int(string phone) {
	if (phone.compare("f") == 0)
		return 0;
	else if (phone.compare("k") == 0)
		return 1;
	else if (phone.compare("n") == 0)
		return 2;
	else if (phone.compare("r") == 0)
		return 3;
	else if (phone.compare("s") == 0)
		return 4;
	else if (phone.compare("t") == 0)
		return 5;
	else if (phone.compare("v") == 0)
		return 6;
	else if (phone.compare("w") == 0)
		return 7;
	else if (phone.compare("z") == 0)
		return 8;
	else if (phone.compare("ah") == 0)
		return 9;
	else if (phone.compare("ao") == 0)
		return 10;
	else if (phone.compare("ay") == 0)
		return 11;
	else if (phone.compare("eh") == 0)
		return 12;
	else if (phone.compare("ey") == 0)
		return 13;
	else if (phone.compare("ih") == 0)
		return 14;
	else if (phone.compare("iy") == 0)
		return 15;
	else if (phone.compare("ow") == 0)
		return 16;
	else if (phone.compare("th") == 0)
		return 17;
	else if (phone.compare("uw") == 0)
		return 18;
	else if (phone.compare("sil") == 0)
		return 19;
	else if (phone.compare("sp") == 0)
		return 20;
	else
		return -1;
}
int word2int(string word) {
	for (int i = 0; i<dic.size(); i++) {
		if (word.compare(dic[i].name) == 0)
			return i;
	}
	return -1;
}

void read_dic() {
	FILE *fp = fopen("dictionary.txt", "r");

	while (!feof(fp)) {
		wordType word;
		word.head = nstate;
		char line[100];
		fgets(line, 100, fp);
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';
		char *tokenized = strtok(line, "\t");
		word.name = string(tokenized);
		tokenized = strtok(NULL, " ");
		do {
			string phone(tokenized);
			if (phone.compare("sp") == 0)
				nstate++;
			else
				nstate += 3;
			word.phones.push_back(phone2int(phone));
			tokenized = strtok(NULL, " ");
		} while (tokenized != NULL);
		word.tail = nstate - 1;
		dic.push_back(word);
	}

	t = new double*[nstate];
	for (int i = 0; i < nstate; i++) {
		t[i] = new double[nstate];
	}

	fclose(fp);
}
void read_prob() {
	FILE *uni = fopen("unigram.txt", "r");

	while (!feof(uni)) {
		char buf[30];
		double prop;
		fscanf(uni, "%s %lf", buf, &prop);
		string word(buf);
		for (int i = 0; i < 3; i++) {
			t[0][dic[word2int(word)].head + i] = prop;
		}
	}

	fclose(uni);

	FILE *bi = fopen("bigram.txt", "r");

	while (!feof(bi)) {
		char buf1[30];
		char buf2[30];
		double prop;
		fscanf(bi, "%s %s %lf", buf1, buf2, &prop);
		string word1(buf1);
		string word2(buf2);
		if (dic[word2int(word1)].phones.back() == 20) {		// if word1 ends with "sp"
			for (int j = 0; j < 3; j++) {
				t[dic[word2int(word1)].tail][dic[word2int(word2)].head + j] = prop;
			}
		}
		else {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					t[dic[word2int(word1)].tail - i][dic[word2int(word2)].head + j] = prop;
				}
			}
		}
	}

	fclose(bi);
}

int main() {
	read_dic();
	read_prob();

	system("pause");
	return 0;
}
