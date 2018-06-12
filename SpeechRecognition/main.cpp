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
double **t;			// Transition probability matrix
double *in, *out;

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
	for (int i = 0; i < dic.size(); i++) {
		if (word.compare(dic[i].name) == 0)
			return i;
	}
	return -1;
}
int nstate_of(int phone) {
	return (phone == 20) ? 1 : 3;
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
			int phone = phone2int(string(tokenized));
			nstate += nstate_of(phone);
			word.phones.push_back(phone);
			tokenized = strtok(NULL, " ");
		} while (tokenized != NULL);
		word.tail = nstate - 1;
		dic.push_back(word);
	}

	fclose(fp);
}

void make_t() {
	t = new double*[nstate];
	for (int i = 0; i < nstate; i++) {
		t[i] = new double[nstate];
		for (int j = 0; j < nstate; j++) {
			t[i][j] = 0.0;
		}
	}
	in = new double[nstate];
	out = new double[nstate];
	for (int i = 0; i < nstate; i++) {
		in[i] = 1.0;
		out[i] = 1.0;
	}

	// word hmm ¸¸µé±â
	for (int i = 0; i < dic.size(); i++) {
		// forward
		int state = dic[i].head;
		for (int j = 0; j < dic[i].phones.size(); j++) {
			int phone = dic[i].phones[j];
			for (int k = 0; k < nstate_of(phone); k++) {
				in[state + k] *= phones[phone].tp[0][k + 1];
				if (j > 0) {
					int prev = dic[i].phones[j - 1];
					in[state + k] *= phones[prev].tp[0][nstate_of(prev) + 1];
				}

				for (int l = 0; l < nstate_of(phone); l++) {
					t[state + k][state + l] = phones[phone].tp[k + 1][l + 1];
				}
			}
			state += nstate_of(phone);
		}
		// backward
		state = dic[i].tail - nstate_of(dic[i].phones.back()) + 1;
		for (int j = dic[i].phones.size() - 1; j >= 0; j--) {
			int phone = dic[i].phones[j];
			for (int k = 0; k < nstate_of(phone); k++) {
				out[state + k] *= phones[phone].tp[k + 1][nstate_of(phone) + 1];
				if (j < dic[i].phones.size() - 1) {
					int next = dic[i].phones[j + 1];
					out[state + k] *= phones[next].tp[0][nstate_of(next) + 1];
				}
			}
			if (j > 0)
				state -= nstate_of(dic[i].phones[j - 1]);
		}
	}
}

void read_prob() {
	FILE *uni = fopen("unigram.txt", "r");

	while (!feof(uni)) {
		char buf[30];
		double prop;
		fscanf(uni, "%s %lf", buf, &prop);
		int word = word2int(string(buf));
		for (int i = dic[word].head; i <= dic[word].tail; i++) {
			t[0][i] += prop * in[i];
		}
	}

	fclose(uni);

	FILE *bi = fopen("bigram.txt", "r");

	while (!feof(bi)) {
		char buf1[30];
		char buf2[30];
		double prop;
		fscanf(bi, "%s %s %lf", buf1, buf2, &prop);
		int word1 = word2int(string(buf1));
		int word2 = word2int(string(buf2));
		for (int i = dic[word1].head; i <= dic[word1].tail; i++) {
			for (int j = dic[word2].head; j <= dic[word2].tail; j++) {
				t[i][j] += prop * out[i] * in[j];
			}
		}
	}

	fclose(bi);
}

int main() {
	read_dic();
	make_t();
	read_prob();

	for (int i = 0; i < nstate; i++) {
		for (int j = 0; j < nstate; j++) {
			cout << i << ":" << j << " = " << t[i][j] << endl;
		}
	}

	system("pause");
	return 0;
}
